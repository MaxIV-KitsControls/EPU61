//=============================================================================
// GapPositionSlit.cpp
//=============================================================================
// abstraction.......Galil GapPositionSlit Gap + Position implementation class
// class.............GapPositionSlit
// original author...N.Leclercq - J.Coquet - SOLEIL
//=============================================================================

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <tango.h>
#include "AxisFactory.h"
#include "GapPositionSlit.h"

//- conventions : 
//- GAP      = MASTER motor
//- POSITION = SLAVE motor


// ============================================================================
// CRITERIA IDENTIFIER (FOR WHY CMD REFUSED)
// ============================================================================

static enum
{
  kGAP_NOT_OFF = 0,
  kPOSITION_NOT_OFF,
  kGAP_NOT_FAULT,
  kPOSITION_NOT_FAULT,
  kGAP_NOT_MOVING,
  kPOSITION_NOT_MOVING,
  kGAP_NO_FORW_LSW_PRESENT,
  kGAP_NO_BACK_LSW_PRESENT,
  kPOSITION_NO_FORW_LSW_PRESENT,
  kPOSITION_NO_BACK_LSW_PRESENT,
  kINDEP_MODE,
  kUNKNOWN_CRITERIA
} GapPositionSlitCmdAllowedCriteria;

// ============================================================================
// CRITERIA STRINGS (FOR WHY CMD REFUSED)
// ============================================================================
static const std::string kWHY[] =
{ 
  "gap motor is off",
  "position motor is off",
  "gap motor is in fault",
  "position motor is in fault",
  "gap motor is moving",
  "position motor is moving",
  "gap limit switch forward present",
  "gap limit switch backward present",
  "position limit switch forward present",
  "position limit switch backward present",
  "mode gearing",
  "the system could not deduce the reason why the command was refused"
};
// ============================================================================
// <CMD REQUEST VALIDATION> CRITERIA
// ============================================================================
#define GAP_MOVE_ALLOWED  \
	      INDEP_MODE &&\
	      GAP_NOT_OFF && \
	      GAP_NOT_FAULT && \
	      GAP_NOT_MOVING 
//-----------------------------------------------------------------------------
static const size_t GAP_MOVE_CRITERIA_ARRAY[] =
{
  4,
  kINDEP_MODE,
  kGAP_NOT_OFF,
  kGAP_NOT_FAULT,
  kGAP_NOT_MOVING
};
//=============================================================================
#define POSITION_MOVE_ALLOWED  \
	      INDEP_MODE &&\
	      POSITION_NOT_OFF && \
	      POSITION_NOT_FAULT && \
	      POSITION_NOT_MOVING 
//-----------------------------------------------------------------------------
static const size_t POSITION_MOVE_CRITERIA_ARRAY[] =
{
  4,
  kINDEP_MODE,
  kPOSITION_NOT_OFF,
  kPOSITION_NOT_FAULT,
  kPOSITION_NOT_MOVING
};
//=============================================================================
//- INSIDE/UP BLADE
#define MASTER_INDEP_MOVE_ALLOWED \
	      INDEP_MODE &&\
	      GAP_NOT_OFF && \
	      GAP_NOT_FAULT && \
				GAP_NOT_MOVING && \
	      POSITION_NOT_OFF && \
	      POSITION_NOT_FAULT && \
				POSITION_NOT_MOVING
//-----------------------------------------------------------------------------
static const size_t MASTER_INDEP_MOVE_CRITERIA_ARRAY[] =
{
  7,
  kINDEP_MODE,
  kGAP_NOT_OFF,
  kGAP_NOT_FAULT,
  kGAP_NOT_MOVING,
  kPOSITION_NOT_OFF,
  kPOSITION_NOT_FAULT,
  kPOSITION_NOT_MOVING
};
//=============================================================================
#define SLAVE_INDEP_MOVE_ALLOWED MASTER_INDEP_MOVE_ALLOWED
//-----------------------------------------------------------------------------
#define SLAVE_INDEP_MOVE_CRITERIA_ARRAY MASTER_INDEP_MOVE_CRITERIA_ARRAY
//=============================================================================
// SEARCH_FAULTY_CRITERIA
//=============================================================================
#define SEARCH_FAULTY_CRITERIA(CRITERIA_ARRAY) \
{ \
  for (size_t c = 1; c < CRITERIA_ARRAY[0]; c++) \
  { \
    if (this->eval_criteria(CRITERIA_ARRAY[c]) == false) \
      return CRITERIA_ARRAY[c]; \
  } \
}



