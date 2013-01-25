//=============================================================================
// GearedSlit.cpp
//=============================================================================
// abstraction.......Galil GearedSlit "geared" implementation class
// class.............GearedSlit
// original author...N.Leclercq - J.Coquet - SOLEIL
//=============================================================================

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <tango.h>
#include "AxisFactory.h"
#include "GearedSlit.h"


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
  kSLIT_NOT_MOVING,
  kSLIT_NOT_MOVING_OR_INDEP,
  kUNKNOWN_CRITERIA
} AbstractSlitCmdAllowedCriteria;

// ============================================================================
// CRITERIA STRINGS (FOR WHY CMD REFUSED)
// ============================================================================
static const int kWHY_MAX_ELEM = 11;
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
  "slit is moving or mode gearing enabled",
  "the system could not deduce the reason why the command was refused"
};
// ============================================================================
// <CMD REQUEST VALIDATION> CRITERIA
// ============================================================================
#define GAP_MOVE_ALLOWED  \
	      MASTER_NOT_OFF && \
	      SLAVE_NOT_OFF && \
	      MASTER_NOT_FAULT && \
	      SLAVE_NOT_FAULT && \
	      MASTER_NOT_MOVING && \
	      SLAVE_NOT_MOVING && \
        SLAVE_NO_FORW_LSW_PRESENT && \
        SLAVE_NO_BACK_LSW_PRESENT 
//-----------------------------------------------------------------------------
static const size_t GAP_MOVE_CRITERIA_ARRAY[] =
{
  8,
  kMASTER_NOT_OFF,
  kSLAVE_NOT_OFF,
  kMASTER_NOT_FAULT,
  kSLAVE_NOT_FAULT,
  kMASTER_NOT_MOVING,
  kSLAVE_NOT_MOVING,
  kSLAVE_NO_FORW_LSW_PRESENT,
  kSLAVE_NO_BACK_LSW_PRESENT,
};
//=============================================================================
#define POSITION_MOVE_ALLOWED  GAP_MOVE_ALLOWED
//-----------------------------------------------------------------------------
#define POSITION_MOVE_CRITERIA_ARRAY GAP_MOVE_CRITERIA_ARRAY
//=============================================================================
#define MASTER_INDEP_MOVE_ALLOWED \
	      MASTER_NOT_OFF && \
	      MASTER_NOT_FAULT && \
				MASTER_NOT_MOVING && \
        SLIT_NOT_MOVING_OR_INDEP
//-----------------------------------------------------------------------------
static const size_t MASTER_INDEP_MOVE_CRITERIA_ARRAY[] =
{
  4,
  kMASTER_NOT_OFF,
  kMASTER_NOT_FAULT,
  kMASTER_NOT_MOVING,
  kSLIT_NOT_MOVING_OR_INDEP
};
//=============================================================================
//- in gearing mode the �code declares always the slave in state MOVING 
#define SLAVE_INDEP_MOVE_ALLOWED \
	      SLAVE_NOT_OFF && \
	      SLAVE_NOT_FAULT && \
        SLIT_NOT_MOVING_OR_INDEP
//-----------------------------------------------------------------------------
static const size_t SLAVE_INDEP_MOVE_CRITERIA_ARRAY[] =
{
  3,
  kSLAVE_NOT_OFF,
  kSLAVE_NOT_FAULT,
  kSLIT_NOT_MOVING_OR_INDEP
};

