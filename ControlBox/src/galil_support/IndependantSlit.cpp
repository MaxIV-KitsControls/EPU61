//=============================================================================
// IndependantSlit.cpp
//=============================================================================
// abstraction.......Galil IndependantSlit "independant" implementation class
// class.............IndependantSlit
// original author...N.Leclercq - J.Coquet - SOLEIL
//=============================================================================

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <tango.h>
#include "AxisFactory.h"
#include "IndependantSlit.h"


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
  kUNKNOWN_COMMAND,
  kUNKNOWN_CRITERIA
} AbstractSlitCmdAllowedCriteria;

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
  "slit is moving or mode gearing enabled",
  "the specified command does not exist",
  "the system could not deduce the reason why the command was refused"
};
// ============================================================================
// <CMD REQUEST VALIDATION> CRITERIA
// ============================================================================
#define GAP_MOVE_ALLOWED  \
	      INDEP_MODE &&\
	      MASTER_NOT_OFF && \
	      SLAVE_NOT_OFF && \
	      MASTER_NOT_FAULT && \
	      SLAVE_NOT_FAULT && \
	      MASTER_NOT_MOVING && \
	      SLAVE_NOT_MOVING
//-----------------------------------------------------------------------------
static const size_t GAP_MOVE_CRITERIA_ARRAY[] =
{
  7,
  kINDEP_MODE,
  kMASTER_NOT_OFF,
  kSLAVE_NOT_OFF,
  kMASTER_NOT_FAULT,
  kSLAVE_NOT_FAULT,
  kMASTER_NOT_MOVING,
  kSLAVE_NOT_MOVING
};
//=============================================================================
#define POSITION_MOVE_ALLOWED  GAP_MOVE_ALLOWED
//-----------------------------------------------------------------------------
#define POSITION_MOVE_CRITERIA_ARRAY GAP_MOVE_CRITERIA_ARRAY
//=============================================================================
#define MASTER_INDEP_MOVE_ALLOWED \
	      INDEP_MODE &&\
	      MASTER_NOT_OFF && \
	      MASTER_NOT_FAULT && \
				MASTER_NOT_MOVING
//-----------------------------------------------------------------------------
static const size_t MASTER_INDEP_MOVE_CRITERIA_ARRAY[] =
{
  4,
  kINDEP_MODE,
  kMASTER_NOT_OFF,
  kMASTER_NOT_FAULT,
  kMASTER_NOT_MOVING
};
//=============================================================================
//- in gearing mode the �code declares always the slave in state MOVING 
#define SLAVE_INDEP_MOVE_ALLOWED \
	      INDEP_MODE &&\
	      SLAVE_NOT_OFF && \
	      SLAVE_NOT_FAULT && \
        SLAVE_NOT_MOVING
//-----------------------------------------------------------------------------
static const size_t SLAVE_INDEP_MOVE_CRITERIA_ARRAY[] =
{
  4,
  kINDEP_MODE,
  kSLAVE_NOT_OFF,
  kSLAVE_NOT_FAULT,
  kSLAVE_NOT_MOVING
};

//=============================================================================
// SEARCH_FAULTY_CRITERIA
//=============================================================================
#define SEARCH_FAULTY_CRITERIA(CRITERIA_ARRAY) \
{ \
  for (size_t c = 1; c <= CRITERIA_ARRAY[0]; c++) \
  { \
    if (this->eval_criteria(CRITERIA_ARRAY[c]) == false) {\
      return CRITERIA_ARRAY[c]; \
      } \
  } \
}



namespace galil
{
//- defined in AbstractSlit.cpp pas bien beau. D'accord.
const char * slit_command_name[] =
{
  "<gap move>",
  "<position move>",
  "<master independant move>",
  "<slave independant move>",
  "<unknown slit command>"
};
//---------------------------------------------------------
// IndependantSlit::command_allowed
//---------------------------------------------------------
// Giving the current status of the IndependantSlit, this 
// method checks whether or not the specified command is 
// allowed. In case the command is refused and the <why> is 
// not NULL, the reason is returned in the provided string.
//---------------------------------------------------------
bool IndependantSlit::command_allowed (AbstractSlitCommandIdentifier _cid, std::string * _why)
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::IndependantSlit::command_allowed");

  bool cmd_allowed = false;
  

