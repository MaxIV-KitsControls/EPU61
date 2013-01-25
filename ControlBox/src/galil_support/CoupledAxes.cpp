//=============================================================================
// CoupledAxes.cpp
//=============================================================================
// abstraction.......Galil CoupledAxes abstraction class
// use...............for special geared axes that cannot be handled by CoupledAxes
//                   this class drives only the master axis
// class.............CoupledAxes
// original author...N.Leclercq - J.Coquet - SOLEIL
//=============================================================================

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <tango.h>
#include "AxisFactory.h"
#include "CoupledAxes.h"

// ============================================================================
// SHORTCUTS TO EACH <CMD REQUEST VALIDATION> CRITERIA COMPONENTS
// ============================================================================
#define MASTER_NOT_OFF              (this->m_master_axis->state() != galil::AXIS_OFF)
//-----------------------------------------------------------------------------
#define MASTER_NOT_FAULT            (this->m_master_axis->state() != galil::AXIS_ERROR)
//-----------------------------------------------------------------------------
#define MASTER_NOT_MOVING           (this->m_master_axis->state() != galil::AXIS_MOVING)
//-----------------------------------------------------------------------------
#define MASTER_NO_FORW_LSW_PRESENT  (! this->m_master_axis->forward_limit_switch_detected())
//-----------------------------------------------------------------------------
#define MASTER_NO_BACK_LSW_PRESENT  (! this->m_master_axis->backward_limit_switch_detected()) 
//-----------------------------------------------------------------------------
#define AXIS_NOT_GEARING_SLAVE      (! this->m_master_axis->is_gearing_slave()) 

//=============================================================================
// CMD NAMES
//=============================================================================
const char * coupled_axes_command_name[] =
{
  "<Forward>",
  "<Backward>",
  "<Abs.Positionning>",
  "<Unknown Galil Command>"
};

// ============================================================================
// CRITERIA IDENTIFIER (FOR WHY CMD REFUSED)
// ============================================================================
static enum
{
  kMASTER_NOT_OFF = 0,
  kMASTER_NOT_FAULT,
  kMASTER_NOT_MOVING,
  kMASTER_NO_FORW_LSW_PRESENT,
  kMASTER_NO_BACK_LSW_PRESENT,
	kAXIS_NOT_GEARING_SLAVE,
  kUNKNOWN_CRITERIA
} CoupledAxesCmdAllowedCriteria;

// ============================================================================
// CRITERIA STRINGS (FOR WHY CMD REFUSED)
// ============================================================================
static const std::string kWHY[] =
{ 
  "master axis is off",
  "master axis is in fault",
  "master axis is moving",
  "master axis limit switch forward detected",
  "master axis limit switch backward detected",
	"master axis is gearing slave"
  "the system could not deduce the reason why the command was refused"
};


// ============================================================================
// <CMD REQUEST VALIDATION> CRITERIA
// ============================================================================
#define ABS_POS_MOVE_ALLOWED  \
	      MASTER_NOT_OFF && \
	      MASTER_NOT_FAULT && \
	      MASTER_NOT_MOVING && \
	      AXIS_NOT_GEARING_SLAVE
//-----------------------------------------------------------------------------
static const size_t ABS_POS_MOVE_CRITERIA_ARRAY[] =
{
  4,
  kMASTER_NOT_OFF,
  kMASTER_NOT_FAULT,
  kMASTER_NOT_MOVING,
	kAXIS_NOT_GEARING_SLAVE
};
//=============================================================================
//=============================================================================
#define JOG_FORWARD_MOVE_ALLOWED \
	      MASTER_NOT_OFF && \
	      MASTER_NOT_FAULT && \
	      MASTER_NOT_MOVING && \
				MASTER_NO_FORW_LSW_PRESENT && \
				AXIS_NOT_GEARING_SLAVE