namespace galil
{
//- defined in AbstractSlit.cpp pas bien beau. D'accord.
const char * gap_pos_slit_command_name[] =
{
  "<gap move>",
  "<position move>",
  "<inside/up blade move>",
  "<outside/down blade move>",
  "<Unknown Slit Command>"
};
//---------------------------------------------------------
// GapPositionSlit::command_allowed
//---------------------------------------------------------
// Giving the current status of the GapPositionSlit, this 
// method checks whether or not the specified command is 
// allowed. In case the command is refused and the <why> is 
// not NULL, the reason is returned in the provided string.
//---------------------------------------------------------
bool GapPositionSlit::command_allowed (AbstractSlitCommandIdentifier _cid, std::string * _why)
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::GapPositionSlit::command_allowed");

  bool cmd_allowed = false;

  //- is the specified command allowed?
  switch (_cid)
  {
    case SLIT_GAP_MOVE:
      YAT_LOG("galil::GapPositionSlit::command_allowed::eval.criteria::GAP_MOVE_ALLOWED"); 
      cmd_allowed = GAP_MOVE_ALLOWED;
      break;
    case SLIT_POS_MOVE:
      YAT_LOG("galil::GapPositionSlit::command_allowed::eval.criteria::POSITION_MOVE_ALLOWED"); 
      cmd_allowed = POSITION_MOVE_ALLOWED;
      break;
    case SLIT_INDEP_MASTER_MOVE:
      YAT_LOG("galil::GapPositionSlit::command_allowed::eval.criteria::INDEP_MASTER_MOVE"); 
      cmd_allowed = MASTER_INDEP_MOVE_ALLOWED;
      break;
    case SLIT_INDEP_SLAVE_MOVE:
      YAT_LOG("galil::GapPositionSlit::command_allowed::eval.criteria::SLAVE_INDEP_MOVE_ALLOWED"); 
      cmd_allowed = SLAVE_INDEP_MOVE_ALLOWED;
      break;
    default:
      break;
  }

  if (! cmd_allowed)
  {
    //- search <first> criteria which failed 
    int c = this->why_cmd_refused(_cid);
    std::cout << "(! cmd_allowed) criteria index " << c << endl; 
    //- verbose
    ERROR_STREAM << "GapPositionSlit::command_allowed::rejecting "
                 << gap_pos_slit_command_name[_cid]
                 << " cmd request::"
                 << kWHY[c]
                 << std::endl;

    //- why is the command refused?
    if (_why)
    {
      //- set the reason for caller 
      yat::OSStream oms;
      oms << gap_pos_slit_command_name[_cid]
          << " cmd rejected::"
          << kWHY[c]
          << std::ends;
      *_why = oms.str(); 
    }
  }

  return cmd_allowed;
}


