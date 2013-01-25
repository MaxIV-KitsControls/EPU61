//=============================================================================
// GearedAxes.cpp
//=============================================================================
// abstraction.......Galil axis abstraction class
// class.............GearedAxes
// original author...N.Leclercq - J.Coquet - SOLEIL
//=============================================================================

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <tango.h>
#include "AxisFactory.h"
#include "GearedAxes.h"

// ============================================================================
// SHORTCUTS TO EACH <CMD REQUEST VALIDATION> CRITERIA COMPONENTS
// ============================================================================
#define MASTER_NOT_OFF             (this->m_master_axis->state() != galil::AXIS_OFF)
//-----------------------------------------------------------------------------
#define SLAVE_NOT_OFF              (this->m_slave_axis->state() != galil::AXIS_OFF)
//-----------------------------------------------------------------------------
#define MASTER_NOT_FAULT           (this->m_master_axis->state() != galil::AXIS_ERROR)
//-----------------------------------------------------------------------------
#define SLAVE_NOT_FAULT            (this->m_slave_axis->state() != galil::AXIS_ERROR)
//-----------------------------------------------------------------------------
#define MASTER_NOT_MOVING          (this->m_master_axis->state() != galil::AXIS_MOVING)
//-----------------------------------------------------------------------------
#define INDEP_MODE                 (! this->m_slave_axis->gearing_enabled())
//-----------------------------------------------------------------------------
#define SLAVE_NOT_MOVING           (! INDEP_MODE || this->m_slave_axis->state() != galil::AXIS_MOVING)
//-----------------------------------------------------------------------------
#define SLAVE_NO_FORW_LSW_PRESENT  (! this->m_slave_axis->forward_limit_switch_detected())
//-----------------------------------------------------------------------------
#define SLAVE_NO_BACK_LSW_PRESENT  (! this->m_slave_axis->backward_limit_switch_detected()) 
//-----------------------------------------------------------------------------
#define GEARED_AXES_NOT_MOVING            (MASTER_NOT_MOVING)
//-----------------------------------------------------------------------------
#define GEARED_AXES_NOT_MOVING_OR_INDEP   ( GEARED_AXES_NOT_MOVING || INDEP_MODE )


// ============================================================================
// CRITERIA IDENTIFIER (FOR WHY CMD REFUSED)
// ============================================================================
static enum
{
  kMASTER_NOT_OFF = 0,
  kSLAVE_NOT_OFF,
  kMASTER_NOT_FAULT,
  kSLAVE_NOT_FAULT,
  kMASTER_NOT_MOVING,
  kSLAVE_NOT_MOVING,
  kSLAVE_NO_FORW_LSW_PRESENT,
  kSLAVE_NO_BACK_LSW_PRESENT,
  kINDEP_MODE,
  kGEARED_AXES_NOT_MOVING,
  kGEARED_AXES_NOT_MOVING_OR_INDEP,
  kUNKNOWN_CRITERIA
} GearedAxesCmdAllowedCriteria;

// ============================================================================
// CRITERIA STRINGS (FOR WHY CMD REFUSED)
// ============================================================================
static const std::string kWHY[] =
{ 
  "master motor is off",
  "slave motor is off",
  "master motor is in fault",
  "slave motor is in fault",
  "master motor is moving",
  "slave motor is moving",
  "slave limit switch forward present",
  "slave limit switch backward present",
  "mode gearing",
  "slit is already moving",
  "slit is moving or mode gearing enabled"
  "the system could not deduce the reason why the command was refused"
};

// ============================================================================
// <CMD REQUEST VALIDATION> CRITERIA
// ============================================================================
#define MASTER_INDEP_MOVE_ALLOWED \
	      MASTER_NOT_OFF && \
	      MASTER_NOT_FAULT && \
				MASTER_NOT_MOVING && \
        GEARED_AXES_NOT_MOVING_OR_INDEP
//-----------------------------------------------------------------------------
static const size_t MASTER_INDEP_MOVE_CRITERIA_ARRAY[] =
{
  4,
  kMASTER_NOT_OFF,
  kMASTER_NOT_FAULT,
  kMASTER_NOT_MOVING,
  kGEARED_AXES_NOT_MOVING_OR_INDEP
};
//=============================================================================
#define SLAVE_INDEP_MOVE_ALLOWED \
	      SLAVE_NOT_OFF && \
	      SLAVE_NOT_FAULT && \
        GEARED_AXES_NOT_MOVING_OR_INDEP
