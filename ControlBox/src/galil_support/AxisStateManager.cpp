//=============================================================================
// AxisStateManagerStateManager.cpp
//=============================================================================
// abstraction.......Galil axis abstraction class
// class.............AxisStateManager
// original author...N.Leclercq - SOLEIL
//=============================================================================

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include "AxisStateManager.h"

// ============================================================================
// UPDATE TMO
// ============================================================================
#define kUPDATE_STATUS_TMO 5000
//-----------------------------------------------------------------------------

// ============================================================================
// AXIS STATUS STRINGS
// ============================================================================
#define kSTATUS_SEP                    std::string("\n")
//-----------------------------------------------------------------------------
#define kUNKNOWN_STATUS                "unknown status"
//-----------------------------------------------------------------------------
#define kTANGO_EX_STATUS               "TANGO exception caught"
//-----------------------------------------------------------------------------
#define kMOVING_STATUS                 "axis is moving"
//-----------------------------------------------------------------------------
#define kSTANDBY_NOT_LOCKED_STATUS     "axis up and ready [standby]"
//-----------------------------------------------------------------------------
#define kSTANDBY_LOCKED_STATUS         "axis position locked [standby]"
//-----------------------------------------------------------------------------
#define kAXIS_OFF_STATUS               "axis off [motor is off]"
//-----------------------------------------------------------------------------
#define kAXIS_CRTL_DISABLED_STATUS     "axis ctrl is disabled"
//-----------------------------------------------------------------------------
#define kREMOTE_CRTL_ACTIVE_STATUS     "manual remote controller is active"
//-----------------------------------------------------------------------------
#define kMOTOR_CRTL_NOT_ALLOWED_STATUS "motor ctrl not allowed for this axis"
//-----------------------------------------------------------------------------
#define kAXIS_BACKWARD_LSW_STATUS      "backward limit switch detected"
//-----------------------------------------------------------------------------
#define kAXIS_FORWARD_LSW_STATUS       "forward limit switch detected"
//-----------------------------------------------------------------------------
#define kAXIS_POS_ERROR_STATUS         "axis positionning failed"
//-----------------------------------------------------------------------------
#define kAXIS_INIT_POS_ERROR_STATUS    "axis not initialized [no initial ref. pos.]"
//-----------------------------------------------------------------------------
#define kUCODE_NOT_RUNNING_STATUS      "uCode is NOT running on the motion controller"
//-----------------------------------------------------------------------------
#define kAXIS_GEARING_MASTER_STATUS    "axis is gearing master"
//-----------------------------------------------------------------------------
#define kAXIS_GEARING_SLAVE_STATUS     "axis is gearing slave"
//-----------------------------------------------------------------------------
#define kNOT_INITIALIZED_STATUS        "Hardware Reset Detected - [exec. Init command] "
//-----------------------------------------------------------------------------
#define kAXIS_POS_LOCKED_STATUS        "axis position is locked"


// ============================================================================
// SHORTCUTS TO EACH <CMD REQUEST VALIDATION> CRITERIA COMPONENTS
// ============================================================================
#define UCODE_RUNNING                 (m_eas.ucs.running())
//-----------------------------------------------------------------------------
#define UCODE_NO_REF_POS_IN_PROGRESS  (! m_eas.ucs.processing_ref_pos())
//-----------------------------------------------------------------------------
#define AXIS_NOT_MOVING               (! m_eas.fas.moving() && m_eas.ucas.pos_done())
//-----------------------------------------------------------------------------
#define AXIS_MOTOR_ON                 (m_acfg.duty_cycle_enabled || ! m_eas.fas.motor_off())
//-----------------------------------------------------------------------------
#define AXIS_NO_CMD_IN_PROGRESS       (! m_eas.ucacs.any_command_in_progress())
//-----------------------------------------------------------------------------
#define AXIS_POWER_CTRL_ALLOWED       (m_eas.ucas.pwr_ctrl_allowed())
//-----------------------------------------------------------------------------
#define AXIS_MOTOR_CTRL_ALLOWED       (m_eas.ucas.motor_ctrl_allowed())
//-----------------------------------------------------------------------------
#define AXIS_NO_REF_POS_IN_PROGRESS   (! m_eas.ucas.ref_pos_in_progress())
//-----------------------------------------------------------------------------
#define AXIS_NO_FOLLOW_ERROR          (! m_eas.ucas.follow_error())
//-----------------------------------------------------------------------------
#define AXIS_NOT_GEARING_MASTER       (! m_eas.ucas.axis_master())
//-----------------------------------------------------------------------------
#define AXIS_NOT_GEARING_SLAVE        (! m_eas.ucas.axis_slave())
//-----------------------------------------------------------------------------
#define AXIS_POS_NOT_LOCKED           (! m_eas.ucas.pos_locked())
//-----------------------------------------------------------------------------
#define AXIS_NO_FORWARD_LSW           (m_acfg.has_inverted_lsw ? m_eas.fas.no_backward_lsw() : m_eas.fas.no_forward_lsw())
//-----------------------------------------------------------------------------
#define AXIS_NO_BACKWARD_LSW          (m_acfg.has_inverted_lsw ? m_eas.fas.no_forward_lsw() : m_eas.fas.no_backward_lsw())
//-----------------------------------------------------------------------------
#define AXIS_NO_ACTIVE_LSW            (AXIS_NO_FORWARD_LSW && AXIS_NO_BACKWARD_LSW)


// ============================================================================
// CRITERIA STRINGS (FOR WHY CMD REFUSED)
// ============================================================================
static const std::string kWHY[] =
{ 
  //- CmdAllowedCriteria::kUCODE_RUNNING
  "micro-code is not running",
  //- CmdAllowedCriteria::kUCODE_NO_REF_POS_IN_PROGRESS
  "a reference positioning is in progress",
  //- CmdAllowedCriteria::kAXIS_NOT_MOVING
  "axis is moving",
  //- CmdAllowedCriteria::kAXIS_MOTOR_ON
  "axis has been shutdown [motor off]",
  //- CmdAllowedCriteria::kAXIS_POWER_CTRL_ALLOWED
  "power control requests are not allowed for this axis",
  //- CmdAllowedCriteria::kAXIS_MOTOR_CTRL_ALLOWED
  "moving requests are not allowed for this axis",
  //- CmdAllowedCriteria::kAXIS_NO_REF_POS_IN_PROGRESS
  "a reference positioning is already in progress for this axis",
  //- CmdAllowedCriteria::kAXIS_NO_FOLLOW_ERROR
  "follow error detected",
  //- CmdAllowedCriteria::kAXIS_NOT_GEARING_MASTER
  "axis is gearing master",
  //- CmdAllowedCriteria::kAXIS_NOT_GEARING_SLAVE
  "axis is gearing slave",
  //- CmdAllowedCriteria::kAXIS_POS_NOT_LOCKED
  "axis position is locked",
  //- CmdAllowedCriteria::kAXIS_NO_ACTIVE_LSW
  "limit switch detected",
  //- CmdAllowedCriteria::kAXIS_NO_CMD_IN_PROGRESS
  "a command is already in progress",
  //- CmdAllowedCriteria::kUNKNOWN_COMMAND
  "the specified command does not exist",
  //- CmdAllowedCriteria::kUNKNOWN_CRITERIA
  "the system could not deduce the reason why the command was refused"
};