  //- is the specified command allowed?
  switch (_cid)
  {
    case SLIT_GAP_MOVE:
      YAT_LOG("galil::IndependantSlit::command_allowed::eval.criteria::GAP_MOVE_ALLOWED"); 
      cmd_allowed = GAP_MOVE_ALLOWED;
      break;
    case SLIT_POS_MOVE:
      YAT_LOG("galil::IndependantSlit::command_allowed::eval.criteria::POSITION_MOVE_ALLOWED"); 
      cmd_allowed = POSITION_MOVE_ALLOWED;
      break;
    case SLIT_INDEP_MASTER_MOVE:
      YAT_LOG("galil::IndependantSlit::command_allowed::eval.criteria::INDEP_MASTER_MOVE"); 
      cmd_allowed = MASTER_INDEP_MOVE_ALLOWED;
      break;
    case SLIT_INDEP_SLAVE_MOVE:
      YAT_LOG("galil::IndependantSlit::command_allowed::eval.criteria::SLAVE_INDEP_MOVE_ALLOWED"); 
      cmd_allowed = SLAVE_INDEP_MOVE_ALLOWED;
      break;
    default:
      _cid = SLIT_MAX_COMMAND;
      break;
  }

  if (! cmd_allowed)
  {
    //- search <first> criteria which failed 
    int c = this->why_cmd_refused(_cid);
    
    if (c < 0 || c > 12)
    {
      c = 12;
    }
    //- verbose
    ERROR_STREAM << "IndependantSlit::command_allowed::rejecting "
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
          << kWHY[c];
      *_why = oms.str(); 
    }
  }

  return cmd_allowed;
}


// ============================================================================
// IndependantSlit::why_cmd_refused 
// ============================================================================
int IndependantSlit::why_cmd_refused (AbstractSlitCommandIdentifier _cid) const
{
  YAT_TRACE("galil::IndependantSlit::why_cmd_refused");

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
      return kUNKNOWN_COMMAND;
      break;
  }

  return kUNKNOWN_CRITERIA;
}