//-----------------------------------------------------------------------------
static const size_t SLAVE_INDEP_MOVE_CRITERIA_ARRAY[] =
{
  4,
  kSLAVE_NOT_OFF,
  kSLAVE_NOT_FAULT,
  kSLAVE_NOT_MOVING,
  kGEARED_AXES_NOT_MOVING_OR_INDEP
};

//=============================================================================
#define ENABLE_GEARING_ALLOWED \
	      MASTER_NOT_MOVING && \
	      SLAVE_NOT_MOVING 
//-----------------------------------------------------------------------------
static const size_t ENABLE_GEARING_CRITERIA_ARRAY[] =
{
  2,
  kMASTER_NOT_MOVING,
  kSLAVE_NOT_MOVING,
};

//=============================================================================
#define DISABLE_GEARING_ALLOWED \
	      MASTER_NOT_MOVING && \
	      SLAVE_NOT_MOVING 
//-----------------------------------------------------------------------------
static const size_t DISABLE_GEARING_CRITERIA_ARRAY[] =
{
  2,
  kMASTER_NOT_MOVING,
  kSLAVE_NOT_MOVING,
};

//=============================================================================
#define GEARED_MOVE_ALLOWED \
	      MASTER_NOT_OFF && \
	      SLAVE_NOT_OFF && \
	      MASTER_NOT_FAULT && \
	      SLAVE_NOT_FAULT && \
	      MASTER_NOT_MOVING && \
	      SLAVE_NOT_MOVING && \
        SLAVE_NO_FORW_LSW_PRESENT && \
        SLAVE_NO_BACK_LSW_PRESENT 
//-----------------------------------------------------------------------------
static const size_t GEARED_MOVE_CRITERIA_ARRAY[] =
{
  9,
  kMASTER_NOT_OFF,
  kSLAVE_NOT_OFF,
  kMASTER_NOT_FAULT,
  kSLAVE_NOT_FAULT,
  kMASTER_NOT_MOVING,
  kSLAVE_NOT_MOVING,
  kSLAVE_NO_FORW_LSW_PRESENT,
  kSLAVE_NO_BACK_LSW_PRESENT,
  kGEARED_AXES_NOT_MOVING
};

//=============================================================================
// CMD NAMES
//=============================================================================
const char * geared_axes_command_name[] =
{
  "<master/slave geared move>",
  "<master independant move>",
  "<slave independant move>",
  "<set geared mode>",
  "<set independant mode>",
  "<Unknown GearedAxes Command>"
};

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
// GearedAxesStatus::GearedAxesStatus
// ============================================================================
GearedAxesStatus::GearedAxesStatus ()
{
  //- noop ctor
}

// ============================================================================
// GearedAxesStatus::operator=
// ============================================================================
void GearedAxesStatus::operator= (const GearedAxesStatus& src)
{
  if (&src == this)
    return;

  this->slave_status  = src.slave_status;
  this->master_status = src.master_status;
	this->slit_status   = src.slit_status;
}

// ============================================================================
// GearedAxes::Config::Config
// ============================================================================
GearedAxes::Config::Config ()
 : master (MIN_AXIS),
   slave (MIN_AXIS),
   user_ratio (1.0),
   host_device (0)
{
  //- noop ctor
}

// ============================================================================
// GearedAxes::Config::operator=
// ============================================================================
void GearedAxes::Config::operator= (const Config& src)
{
  this->master = src.master;
  this->slave = src.slave; 
  this->user_ratio = src.user_ratio;
  this->host_device = src.host_device;
}

// ============================================================================
// GearedAxes::GearedAxes
// ============================================================================
GearedAxes::GearedAxes ()
 : yat4tango::TangoLogAdapter (0),
   m_cfg (),
   m_master_axis(0),
   m_slave_axis(0)
{
	YAT_TRACE("galil::GearedAxes::GearedAxes");
}

// ============================================================================
// GearedAxes::~GearedAxes
// ============================================================================
GearedAxes::~GearedAxes (void)
{
  YAT_TRACE("galil::GearedAxes::~GearedAxes");

  //- release reference to the salve Axis
  SAFE_RELEASE(this->m_slave_axis);

  //- release reference to the master Axis
  SAFE_RELEASE(this->m_master_axis);
}