// ============================================================================
// <CMD REQUEST VALIDATION> CRITERIA
// ============================================================================
#define GEARING_CMD_ALLOWED \
  UCODE_RUNNING && \
  AXIS_NOT_MOVING && \
  AXIS_NO_REF_POS_IN_PROGRESS && \
  AXIS_POS_NOT_LOCKED && \
  AXIS_NO_CMD_IN_PROGRESS
//-----------------------------------------------------------------------------
static const size_t GEARING_CMD_CRITERIA_ARRAY[] =
{
  5,
  galil::kUCODE_RUNNING,
  galil::kAXIS_NOT_MOVING,
  galil::kAXIS_NO_REF_POS_IN_PROGRESS,
  galil::kAXIS_POS_NOT_LOCKED,
  galil::kAXIS_NO_CMD_IN_PROGRESS
};
//=============================================================================
#define MOTOR_OFF_CMD_ALLOWED \
  UCODE_RUNNING && \
  AXIS_POWER_CTRL_ALLOWED && \
  AXIS_NO_REF_POS_IN_PROGRESS && \
  AXIS_NOT_MOVING && \
  AXIS_POS_NOT_LOCKED && \
  AXIS_NO_CMD_IN_PROGRESS
//-----------------------------------------------------------------------------
static const size_t MOTOR_OFF_CMD_CRITERIA_ARRAY[] =
{
  6,
  galil::kUCODE_RUNNING,
  galil::kAXIS_POWER_CTRL_ALLOWED,
  galil::kAXIS_NO_REF_POS_IN_PROGRESS,
  galil::kAXIS_NOT_MOVING,
  galil::kAXIS_POS_NOT_LOCKED,
  galil::kAXIS_NO_CMD_IN_PROGRESS
};
//=============================================================================
#define MOTOR_ON_CMD_ALLOWED \
  UCODE_RUNNING && \
  AXIS_POWER_CTRL_ALLOWED && \
  AXIS_NO_CMD_IN_PROGRESS 
//-----------------------------------------------------------------------------
static const size_t MOTOR_ON_CMD_CRITERIA_ARRAY[] =
{
  3,
  galil::kUCODE_RUNNING,
  galil::kAXIS_POWER_CTRL_ALLOWED,
  galil::kAXIS_NO_CMD_IN_PROGRESS
};
//=============================================================================
#define REF_POS_CMD_ALLOWED \
  UCODE_RUNNING && \
  AXIS_NOT_MOVING && \
  AXIS_MOTOR_ON && \
  AXIS_MOTOR_CTRL_ALLOWED && \
  UCODE_NO_REF_POS_IN_PROGRESS && \
  AXIS_NOT_GEARING_MASTER && \
  AXIS_POS_NOT_LOCKED && \
  AXIS_NO_CMD_IN_PROGRESS
//-----------------------------------------------------------------------------
static const size_t REF_POS_CMD_CRITERIA_ARRAY[] =
{
  8,
  galil::kUCODE_RUNNING,
  galil::kAXIS_NOT_MOVING,
  galil::kAXIS_MOTOR_ON,
  galil::kAXIS_MOTOR_CTRL_ALLOWED,
  galil::kUCODE_NO_REF_POS_IN_PROGRESS,
  galil::kAXIS_NOT_GEARING_MASTER,
  galil::kAXIS_POS_NOT_LOCKED,
  galil::kAXIS_NO_CMD_IN_PROGRESS
};
//=============================================================================
#define ABS_POS_CMD_ALLOWED \
  UCODE_RUNNING && \
  AXIS_NOT_MOVING && \
  AXIS_MOTOR_ON && \
  AXIS_MOTOR_CTRL_ALLOWED && \
  AXIS_NO_REF_POS_IN_PROGRESS && \
  AXIS_POS_NOT_LOCKED && \
  AXIS_NO_CMD_IN_PROGRESS
//-----------------------------------------------------------------------------
static const size_t ABS_POS_CMD_CRITERIA_ARRAY[] =
{
  8,
  galil::kUCODE_RUNNING,
  galil::kAXIS_NOT_MOVING,
  galil::kAXIS_MOTOR_ON,
  galil::kAXIS_MOTOR_CTRL_ALLOWED,
  galil::kAXIS_NO_REF_POS_IN_PROGRESS,
  galil::kAXIS_POS_NOT_LOCKED,
  galil::kAXIS_NO_CMD_IN_PROGRESS
};
//=============================================================================
#define JOG_MINUS_CMD_ALLOWED \
   ABS_POS_CMD_ALLOWED
//-----------------------------------------------------------------------------
#define JOG_MINUS_CMD_CRITERIA_ARRAY ABS_POS_CMD_CRITERIA_ARRAY
//============================================================================= 
#define JOG_PLUS_CMD_ALLOWED \
  JOG_MINUS_CMD_ALLOWED
//-----------------------------------------------------------------------------
#define JOG_PLUS_CMD_CRITERIA_ARRAY JOG_MINUS_CMD_CRITERIA_ARRAY
//============================================================================= 
#define STOP_CMD_ALLOWED \
  UCODE_RUNNING && \
  AXIS_MOTOR_CTRL_ALLOWED && \
  AXIS_NO_CMD_IN_PROGRESS
//-----------------------------------------------------------------------------
static const size_t STOP_CMD_CRITERIA_ARRAY[] =
{
  3,
  galil::kUCODE_RUNNING,
  galil::kAXIS_MOTOR_CTRL_ALLOWED,
  galil::kAXIS_NO_CMD_IN_PROGRESS
};
//============================================================================= 
#define ACCEL_WRITE_ALLOWED \
  AXIS_NO_REF_POS_IN_PROGRESS 