// ============================================================================
// GapPositionSlit::why_cmd_refused 
// ============================================================================
int GapPositionSlit::why_cmd_refused (AbstractSlitCommandIdentifier _cid) const
{
  YAT_TRACE("galil::GapPositionSlit::why_cmd_refused");

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
bool GapPositionSlit::eval_criteria (int cid) const
{
  YAT_TRACE("galil::GapPositionSlit::eval_criteria");

  bool criteria_eval = false;

  switch (cid)
  {
    case kGAP_NOT_OFF:
      criteria_eval = GAP_NOT_OFF;
      break;
    case kPOSITION_NOT_OFF:
      criteria_eval = POSITION_NOT_OFF;
      break;
    case kGAP_NOT_FAULT:
      criteria_eval = GAP_NOT_FAULT;
      break;
    case kPOSITION_NOT_FAULT:
      criteria_eval = POSITION_NOT_FAULT;
      break;
    case kGAP_NOT_MOVING:
      criteria_eval = GAP_NOT_MOVING;
      break;
    case kPOSITION_NOT_MOVING:
      criteria_eval = POSITION_NOT_MOVING;
      break;
    case kGAP_NO_FORW_LSW_PRESENT:
      criteria_eval = GAP_NO_FORW_LSW_PRESENT;
      break;
    case kGAP_NO_BACK_LSW_PRESENT:
      criteria_eval = GAP_NO_BACK_LSW_PRESENT;
      break;
    case kPOSITION_NO_FORW_LSW_PRESENT:
      criteria_eval = POSITION_NO_FORW_LSW_PRESENT;
      break;
    case kPOSITION_NO_BACK_LSW_PRESENT:
      criteria_eval = POSITION_NO_BACK_LSW_PRESENT;
      break;
    case kINDEP_MODE:
      criteria_eval = INDEP_MODE;
      break;
    default :
      break;

  }

  return criteria_eval;
}

// ============================================================================
// GapPositionSlit::GapPositionSlit
// ============================================================================
GapPositionSlit::GapPositionSlit ()
 : AbstractSlit()
{
	YAT_TRACE("galil::GapPositionSlit::GapPositionSlit");
}

// ============================================================================
// GapPositionSlit::~GapPositionSlit
// ============================================================================
GapPositionSlit::~GapPositionSlit ()
{
	//- noop DTor
	YAT_TRACE("galil::GapPositionSlit::~GapPositionSlit");
}


// ============================================================================
// GapPositionSlit::gap
// overloaded pure virtual AbstractSlit gap mutator
// ============================================================================
void GapPositionSlit::gap (double _g) throw (Tango::DevFailed)
{
	YAT_TRACE("galil::GapPositionSlit::gap write");

	if(! INDEP_MODE)
		AbstractSlit::disable_gearing();

  std::string why_not;
  if (! this->command_allowed(galil::SLIT_GAP_MOVE, &why_not))
  {
    //- usefull debug

    THROW_DEVFAILED(_CPTC("COMMAND_NOT_ALLOWED"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::GapPositionSlit::gap"));
  }

  //- set master (GAP Axis )postion
  this->m_master_axis->abs_pos(_g);
}
// ============================================================================
// GapPositionSlit::gap
// overload  AbstractSlit gap accessor
// ============================================================================
double GapPositionSlit::gap (void)
{
	YAT_TRACE("galil::GapPositionSlit::gap read");
  return   this->m_master_axis->abs_pos();
}


// ============================================================================
// GapPositionSlit::position
// overloaded pure virtual AbstractSlit position mutator
// ============================================================================
void GapPositionSlit::position (double _p) throw (Tango::DevFailed)
{
	YAT_TRACE("galil::GapPositionSlit::position");

  std::string why_not;
  if (! this->command_allowed(galil::SLIT_POS_MOVE, &why_not))
  {
    THROW_DEVFAILED(_CPTC("COMMAND_NOT_ALLOWED"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::GapPositionSlit::position"));
  }
	//- invert position if needed 
	if(this->m_cfg.invert_pos_dir)
  {
		_p*=-1.;
  }
  this->m_slave_axis->abs_pos(_p);
}

// ============================================================================
// GapPositionSlit::position
// overload  AbstractSlit position accessor
// ============================================================================
double GapPositionSlit::position (void)
{
	YAT_TRACE("galil::GapPositionSlit::position read");
  return   this->m_slave_axis->abs_pos();
}


// ============================================================================
// GapPositionSlit::master_position
// overloaded pure virtual AbstractSlit master position mutator
// ============================================================================
void GapPositionSlit::master_position (double _mp) throw (Tango::DevFailed)
{
	YAT_TRACE("galil::GapPositionSlit::master_position write");

  std::string why_not;
  if (! this->command_allowed(galil::SLIT_INDEP_MASTER_MOVE, & why_not))
  {
    THROW_DEVFAILED(_CPTC("COMMAND_NOT_ALLOWED"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::GapPositionSlit::master_position write"));
  }

  //- retreive current "slave" position (computed through Gap and Position positions)
  double slave = this->slave_position();
  //- compute gap and position axis positions
  double new_gap = _mp + slave;
  double new_pos = (slave - _mp) / 2.;

  //- set gap and position axis positions
  this->gap (new_gap);
  this->position (new_pos);
}

// ============================================================================
// GapPositionSlit::master_position
// overloaded master position accessor
// ============================================================================
double GapPositionSlit::master_position (void)
{
	YAT_TRACE("galil::GapPositionSlit::master_position read");

  //- compute gap and position axis positions
  double gap = this->m_master_axis->abs_pos();
  double pos = this->m_slave_axis->abs_pos();
  //- set gap and position axis positions
  return (- pos + (gap/2.));
}


// ============================================================================
// GapPositionSlit::slave_position
// overloaded pure virtual AbstractSlit slave position mutator
// ============================================================================
void GapPositionSlit::slave_position (double _sp) throw (Tango::DevFailed)
{
	YAT_TRACE("galil::GapPositionSlit::slave_position");

	if(! INDEP_MODE)
		AbstractSlit::disable_gearing();

  std::string why_not;
  if (! this->command_allowed(galil::SLIT_INDEP_SLAVE_MOVE, &why_not))
  {
    THROW_DEVFAILED(_CPTC("COMMAND_NOT_ALLOWED"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::GapPositionSlit::slave_position"));
  }

  //- retreive "master" (inside/up) blade position
  double mast = this->master_position();
  //- compute gap and position axis positions
  double new_gap = _sp + mast;
  double new_pos = (_sp - mast) / 2.;

  //- set gap and position axis positions
  this->gap (new_gap);
  this->position (new_pos);
}

// ============================================================================
// GapPositionSlit::slave_position
// overloaded slave position accessor
// ============================================================================
double GapPositionSlit::slave_position (void)
{
	YAT_TRACE("galil::GapPositionSlit::slave_position read");

  //- compute gap and position axis positions
  double gap = this->m_master_axis->abs_pos();
  double pos = this->m_slave_axis->abs_pos();
  //- set gap and position axis positions
  return (pos + (gap/2.));
}

// ============================================================================
// GapPositionSlit::stop 
// overloaded pure virtual AbstractSlit gap mutator
// ============================================================================
void GapPositionSlit::stop () throw (Tango::DevFailed)
{
	YAT_TRACE("galil::GapPositionSlit::stop");

	if(! INDEP_MODE)
		AbstractSlit::disable_gearing();

  //- stop master and slave axis
  this->m_master_axis->stop();
  this->m_slave_axis->stop();
}

// ============================================================================
// GapPositionSlit::state 
// overloaded pure virtual AbstractSlit state accessor
// ============================================================================
AxisState GapPositionSlit::state () throw (Tango::DevFailed)
{
	YAT_TRACE("galil::GapPositionSlit::state");

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

  //- if gearing mode or at least 1 motor in alarm --
  if (this->m_slave_axis->gearing_enabled()
		     ||
		   (master_state == galil::AXIS_ALARM) 
         ||
       (slave_state == galil::AXIS_ALARM)
     )
    return galil::AXIS_ALARM;
  //- if at least 1 motor control disabled ----------
  if ( 
       (master_state == galil::AXIS_CRTL_DISABLED) 
         ||
       (slave_state == galil::AXIS_CRTL_DISABLED)
     )
     return galil::AXIS_CRTL_DISABLED;

  //- if master or slave moving -------------
  if ( 
       (master_state == galil::AXIS_MOVING) 
         ||
       (slave_state == galil::AXIS_MOVING)
     )
     return galil::AXIS_MOVING;

  //- if standby ----------------------------
  if ( 
       (master_state == galil::AXIS_STANDBY) 
         ||
       (slave_state == galil::AXIS_STANDBY)
     )
     return galil::AXIS_STANDBY;

  //- we should never pass here -------------
  return galil::AXIS_UNKNOWN;
}

// ============================================================================
// GapPositionSlit::status 
// overloaded pure virtual AbstractSlit status accessor
// ============================================================================
void GapPositionSlit::status (SlitStatus & status_)  throw (Tango::DevFailed)
{
	YAT_TRACE("galil::GapPositionSlit::status");

  //- get slave status
  this->m_slave_axis->status(status_.slave_status);

  //- get master status
  this->m_master_axis->status(status_.master_status);

  if (
       this->m_slave_axis->backward_limit_switch_detected()
         ||
       this->m_slave_axis->forward_limit_switch_detected()
			   ||
       this->m_master_axis->backward_limit_switch_detected()
         ||
       this->m_master_axis->forward_limit_switch_detected()
     )
  {
    //- help message in status give problem resolution tip
    status_.slit_status =  "Limit switch detected \n";
		status_.slit_status += "Either gap or position migth be incorrect";
  }
  else
  {
    //- normal operation message
    status_.slit_status = "GapPositionSlit Normal operation";
  }
}

} // namespace galil