const char * slit_command_name[] =
{
  "<gap move>",
  "<position move>",
  "<master independant move>",
  "<slave independant move>",
  "<Unknown Slit Command>"
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

//---------------------------------------------------------
// GearedSlit::command_allowed
//---------------------------------------------------------
// Giving the current status of the GearedSlit, this 
// method checks whether or not the specified command is 
// allowed. In case the command is refused and the <why> is 
// not NULL, the reason is returned in the provided string.
//---------------------------------------------------------
bool GearedSlit::command_allowed (AbstractSlitCommandIdentifier _cid, std::string * _why)
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::GearedSlit::command_allowed");

  bool cmd_allowed = false;

  //- is the specified command allowed?
  switch (_cid)
  {
    case SLIT_GAP_MOVE:
      YAT_LOG("galil::GearedSlit::command_allowed::eval.criteria::GAP_MOVE_ALLOWED"); 
      cmd_allowed = GAP_MOVE_ALLOWED;
      break;
    case SLIT_POS_MOVE:
      YAT_LOG("galil::GearedSlit::command_allowed::eval.criteria::POSITION_MOVE_ALLOWED"); 
      cmd_allowed = POSITION_MOVE_ALLOWED;
      break;
    case SLIT_INDEP_MASTER_MOVE:
      YAT_LOG("galil::GearedSlit::command_allowed::eval.criteria::INDEP_MASTER_MOVE"); 
      cmd_allowed = MASTER_INDEP_MOVE_ALLOWED;
      break;
    case SLIT_INDEP_SLAVE_MOVE:
      YAT_LOG("galil::GearedSlit::command_allowed::eval.criteria::SLAVE_INDEP_MOVE_ALLOWED"); 
      cmd_allowed = SLAVE_INDEP_MOVE_ALLOWED;
      break;
    default:
      break;
  }



  if (! cmd_allowed)
  {
    //- search <first> criteria which failed 
    int c = this->why_cmd_refused(_cid);


    if (c > kWHY_MAX_ELEM) 
       c = kWHY_MAX_ELEM;


    //- verbose
  ERROR_STREAM << "GearedSlit::command_allowed::rejecting "
                 << slit_command_name[_cid]
                 << " cmd request::"
                 << kWHY[c]
                 << std::endl;

    //- why is the command refused?
    if (_why)
    {
     //- set the reason for caller 
      yat::OSStream oms;
      oms << slit_command_name[_cid]
          << " cmd rejected::"
          << kWHY[c]
          << std::ends;
      *_why = oms.str(); 
    }


  }

  return cmd_allowed;
}


// ============================================================================
// GearedSlit::why_cmd_refused 
// ============================================================================
int GearedSlit::why_cmd_refused (AbstractSlitCommandIdentifier _cid) const
{
  YAT_TRACE("galil::GearedSlit::why_cmd_refused");

  switch (_cid)
  {
    case SLIT_GAP_MOVE:
      SEARCH_FAULTY_CRITERIA (GAP_MOVE_CRITERIA_ARRAY);
      break;
    case SLIT_POS_MOVE:
      SEARCH_FAULTY_CRITERIA (POSITION_MOVE_CRITERIA_ARRAY);
      break;
    case SLIT_INDEP_MASTER_MOVE:
      SEARCH_FAULTY_CRITERIA (MASTER_INDEP_MOVE_CRITERIA_ARRAY);
      break;
    case SLIT_INDEP_SLAVE_MOVE:
      SEARCH_FAULTY_CRITERIA (SLAVE_INDEP_MOVE_CRITERIA_ARRAY);
      break;
    default:
      return NO_COMMAND;
      break;
  }

  return kUNKNOWN_CRITERIA;
}

//=============================================================================
// AbstractSlit::eval_criteria
//=============================================================================
bool GearedSlit::eval_criteria (int cid) const
{
  YAT_TRACE("galil::GearedSlit::eval_criteria");

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
    case kSLIT_NOT_MOVING:
      criteria_eval = SLIT_NOT_MOVING;
      break;
    case kSLIT_NOT_MOVING_OR_INDEP:
      criteria_eval = SLIT_NOT_MOVING_OR_INDEP;
      break;
    case kINDEP_MODE:
      criteria_eval = INDEP_MODE;
      break;
  }

  return criteria_eval;
}

// ============================================================================
// GearedSlit::GearedSlit
// ============================================================================
GearedSlit::GearedSlit ()
 : AbstractSlit()
{
	//- noop CTor
	YAT_TRACE("galil::GearedSlit::GearedSlit");
}

// ============================================================================
// GearedSlit::~GearedSlit
// ============================================================================
GearedSlit::~GearedSlit ()
{
	//- noop DTor
	YAT_TRACE("galil::GearedSlit::~GearedSlit");
}