//-----------------------------------------------------------------------------
static const size_t ACCEL_WRITE_CRITERIA_ARRAY[] =
{
  1,
  galil::kAXIS_NO_REF_POS_IN_PROGRESS
};
//============================================================================= 
#define DECEL_WRITE_ALLOWED \
  AXIS_NOT_MOVING && \
  AXIS_NO_REF_POS_IN_PROGRESS 
//-----------------------------------------------------------------------------
static const size_t DECEL_WRITE_CRITERIA_ARRAY[] =
{
  2,
  galil::kAXIS_NOT_MOVING,
  galil::kAXIS_NO_REF_POS_IN_PROGRESS
};
//============================================================================= 
#define VELOCITY_WRITE_ALLOWED \
  AXIS_NOT_GEARING_SLAVE && \
  AXIS_NO_REF_POS_IN_PROGRESS 
//-----------------------------------------------------------------------------
static const size_t VELOCITY_WRITE_CRITERIA_ARRAY[] =
{
  2,
  galil::kAXIS_NOT_GEARING_SLAVE,
  galil::kAXIS_NO_REF_POS_IN_PROGRESS
};
//============================================================================= 
#define DEADBAND_WRITE_ALLOWED \
  AXIS_NOT_MOVING && \
  AXIS_NO_REF_POS_IN_PROGRESS 
//-----------------------------------------------------------------------------
static const size_t DEADBAND_WRITE_CRITERIA_ARRAY[] =
{
  2,
  galil::kAXIS_NOT_MOVING,
  galil::kAXIS_NO_REF_POS_IN_PROGRESS
};
//============================================================================= 
#define BACKLASH_WRITE_ALLOWED \
  AXIS_NOT_MOVING && \
  AXIS_NO_REF_POS_IN_PROGRESS 
//-----------------------------------------------------------------------------
static const size_t BACKLASH_WRITE_CRITERIA_ARRAY[] =
{
  2,
  galil::kAXIS_NOT_MOVING,
  galil::kAXIS_NO_REF_POS_IN_PROGRESS
};
//============================================================================= 
#define LOCK_POS_ALLOWED \
  UCODE_RUNNING && \
  AXIS_NOT_MOVING && \
  AXIS_MOTOR_ON && \
  AXIS_MOTOR_CTRL_ALLOWED && \
  AXIS_NO_REF_POS_IN_PROGRESS && \
  AXIS_NO_ACTIVE_LSW && \
  AXIS_NO_CMD_IN_PROGRESS
//-----------------------------------------------------------------------------
static const size_t LOCK_POS_CMD_CRITERIA_ARRAY[] =
{
  7,
  galil::kUCODE_RUNNING,
  galil::kAXIS_NOT_MOVING,
  galil::kAXIS_MOTOR_ON,
  galil::kAXIS_MOTOR_CTRL_ALLOWED,
  galil::kAXIS_NO_REF_POS_IN_PROGRESS,
  galil::kAXIS_NO_ACTIVE_LSW,
  galil::kAXIS_NO_CMD_IN_PROGRESS
};

//-----------------------------------------------------------------------------
#define kDEFAULT_WCP_SLEEP_TIME 20 