// ============================================================================
// GearedAxes::configure
// ============================================================================
void GearedAxes::configure (const Config& _cfg) throw (Tango::DevFailed)
{
	YAT_TRACE("galil::GearedAxes::configure");

  //- release reference to the slave Axis
  SAFE_RELEASE(this->m_slave_axis);

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
                    _CPTC("galil::GearedAxes::configure"));
  }

  //- get a ref. to slave axis
  try 
  {
    this->m_slave_axis = AXIS_FACTORY->get_axis(this->m_cfg.slave);
  }
  catch (Tango::DevFailed& df)
  {
    ERROR_STREAM << df << std::endl;
    yat::OSStream err_desc;
    err_desc << "slave axis error - axis " 
             << Axis::axis_id_to_string(this->m_cfg.slave)
             << " does not exist - should be previously instanciated";
    THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                    _CPTC(err_desc.str().c_str()),
                    _CPTC("galil::GearedAxes::configure"));
  }

  //- compute m/s ratio - avoid divide by 0!
  double ms_ratio = 0.;
  try
  {
    ms_ratio = this->m_master_axis->global_ratio() / this->m_slave_axis->global_ratio();

    ms_ratio *= this->m_cfg.user_ratio;

	std::cout << " ratio : " << ms_ratio << endl;
	std::cout << " this->m_cfg.user_ratio : " << this->m_cfg.user_ratio << endl;
	std::cout << " this->m_master_axis->global_ratio() : " << this->m_master_axis->global_ratio() << endl;
	std::cout << " this->m_slave_axis->global_ratio() : " << this->m_slave_axis->global_ratio() << endl;

    INFO_STREAM << "- master_ratio...." << this->m_master_axis->global_ratio()
                << "- slave ratio....." << this->m_slave_axis->global_ratio()
                << " - user ratio..." << this->m_cfg.user_ratio
                << "- gearing ratio..." << ms_ratio
                << std::endl;
  }
  catch (...)
  {
    yat::OSStream err_desc;
    err_desc << " GearedAxes configuration error - the slave axis motor ratio is 0"
             << " [ask your motorist to setup axis properly]";
    THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                    _CPTC(err_desc.str().c_str()),
                    _CPTC("galil::GearedAxes::configure"));
  }
  //- apply the "user" ratio

  //- set master Axis gearing configuration
  Axis::GearingConfig master_config;
  master_config.mode = Axis::MASTER_AXIS;
  master_config.associated_axis = this->m_cfg.slave;
  master_config.ms_ratio = ms_ratio;
  this->m_master_axis->configure_gearing(master_config);

  //- set slave Axis gearing configuration
  Axis::GearingConfig slave_config;
  slave_config.mode = Axis::SLAVE_AXIS;
  slave_config.associated_axis = this->m_cfg.master;
  slave_config.ms_ratio = ms_ratio;
  this->m_slave_axis->configure_gearing(slave_config);

  //- do not enable gearing at startup...
  //- reference position may need to be initialized on each axis !
}