// ============================================================================
// GearedSlit::gap
// overloaded pure virtual AbstractSlit gap mutator
// ============================================================================
void GearedSlit::gap (double _g) throw (Tango::DevFailed)
{
	YAT_TRACE("galil::GearedSlit::gap");

  //- get slave extended status
  const ExtendedAxisStatus & seas = this->m_slave_axis->extended_axis_status();

  
  std::string why_not;
  if (! this->command_allowed(galil::SLIT_GAP_MOVE, &why_not))
  {
    //- usefull debug
/*    FATAL_STREAM << " <<<<<<<<<<<<<<<<<<<<<<Gap Move Rejected : >>>>>>>>>>>>>>>>>>>>>>>>" << std::endl
                 << " valeur SLIT_GAP_MOVE :             " << SLIT_GAP_MOVE << endl 
                 << " valeur MASTER_NOT_OFF :            " << MASTER_NOT_OFF << endl 
                 << " valeur SLAVE_NOT_OFF :             " << SLAVE_NOT_OFF << endl 
                 << " valeur MASTER_NOT_FAULT :          " << MASTER_NOT_FAULT << endl 
                 << " valeur SLAVE_NOT_FAULT :           " << SLAVE_NOT_FAULT << endl 
                 << " valeur MASTER_NOT_MOVING :         " << MASTER_NOT_MOVING << endl 
                 << " valeur SLAVE_NOT_MOVING :          " << SLAVE_NOT_MOVING << endl 
                 << " valeur SLAVE_NO_FORW_LSW_PRESENT : " << SLAVE_NO_FORW_LSW_PRESENT << endl 
                 << " valeur SLAVE_NO_BACK_LSW_PRESENT : " << SLAVE_NO_BACK_LSW_PRESENT << endl 
                 << " valeur SLIT_NOT_MOVING :           " << SLIT_NOT_MOVING << endl ;
    std::string tmp = this->m_slave_axis->state() == galil::AXIS_MOVING? "Axis is MOVING" :"Axis is stopped";
    FATAL_STREAM << " " << std::endl
                 << " slave state :          " << this->m_slave_axis->state()  << "  " << tmp << endl
                 << " gearing enabled :      " << this->m_slave_axis->gearing_enabled() << endl;*/


    THROW_DEVFAILED(_CPTC("COMMAND_NOT_ALLOWED"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::GearedSlit::gap"));
  }

  //- get both master & slave position
  double slave_pos = AbstractSlit::slave_position();
  double master_pos = AbstractSlit::master_position();

  //- compute: delta_gap = _g - current_gap
  double delta_gap = _g - (slave_pos + master_pos);

  //- be sure the ms_ratio is positive
  this->m_slave_axis->sign_of_gearing_ratio(+1);

  //- is gearing enabled on slave axis (enough to conclude)
  if (! this->m_slave_axis->gearing_enabled())
    this->m_slave_axis->enable_gearing();

  DEBUG_STREAM << "- current master position........" << master_pos << std::endl;
  DEBUG_STREAM << "- current slave position........." << slave_pos << std::endl;
  DEBUG_STREAM << "- current gap...................." << slave_pos + master_pos << std::endl;
  DEBUG_STREAM << "- requested gap.................." << _g << std::endl;
  DEBUG_STREAM << "- setpoint for master position..." << master_pos + (delta_gap / 2.) << std::endl;

  //- set master axis postion (slave will follow)
  this->m_master_axis->abs_pos(master_pos + (delta_gap / 2.));
}


// ============================================================================
// GearedSlit::position
// overloaded pure virtual AbstractSlit position mutator
// ============================================================================
void GearedSlit::position (double _p) throw (Tango::DevFailed)
{
	YAT_TRACE("galil::GearedSlit::position");

  //- get slave extended status
  const ExtendedAxisStatus & seas = this->m_slave_axis->extended_axis_status();
  
  std::string why_not;
  if (! this->command_allowed(galil::SLIT_POS_MOVE, &why_not))
  {
    THROW_DEVFAILED(_CPTC("COMMAND_NOT_ALLOWED"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::GearedSlit::position"));
  }

  //- get both master & slave position
  double slave_pos = AbstractSlit::slave_position();
  double master_pos = AbstractSlit::master_position();

  //- compute position to reach
//  double new_p = -1. * (_p - ((slave_pos + master_pos) / 2.));
  double new_p = 0.;

	//- invert position if needed 
	if(this->m_cfg.invert_pos_dir)
  {
		new_p = (_p + ((slave_pos + master_pos) / 2.));
  }
  //- default direction for position
  else
  {
   new_p = -1. * (_p - ((slave_pos + master_pos) / 2.));
  }

  DEBUG_STREAM << "- current master position........" << master_pos << std::endl;
  DEBUG_STREAM << "- current slave position........." << slave_pos << std::endl;
  DEBUG_STREAM << "- current position..............." << (slave_pos - master_pos) / 2. << std::endl;
  DEBUG_STREAM << "- requested position............." << _p << std::endl;
  DEBUG_STREAM << "- setpoint for master position..." << new_p << std::endl;

  //- is gearing enabled on slave axis (enough to conclude)
  if (! this->m_slave_axis->gearing_enabled())
  {
    this->m_slave_axis->enable_gearing();
  }
  //- be sure the ms_ratio is negative
  this->m_slave_axis->sign_of_gearing_ratio(-1);

  //- set master axis postion (slave will follow)
  this->m_master_axis->abs_pos(new_p);
}


// ============================================================================
// GearedSlit::master_position
// overloaded pure virtual AbstractSlit master position mutator
// ============================================================================
void GearedSlit::master_position (double _mp) throw (Tango::DevFailed)
{
	YAT_TRACE("galil::GearedSlit::master_position");

  //- disable gearing on slave axis (enough to disable on both axis)
  if (this->m_slave_axis->gearing_enabled())
    this->m_slave_axis->disable_gearing();

  std::string why_not;
  if (! this->command_allowed(galil::SLIT_INDEP_MASTER_MOVE, & why_not))
  {
    THROW_DEVFAILED(_CPTC("COMMAND_NOT_ALLOWED"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::GearedSlit::master_position"));
  }

  //- set master axis position
  this->m_master_axis->abs_pos(_mp);
}


// ============================================================================
// GearedSlit::slave_position
// overloaded pure virtual AbstractSlit slave position mutator
// ============================================================================
void GearedSlit::slave_position (double _sp) throw (Tango::DevFailed)
{
	YAT_TRACE("galil::GearedSlit::slave_position");

  //- disable gearing on slave axis (enough to disable on both axis)
  if (this->m_slave_axis->gearing_enabled())
    this->m_slave_axis->disable_gearing();

  std::string why_not;
  if (! this->command_allowed(galil::SLIT_INDEP_SLAVE_MOVE, &why_not))
  {
    THROW_DEVFAILED(_CPTC("COMMAND_NOT_ALLOWED"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::GearedSlit::slave_position"));
  }

  //- set slave axis position
  this->m_slave_axis->abs_pos(_sp);
}


// ============================================================================
// GearedSlit::stop 
// overloaded pure virtual AbstractSlit gap mutator
// ============================================================================
void GearedSlit::stop () throw (Tango::DevFailed)
{
	YAT_TRACE("galil::GearedSlit::stop");

  //- stop master axis
  this->m_master_axis->stop();

  //- stop slave if gearing disabled
  if (! this->m_slave_axis->gearing_enabled())
    this->m_slave_axis->stop();
}

// ============================================================================
// GearedSlit::state 
// overloaded pure virtual AbstractSlit state accessor
// ============================================================================
AxisState GearedSlit::state () throw (Tango::DevFailed)
{
	YAT_TRACE("galil::GearedSlit::state");

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
// GearedSlit::status 
// overloaded pure virtual AbstractSlit status accessor
// ============================================================================
void GearedSlit::status (SlitStatus & status_)  throw (Tango::DevFailed)
{
	YAT_TRACE("galil::GearedSlit::status");

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
		status_.slit_status += "When Slave is out of its Limit switch Slit can be operated normally";
  }
  else if (
           this->m_master_axis->backward_limit_switch_detected()
            ||
           this->m_master_axis->forward_limit_switch_detected()
          )
  {
    //- help message in status give problem resolution tip
    status_.slit_status =  "Slit movement stopped on Master Limit Switch\n";
		status_.slit_status += "Move slit gap or slit position to come out of limit switch";
  }
  else
  {
    //- normal operation message
    status_.slit_status = "GearedSlit Normal operation";
  }
}




} // namespace galil