namespace galil
{

//=============================================================================
// CMD NAMES
//=============================================================================
const char * galil_command_name[] =
{
  "<Gearing>",
  "<Motor Off>",
  "<Motor On>",
  "<Ref.Positionning>",
  "<Abs.Positionning>",
  "<Backward>",
  "<Forward>",
  "<Stop>",
  "<Acceleration>",
  "<Deceleration>",
  "<Velocitye>",
  "<Accuracy>",
  "<Backlash>",
  "<Lock Pos.>",
  "<Unknown Galil Command>"
};

//=============================================================================
// AXIS NAMES
//=============================================================================
#if defined(DMC_4143)
const char * galil_axis_name[MAX_AXIS] = {"A","B","C","D"};
#else
const char * galil_axis_name[MAX_AXIS] = {"A","B","C","D","E","F","G","H"};
#endif
// ============================================================================
// AxisConfig::AxisConfig
// ============================================================================
AxisStateManager::AxisConfig::AxisConfig ()
 : id (MIN_AXIS),
   has_inverted_lsw (false),
   duty_cycle_enabled (false),
   host_device (0)
{
  //- noop ctor
}

// ============================================================================
// AxisConfig::AxisConfig
// ============================================================================
AxisStateManager::AxisConfig::AxisConfig (const AxisConfig& _src)
{
  *this = _src;
}

// ============================================================================
// AxisConfig::operator=
// ============================================================================
void AxisStateManager::AxisConfig::operator= (const AxisConfig& _src)
{
  this->id                 = _src.id;
  this->has_inverted_lsw   = _src.has_inverted_lsw;
  this->duty_cycle_enabled = _src.duty_cycle_enabled;
  this->host_device        = _src.host_device;
}

// ============================================================================
// AxisStatus::AxisStatus
// ============================================================================
AxisStatus::AxisStatus ()
 : state (AXIS_UNKNOWN),
   status (kUNKNOWN_STATUS),
   tc_error (0),
   tc_error_str (tc_error_txt[0])
{
  //- noop ctor
}

// ============================================================================
// AxisStatus::operator=
// ============================================================================
void AxisStatus::operator= (const AxisStatus& src)
{
  if (&src == this)
    return;
    
  state = src.state;
  status = src.status;
  tc_error = src.tc_error;
  tc_error_str = src.tc_error_str;
}

// ============================================================================
// AxisStateManager::AxisStateManager
// ============================================================================
AxisStateManager::AxisStateManager ()
 : yat4tango::TangoLogAdapter (0), 
   m_acfg ()
{
	YAT_TRACE("galil::AxisStateManager::AxisStateManager");
}

// ============================================================================
// AxisStateManager::AxisStateManager
// ============================================================================
AxisStateManager::AxisStateManager (const AxisConfig & _acfg)
 : yat4tango::TangoLogAdapter (_acfg.host_device),
   m_acfg (_acfg)
{
	YAT_TRACE("galil::AxisStateManager::AxisStateManager");
}

// ============================================================================
// AxisStateManager::~AxisStateManager
// ============================================================================
AxisStateManager::~AxisStateManager (void)
{
	YAT_TRACE("galil::AxisStateManager::~AxisStateManager");
}

// ============================================================================
// AxisStateManager::status
// ============================================================================
void AxisStateManager::status (AxisStatus & status_)
{
  YAT_TRACE("galil::AxisStateManager::status");
  
  try
  {
    //- get extended axis status 
    const ExtendedAxisStatus & eas = this->extended_axis_status(false);

    //- set hardware status info
    status_.tc_error = eas.hws.tc_error;
    status_.tc_error_str = eas.hws.tc_error_str;

    //- is motor ctrl currently disabled?
    bool motor_ctrl_disabled = 
      ! eas.ucas.motor_ctrl_allowed() || eas.ucas.remote_manual();

    //- case: UCODE NOT RUNNING ----------------------------
    if (! eas.ucs.running())
    {
      //- switch this Axis to ERROR state
      status_.state = AXIS_ERROR;
      //- set the status
      status_.status += kUCODE_NOT_RUNNING_STATUS;
    }
    //- case: MOTOR OFF ------------------------------------
    else if (eas.fas.motor_off() && ! m_acfg.duty_cycle_enabled)
    {
			//- mark this Axis as OFF
			status_.state = AXIS_OFF;
			//- set the status
			status_.status = kAXIS_OFF_STATUS;
    }
    //- case: MOVING ---------------------------------------
    else if (! eas.ucas.pos_done() || eas.fas.moving())
    {
      //- whatever is this command, mark this Axis as MOVING
      status_.state = AXIS_MOVING;
      //- set the status
      status_.status = kMOVING_STATUS;
      //- add more info to the status
      if (eas.ucas.remote_manual())
        status_.status += kSTATUS_SEP + kREMOTE_CRTL_ACTIVE_STATUS;
      else if (! eas.ucas.motor_ctrl_allowed())
        status_.status += kSTATUS_SEP + kMOTOR_CRTL_NOT_ALLOWED_STATUS;
    }
    //- case: AXIS CTRL DISABLED ----------------------------
    else if (motor_ctrl_disabled)
    {
      //- Axis is in the STANDBY state
      status_.state = AXIS_CRTL_DISABLED;
      //- set the status
      status_.status = kAXIS_CRTL_DISABLED_STATUS;
      //- add more info to the status
      if (eas.ucas.remote_manual())
        status_.status += kSTATUS_SEP + kREMOTE_CRTL_ACTIVE_STATUS;
      else if (! eas.ucas.motor_ctrl_allowed())
        status_.status += kSTATUS_SEP + kMOTOR_CRTL_NOT_ALLOWED_STATUS;
    }
    //- case: LS-BACKWARD -----------------------------------
    else if (! eas.fas.no_backward_lsw())
    {
      //- Axis is in the STANDBY state
      status_.state = AXIS_ALARM;
      //- set the status
      if (! this->m_acfg.has_inverted_lsw)
        status_.status = kAXIS_BACKWARD_LSW_STATUS;
      else
        status_.status = kAXIS_FORWARD_LSW_STATUS;
      //- add more info to the status
      if (eas.ucas.remote_manual())
        status_.status += kSTATUS_SEP + kREMOTE_CRTL_ACTIVE_STATUS;
      else if (! eas.ucas.motor_ctrl_allowed())
        status_.status += kSTATUS_SEP + kMOTOR_CRTL_NOT_ALLOWED_STATUS;
    }
    //- case: LS-FORWARD ------------------------------------
    else if (! eas.fas.no_forward_lsw())
    {
      //- Axis is in the STANDBY state
      status_.state = AXIS_ALARM;
      //- set the status
      if (! this->m_acfg.has_inverted_lsw)
        status_.status = kAXIS_FORWARD_LSW_STATUS;
      else
        status_.status = kAXIS_BACKWARD_LSW_STATUS;
      //- add more info to the status
      if (eas.ucas.remote_manual())
        status_.status += kSTATUS_SEP + kREMOTE_CRTL_ACTIVE_STATUS;
      else if (! eas.ucas.motor_ctrl_allowed())
        status_.status += kSTATUS_SEP + kMOTOR_CRTL_NOT_ALLOWED_STATUS;
    }
    //- case: POS-ERROR -------------------------------------
    else if (eas.ucas.pos_err())
    {
#if ! defined(USE_REF_POS)
      //- Axis is in the STANDBY state
      status_.state = AXIS_ALARM;
      //- set the status
      status_.status = kAXIS_POS_ERROR_STATUS;
      //- add more info to the status
      if (eas.ucas.remote_manual())
        status_.status += kSTATUS_SEP + kREMOTE_CRTL_ACTIVE_STATUS;
      else if (! eas.ucas.motor_ctrl_allowed())
        status_.status += kSTATUS_SEP + kMOTOR_CRTL_NOT_ALLOWED_STATUS;
#else
      status_.state = AXIS_STANDBY;
      status_.status = kSTANDBY_NOT_LOCKED_STATUS;
#endif
    }
    //- case: STANDBY (POS MIGHT BY LOCKED) ----------------------
    else if (eas.ucas.pos_done() && ! eas.fas.moving())
    {
      //- axis is in the STANDBY state
      status_.state = AXIS_STANDBY;
      //- set the status
      if (eas.ucas.pos_locked())
        status_.status = kSTANDBY_LOCKED_STATUS;
      else
        status_.status = kSTANDBY_NOT_LOCKED_STATUS;
      //- add more info to the status
      if (eas.ucas.remote_manual())
        status_.status += kSTATUS_SEP + kREMOTE_CRTL_ACTIVE_STATUS;
      else if (! eas.ucas.motor_ctrl_allowed())
        status_.status += kSTATUS_SEP + kMOTOR_CRTL_NOT_ALLOWED_STATUS;
    }

    if  (eas.ucas.axis_master())
       status_.status += kSTATUS_SEP + kAXIS_GEARING_MASTER_STATUS;
    else if  (eas.ucas.axis_slave())
       status_.status += kSTATUS_SEP + kAXIS_GEARING_SLAVE_STATUS;

		//- case: RESET HW detected ----------------------------
    if (! eas.ucas.is_initialised())
    {
      //- switch this Axis to ERROR state
      status_.state = AXIS_ERROR;
      //- set the status
      status_.status += kSTATUS_SEP + kNOT_INITIALIZED_STATUS;
    }
  }
  catch (Tango::DevFailed& df)
  {
    //-TODO: use TC error info to distinguish between HW and COM failure????

    //- the AxisStateManager could not update the status info 
    //-------------------------------------------------------
    //- switch state to FAULT
    status_.state = AXIS_ERROR;
    //- search origin of the problem: deepest error in the stack
    Tango::DevError origin_of_pb = df.errors[df.errors.length() - 1];
    //- set status string
    status_.status = kTANGO_EX_STATUS
                   + std::string(" [")
                   + std::string(origin_of_pb.desc.in())
                   + std::string("]");
    //- log exception
    ERROR_STREAM << "AxisStateManager::status::caught the following TANGO exception..." << std::endl;
    ERROR_STREAM << df << std::endl; 
  }
  catch (...)
  {
    //- the worse situation: conclude to unknown status
    //-------------------------------------------------
    //- Axis is in the UNKNOWN state
    status_.state = AXIS_UNKNOWN;
    status_.status = kUNKNOWN_STATUS 
                   + std::string(" [unknown exception caught]");
    //- log exception
    ERROR_STREAM << "AxisStateManager::status::unknown exception caught" << std::endl;
  }
}

// ============================================================================
// AxisStateManager::state
// ============================================================================
AxisState AxisStateManager::state() {
	YAT_TRACE("galil::AxisStateManager::state");

	try {
		//- get extended axis status
		const ExtendedAxisStatus & eas = this->extended_axis_status(false);

		//- case: HW RESET detected ----------------------------
		if (!eas.ucas.is_initialised())
			return AXIS_ERROR;

		//- case: UCODE NOT RUNNING ----------------------------
		if (!eas.ucs.running())
			return AXIS_ERROR;

		//- case: MOTOR OFF ------------------------------------
		if (eas.fas.motor_off() && !m_acfg.duty_cycle_enabled)
			return AXIS_OFF;

		//- case: MOVING ---------------------------------------
		if ((!eas.ucas.pos_done() || eas.fas.moving())
				&& !eas.ucas.remote_manual())
			return AXIS_MOVING;

		//- is motor ctrl currently disabled?
		bool motor_ctrl_disabled = !eas.ucas.motor_ctrl_allowed()
				|| eas.ucas.remote_manual();

		//- case: AXIS CTRL DISABLED ----------------------------
		if (motor_ctrl_disabled)
			return AXIS_CRTL_DISABLED;

		//- case: LS-BACKWARD -----------------------------------
		if (!eas.fas.no_backward_lsw())
			return AXIS_ALARM;

		//- case: LS-FORWARD ------------------------------------
		if (!eas.fas.no_forward_lsw())
			return AXIS_ALARM;
#if !defined(USE_REF_POS)
		//- case: POS-ERROR -------------------------------------
		if (eas.ucas.pos_err())
			return AXIS_ALARM;
#endif
		//- case: STANDBY ---------------------------------------
		if (eas.ucas.pos_done() && !eas.fas.moving())
			return AXIS_STANDBY;
	} catch (Tango::DevFailed& df) {
		//- log exception
		ERROR_STREAM
				<< "AxisStateManager::state::caught the following TANGO exception..."
				<< std::endl;
		ERROR_STREAM << df << std::endl;
		return AXIS_ERROR;
	} catch (...) {
		//- log exception
		ERROR_STREAM << "AxisStateManager::state::unknown exception caught"
				<< std::endl;
		return AXIS_UNKNOWN;
	}

	return AXIS_UNKNOWN;
}

// ============================================================================
// AxisStateManager::backward_lsw_detected()
// ============================================================================
bool AxisStateManager::backward_lsw_detected ()
	throw (Tango::DevFailed)
{
  YAT_TRACE("galil::AxisStateManager::backward_lsw_detected");
  
  bool detected = false;

  try
  {
    //- get extended axis status 
    const ExtendedAxisStatus & eas = this->extended_axis_status(false);

//std::cout << "############################################" << std::endl;
  //  		std::cout << this->m_acfg.id << std::endl;
    // 		std::cout << eas << std::endl;
    //std::cout << "############################################" << std::endl;

    if (this->m_acfg.has_inverted_lsw)
      detected = ! eas.fas.no_forward_lsw();
    else
      detected = ! eas.fas.no_backward_lsw();
      
  }
  catch (Tango::DevFailed& df)
  {
    //- log exception
    ERROR_STREAM << "AxisStateManager::backward_lsw_detected::caught the following TANGO exception..." << std::endl;
    ERROR_STREAM << df << std::endl; 
    RETHROW_DEVFAILED(df,
                      _CPTC("SOFTWARE_ERROR"),
                      _CPTC("could not obtain the backward limit switch state"),
                      _CPTC("galil::AxisStateManager::backward_lsw_detected")); 
  }
  catch (...)
  {
    //- log exception
    ERROR_STREAM << "AxisStateManager::backward_lsw_detected::unknown exception caught" << std::endl;
    THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                    _CPTC("could not obtain the backward limit switch state [unknown exception caught]"),
                    _CPTC("galil::AxisStateManager::backward_lsw_detected")); 
  }