// ============================================================================
// GearedAxes::master_position
// ============================================================================
void GearedAxes::master_position (double _mp) throw (Tango::DevFailed)
{
	YAT_TRACE("galil::GearedAxes::master_position");

  std::string why_not;

  if (!this->m_slave_axis->gearing_enabled())
  {
    if (! this->command_allowed(galil::GEARED_AXES_INDEP_MASTER_MOVE, & why_not))
    {
      THROW_DEVFAILED(_CPTC("COMMAND_NOT_ALLOWED"),
                      _CPTC(why_not.c_str()),
                      _CPTC("galil::GearedAxes::master_position"));
    }
  }
  else if (! this->command_allowed(galil::GEARED_AXES_GEARED_MOVE, & why_not))
  {
    THROW_DEVFAILED(_CPTC("COMMAND_NOT_ALLOWED"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::GearedAxes::master_position"));
  }


  //- set master axis position
  this->m_master_axis->abs_pos(_mp);
}

// ============================================================================
// GearedAxes::master_position
// ============================================================================
double GearedAxes::master_position ()
{
	YAT_TRACE("galil::GearedAxes::master_position");

  double current_pos;

  //- get current master pos
  try
  {
    current_pos = this->m_master_axis->abs_pos();
  }
  catch (Tango::DevFailed& df)
  {
    ERROR_STREAM << "GearedAxes::master_position::" << df << std::endl;
    RETHROW_DEVFAILED(df,
                      _CPTC("HARDWARE_ERROR"),
                      _CPTC("could not obtain the master axis position"),
                      _CPTC("galil::GearedAxes::master_position"));
  }

  return current_pos;
}

// ============================================================================
// GearedAxes::slave_position
// ============================================================================
void GearedAxes::slave_position (double _sp) throw (Tango::DevFailed)
{
	YAT_TRACE("galil::GearedAxes::slave_position");

  //- disable gearing on slave axis (enough to disable on both axis)
  if (this->m_slave_axis->gearing_enabled())
    this->m_slave_axis->disable_gearing();

  std::string why_not;
  if (! this->command_allowed(galil::GEARED_AXES_INDEP_SLAVE_MOVE, &why_not))
  {
    THROW_DEVFAILED(_CPTC("COMMAND_NOT_ALLOWED"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::GearedAxes::slave_position"));
  }

  //- set slave axis position
  this->m_slave_axis->abs_pos(_sp);
}

// ============================================================================
// GearedAxes::slave_position
// ============================================================================
double GearedAxes::slave_position ()
{
	YAT_TRACE("galil::GearedAxes::slave_position");

  double current_pos;

  //- get current slave pos
  try
  {
    current_pos = this->m_slave_axis->abs_pos();
  }
  catch (Tango::DevFailed& df)
  {
    ERROR_STREAM << "GearedAxes::slave_position::" << df << std::endl;
    RETHROW_DEVFAILED(df,
                      _CPTC("HARDWARE_ERROR"),
                      _CPTC("could not obtain the slave axis position"),
                      _CPTC("galil::GearedAxes::slave_position"));
  }

  return current_pos;
}

// ============================================================================
// GearedAxes::stop 
// ============================================================================
void GearedAxes::stop () throw (Tango::DevFailed)
{
	YAT_TRACE("galil::GearedAxes::stop");

  //- stop master axis
  this->m_master_axis->stop();

  //- stop slave if gearing disabled
  if (! this->m_slave_axis->gearing_enabled())
    this->m_slave_axis->stop();
}

// ============================================================================
// GearedAxes::disable_gearing 
// ============================================================================
void GearedAxes::disable_gearing () throw (Tango::DevFailed)
{
	YAT_TRACE("galil::GearedAxes::disable_gearing");

  std::string why_not;
  if (! this->command_allowed(galil::GEARED_AXES_DISABLE_GEARING, &why_not))
  {
    THROW_DEVFAILED(_CPTC("COMMAND_NOT_ALLOWED"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::GearedAxes::disable_gearing"));
  }
  //- disable gearing on slave axis (enough to disable on both axis)
  if (this->m_slave_axis->gearing_enabled())
    this->m_slave_axis->disable_gearing();
}

// ============================================================================
// GearedAxes::enable_gearing
// ============================================================================
void GearedAxes::enable_gearing () throw (Tango::DevFailed)
{
  YAT_TRACE("galil::GearedAxes::enable_gearing");

  std::string why_not;
  if (! this->command_allowed(galil::GEARED_AXES_ENABLE_GEARING, &why_not))
  {
    THROW_DEVFAILED(_CPTC("COMMAND_NOT_ALLOWED"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::GearedAxes::enable_gearing"));
  }
  //- is gearing enabled on slave axis (enough to conclude)
  if (! this->m_slave_axis->gearing_enabled())
    this->m_slave_axis->enable_gearing();

}

// ============================================================================
// GearedAxes::state 
// ============================================================================
AxisState GearedAxes::state ()
{
	YAT_TRACE("galil::GearedAxes::state");

  galil::AxisState master_state = this->m_master_axis->state();
  galil::AxisState slave_state  = this->m_slave_axis->state();

  //- at least one motor OFF ----------------
  if ( 
       (master_state == galil::AXIS_OFF) 
         ||
       (slave_state == galil::AXIS_OFF)
     )
     return galil::AXIS_OFF;

  //- at least one motor in FAULT state------
  if ( 
       (master_state == galil::AXIS_ERROR)
         ||
       (slave_state == galil::AXIS_ERROR)
     )
     return galil::AXIS_ERROR;

  //- at least one motor in UNKNOWN state ---
  if ( 
       (master_state == galil::AXIS_UNKNOWN) 
         ||
       (slave_state == galil::AXIS_UNKNOWN)
     )
     return galil::AXIS_UNKNOWN;

  //- if gearing mode -----------------------
  if (this->m_slave_axis->gearing_enabled())
  {
	//- slave limit switch present: turn to ALARM State --
  if (
       this->m_slave_axis->backward_limit_switch_detected()
         ||
       this->m_slave_axis->forward_limit_switch_detected()
     )
    return galil::AXIS_ALARM;

    switch (master_state)
    {
      case galil::AXIS_STANDBY:
      case galil::AXIS_MOVING:
      case galil::AXIS_ALARM:
      case galil::AXIS_CRTL_DISABLED:
        return master_state;
        break;
      default:
        break;
    }
  }
  //- independant mode ----------------------
  else
  {
    //- standby -----------
    if (
         (master_state == galil::AXIS_STANDBY) 
           &&
         (slave_state  == galil::AXIS_STANDBY) 
       )
       return master_state;
    //- moving  -----------
    if ( 
         (master_state == galil::AXIS_MOVING) 
           ||
         (slave_state  == galil::AXIS_MOVING) 
       )
       return galil::AXIS_MOVING;
    //- alarm -------------
    if ( 
         (master_state == galil::AXIS_ALARM) 
           ||
         (slave_state  == galil::AXIS_ALARM) 
       )
       return galil::AXIS_ALARM;
    //- control disabled --
    if ( 
         (master_state == galil::AXIS_CRTL_DISABLED) 
           ||
         (slave_state  == galil::AXIS_CRTL_DISABLED) 
       )
       return galil::AXIS_CRTL_DISABLED;
  }

  //- we should never pass here -------------
  return galil::AXIS_UNKNOWN;
}

// ============================================================================
// GearedAxes::status 
// ============================================================================
void GearedAxes::status (GearedAxesStatus & status_)  throw (Tango::DevFailed)
{
	YAT_TRACE("galil::GearedAxes::status");

  //- get slave status
  this->m_slave_axis->status(status_.slave_status);

  //- get master status
  this->m_master_axis->status(status_.master_status);

  if (
       this->m_slave_axis->backward_limit_switch_detected()
         ||
       this->m_slave_axis->forward_limit_switch_detected()
     )
  {
    //- help message in status give problem resolution tip
    status_.slit_status  = "Slave stopped on Limit switch.\n";
		status_.slit_status += "Gearing is disabled. To clear situation :\n";
		status_.slit_status += "Move Slave motor position to come out of limit switch\n";
		status_.slit_status += "Using the position attribute of the slave motor\n";
		status_.slit_status += "Or SetIndependantMode and act on the GalilAxis device on the slave motor ";
		status_.slit_status += "When Slave is out of its Limit switch GearedAxes can be operated normally";
  }
  else if (
           this->m_master_axis->backward_limit_switch_detected()
            ||
           this->m_master_axis->forward_limit_switch_detected()
          )
  {
    //- help message in status give problem resolution tip
    status_.slit_status =  "GearedAxes move stopped on Master Limit Switch\n";
		status_.slit_status += "Move position to come out of limit switch";
  }
  else
  {
    //- normal operation message
    status_.slit_status = "GearedAxes Normal operation";
  }
}

//---------------------------------------------------------
// GearedAxes::command_allowed
//---------------------------------------------------------
// Giving the current status of the slit, this 
// method checks whether or not the specified command is 
// allowed. In case the command is refused and the <why> is 
// not NULL, the reason is returned in the provided string.
//---------------------------------------------------------
bool GearedAxes::command_allowed (GearedAxesCommandIdentifier _cid, std::string * _why)
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::GearedAxes::command_allowed");

  bool cmd_allowed = false;

  //- is the specified command allowed?
  switch (_cid)
  {
    case GEARED_AXES_GEARED_MOVE:
      YAT_LOG("galil::GearedAxes::command_allowed::eval.criteria::GEARED_MOVE_ALLOWED"); 
      cmd_allowed = GEARED_MOVE_ALLOWED;
      break;
    case GEARED_AXES_ENABLE_GEARING:
      YAT_LOG("galil::GearedAxes::command_allowed::eval.criteria::ENABLE_GEARING_ALLOWED"); 
      cmd_allowed = ENABLE_GEARING_ALLOWED;
      break;
    case GEARED_AXES_DISABLE_GEARING:
      YAT_LOG("galil::GearedAxes::command_allowed::eval.criteria::DISABLE_GEARING_ALLOWED"); 
      cmd_allowed = DISABLE_GEARING_ALLOWED;
      break;
    case GEARED_AXES_INDEP_MASTER_MOVE:
      YAT_LOG("galil::GearedAxes::command_allowed::eval.criteria::INDEP_MASTER_MOVE"); 
      cmd_allowed = MASTER_INDEP_MOVE_ALLOWED;
      break;
    case GEARED_AXES_INDEP_SLAVE_MOVE:
      YAT_LOG("galil::GearedAxes::command_allowed::eval.criteria::SLAVE_INDEP_MOVE_ALLOWED"); 
      cmd_allowed = SLAVE_INDEP_MOVE_ALLOWED;
      break;
    default:
      break;
  }

  if (! cmd_allowed)
  {
    //- search <first> criteria which failed 
    int c = this->why_cmd_refused(_cid);

    //- verbose
    ERROR_STREAM << "GearedAxes::command_allowed::rejecting "
                 << geared_axes_command_name[_cid]
                 << " cmd request::"
                 << kWHY[c]
                 << std::endl;

    //- why is the command refused?
    if (_why)
    {
      //- set the reason for caller 
      yat::OSStream oms;
      oms << galil_command_name[_cid]
          << " cmd rejected::"
          << kWHY[c]
          << std::ends;
      *_why = oms.str(); 
    }
  }

  return cmd_allowed;
}


// ============================================================================
// GearedAxes::why_cmd_refused 
// ============================================================================
int GearedAxes::why_cmd_refused (GearedAxesCommandIdentifier _cid) const
{
  YAT_TRACE("galil::GearedAxes::why_cmd_refused");

  switch (_cid)
  {
    case GEARED_AXES_GEARED_MOVE:
      SEARCH_FAULTY_CRITERIA (GEARED_MOVE_CRITERIA_ARRAY);
      break;
    case GEARED_AXES_ENABLE_GEARING:
      SEARCH_FAULTY_CRITERIA (ENABLE_GEARING_CRITERIA_ARRAY);
      break;
    case GEARED_AXES_DISABLE_GEARING:
      SEARCH_FAULTY_CRITERIA (DISABLE_GEARING_CRITERIA_ARRAY);
      break;
    case GEARED_AXES_INDEP_MASTER_MOVE:
      SEARCH_FAULTY_CRITERIA (MASTER_INDEP_MOVE_CRITERIA_ARRAY);
      break;
    case GEARED_AXES_INDEP_SLAVE_MOVE:
      SEARCH_FAULTY_CRITERIA (SLAVE_INDEP_MOVE_CRITERIA_ARRAY);
      break;
    default:
      return NO_COMMAND;
      break;
  }

  return kUNKNOWN_CRITERIA;
}


//=============================================================================
// GearedAxes::eval_criteria
//=============================================================================
bool GearedAxes::eval_criteria (int cid) const
{
  YAT_TRACE("galil::GearedAxes::eval_criteria");

  bool criteria_eval = false;

  switch (cid)
  {
    case kMASTER_NOT_OFF:
      criteria_eval = MASTER_NOT_OFF;
      break;
    case kSLAVE_NOT_OFF:
      criteria_eval = SLAVE_NOT_OFF;
      break;
    case kMASTER_NOT_FAULT:
      criteria_eval = MASTER_NOT_FAULT;
      break;
    case kSLAVE_NOT_FAULT:
      criteria_eval = SLAVE_NOT_FAULT;
      break;
    case kMASTER_NOT_MOVING:
      criteria_eval = MASTER_NOT_MOVING;
      break;
    case kSLAVE_NOT_MOVING:
      criteria_eval = SLAVE_NOT_MOVING;
      break;
    case kSLAVE_NO_FORW_LSW_PRESENT:
      criteria_eval = SLAVE_NO_FORW_LSW_PRESENT;
      break;
    case kSLAVE_NO_BACK_LSW_PRESENT:
      criteria_eval = SLAVE_NO_BACK_LSW_PRESENT;
      break;
    case kGEARED_AXES_NOT_MOVING:
      criteria_eval = GEARED_AXES_NOT_MOVING;
      break;
  }

  return criteria_eval;
}

} // namespace galil