//-----------------------------------------------------------------------------
static const size_t JOG_FORWARD_MOVE_CRITERIA_ARRAY[] =
{
  5,
  kMASTER_NOT_OFF,
  kMASTER_NOT_FAULT,
  kMASTER_NOT_MOVING,
	kMASTER_NO_FORW_LSW_PRESENT,
	kAXIS_NOT_GEARING_SLAVE
};
//=============================================================================
//=============================================================================
#define JOG_BACKWARD_MOVE_ALLOWED \
	      MASTER_NOT_OFF && \
	      MASTER_NOT_FAULT && \
	      MASTER_NOT_MOVING && \
				MASTER_NO_BACK_LSW_PRESENT && \
				AXIS_NOT_GEARING_SLAVE
//-----------------------------------------------------------------------------
static const size_t JOG_BACKWARD_MOVE_CRITERIA_ARRAY[] =
{
  5,
  kMASTER_NOT_OFF,
  kMASTER_NOT_FAULT,
  kMASTER_NOT_MOVING,
	kMASTER_NO_BACK_LSW_PRESENT,
	kAXIS_NOT_GEARING_SLAVE
};
//=============================================================================

//=============================================================================
// SEARCH_FAULTY_CRITERIA
//=============================================================================
#define SEARCH_FAULTY_CRITERIA(CRITERIA_ARRAY) \
{ \
  for (size_t c = 1; c <= CRITERIA_ARRAY[0]; c++) \
  { \
    if (this->eval_criteria(CRITERIA_ARRAY[c]) == false) \
      return CRITERIA_ARRAY[c]; \
  } \
}

namespace galil
{
// ============================================================================
// CoupledAxesStatus::CoupledAxesStatus
// ============================================================================
CoupledAxesStatus::CoupledAxesStatus ()
{
  //- noop ctor
}

	
// ============================================================================
// CoupledAxesStatus::operator=
// ============================================================================
void CoupledAxesStatus::operator= (const CoupledAxesStatus& src)
{
  if (&src == this)
    return;

  this->master_status = src.master_status;
	this->coupled_axes_status = src.coupled_axes_status;
}

// ============================================================================
// CoupledAxes::Config::Config
// ============================================================================
CoupledAxes::Config::Config ()
 : master (MIN_AXIS),
   host_device (0)
{
  //- noop ctor
}

// ============================================================================
// CoupledAxes::Config::operator=
// ============================================================================
void CoupledAxes::Config::operator= (const Config& src)
{
  if (&src == this)
    return;

  this->master = src.master;
  this->host_device = src.host_device;
}

// ============================================================================
// CoupledAxes::CoupledAxes 
// ============================================================================
CoupledAxes::CoupledAxes ()
 : yat4tango::TangoLogAdapter (0),
   m_cfg (),
   m_master_axis(0)
{
	YAT_TRACE("galil::CoupledAxes::CoupledAxes");
}

// ============================================================================
// CoupledAxes::~CoupledAxes 
// ============================================================================
CoupledAxes::~CoupledAxes  (void)
{
  YAT_TRACE("galil::CoupledAxes::~CoupledAxes");

  //- release reference to the master Axis
  SAFE_RELEASE(this->m_master_axis);
}

// ============================================================================
// CoupledAxes::configure
// ============================================================================
void CoupledAxes::configure (const Config& _cfg) throw (Tango::DevFailed)
{
	YAT_TRACE("galil::CoupledAxes::configure");

  //- release reference to the master Axis
  SAFE_RELEASE(this->m_master_axis);

  //- copy the config locally
  this->m_cfg = _cfg;

  //- set the host device
  this->host(this->m_cfg.host_device);

  //- get a ref. master axis
  try 
  {
    this->m_master_axis = AXIS_FACTORY->get_axis(this->m_cfg.master);
  }
  catch (Tango::DevFailed& df)
  {
    ERROR_STREAM << df << std::endl;
    yat::OSStream err_desc;
    err_desc << "master axis error - axis " 
             << Axis::axis_id_to_string(this->m_cfg.master)
             << " does not exist - should be previously instanciated";
    THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                    _CPTC(err_desc.str().c_str()),
                    _CPTC("galil::CoupledAxes::configure"));
  }
}