  return detected;
}

// ============================================================================
// AxisStateManager::forward_lsw_detected()
// ============================================================================
bool AxisStateManager::forward_lsw_detected ()
	throw (Tango::DevFailed)
{
  YAT_TRACE("galil::AxisStateManager::forward_lsw_detected");
  
  bool detected = false;

  try
  {
    //- get extended axis status 
    const ExtendedAxisStatus & eas = this->extended_axis_status(false);

    if (this->m_acfg.has_inverted_lsw)
      detected = ! eas.fas.no_backward_lsw();
    else
      detected = ! eas.fas.no_forward_lsw();
  }
  catch (Tango::DevFailed& df)
  {
    //- log exception
    ERROR_STREAM << "AxisStateManager::forward_lsw_detected::caught the following TANGO exception..." << std::endl;
    ERROR_STREAM << df << std::endl; 
    RETHROW_DEVFAILED(df,
                      _CPTC("SOFTWARE_ERROR"),
                      _CPTC("could not obtain the forward limit switch state"),
                      _CPTC("galil::AxisStateManager::forward_lsw_detected")); 
  }
  catch (...)
  {
    //- log exception
    ERROR_STREAM << "AxisStateManager::forward_lsw_detected::unknown exception caught" << std::endl;
    THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                    _CPTC("could not obtain the forward limit switch state [unknown exception caught]"),
                    _CPTC("galil::AxisStateManager::forward_lsw_detected")); 
  }

  return detected;
}