//=============================================================================
// AbstractSlit::eval_criteria
//=============================================================================
bool IndependantSlit::eval_criteria (int cid) const
{
  YAT_TRACE("galil::IndependantSlit::eval_criteria");

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
// IndependantSlit::IndependantSlit
// ============================================================================
IndependantSlit::IndependantSlit ()
 : AbstractSlit()
{
	//- noop CTor
	YAT_TRACE("galil::IndependantSlit::IndependantSlit");
}

// ============================================================================
// IndependantSlit::~IndependantSlit
// ============================================================================
IndependantSlit::~IndependantSlit ()
{
	//- noop DTor
	YAT_TRACE("galil::IndependantSlit::~IndependantSlit");
}


// ============================================================================
// IndependantSlit::gap
// overloaded pure virtual AbstractSlit gap mutator
// ============================================================================
void IndependantSlit::gap (double _g) throw (Tango::DevFailed)
{
	YAT_TRACE("galil::IndependantSlit::gap");

	if(! INDEP_MODE)
		AbstractSlit::disable_gearing();

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
                 << " valeur SLIT_NOT_MOVING :           " << SLIT_NOT_MOVING << endl ;
    std::string tmp = this->m_slave_axis->state() == galil::AXIS_MOVING? "Axis is MOVING" :"Axis is stopped";
    FATAL_STREAM << " " << std::endl
                 << " slave state :          " << this->m_slave_axis->state()  << "  " << tmp << endl
                 << " gearing enabled :      " << this->m_slave_axis->gearing_enabled() << endl;*/


    THROW_DEVFAILED(_CPTC("COMMAND_NOT_ALLOWED"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::IndependantSlit::gap"));
  }

  //- get both master & slave position
  double slave_pos = AbstractSlit::slave_position();
  double master_pos = AbstractSlit::master_position();

  //- compute: delta_gap = _g - current_gap
  double delta_gap = _g - (slave_pos + master_pos);


  DEBUG_STREAM << "- current master position........" << master_pos << std::endl;
  DEBUG_STREAM << "- current slave position........." << slave_pos << std::endl;
  DEBUG_STREAM << "- current gap...................." << slave_pos + master_pos << std::endl;
  DEBUG_STREAM << "- requested gap.................." << _g << std::endl;
  DEBUG_STREAM << "- setpoint for master position..." << master_pos + (delta_gap / 2.) << std::endl;

  //- set master axis postion (slave will follow)
  this->m_master_axis->abs_pos(master_pos + (delta_gap / 2.));
  this->m_slave_axis->abs_pos(slave_pos + (delta_gap / 2.));
}


// ============================================================================
// IndependantSlit::position
// overloaded pure virtual AbstractSlit position mutator
// ============================================================================
void IndependantSlit::position (double _p) throw (Tango::DevFailed)
{
	YAT_TRACE("galil::IndependantSlit::position");

	if(! INDEP_MODE)
		AbstractSlit::disable_gearing();

  std::string why_not;
  if (! this->command_allowed(galil::SLIT_POS_MOVE, &why_not))
  {
    THROW_DEVFAILED(_CPTC("COMMAND_NOT_ALLOWED"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::IndependantSlit::position"));
  }

  //- get both master & slave position
  double master_pos = AbstractSlit::master_position();
  double slave_pos = AbstractSlit::slave_position();

	double new_master_pos, new_slave_pos;
	new_master_pos = new_slave_pos = 0. ;

	//- invert position if needed 
	if(this->m_cfg.invert_pos_dir)
  {
		new_master_pos = (((slave_pos + master_pos) / 2.) + _p  );
		new_slave_pos =  (((slave_pos + master_pos) / 2.) - _p );
  }
  //- default direction for position
  else
  {
		new_master_pos = (((slave_pos + master_pos) / 2.) - _p);
		new_slave_pos =  (((slave_pos + master_pos) / 2.) + _p);
  }

  DEBUG_STREAM << "- current master position........" << master_pos << std::endl;
  DEBUG_STREAM << "- current slave position........." << slave_pos << std::endl;
  DEBUG_STREAM << "- current position..............." << (slave_pos - master_pos) / 2. << std::endl;
  DEBUG_STREAM << "- requested position............." << _p << std::endl;
  DEBUG_STREAM << "- setpoint for master position..." << new_master_pos << std::endl;
  DEBUG_STREAM << "- setpoint for slave position...." << new_slave_pos << std::endl;

  
  //- set master axis postion (slave will follow)
  this->m_master_axis->abs_pos(new_master_pos);
  this->m_slave_axis->abs_pos(new_slave_pos);
}


// ============================================================================
// IndependantSlit::master_position
// overloaded pure virtual AbstractSlit master position mutator
// ============================================================================
void IndependantSlit::master_position (double _mp) throw (Tango::DevFailed)
{
	YAT_TRACE("galil::IndependantSlit::master_position");

	if(! INDEP_MODE)
		AbstractSlit::disable_gearing();

  std::string why_not;
  if (! this->command_allowed(galil::SLIT_INDEP_MASTER_MOVE, & why_not))
  {
    THROW_DEVFAILED(_CPTC("COMMAND_NOT_ALLOWED"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::IndependantSlit::master_position"));
  }

  //- set master axis position
  this->m_master_axis->abs_pos(_mp);
}


// ============================================================================
// IndependantSlit::slave_position
// overloaded pure virtual AbstractSlit slave position mutator
// ============================================================================
void IndependantSlit::slave_position (double _sp) throw (Tango::DevFailed)
{
	YAT_TRACE("galil::IndependantSlit::slave_position");

	if(! INDEP_MODE)
		AbstractSlit::disable_gearing();

  std::string why_not;
  if (! this->command_allowed(galil::SLIT_INDEP_SLAVE_MOVE, &why_not))
  {
    THROW_DEVFAILED(_CPTC("COMMAND_NOT_ALLOWED"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::IndependantSlit::slave_position"));
  }

  //- set slave axis position
  this->m_slave_axis->abs_pos(_sp);
}


// ============================================================================
// IndependantSlit::stop 
// overloaded pure virtual AbstractSlit gap mutator
// ============================================================================
void IndependantSlit::stop () throw (Tango::DevFailed)
{
	YAT_TRACE("galil::IndependantSlit::stop");

	if(! INDEP_MODE)
		AbstractSlit::disable_gearing();

  //- stop master and slave axis
  this->m_master_axis->stop();
  this->m_slave_axis->stop();
}

// ============================================================================
// IndependantSlit::state 
// overloaded pure virtual AbstractSlit state accessor
// ============================================================================
AxisState IndependantSlit::state () throw (Tango::DevFailed)
{
	YAT_TRACE("galil::IndependantSlit::state");

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
// IndependantSlit::status 
// overloaded pure virtual AbstractSlit status accessor
// ============================================================================
void IndependantSlit::status (SlitStatus & status_)  throw (Tango::DevFailed)
{
	YAT_TRACE("galil::IndependantSlit::status");

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
    status_.slit_status = "IndependantSlit Normal operation";
  }
}

} // namespace galil