// ============================================================================
// CoupledAxes::position
// ============================================================================
void CoupledAxes::position (double _p) throw (Tango::DevFailed)
{
	YAT_TRACE("galil::CoupledAxes::position");

  std::string why_not;

  if (! this->command_allowed(galil::COUPLED_DEVICE_ABS_POS_CMD, &why_not))
  {
    THROW_DEVFAILED(_CPTC("COMMAND_NOT_ALLOWED"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::CoupledAxes::position"));
  }
  
  //- set master axis postion (slave will follow)
  this->m_master_axis->abs_pos(_p);
}

// ============================================================================
// CoupledAxes::position
// ============================================================================
double CoupledAxes::position ()
{
	YAT_TRACE("galil::CoupledAxes::position");
	double new_p;
	new_p = this->m_master_axis->abs_pos();
  return new_p;
}

// ============================================================================
// CoupledAxes::forward
// ============================================================================
void CoupledAxes::forward() throw (Tango::DevFailed)
{
	YAT_TRACE("galil::CoupledAxes::forward");

  std::string why_not;

  if (! this->command_allowed(galil::COUPLED_DEVICE_FORWARD_CMD, &why_not))
  {
    THROW_DEVFAILED(_CPTC("COMMAND_NOT_ALLOWED"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::CoupledAxes::forward"));
  }
  
  //- master axis forward command (slaves will follow)
  this->m_master_axis->forward();
}

// ============================================================================
// CoupledAxes::backward
// ============================================================================
void CoupledAxes::backward() throw (Tango::DevFailed)
{
	YAT_TRACE("galil::CoupledAxes::backward");

  std::string why_not;

  if (! this->command_allowed(galil::COUPLED_DEVICE_BACKWARD_CMD, &why_not))
  {
    THROW_DEVFAILED(_CPTC("COMMAND_NOT_ALLOWED"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::CoupledAxes::backward"));
  }
  
  //- master axis backward command (slaves will follow)
  this->m_master_axis->backward();
}

// ============================================================================
// CoupledAxes::stop
// ============================================================================
void CoupledAxes::stop() throw (Tango::DevFailed)
{
	YAT_TRACE("galil::CoupledAxes::stop");

  //- master axis stop command (slaves will follow)
  this->m_master_axis->stop();
}

// ============================================================================
// CoupledAxes::state 
// ============================================================================
AxisState CoupledAxes::state ()
{
	YAT_TRACE("galil::CoupledAxes::state");

  galil::AxisState master_state = this->m_master_axis->state();
  return master_state;


  //- we should never pass here -------------
  return galil::AXIS_UNKNOWN;
}

// ============================================================================
// CoupledAxes::status 
// ============================================================================
void CoupledAxes::status (CoupledAxesStatus & status_)  throw (Tango::DevFailed)
{
	YAT_TRACE("galil::CoupledAxes::status");

  //- get master status
  this->m_master_axis->status(status_.master_status);

	//- for extensions : here will go specific coupled axes messages 
  //- normal operation message
  status_.coupled_axes_status = "CoupledAxes normal operation";

}

//---------------------------------------------------------
// CoupledAxes::command_allowed
//---------------------------------------------------------
// Giving the current status of the CoupledAxes, this 
// method checks whether or not the specified command is 
// allowed. In case the command is refused and the <why> is 
// not NULL, the reason is returned in the provided string.
//---------------------------------------------------------
bool CoupledAxes::command_allowed (CoupledAxesCommandIdentifier _cid, std::string * _why)
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::CoupledAxes::command_allowed");

//- usefull debug
/*FATAL_STREAM << " <<<<<<<<<<<<<<<<<<<<<<CoupledAxes::command_allowed : >>>>>>>>>>>>>>>>>>>>>>>>" << std::endl
             << " valeur MASTER_NOT_OFF :             " << MASTER_NOT_OFF << endl 
             << " valeur MASTER_NOT_FAULT :           " << MASTER_NOT_FAULT << endl 
             << " valeur MASTER_NOT_MOVING :          " << MASTER_NOT_MOVING << endl 
             << " valeur MASTER_NO_FORW_LSW_PRESENT : " << MASTER_NO_FORW_LSW_PRESENT << endl 
             << " valeur MASTER_NO_BACK_LSW_PRESENT : " << MASTER_NO_FORW_LSW_PRESENT << endl 
             << " valeur AXIS_NOT_GEARING_SLAVE :     " << AXIS_NOT_GEARING_SLAVE << endl ;

*/

  bool cmd_allowed = false;

  //- is the specified command allowed?
  switch (_cid)
  {
    case COUPLED_DEVICE_FORWARD_CMD:
      YAT_LOG("galil::CoupledAxes::command_allowed::eval.criteria::COUPLED_DEVICE_FORWARD_CMD"); 
      cmd_allowed = JOG_FORWARD_MOVE_ALLOWED;
      break;
    case COUPLED_DEVICE_BACKWARD_CMD:
      YAT_LOG("galil::CoupledAxes::command_allowed::eval.criteria::COUPLED_DEVICE_BACKWARD_CMD"); 
      cmd_allowed = JOG_BACKWARD_MOVE_ALLOWED;
      break;
    case COUPLED_DEVICE_ABS_POS_CMD:
      YAT_LOG("galil::CoupledAxes::command_allowed::eval.criteria::COUPLED_DEVICE_ABS_POS_CMD"); 
      cmd_allowed = ABS_POS_MOVE_ALLOWED;
      break;
    default:
      break;
  }

  if (! cmd_allowed)
  {
    //- search <first> criteria which failed 
    int c = this->why_cmd_refused(_cid);

    //- verbose
    ERROR_STREAM << "CoupledAxes::command_allowed::rejecting "
                 << coupled_axes_command_name[_cid]
                 << " cmd request::"
                 << kWHY[c]
                 << std::endl;

    //- why is the command refused?
    if (_why)
    {
      //- set the reason for caller 
      yat::OSStream oms;
      oms << coupled_axes_command_name[_cid]
          << " cmd rejected::"
          << kWHY[c]
          << std::ends;
      *_why = oms.str(); 
    }
  }

  return cmd_allowed;
}

// ============================================================================
// CoupledAxes::why_cmd_refused 
// ============================================================================
int CoupledAxes::why_cmd_refused (CoupledAxesCommandIdentifier _cid) const
{
  YAT_TRACE("galil::CoupledAxes::why_cmd_refused");

  switch (_cid)
  {
    case COUPLED_DEVICE_FORWARD_CMD:
      SEARCH_FAULTY_CRITERIA (JOG_FORWARD_MOVE_CRITERIA_ARRAY);
      break;
    case COUPLED_DEVICE_BACKWARD_CMD:
      SEARCH_FAULTY_CRITERIA (JOG_BACKWARD_MOVE_CRITERIA_ARRAY);
      break;
    case COUPLED_DEVICE_ABS_POS_CMD:
      SEARCH_FAULTY_CRITERIA (ABS_POS_MOVE_CRITERIA_ARRAY);
      break;
    default:
      return NO_COMMAND;
      break;
  }

  return kUNKNOWN_CRITERIA;
}


//=============================================================================
// CoupledAxes::eval_criteria
//=============================================================================
bool CoupledAxes::eval_criteria (int cid) const
{
  YAT_TRACE("galil::CoupledAxes::eval_criteria");

  bool criteria_eval = false;

  switch (cid)
  {
    case kMASTER_NOT_OFF:
      criteria_eval = MASTER_NOT_OFF;
      break;
    case kMASTER_NOT_FAULT:
      criteria_eval = MASTER_NOT_FAULT;
      break;
    case kMASTER_NOT_MOVING:
      criteria_eval = MASTER_NOT_MOVING;
      break;
    case kMASTER_NO_FORW_LSW_PRESENT:
      criteria_eval = MASTER_NO_FORW_LSW_PRESENT;
      break;
    case kMASTER_NO_BACK_LSW_PRESENT:
      criteria_eval = MASTER_NO_BACK_LSW_PRESENT;
      break;
    case kAXIS_NOT_GEARING_SLAVE:
      criteria_eval = AXIS_NOT_GEARING_SLAVE;
      break;
  }

  return criteria_eval;
}

} //- namespace galil