// ============================================================================
// AxisStateManager::is_gearing_master()
// ============================================================================
bool AxisStateManager::is_gearing_master ()
	throw (Tango::DevFailed)
{
  YAT_TRACE("galil::AxisStateManager::is_gearing_master");
  
  bool geared = false;

  try
  {
    //- get extended axis status 
    const ExtendedAxisStatus & eas = this->extended_axis_status(false);

      geared = eas.ucas.axis_master();
      
  }
  catch (Tango::DevFailed& df)
  {
    //- log exception
    ERROR_STREAM << "AxisStateManager::is_gearing_master::caught the following TANGO exception..." << std::endl;
    ERROR_STREAM << df << std::endl; 
    RETHROW_DEVFAILED(df,
                      _CPTC("SOFTWARE_ERROR"),
                      _CPTC("could not obtain the backward limit switch state"),
                      _CPTC("galil::AxisStateManager::is_gearing_master")); 
  }
  catch (...)
  {
    //- log exception
    ERROR_STREAM << "AxisStateManager::is_gearing_master::unknown exception caught" << std::endl;
    THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                    _CPTC("could not obtain the backward limit switch state [unknown exception caught]"),
                    _CPTC("galil::AxisStateManager::is_gearing_master")); 
  }

  return geared;
}

// ============================================================================
// AxisStateManager::is_gearing_slave()
// ============================================================================
bool AxisStateManager::is_gearing_slave ()
	throw (Tango::DevFailed)
{
  YAT_TRACE("galil::AxisStateManager::is_gearing_slave");
  
  bool geared = false;

  try
  {
    //- get extended axis status 
    const ExtendedAxisStatus & eas = this->extended_axis_status(false);

    geared = eas.ucas.axis_slave();
  }
  catch (Tango::DevFailed& df)
  {
    //- log exception
    ERROR_STREAM << "AxisStateManager::is_gearing_slave::caught the following TANGO exception..." << std::endl;
    ERROR_STREAM << df << std::endl; 
    RETHROW_DEVFAILED(df,
                      _CPTC("SOFTWARE_ERROR"),
                      _CPTC("could not obtain the backward limit switch state"),
                      _CPTC("galil::AxisStateManager::is_gearing_slave")); 
  }
  catch (...)
  {
    //- log exception
    ERROR_STREAM << "AxisStateManager::is_gearing_slave::unknown exception caught" << std::endl;
    THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                    _CPTC("could not obtain the backward limit switch state [unknown exception caught]"),
                    _CPTC("galil::AxisStateManager::is_gearing_slave")); 
  }

  return geared;
}

// ============================================================================
// AxisStateManager::is_position_locked()
// ============================================================================
bool AxisStateManager::is_position_locked ()
	throw (Tango::DevFailed)
{
  YAT_TRACE("galil::AxisStateManager::is_position_locked");
  
  bool locked = false;

  try
  {
    //- get extended axis status 
    const ExtendedAxisStatus & eas = this->extended_axis_status(false);

    locked = eas.ucas.pos_locked();
  }
  catch (Tango::DevFailed& df)
  {
    //- log exception
    ERROR_STREAM << "AxisStateManager::is_position_locked::caught the following TANGO exception..." << std::endl;
    ERROR_STREAM << df << std::endl; 
    RETHROW_DEVFAILED(df,
                      _CPTC("SOFTWARE_ERROR"),
                      _CPTC("could not obtain the axis position locking state"),
                      _CPTC("galil::AxisStateManager::is_position_locked")); 
  }
  catch (...)
  {
    //- log exception
    ERROR_STREAM << "AxisStateManager::is_gearing_slave::unknown exception caught" << std::endl;
    THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                    _CPTC("could not obtain the axis position locking state [unknown exception caught]"),
                    _CPTC("galil::AxisStateManager::is_position_locked")); 
  }

  return locked;
}

// ============================================================================
// AxisStateManager::update_ext_axis_status_i
// ============================================================================
const ExtendedAxisStatus & AxisStateManager::update_ext_axis_status_i (bool _force_update)
	throw (Tango::DevFailed)
{
  YAT_TRACE("galil::AxisStateManager::update_ext_axis_status_i");

  try
  {
    //- does the caller want to force the update on the HwStatusProxy?
    if (_force_update)
    {
      int mask = 0;
      mask |= HwStatusProxy::UPDATE_UC_STATUS;
      mask |= HwStatusProxy::UPDATE_UC_AXIS_STATUS;
      mask |= HwStatusProxy::UPDATE_UC_AXIS_CMD_STATUS;
      //- force status update on the HwStatusProxy
      HW_STATUS_PROXY->force_status_update(mask);
    }

    //- get our current axis status from the HwStatusProxy
	  HW_STATUS_PROXY->extended_axis_status(this->m_acfg.id, this->m_eas);

    //- this->dump();
  }
  catch (Tango::DevFailed& df)
  {
    RETHROW_DEVFAILED(df,
                      _CPTC("HARDWARE_ERROR"),
                      _CPTC("could not obtain the axis status information"),
                      _CPTC("galil::AxisStateManager::update_ext_axis_status_i")); 
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("could not obtain the axis status information"),
                    _CPTC("galil::AxisStateManager::update_ext_axis_status_i")); 
  }

  return this->m_eas;
}

// ============================================================================
// AxisStateManager::command_allowed
// ============================================================================
bool AxisStateManager::command_allowed (CommandIdentifier _cid, 
                                        std::string * _why_str,
                                        CmdAllowedCriteria * _why_id)
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::AxisStateManager::command_allowed");

  bool cmd_allowed = false;

  //- update current axis state (force status reading on hw)
  this->update_ext_axis_status_i(true);

  //- is the specified command allowed?
  switch (_cid)
  {
    case GEARING_CMD:
      YAT_LOG("galil::AxisStateManager::command_allowed::eval.criteria::GEARING_CMD_ALLOWED"); 
      cmd_allowed = GEARING_CMD_ALLOWED;
      break;
    case MOTOR_OFF_CMD:
      YAT_LOG("galil::AxisStateManager::command_allowed::eval.criteria::MOTOR_OFF_CMD_ALLOWED"); 
      cmd_allowed = MOTOR_OFF_CMD_ALLOWED;
      break;
    case MOTOR_ON_CMD:
      YAT_LOG("galil::AxisStateManager::command_allowed::eval.criteria::MOTOR_ON_CMD_ALLOWED"); 
      cmd_allowed = MOTOR_ON_CMD_ALLOWED;
      break;
    case REF_POS_CMD:
      YAT_LOG("galil::AxisStateManager::command_allowed::eval.criteria::REF_POS_CMD_ALLOWED"); 
      cmd_allowed = REF_POS_CMD_ALLOWED;
      break;
    case ABS_POS_CMD:
      YAT_LOG("galil::AxisStateManager::command_allowed::eval.criteria::ABS_POS_CMD_ALLOWED"); 
      cmd_allowed = ABS_POS_CMD_ALLOWED;
      break;
    case JOG_MINUS_CMD:
      YAT_LOG("galil::AxisStateManager::command_allowed::eval.criteria::JOG_MINUS_CMD_ALLOWED"); 
      cmd_allowed = JOG_MINUS_CMD_ALLOWED;
      break;
    case JOG_PLUS_CMD:
      YAT_LOG("galil::AxisStateManager::command_allowed::eval.criteria::JOG_PLUS_CMD_ALLOWED"); 
      cmd_allowed = JOG_PLUS_CMD_ALLOWED;
      break;
    case STOP_CMD:
      YAT_LOG("galil::AxisStateManager::command_allowed::eval.criteria::STOP_CMD_ALLOWED"); 
      cmd_allowed = STOP_CMD_ALLOWED;
      break;
    case ACCEL_WRITE:
      YAT_LOG("galil::AxisStateManager::command_allowed::eval.criteria::ACCEL_WRITE_ALLOWED"); 
      cmd_allowed = ACCEL_WRITE_ALLOWED;
      break;
    case DECEL_WRITE:
      YAT_LOG("galil::AxisStateManager::command_allowed::eval.criteria::DECEL_WRITE_ALLOWED"); 
      cmd_allowed = DECEL_WRITE_ALLOWED;
      break;
    case VELOCITY_WRITE:
      YAT_LOG("galil::AxisStateManager::command_allowed::eval.criteria::VELOCITY_WRITE_ALLOWED"); 
      cmd_allowed = VELOCITY_WRITE_ALLOWED;
      break;
    case DEADBAND_WRITE:
      YAT_LOG("galil::AxisStateManager::command_allowed::eval.criteria::DEADBAND_WRITE_ALLOWED"); 
      cmd_allowed = DEADBAND_WRITE_ALLOWED;
      break;
    case BACKLASH_WRITE:
      YAT_LOG("galil::AxisStateManager::command_allowed::eval.criteria::BACKLASH_WRITE_ALLOWED"); 
      cmd_allowed = BACKLASH_WRITE_ALLOWED;
      break;
    case LOCK_POS_CMD:
      YAT_LOG("galil::AxisStateManager::command_allowed::eval.criteria::LOCK_POS_ALLOWED"); 
      cmd_allowed = LOCK_POS_ALLOWED;
      break;
    default:
      _cid = MAX_COMMAND;
      break;
  }

  if (! cmd_allowed)
  {
    //- search <first> criteria which failed 
    size_t c = this->why_cmd_refused(_cid);

    //- verbose
    ERROR_STREAM << "AxisStateManager::command_allowed::rejecting "
                 << galil_command_name[_cid]
                 << " cmd request::"
                 << kWHY[c]
                 << std::endl;

    //- why is the command refused?
    if (_why_str)
    {
      //- set the reason for caller 
      yat::OSStream oms;
      oms << galil_command_name[_cid]
          << " cmd rejected::"
          << kWHY[c];
      *_why_str = oms.str(); 
    }

    //- why is the command refused?
    if (_why_id)
      *_why_id = static_cast<CmdAllowedCriteria>(c); 
  }

  return cmd_allowed;
}

// ============================================================================
// AxisStateManager::why_cmd_refused 
// ============================================================================
CmdAllowedCriteria AxisStateManager::why_cmd_refused (CommandIdentifier _cid) const
{
  YAT_TRACE("galil::AxisStateManager::why_cmd_refused");

  #define SEARCH_FAULTY_CRITERIA(CRITERIA_ARRAY) \
  { \
    for (size_t c = 1; c <= CRITERIA_ARRAY[0]; c++) \
    { \
      if (this->eval_criteria(CRITERIA_ARRAY[c]) == false) \
        return static_cast<CmdAllowedCriteria>(CRITERIA_ARRAY[c]); \
    } \
  }

  switch (_cid)
  {
    case GEARING_CMD:
      SEARCH_FAULTY_CRITERIA (GEARING_CMD_CRITERIA_ARRAY);
      break;
    case MOTOR_OFF_CMD:
      SEARCH_FAULTY_CRITERIA (MOTOR_OFF_CMD_CRITERIA_ARRAY);
      break;
    case MOTOR_ON_CMD:
      SEARCH_FAULTY_CRITERIA (MOTOR_ON_CMD_CRITERIA_ARRAY);
      break;
    case REF_POS_CMD:
      SEARCH_FAULTY_CRITERIA (REF_POS_CMD_CRITERIA_ARRAY);
      break;
    case ABS_POS_CMD:
      SEARCH_FAULTY_CRITERIA (ABS_POS_CMD_CRITERIA_ARRAY);
      break;
    case JOG_MINUS_CMD:
      SEARCH_FAULTY_CRITERIA (JOG_MINUS_CMD_CRITERIA_ARRAY);
      break;
    case JOG_PLUS_CMD:
      SEARCH_FAULTY_CRITERIA (JOG_PLUS_CMD_CRITERIA_ARRAY);
      break;
    case STOP_CMD:
      SEARCH_FAULTY_CRITERIA (STOP_CMD_CRITERIA_ARRAY);
      break;
    case ACCEL_WRITE:
      SEARCH_FAULTY_CRITERIA (ACCEL_WRITE_CRITERIA_ARRAY);
      break;
    case DECEL_WRITE:
      SEARCH_FAULTY_CRITERIA (DECEL_WRITE_CRITERIA_ARRAY);
      break;
    case VELOCITY_WRITE:
      SEARCH_FAULTY_CRITERIA (VELOCITY_WRITE_CRITERIA_ARRAY);
      break;
    case DEADBAND_WRITE:
      SEARCH_FAULTY_CRITERIA (DEADBAND_WRITE_CRITERIA_ARRAY);
      break;
    case BACKLASH_WRITE:
      SEARCH_FAULTY_CRITERIA (BACKLASH_WRITE_CRITERIA_ARRAY);
      break;
    case LOCK_POS_CMD:
      SEARCH_FAULTY_CRITERIA (LOCK_POS_CMD_CRITERIA_ARRAY);
      break;
    default:
      return kUNKNOWN_COMMAND;
      break;
  }

  return kUNKNOWN_CRITERIA;
}

//=============================================================================
// AxisStateManager::eval_criteria
//=============================================================================
bool AxisStateManager::eval_criteria (int cid) const
{
  YAT_TRACE("galil::AxisStateManager::eval_criteria");

  bool criteria_eval = false;

  switch (cid)
  {
    case kUCODE_RUNNING:
      criteria_eval = UCODE_RUNNING;
      break;
    case kUCODE_NO_REF_POS_IN_PROGRESS:
      criteria_eval = UCODE_NO_REF_POS_IN_PROGRESS;
      break;
    case kAXIS_NOT_MOVING:
      criteria_eval = AXIS_NOT_MOVING;
      break;
    case kAXIS_MOTOR_ON:
      criteria_eval = AXIS_MOTOR_ON;
      break;
    case kAXIS_POWER_CTRL_ALLOWED:
      criteria_eval = AXIS_POWER_CTRL_ALLOWED;
      break;
    case kAXIS_MOTOR_CTRL_ALLOWED:
      criteria_eval = AXIS_MOTOR_CTRL_ALLOWED;
      break;
    case kAXIS_NO_REF_POS_IN_PROGRESS:
      criteria_eval = AXIS_NO_REF_POS_IN_PROGRESS;
      break;
    case kAXIS_NO_FOLLOW_ERROR:
      criteria_eval = AXIS_NO_FOLLOW_ERROR;
      break;
    case kAXIS_NOT_GEARING_MASTER:
      criteria_eval = AXIS_NOT_GEARING_MASTER;
      break;
    case kAXIS_NOT_GEARING_SLAVE:
      criteria_eval = AXIS_NOT_GEARING_SLAVE;
      break;
    case kAXIS_POS_NOT_LOCKED:
      criteria_eval = AXIS_POS_NOT_LOCKED;
      break;
    case kAXIS_NO_CMD_IN_PROGRESS:
      criteria_eval = AXIS_NO_CMD_IN_PROGRESS;
      break;
    default:
      break;
  }

  return criteria_eval;
}

//=============================================================================
// AxisStateManager::wait_current_command_processed
//=============================================================================
void AxisStateManager::wait_current_command_processed (size_t _tmo_ms, bool _update_state)
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::AxisStateManager::wait_current_command_processed");

  yat::Timestamp start, now;
  _GET_TIME(start);

  bool cmd_in_progress;
  bool tmo_expired;
  double tmo_sec = 1.E-3 * _tmo_ms;

  UCodeAxisCommandState ucacs;

  YAT_LOG("AxisStateManager::wait_current_command_processed::checking status...");

  do
  {
    //- force axis Cmd[] status update...
    HW_STATUS_PROXY->force_status_update(HwStatusProxy::UPDATE_UC_AXIS_CMD_STATUS);

    //- ... then get it 
    HW_STATUS_PROXY->ucode_axis_cmd_state(this->m_acfg.id, ucacs);

    //- check time 
    _GET_TIME(now);
    tmo_expired = _TMO_EXPIRED(start, now, tmo_sec);

    //- is the command processed by the ucode?
    cmd_in_progress = ucacs.any_command_in_progress();

    if (cmd_in_progress && ! tmo_expired)
      yat::Thread::sleep(kDEFAULT_WCP_SLEEP_TIME);
  }
  while (cmd_in_progress && ! tmo_expired);

  if (cmd_in_progress && tmo_expired)
  {
    YAT_LOG("AxisStateManager::wait_current_command_processed::ko - tmo expired");
    THROW_DEVFAILED(_CPTC("HARDWARE_ERROR"),
                    _CPTC("timeout expired while waiting for the current cmd to be processed by the Galil-box ucode"),
                    _CPTC("galil::AxisStateManager::wait_current_command_processed")); 
  }

  if (_update_state)
    HW_STATUS_PROXY->force_status_update(HwStatusProxy::UPDATE_UC_AXIS_STATUS);

  YAT_LOG("AxisStateManager::wait_current_command_processed::ok - cmd processed by ucode");
}

//=============================================================================
// AxisStateManager::wait_motor_stopped
//=============================================================================
void AxisStateManager::wait_motor_stopped (size_t _tmo_ms)
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::AxisStateManager::wait_motor_stopped");

  yat::Timestamp start, now;
  _GET_TIME(start);

  bool motor_stopped;
  bool tmo_expired;
  double tmo_sec = 1.E-3 * _tmo_ms;

  FirmwareAxisState fas;

  YAT_LOG("AxisStateManager::wait_motor_stopped::checking status...");

	//- status supdate mask
	int mask = 0;
	mask |= HwStatusProxy::UPDATE_UC_STATUS;
  mask |= HwStatusProxy::UPDATE_HW_STATUS;

  do
  {
    //- force ucode & hw status update
    HW_STATUS_PROXY->force_status_update(mask);

		//- get a local copy of both status
		HW_STATUS_PROXY->firmware_axis_state(this->m_acfg.id, this->m_eas.fas);
		HW_STATUS_PROXY->ucode_axis_state(this->m_acfg.id, this->m_eas.ucas);

    //- check time 
    _GET_TIME(now);
    tmo_expired = _TMO_EXPIRED(start, now, tmo_sec);

    //- is the motor still moving?
    motor_stopped = AXIS_NOT_MOVING;

    if (! motor_stopped && ! tmo_expired)
      yat::Task::sleep(kDEFAULT_WCP_SLEEP_TIME);
  } 
  while (! motor_stopped && ! tmo_expired);

  if (! motor_stopped && tmo_expired)
  {
    YAT_LOG("AxisStateManager::wait_motor_stopped::ko - tmo expired");
    THROW_DEVFAILED(_CPTC("HARDWARE_ERROR"),
                    _CPTC("timeout expired while waiting for the motor to stop"),
                    _CPTC("galil::AxisStateManager::wait_motor_stopped")); 
  }

  YAT_LOG("AxisStateManager::wait_motor_stopped::ok - motor stopped");
}

} // namespace galil
