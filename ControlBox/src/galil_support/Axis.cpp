//=============================================================================
// Axis.cpp
//=============================================================================
// abstraction.......Galil axis abstraction class
// class.............Axis
// original author...N.Leclercq - J.Coquet - SOLEIL
//=============================================================================

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <math.h>
#include <tango.h>
#include <yat/LogHelper.h>
#include <yat/time/Timer.h>
#include <yat4tango/ExceptionHelper.h>
#include "BitsRecords.h"
#include "Axis.h"
#include <yat/utils/XString.h>
#include "ObjectManager.h"

// ============================================================================
// SOME CONSTs
// ============================================================================
#define kDEFAULT_WAIT_CMD_PROC_TMO 2000
//- user to hw unit ratio -----------------------------------------------------
#define kUSER_TO_HW_UNIT_RATIO (this->m_cfg.encoder_ratio * this->m_cfg.motor_ratio) 
//-----------------------------------------------------------------------------
#define USER_TO_HW_UNITS(X) ((X) / kUSER_TO_HW_UNIT_RATIO)
//----------------------------------------------------------------------------
#define HW_TO_USER_UNITS(X) ((X) * kUSER_TO_HW_UNIT_RATIO)
//- acceleration in hw units --------------------------------------------------
#define kMIN_ACC_HW_UNIT 1024       
#define kMAX_ACC_HW_UNIT 67107840 
//- acceleration in user units ------------------------------------------------
#define kMIN_ACC ::fabs(HW_TO_USER_UNITS(kMIN_ACC_HW_UNIT))
#define kMAX_ACC ::fabs(HW_TO_USER_UNITS(kMAX_ACC_HW_UNIT))
//- deceleration range in user units ------------------------------------------
#define kMIN_DEC kMIN_ACC  
#define kMAX_DEC kMAX_ACC  
//- velocity range in hw units ------------------------------------------------
#define kMIN_SPD_HW_UNIT 2          
#define kMAX_SPD_HW_UNIT 2000000 
//- velocity range in user units ----------------------------------------------
#define kMIN_SPD ::fabs(HW_TO_USER_UNITS(kMIN_SPD_HW_UNIT))
#define kMAX_SPD ::fabs(HW_TO_USER_UNITS(kMAX_SPD_HW_UNIT))
//- accuracy range in hw units ------------------------------------------------
#define kMIN_DBW_HW_UNIT 0          
#define kMAX_DBW_HW_UNIT 1000000
//- accuracy range in user units ----------------------------------------------
#define kMIN_DBW ::fabs(USER_TO_HW_UNITS(kMIN_DBW_HW_UNIT))
#define kMAX_DBW ::fabs(USER_TO_HW_UNITS(kMAX_DBW_HW_UNIT)) 
//- backlash range in hw units ------------------------------------------------
#define kMIN_BCK_HW_UNIT -1000000   
#define kMAX_BCK_HW_UNIT +1000000
//- backlash range in user units ----------------------------------------------
#define kMIN_BCK USER_TO_HW_UNITS(kMIN_BCK_HW_UNIT)
#define kMAX_BCK USER_TO_HW_UNITS(kMAX_BCK_HW_UNIT)  
//- gearing -------------------------------------------------------------------
#define kNO_MASTER_AXIS -1
//- gearing origin types-------------------------------------------------------
#define kORIGIN_OF_GEARING_COMMANDED_REF_POS 10
#define kORIGIN_OF_GEARING_MAIN_ENCODER       0
#define kORIGIN_OF_GEARING_AUX_ENCODER       20
//-----------------------------------------------------------------------------
#define kSTATUS_SEP                 std::string("\n")
//-----------------------------------------------------------------------------
#define kAXIS_INIT_POS_ERROR_STATUS "axis not initialized [no initial ref. pos.]"
//-----------------------------------------------------------------------------
#define kDEFAULT_PERCENT 1.
#define kDEFAULT_RETRY   3
#define kDEFAULT_SETTLING_TIME 100
//-----------------------------------------------------------------------------
#define _NAN_ ::sqrt(static_cast<double>(-1.))
//-----------------------------------------------------------------------------
#if defined(WIN32)
# define IS_NAN(X) _isnan(X)
#else
# define IS_NAN(X) isnan(X)
#endif
//-----------------------------------------------------------------------------
#define kDEFAULT_SP_MINI 32
//-----------------------------------------------------------------------------
#define kMOTION_REQUEST_TMO_IN_MSECS 500
//-----------------------------------------------------------------------------

namespace galil
{
// ==========================================================================
// SOME USER DEFINED MESSAGES FOR THE <AxisPositionLocker> CLASS
// ==========================================================================
#define kLOCK_POS         (yat::FIRST_USER_MSG + 1000)
#define kUNLOCK_POS       (yat::FIRST_USER_MSG + 1001)
#define kMOTION_REQUESTED (yat::FIRST_USER_MSG + 1002)
#define kSTOP_REQUESTED   (yat::FIRST_USER_MSG + 1003)
#define kJOG_REQUESTED    (yat::FIRST_USER_MSG + 1004)

// ==========================================================================
//- AxisPositionLocker::AxisPositionLocker
// ==========================================================================
AxisPositionLocker::AxisPositionLocker(Axis * ax, 
                                       double pc, 
                                       size_t cp, 
                                       size_t aw, 
                                       Tango::DeviceImpl * h)
  : yat4tango::DeviceTask(h),
    m_axis(ax), 
    m_axis_id(MIN_AXIS),
    m_locking_enabled(false),
    m_unlock_pending(false),
    m_target_position(_NAN_), 
    m_position_constraint(pc),
    m_ckecking_period_secs(cp), 
    m_locked_pos_averaging_window(aw),
    m_tmo(kMOTION_REQUEST_TMO_IN_MSECS, yat::Timeout::TMO_UNIT_MSEC, true)
{
  this->m_axis_id = this->m_axis ? this->m_axis->identifier() : MIN_AXIS;
}

// ==========================================================================
//- AxisPositionLocker::~AxisPositionLocker
// ==========================================================================
AxisPositionLocker::~AxisPositionLocker() 
{
  //-noop
}

// ============================================================================
// AxisPositionLocker::lock_position
// ============================================================================
void AxisPositionLocker::lock_position (const double & p)
	throw (Tango::DevFailed)
{
  //- notify the manager task
  yat::Message * m = new yat::Message(kLOCK_POS, MAX_USER_PRIORITY, true);
  m->attach_data(p);
  this->wait_msg_handled(m);
}

// ============================================================================
// AxisPositionLocker::unlock_position
// ============================================================================
void AxisPositionLocker::unlock_position ()
	throw (Tango::DevFailed)
{
  //- notify the manager task
  yat::Message * m = new yat::Message(kUNLOCK_POS, MAX_USER_PRIORITY, true);
  this->wait_msg_handled(m);
}

// ============================================================================
// AxisPositionLocker::motion_requested
// ============================================================================
void AxisPositionLocker::motion_requested (const double & p)
	throw (Tango::DevFailed)
{
  //- notify the manager task
  yat::Message * m = new yat::Message(kMOTION_REQUESTED, MAX_USER_PRIORITY, true);
  m->attach_data(p);
  this->wait_msg_handled(m);
}

// ============================================================================
// AxisPositionLocker::stop_requested
// ============================================================================
void AxisPositionLocker::stop_requested ()
	throw (Tango::DevFailed)
{
  //- notify the manager task
  yat::Message * m = new yat::Message(kSTOP_REQUESTED, MAX_USER_PRIORITY, true);
  this->wait_msg_handled(m);
}

// ============================================================================
// AxisPositionLocker::jog_requested
// ============================================================================
void AxisPositionLocker::jog_requested ()
	throw (Tango::DevFailed)
{
  //- notify the manager task
  yat::Message * m = new yat::Message(kJOG_REQUESTED, MAX_USER_PRIORITY, true);
  this->wait_msg_handled(m);
}

// ============================================================================
// AxisPositionLocker::process_message
// ============================================================================
void AxisPositionLocker::process_message (yat::Message& _msg)
	throw (Tango::DevFailed)
{
  if (! this->m_axis)
    return;

  try
  {
	  switch (_msg.type())
	  {
		  case yat::TASK_INIT: //--------------------------------------------------
			  {
				  INFO_STREAM << "AxisPositionLocker::starting locking manager for axis-"
                      << Axis::axis_id_to_string(this->m_axis_id)
                      << std::endl;
          //- resize averaging buffer then fill it with current position
          this->m_locked_pos_averaging_buffer.capacity(this->m_locked_pos_averaging_window);
          this->m_locked_pos_averaging_buffer.fill(0.);
          //- enable periodic activity
          this->set_periodic_msg_period(1000 * this->m_ckecking_period_secs);
          this->enable_periodic_msg(true);
			  } 
			  break;
		  case yat::TASK_EXIT: //--------------------------------------------------
			  {
          //- warning: can't log here cause the device might have already been killed 
          //- by the Tango layer (see how ObjectManager is managed in main.cpp code).  
          //- stop <locked motion> on ctrlbox side (for the ucode, STOP means UNLOCK)
          if (this->m_locking_enabled)
          {
			      this->m_locking_enabled = false;
            this->m_target_position = _NAN_;
            this->m_axis->stop_i();
          }
			  } 
			  break;
		  case kLOCK_POS: //-------------------------------------------------------
			  {
          //- extract target axis position from message
          double requested_locked_pos = _msg.get_data<double>();
          if (this->m_target_position != requested_locked_pos)
          {
            //- store target position
            this->m_target_position = requested_locked_pos;
				    INFO_STREAM << "AxisPositionLocker::axis-"
                        << Axis::axis_id_to_string(this->m_axis_id)
                        << "::target position is now "
                        << this->m_target_position
                        << std::endl;
            //- resize averaging buffer then fill it with current position
            this->m_locked_pos_averaging_buffer.capacity(this->m_locked_pos_averaging_window);
            this->m_locked_pos_averaging_buffer.fill(this->m_target_position);
          }
          //- reset any pending <unlock request>
          this->m_unlock_pending = false;
          //- mark locking mecanism as enabled
          this->m_locking_enabled = true;
			  }
			  break;
		  case kUNLOCK_POS: //-----------------------------------------------------
			  {
          //- if the axis is moving...
          if (this->m_axis->state() == AXIS_MOVING)
          {
 				    INFO_STREAM << "AxisPositionLocker::axis-"
                        << Axis::axis_id_to_string(this->m_axis_id)
                        << "::axis is moving::unlock resquest postponed!"
                        << std::endl;
            //- in case a motion is in progress just mark this <unlock request> as pending
            this->m_unlock_pending = true;
            return;
          }
          //- otherwise, mark locking mecanism as disabled and invalidate target position
			    this->m_locking_enabled = false;
          this->m_target_position = _NAN_;
          //- finally, stop <locked motion> on ctrlbox side (for the microcode, STOP means UNLOCK)
          try { 
 				    INFO_STREAM << "AxisPositionLocker::axis-"
                        << Axis::axis_id_to_string(this->m_axis_id)
                        << "::unlocking axis position"
                        << std::endl;
            this->m_axis->stop_i();
          }
          catch (Tango::DevFailed& df) {
            ERROR_STREAM << "AxisPositionLocker::axis-"
                         << Axis::axis_id_to_string(this->m_axis_id)
                         << "::failed to unlock axis position on ctrlbox side [stop cmd failed]"
                         << std::endl;
            ERROR_STREAM << df
                         << std::endl;
          }
          catch (...) {
            ERROR_STREAM << "AxisPositionLocker::axis-"
                         << Axis::axis_id_to_string(this->m_axis_id)
                         << "::failed to unlock axis position on ctrlbox side [stop cmd failed]"
                         << std::endl;
          }
			  }
			  break;
      case kMOTION_REQUESTED: //-----------------------------------------------
			  {
          //- nothing to do if the locking mecanism is disabled
          if (! this->m_locking_enabled)
          {
				    DEBUG_STREAM << "AxisPositionLocker::axis-"
                         << Axis::axis_id_to_string(this->m_axis_id)
                         << "::motion resquest notified::locking disabled [returning...]"
                         << std::endl;
            return;
          }
          //- this message is sent by the associated axis uppon receipt of a motion
          //- request (see Axis::abs_pos). it means that no motion is already in 
          //- progress (guaranteed by the axis state machine). consequently, it's 
          //- safe to unlock the pos (i.e. to stop any "locked motion" previously 
          //- sent to the control box). from the <AxisPositionLocker> point of view
          //- the locking mecanism is still enabled we just change the target/locked
          //- position
          DEBUG_STREAM << "AxisPositionLocker::axis-"
                       << Axis::axis_id_to_string(this->m_axis_id)
                       << "::motion resquest notified::unlocking pos. on ctrlbox side..."
                       << std::endl;
          try { 
            //- for the microcode, STOP also means UNLOCK
            //- important note: the <Axis::abs_pos> impl guarantee that the axis is not moving
            this->m_axis->stop_i();
          }
          catch (Tango::DevFailed& df) {
            ERROR_STREAM << "AxisPositionLocker::axis-"
                         << Axis::axis_id_to_string(this->m_axis_id)
                         << "::failed to unlock axis position on ctrlbox side [stop cmd failed]"
                         << std::endl;
            ERROR_STREAM << df
                         << std::endl;
          }
          catch (...) {
            ERROR_STREAM << "AxisPositionLocker::axis-"
                         << Axis::axis_id_to_string(this->m_axis_id)
                         << "::failed to unlock axis position on ctrlbox side [stop cmd failed]"
                         << std::endl;
          }
          //- extract target axis position from message
          double requested_locked_pos = _msg.get_data<double>();
          if (this->m_target_position != requested_locked_pos)
          {
            //- store target position
            this->m_target_position = requested_locked_pos;
				    INFO_STREAM << "AxisPositionLocker::axis-"
                        << Axis::axis_id_to_string(this->m_axis_id)
                        << "::target position is now "
                        << this->m_target_position
                        << std::endl;
            //- resize averaging buffer then fill it with current position
            this->m_locked_pos_averaging_buffer.capacity(this->m_locked_pos_averaging_window);
            this->m_locked_pos_averaging_buffer.fill(this->m_target_position);
          }
          //- restart our <tmo>
          this->m_tmo.restart();
			  }
			  break;
      case kSTOP_REQUESTED: //-------------------------------------------------
			  {
          //- nothing to do if the locking mecanism is disabled
          if (! this->m_locking_enabled)
          {
				    DEBUG_STREAM << "AxisPositionLocker::axis-"
                         << Axis::axis_id_to_string(this->m_axis_id)
                         << "::stop resquest notified::locking disabled [returning...]"
                         << std::endl;
            return;
          }
				  DEBUG_STREAM << "AxisPositionLocker::axis-"
                       << Axis::axis_id_to_string(this->m_axis_id)
                       << "::stop resquest notified::disabling pos. locking"
                       << std::endl;
          //- mark locking mecanism as disabled and invalidate target position
			    this->m_locking_enabled = false;
          this->m_unlock_pending  = false;
          this->m_target_position = _NAN_;
			  }
			  break;
      case kJOG_REQUESTED: //--------------------------------------------------
			  {
          //- nothing to do if the locking mecanism is disabled
          if (! this->m_locking_enabled)
          {
				    DEBUG_STREAM << "AxisPositionLocker::axis-"
                         << Axis::axis_id_to_string(this->m_axis_id)
                         << "::jog resquest notified::locking disabled [returning...]"
                         << std::endl;
            return;
          }
          //- this message is sent by the associated axis uppon receipt of a motion
          //- request (see Axis::forward/backward). it means that no motion is already
          //- in progress (guaranteed by the axis state machine). consequently, it's 
          //- safe to unlock the pos (i.e. to stop any "locked motion" previously 
          //- sent to the control box). from the <AxisPositionLocker> point of view
          //- the locking mecanism is still enabled we just change the target/locked
          //- position
          DEBUG_STREAM << "AxisPositionLocker::axis-"
                       << Axis::axis_id_to_string(this->m_axis_id)
                       << "::jog resquest notified::unlocking pos. on ctrlbox side..."
                       << std::endl;
          try { 
            //- for the microcode, STOP also means UNLOCK
            //- important note: the <Axis::forward/backward> impl guarantee that the axis is not moving
            this->m_axis->stop_i();
          }
          catch (Tango::DevFailed& df) {
            ERROR_STREAM << "AxisPositionLocker::axis-"
                         << Axis::axis_id_to_string(this->m_axis_id)
                         << "::failed to unlock axis position on ctrlbox side [stop cmd failed]"
                         << std::endl;
            ERROR_STREAM << df
                         << std::endl;
          }
          catch (...) {
            ERROR_STREAM << "AxisPositionLocker::axis-"
                         << Axis::axis_id_to_string(this->m_axis_id)
                         << "::failed to unlock axis position on ctrlbox side [stop cmd failed]"
                         << std::endl;
          }
          //- mark locking mecanism as disabled and invalidate target position
			    this->m_locking_enabled = false;
          this->m_unlock_pending  = false;
          this->m_target_position = _NAN_;
			  }
			  break;
		  case yat::TASK_PERIODIC: //----------------------------------------------
			  {
          this->manage_position_locking();
			  }
			  break;
      default: //--------------------------------------------------------------
        break;
    }
	}
	catch (Tango::DevFailed& te)
	{
    ERROR_STREAM << "AxisPositionLocker::process_message::Tango exception caught" << std::endl; 
    ERROR_STREAM << te << std::endl; 
	}
	catch (...)
	{		
    ERROR_STREAM << "AxisPositionLocker::process_message::unknown exception caught" << std::endl; 
	}
}

// ==========================================================================
//- AxisPositionLocker::manage_position_locking
// ==========================================================================
void AxisPositionLocker::manage_position_locking ()
    throw (Tango::DevFailed)
{
  //- anything to do? can we do something?
  if (! this->m_axis || ! this->m_locking_enabled || IS_NAN(this->m_target_position))
  {
    DEBUG_STREAM << "AxisPositionLocker::nothing to do or can't do nothing [returning...]"
                 << std::endl;
    return;
  }

  //- motion request timeout expired?
  if (! this->m_tmo.expired())
  {
    //- a motion request has been emitted a less than <m_tmo.get_value()> ago
    //- do nothing in this case, cause the state should not be updated on ctrlbox
    //- side...
    DEBUG_STREAM << "AxisPositionLocker::tmo [returning...]"
                 << std::endl;
    return;
  }

  switch (this->m_axis->state())
  {
    case AXIS_STANDBY:
      break;
    case AXIS_MOVING:
      DEBUG_STREAM << "AxisPositionLocker::axis is <MOVING> [returning...]"
                   << std::endl;
      break;
    default:
      DEBUG_STREAM << "AxisPositionLocker::axis is in bad state [unlocking pos...]"
                   << std::endl;
      this->m_unlock_pending = true;
      break;
  }

  //- treat any pending <unlock request>...
  if (this->m_unlock_pending)
  {
    DEBUG_STREAM << "AxisPositionLocker::handling postponed <unlock request>"
                 << std::endl;
    //- mark locking as disabled and invalidate target position
    this->m_locking_enabled = false;
    this->m_target_position = _NAN_;
    //- stop <locked motion> on ctrlbox side (for the ucode, STOP means UNLOCK)
    try { 
      this->m_axis->stop_i();
    }
    catch (Tango::DevFailed& df) {
      ERROR_STREAM << "AxisPositionLocker::axis-"
                   << Axis::axis_id_to_string(this->m_axis_id)
                   << "::failed to unlock axis position on ctrlbox side [stop cmd failed]"
                   << std::endl;
      ERROR_STREAM << df
                   << std::endl;
    }
    catch (...) {
      ERROR_STREAM << "AxisPositionLocker::axis-"
                   << Axis::axis_id_to_string(this->m_axis_id)
                   << "::failed to unlock axis position on ctrlbox side [stop cmd failed]"
                   << std::endl;
    }
    //- unlock request handled...
    this->m_unlock_pending = false;
    DEBUG_STREAM << "AxisPositionLocker::postponed <unlock request> treated successfully"
                 << std::endl;
    //- skip remaining code
    return;
  }

  //- get current position 
  double current_position = this->m_axis->abs_pos();

  //- push current pos in averaging circular buffer
  this->m_locked_pos_averaging_buffer.push(current_position);

  //- compute average position
  const OrderedHistoryBuffer & pos_data = this->m_locked_pos_averaging_buffer.ordered_data();
  size_t pos_data_size = pos_data.length();
  double pos_data_sum = 0.;
  for (size_t i = 0; i < pos_data_size; i++)
    pos_data_sum += pos_data[i];
  double average_position = pos_data_sum / static_cast<double>(pos_data_size);
    
  //- compare <average_position> to <locked_position>
  double derivation = ::fabs(m_target_position - average_position);
    
  //- log...
  DEBUG_STREAM << "AxisPositionLocker::checking <locked position>" << std::endl;

  //- <pos_derivation> over specified threshold?
  if (derivation >= this->m_position_constraint)
  {
    DEBUG_STREAM << "AxisPositionLocker::axis-"
                 << Axis::axis_id_to_string(this->m_axis_id)
                 << "::position derivation over threshold [cp: "
                 << current_position
                 << " - lp: "
                 << m_target_position
                 << " - dp:"
                 << derivation
                 << "]"
                 << std::endl;
    try
    {
      //- apply correction...
      this->m_axis->lock_pos_i(this->m_target_position);
      
      //- get correction timestamp...
      yat::Timestamp ts;
      _GET_TIME(ts);
      double now = static_cast<double>(ts.tv_sec) + 1.e-6 * static_cast<double>(ts.tv_usec);

      DEBUG_STREAM << "AxisPositionLocker::axis-"
                   << Axis::axis_id_to_string(this->m_axis_id)
                   << "::position corrected to "
                   << this->m_target_position
                   << std::endl;

      //- push correction info into the history...
      {
        yat::MutexLock guard(this->m_axis->m_locked_pos_history_mutex);
        this->m_axis->m_locked_pos_history_pos.push(this->m_target_position);
        this->m_axis->m_locked_pos_history_tms.push(now);
        this->m_axis->m_locked_pos_history_drv.push(derivation);
      }
    }
    catch (Tango::DevFailed& df)
    {
      ERROR_STREAM << "AxisPositionLocker::<Axis::lock_pos_i> failed [Tango error]" << std::endl;
      ERROR_STREAM << df << std::endl;
    }
    catch (...)
    {
      ERROR_STREAM << "AxisPositionLocker::<Axis::lock_pos_i> failed [unknown error]" << std::endl;
    }
  }
  else
  {
    DEBUG_STREAM << "AxisPositionLocker::<locked position> checked [cur.pos: " 
                 << current_position 
                 << " - av.pos: "
                 << average_position
                 << " - tg-pos: "
                 << this->m_target_position
                 << "]"
                 << std::endl;
  }
}

// ============================================================================
// AXIS IDENTIFIERS (a la Galil)
// ============================================================================
#if defined(DMC_4143)
const char Axis::axis_letter[MAX_AXIS] = {'A','B','C','D'};
#else
const char Axis::axis_letter[MAX_AXIS] = {'A','B','C','D','E','F','G','H'};
#endif
//-----------------------------------------------------------------------------
const double Axis::servo_motor_ids[kNUM_SERVO_ID] = {-1.0, 1.0};
//-----------------------------------------------------------------------------
const double Axis::stepper_motor_ids[kNUM_STEPPER_ID] = {-2.5, -2.0, 2.0, 2.5};
//-----------------------------------------------------------------------------

// ============================================================================
// Axis::Config::Config
// ============================================================================
Axis::Config::Config ()
 : id (MIN_AXIS),
   motor_type (Axis::MOT_UNKNOWN),
   encoder_type (Axis::ENC_NONE),
   enable_maintenance_mode (false),
   enable_error_correction (false),
   allow_remote_cbox (false),
   encoder_ratio (1.),
   motor_ratio (1.),
   init_type (0),
   init_pos (0.),
   init_velocity (0.),
   duty_cycle_enabled (false),
   percent (kDEFAULT_PERCENT),
   retry (kDEFAULT_RETRY),
   settling_time (kDEFAULT_SETTLING_TIME),
   position_locking_constraint(1000),
   position_locking_checking_period_secs(1),
   position_locking_history_depth(128),
   position_locking_avaraging_window_width(10),
   host_device (0)
{
  //- noop ctor
}

// ============================================================================
// Axis::Config::operator=
// ============================================================================
void Axis::Config::operator= (const Config& _src)
{
  this->id                      = _src.id;
  this->motor_type              = _src.motor_type;
  this->encoder_type            = _src.encoder_type;
  this->enable_maintenance_mode = _src.enable_maintenance_mode;
  this->enable_error_correction = _src.enable_error_correction;
  this->allow_remote_cbox       = _src.allow_remote_cbox;
  this->encoder_ratio           = _src.encoder_ratio;
  this->motor_ratio             = _src.motor_ratio;
  this->init_type               = _src.init_type;
  this->init_pos                = _src.init_pos;
  this->init_velocity           = _src.init_velocity;
  this->duty_cycle_enabled      = _src.duty_cycle_enabled;
  this->percent                 = _src.percent;
  this->retry                   = _src.retry;
  this->settling_time           = _src.settling_time;

  this->position_locking_constraint             = _src.position_locking_constraint;
  this->position_locking_checking_period_secs   = _src.position_locking_checking_period_secs;
  this->position_locking_history_depth          = _src.position_locking_history_depth;
  this->position_locking_avaraging_window_width = _src.position_locking_avaraging_window_width;

  this->host_device = _src.host_device;

  //- be sure the <encoder_ratio> is valid
  if (this->encoder_ratio == 0.) 
    this->encoder_ratio = 1.0;

  //- be sure the <motor_ratio> is valid
  if (this->motor_ratio == 0.) 
    this->motor_ratio = 1.0;
}

// ============================================================================
// Axis::GearingConfig::GearingConfig
// ============================================================================
Axis::GearingConfig::GearingConfig ()
 : mode (Axis::NO_GEARING_AXIS), 
   associated_axis(MIN_AXIS), 
   ms_ratio(1.0),
   gearing_enabled(false)
{
  //- noop ctor
}

// ============================================================================
// Axis::GearingConfig::operator=
// ============================================================================
void Axis::GearingConfig::operator= (const GearingConfig& _src)
{
  this->mode = _src.mode;
  this->associated_axis = _src.associated_axis;
  this->ms_ratio = _src.ms_ratio;
  this->gearing_enabled = _src.gearing_enabled;
}

// ============================================================================
// Axis::Axis
// ============================================================================
Axis::Axis ()
 : yat4tango::TangoLogAdapter (0),
   m_axis_pos_locker(0),
   m_cfg (), 
   m_state_man (),
   m_offset (0.),
   m_ya_defined (false),
   m_yb_defined (false),
   m_yc_defined (false),
   m_gearing_cfg ()
{
	YAT_TRACE("galil::Axis::Axis");
}

// ============================================================================
// Axis::~Axis
// ============================================================================
Axis::~Axis (void)
{
  YAT_TRACE("galil::Axis::~Axis");

  if (this->m_axis_pos_locker)
  {
    this->m_axis_pos_locker->exit();
    this->m_axis_pos_locker = 0;
  }
}

// ============================================================================
// Axis::configure
// ============================================================================
void Axis::configure (const Config& _cfg) throw (Tango::DevFailed)
{
	YAT_TRACE("galil::Axis::configure");

  this->m_cfg = _cfg;

  //- stop any existing <AxisPositionLocker>
  if (this->m_axis_pos_locker)
  {
    this->m_axis_pos_locker->exit();
    this->m_axis_pos_locker = 0;
  }
  //- instanciate the <AxisPositionLocker>
  this->m_axis_pos_locker = new AxisPositionLocker(this,
                                                   this->m_cfg.position_locking_constraint,
                                                   this->m_cfg.position_locking_checking_period_secs,
                                                   this->m_cfg.position_locking_avaraging_window_width,
                                                   this->m_cfg.host_device);
  //- spawn the <AxisPositionLocker>
  this->m_axis_pos_locker->go();

  {
    yat::MutexLock guard(this->m_locked_pos_history_mutex);

    this->m_locked_pos_history.resize(3, _cfg.position_locking_history_depth);
    this->m_locked_pos_history.fill(_NAN_);

    this->m_locked_pos_history_pos.capacity(_cfg.position_locking_history_depth);
    this->m_locked_pos_history_pos.fill(_NAN_);

    this->m_locked_pos_history_tms.capacity(_cfg.position_locking_history_depth);
    this->m_locked_pos_history_tms.fill(_NAN_);

    this->m_locked_pos_history_drv.capacity(_cfg.position_locking_history_depth);
    this->m_locked_pos_history_drv.fill(_NAN_);
  }

  this->host(this->m_cfg.host_device);

  AxisStateManager::AxisConfig acfg;
  acfg.id = this->m_cfg.id;
  acfg.has_inverted_lsw = (this->m_cfg.encoder_ratio < 0) ? true : false;
  acfg.duty_cycle_enabled = this->m_cfg.duty_cycle_enabled;
  acfg.host_device = this->m_cfg.host_device;

  this->m_state_man.attached_axis(acfg);

  //- get ucode axis state
  UCodeAxisState ucas;
  HW_STATUS_PROXY->ucode_axis_state(this->m_cfg.id, ucas);

  //- set associated motor type
  this->m_cfg.motor_type = ucas.mot_type()
                         ? Axis::MOT_SERVO_OR_PIEZO
                         : Axis::MOT_STEPPER;

  yat::OSStream cmd;
  //- get YA, YB & YC if motor is Axis::MOT_STEPPER & encoder != Axis::ENC_NONE
  if (
       this->m_cfg.motor_type == Axis::MOT_STEPPER
         &&
       this->m_cfg.encoder_type != galil::Axis::ENC_NONE
     )
  {
    //- get motor ratio: get YA
    cmd.str("");
    cmd << "MG_YA" 
        << Axis::axis_letter[this->m_cfg.id]  
        << "\r";
    std::string ya_str;
    SHARED_HW_IO->inout(cmd.str(), ya_str);
    double ya = 0;
    try
    {
      ya = yat::XString<double>::to_num(ya_str);;
    }
    catch (yat::Exception & ye)
    {
      _YAT_TO_TANGO_EXCEPTION(ye, te);
      RETHROW_DEVFAILED(te,
                        _CPTC("SOFTWARE_ERROR"),
                        _CPTC("yat::Exception caught while trying to extract <Ya> from std::tring"),
                        _CPTC("Axis::configure"));
    }
    catch (...)
    {
      THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                      _CPTC("unknown exception caught while trying to extract <Ya>from std::tring"),
                      _CPTC("Axis::configure"));
    }
    if (ya != 0.)
      m_ya_defined = true;

    //- get motor ratio: get YB
    cmd.str("");
    cmd << "MG_YB" 
        << Axis::axis_letter[this->m_cfg.id]  
        << "\r";
    std::string yb_str;
    SHARED_HW_IO->inout(cmd.str(), yb_str); 
    double yb = 0;
    try
    {
      yb = yat::XString<double>::to_num(yb_str);
    }
    catch (yat::Exception & ye)
    {
      _YAT_TO_TANGO_EXCEPTION(ye, te);
      RETHROW_DEVFAILED(te,
                        _CPTC("SOFTWARE_ERROR"),
                        _CPTC("yat::Exception caught while trying to extract <Yb> from std::tring"),
                        _CPTC("Axis::configure"));
    }
    catch (...)
    {
      THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                      _CPTC("unknown exception caught while trying to extract <Yb> from std::tring"),
                      _CPTC("Axis::configure"));
    }
    if (yb != 0.) 
      m_yb_defined = true;

    //- get motor ratio: get YC
    cmd.str("");
    cmd << "MG_YC" 
        << Axis::axis_letter[this->m_cfg.id]  
        << "\r";
    std::string yc_str;
    SHARED_HW_IO->inout(cmd.str(), yc_str);
    double yc = 0;
    try
    {
      yc = yat::XString<double>::to_num(yc_str);
    }
    catch (yat::Exception & ye)
    {
      _YAT_TO_TANGO_EXCEPTION(ye, te);
      RETHROW_DEVFAILED(te,
                        _CPTC("SOFTWARE_ERROR"),
                        _CPTC("yat::Exception caught while trying to extract <Yc> from std::tring"),
                        _CPTC("Axis::configure"));
    }
    catch (...)
    {
      THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                      _CPTC("unknown exception caught while trying to extract <Yc> from std::tring"),
                      _CPTC("Axis::configure"));
    }
    if (yc != 0.) 
      m_yc_defined = true;

    //- compute motor ratio
    if (m_ya_defined && m_yb_defined && m_yc_defined) 
      this->m_cfg.motor_ratio = yc / (ya * yb);
    else
      this->m_cfg.motor_ratio = 1.;
  }
  //- no stepper motor or no encoder
  else
  {
    this->m_cfg.motor_ratio = 1.;
  }

  //- write "percent"
  cmd.str("");
  cmd << "Percent["
      << static_cast<int>(this->m_cfg.id) 
      << "]="
      << this->m_cfg.percent
      << "\r";
  SHARED_HW_IO->in(cmd.str(), IOCheckError);

  //- write "retry"
  cmd.str("");
  cmd << "NbRetry["
      << static_cast<int>(this->m_cfg.id)
      << "]="
      << this->m_cfg.retry
      << "\r";
  SHARED_HW_IO->in(cmd.str(), IOCheckError);

  //- configure encoder type
	unsigned int cpmp = static_cast<unsigned int>(this->m_cfg.encoder_type);
  //- configure maintenance mode
	cpmp = this->m_cfg.enable_maintenance_mode ? cpmp + 4 : cpmp;
  //- configure error correction mode
	cpmp = this->m_cfg.enable_error_correction ? cpmp + 16 : cpmp;
  //- Allow RemoteCbox remote command
	cpmp = this->m_cfg.allow_remote_cbox ? cpmp + 8 : cpmp;
  //- write CpMp[x].bit 15 to indicate that configuration values have been written on HW
	cpmp += 0x8000;
	//- write on HW
	cmd.str("");
  cmd << "CpMp[" 
      << static_cast<int>(this->m_cfg.id) 
      << "]="
      << cpmp
      << "\r";
  this->exec_command(cmd.str());

  //- write settling time 
  cmd.str("");
  cmd << "StabTime[" 
      << static_cast<int>(this->m_cfg.id) 
      << "]="
      << this->m_cfg.settling_time
      << "\r";
  this->exec_command(cmd.str());

  //- retreive the SpMini in the CB
  cmd.str("");
  cmd << "SpMini=?" 
      << "\r";
  std::string sp_mini_str;
  this->m_sp_mini = kDEFAULT_SP_MINI;
  SHARED_HW_IO->inout(cmd.str(), sp_mini_str);
  try
  {
    m_sp_mini = yat::XString<double>::to_num(sp_mini_str);
  }
  catch (yat::Exception & ye)
  {
    _YAT_TO_TANGO_EXCEPTION(ye, te);
    RETHROW_DEVFAILED(te,
                      _CPTC("SOFTWARE_ERROR"),
                      _CPTC("yat::Exception caught while trying to extract <SpMini> from std::string"),
                      _CPTC("Axis::configure"));
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught while trying to extract <SpMini>from std::string"),
                    _CPTC("Axis::configure"));
  }


/* ATTENTE DE TESTS DE LA PART DE ECA 
//- set the InitTyp value (for specific microcode purpose)
  //- specify the positionning strategy
  cmd.str("");
  cmd << "IniTyp[" 
      << static_cast<int>(this->m_cfg.id) 
      << "]="
			<< this->m_cfg.init_type
      << "\r" ;
  this->exec_command(cmd.str());*/
}

// ============================================================================
// Axis::configure_gearing
// ============================================================================
void Axis::configure_gearing (const GearingConfig& _gcfg)
    throw (Tango::DevFailed)
{
  //- copy config localy
  this->m_gearing_cfg = _gcfg;

  //- enable gearing if required
  if (this->m_gearing_cfg.gearing_enabled)
    this->enable_gearing();
}

// ============================================================================
// Axis::enable_gearing
// ============================================================================
void Axis::enable_gearing (void)
    throw (Tango::DevFailed)
{
  //- if SLAVE_AXIS then tell the �code what the config is
  if (this->m_gearing_cfg.mode == SLAVE_AXIS)
  {
    //- set the actual master identifier
    int master_id = this->m_gearing_cfg.associated_axis + kORIGIN_OF_GEARING_COMMANDED_REF_POS;
    //- set this axis' master Axis (GrAxis �code var).
    yat::OSStream master_axis_cmd;
    master_axis_cmd << "GrAxis[" 
                    << static_cast<int>(this->m_cfg.id) 
                    << "]="
                    << master_id
                    << "\r";
    this->exec_command(master_axis_cmd.str());

    //- set the master/slave ratio (GrRatio �code var).
    yat::OSStream ms_ratio_cmd;
    ms_ratio_cmd << "GrRatio[" 
                 << static_cast<int>(this->m_cfg.id) 
                 << "]="
                 << std::fixed
                 << std::setprecision(4)
                 << this->m_gearing_cfg.ms_ratio
                 << "\r";
    this->exec_command(ms_ratio_cmd.str());

    //- exec the Gearing Update cmd
    yat::OSStream update_gearing_cmd;
    update_gearing_cmd << "Cmd[" 
                       << static_cast<int>(this->m_cfg.id) 
                       << "]=$80\r";
    this->exec_command(update_gearing_cmd.str());

    //- be sure the cmd is processed at ucode level
    this->m_state_man.wait_current_command_processed(kDEFAULT_WAIT_CMD_PROC_TMO);
  }
}

// ============================================================================
// Axis::disable_gearing
// ============================================================================
void Axis::disable_gearing (void)
    throw (Tango::DevFailed)
{
  //- if SLAVE_AXIS then tell the �code what the config is
  if (this->m_gearing_cfg.mode == SLAVE_AXIS)
  {
    //- reset the master Axis into the GrAxis var.
    yat::OSStream master_axis_cmd;
    master_axis_cmd << "GrAxis[" 
                    << static_cast<int>(this->m_cfg.id) 
                    << "]="
                    << kNO_MASTER_AXIS
                    << "\r";
    this->exec_command(master_axis_cmd.str());

    //- exec the Gearing Update cmd
    yat::OSStream update_gearing_cmd;
    update_gearing_cmd << "Cmd[" 
                       << static_cast<int>(this->m_cfg.id) 
                       << "]=$80\r";
    this->exec_command(update_gearing_cmd.str());

    //- be sure the cmd is processed at ucode level
    this->m_state_man.wait_current_command_processed(kDEFAULT_WAIT_CMD_PROC_TMO);
  }
}

// ============================================================================
// Axis::gearing_enabled
// ============================================================================
bool Axis::gearing_enabled (void)
    throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::gearing_enabled");

  return this->m_state_man.extended_axis_status().ucas.axis_slave() 
      || this->m_state_man.extended_axis_status().ucas.axis_master();
}

// ============================================================================
// Axis::sign_of_gearing_ratio
// ============================================================================
void Axis::sign_of_gearing_ratio (int _sign_adapter)
  throw (Tango::DevFailed)
{
  //- store sign of <sign_adapter> parameter
  static int last_sign = 0;

  //- set the master/slave ratio (GrRatio �code var).
  yat::OSStream ms_ratio_cmd;
  ms_ratio_cmd << "GrRatio[" 
               << static_cast<int>(this->m_cfg.id) 
               << "]="
               << std::fixed
               << std::setprecision(4)
               << static_cast<double>(_sign_adapter) * this->m_gearing_cfg.ms_ratio
               << "\r";
  this->exec_command(ms_ratio_cmd.str());

  //- workaround : the if below prevented the sign write in 1 case : 
  //- after device startup and slit is in independant mode 
  //- and slit position is the first attribute changed
  //  if (last_sign == 0 || last_sign != Axis::signof(_sign_adapter))
  {
    //- the gearing ration is possibly taken in account only on gearing update?
    yat::OSStream update_gearing_cmd;
    update_gearing_cmd << "Cmd[" 
                       << static_cast<int>(this->m_cfg.id) 
                       << "]=$80\r";
    this->exec_command(update_gearing_cmd.str());

    //- store sign of sign_adapter for next call
    last_sign = Axis::signof(_sign_adapter);
  }

  //- be sure the cmd is processed at ucode level
  this->m_state_man.wait_current_command_processed(kDEFAULT_WAIT_CMD_PROC_TMO);
}

// ============================================================================
// Axis::on
// ============================================================================
void Axis::on () throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::on");

  //- avoid locking (minor) side effect: on if motor off
  const ExtendedAxisStatus & eas = this->extended_axis_status(false);
  if (! eas.fas.motor_off() || this->m_cfg.duty_cycle_enabled)
    return;

  //- is the command allowed?
  std::string why_not;
  if (! this->m_state_man.command_allowed(galil::MOTOR_ON_CMD, &why_not))
  {
    THROW_DEVFAILED(_CPTC("BAD_STATE"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::Axis::on"));
  }

  //- build Cmd[] command
  yat::OSStream cmd;
  cmd << "Cmd[" 
      << static_cast<int>(this->m_cfg.id) 
      << "]=$20\r";

  this->exec_command(cmd.str());

  //- be sure the cmd is processed at ucode level
  this->m_state_man.wait_current_command_processed(kDEFAULT_WAIT_CMD_PROC_TMO);
}

// ============================================================================
// Axis::off
// ============================================================================
void Axis::off () throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::off");

  //- is the command allowed?
  std::string why_not;
  if (! this->m_state_man.command_allowed(galil::MOTOR_OFF_CMD, &why_not))
  {
    THROW_DEVFAILED(_CPTC("BAD_STATE"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::Axis::off"));
  }

  //- build Cmd[] command
  yat::OSStream cmd;
  cmd << "Cmd[" 
      << static_cast<int>(this->m_cfg.id) 
      << "]=$40\r";

  this->exec_command(cmd.str());

  //- be sure the cmd is processed at ucode level
  this->m_state_man.wait_current_command_processed(kDEFAULT_WAIT_CMD_PROC_TMO);
}

// ============================================================================
// Axis::forward
// ============================================================================
void Axis::forward () throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::forward");

  //- jog+ requested, have to notify our <AxisPositionLocker>
  //---------------------------------------------------------------------------
  //- we have to do this before invoking the axis state manager cause the 
  //- ctrlbox might be correcting the "locked position" - in which case the 
  //- motion request could be rejected (command in progress) - we unlock the
  //- axis pos. if locking is enabled and if the axis is not already moving 
  if (this->m_axis_pos_locker && this->m_axis_pos_locker->locking_enabled())
  {
    if (this->state() != AXIS_MOVING)
    {
      this->m_axis_pos_locker->jog_requested();
    }
    else
    {
      THROW_DEVFAILED(_CPTC("BAD_STATE"),
                      _CPTC("<Forward> cmd rejected::axis is moving"),
                      _CPTC("galil::Axis::forward"));
    }
  }

  //- is the command allowed?
  std::string why_not;
  if (! this->m_state_man.command_allowed(galil::JOG_PLUS_CMD, &why_not))
  {
    THROW_DEVFAILED(_CPTC("BAD_STATE"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::Axis::forward"));
  }

  //- actual cmd is <ratio> dependent 
  std::string actual_cmd = "$2";
  if (kUSER_TO_HW_UNIT_RATIO < 0.)
    actual_cmd = "$4";

  //- build Cmd[] command
  yat::OSStream cmd;
  cmd << "Cmd[" 
      << static_cast<int>(this->m_cfg.id) 
      << "]="
      << actual_cmd
      << "\r";
  
  this->exec_command(cmd.str());

  //- be sure the cmd is processed at ucode level
  this->m_state_man.wait_current_command_processed(kDEFAULT_WAIT_CMD_PROC_TMO, true);
}

// ============================================================================
// Axis::backward
// ============================================================================
void Axis::backward () throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::backward");

  //- jog- requested, have to notify our <AxisPositionLocker>
  //---------------------------------------------------------------------------
  //- we have to do this before invoking the axis state manager cause the 
  //- ctrlbox might be correcting the "locked position" - in which case the 
  //- motion request could be rejected (command in progress) - we unlock the
  //- axis pos. if locking is enabled and if the axis is not already moving 
  if (this->m_axis_pos_locker && this->m_axis_pos_locker->locking_enabled())
  {
    if (this->state() != AXIS_MOVING)
    {
      this->m_axis_pos_locker->jog_requested();
    }
    else
    {
      THROW_DEVFAILED(_CPTC("BAD_STATE"),
                      _CPTC("<Backward> cmd rejected::axis is moving"),
                      _CPTC("galil::Axis::backward"));
    }
  }

  //- is the command allowed?
  std::string why_not;
  if (! this->m_state_man.command_allowed(galil::JOG_MINUS_CMD, &why_not))
  {
    THROW_DEVFAILED(_CPTC("BAD_STATE"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::Axis::backward"));
  }

  //- actual cmd is <ratio> dependent 
  std::string actual_cmd = "$4";
  if (kUSER_TO_HW_UNIT_RATIO < 0.)
    actual_cmd = "$2";

  //- build Cmd[] command
  yat::OSStream cmd;
  cmd << "Cmd[" 
      << static_cast<int>(this->m_cfg.id) 
      << "]="
      << actual_cmd
      << "\r";
  
  this->exec_command(cmd.str());
  
  //- be sure the cmd is processed at ucode level
  this->m_state_man.wait_current_command_processed(kDEFAULT_WAIT_CMD_PROC_TMO, true);
}

// ============================================================================
// Axis::abs_pos
// ============================================================================
void Axis::abs_pos (const double& _abs_pos) throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::abs_pos");

  //- motion requested, have to notify our <AxisPositionLocker>
  //---------------------------------------------------------------------------
  //- we have to do this before invoking the axis state manager cause the 
  //- ctrlbox might be correcting the "locked position" - in which case the 
  //- motion request could be rejected (command in progress) - we unlock the
  //- axis pos. if locking is enabled and if the axis is not already moving 
  if (this->m_axis_pos_locker && this->m_axis_pos_locker->locking_enabled())
  {
    if (this->state() != AXIS_MOVING)
    {
      this->m_axis_pos_locker->motion_requested(_abs_pos);
    }
    else
    {
      THROW_DEVFAILED(_CPTC("BAD_STATE"),
                      _CPTC("<Abs.Positionning> cmd rejected::axis is moving"),
                      _CPTC("galil::Axis::abs_pos"));
    }
  }

  //- is the command allowed?
  std::string why_not;
  if (! this->m_state_man.command_allowed(galil::ABS_POS_CMD, &why_not))
  {
    THROW_DEVFAILED(_CPTC("BAD_STATE"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::Axis::abs_pos"));
  }

  double mot_pos = USER_TO_HW_UNITS(_abs_pos - this->m_offset);

  YAT_LOG("Axis::abs_pos::sending axis to " << mot_pos);

  //- build Pos[] command
  yat::OSStream cmd;
  cmd << "Pos[" 
      << static_cast<int>(this->m_cfg.id) 
      << "]="
      << std::fixed
      << std::setprecision(4)
      << mot_pos
      << "\r";

  this->exec_command(cmd.str());

  //- build Cmd[] command
  cmd.str("");
  cmd << "Cmd[" 
      << static_cast<int>(this->m_cfg.id) 
      << "]=$8\r";
  
  this->exec_command(cmd.str());

  //- be sure the cmd is processed at ucode level
  this->m_state_man.wait_current_command_processed(kDEFAULT_WAIT_CMD_PROC_TMO, true);
}

// ============================================================================
// Axis::abs_pos
// ============================================================================
double Axis::abs_pos () throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::abs_pos");

  //- get current axis status
  const ExtendedAxisStatus & cas = this->m_state_man.extended_axis_status();
  
  double current_pos = 0.;

  //- get encoder value
  switch (this->m_cfg.encoder_type)
  {
    case ENC_NONE:
      YAT_LOG("galil::Axis::abs_pos::encoder type::ENC_NONE");
      current_pos = static_cast<double>(cas.fas.aux_pos());
      break;
    case ENC_INCREMENTAL:
    case ENC_ABSOLUTE:
      //YAT_LOG("galil::Axis::abs_pos::encoder type::ENC_INCREMENTAL or ENC_ABSOLUTE");
      current_pos = static_cast<double>(cas.fas.mot_pos());
      break;
  }

 // YAT_LOG("galil::Axis::abs_pos::encoder value::" << current_pos);

  //- convert to user unit
  current_pos *= this->m_cfg.encoder_ratio;
  current_pos += this->m_offset;

  //YAT_LOG("galil::Axis::abs_pos::pos in user unit::" << current_pos);

  return current_pos;
}

// ============================================================================
// Axis::lock_pos
// ============================================================================
void Axis::lock_pos (const double& _abs_pos) throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::lock_pos");

  //- can't lock an axis which is not in the <AXIS_STANDBY> state
  if (this->state() != AXIS_STANDBY)
  {
    THROW_DEVFAILED(_CPTC("BAD_STATE"),
                    _CPTC("can't lock an axis which is not in the <STANDBY> state"),
                    _CPTC("AxisPositionLocker::lock_position"));
  }

  //- check <AxisPositionLocker> instance
  if (! this->m_axis_pos_locker)
  {
    THROW_DEVFAILED(_CPTC("INTERNAL_ERROR"),
                    _CPTC("the <AxisPositionLocker> is not properly initialized"),
                    _CPTC("galil::Axis::lock_pos"));
  }

  DEBUG_STREAM << "Axis::lock_pos::locking pos @ " <<  _abs_pos << std::endl;

  //- exec <lock pos.> cmd
  this->lock_pos_i(_abs_pos);

  DEBUG_STREAM << "Axis::lock_pos::pos successfully locked @ " <<  _abs_pos << std::endl;

  //- timestamp...
  yat::Timestamp ts;
  _GET_TIME(ts);
  double now = static_cast<double>(ts.tv_sec) + 1.e-6 * static_cast<double>(ts.tv_usec);

  //- push position lock info into the history...
  {
    yat::MutexLock guard(this->m_locked_pos_history_mutex);
    this->m_locked_pos_history_pos.push(_abs_pos);
    this->m_locked_pos_history_tms.push(now);
    this->m_locked_pos_history_drv.push(0.);
  }

  //- ask our <AxisPositionLocker> to enable the locking mecanism
  this->m_axis_pos_locker->lock_position(_abs_pos);
}

// ============================================================================
// Axis::lock_pos_i
// ============================================================================
void Axis::lock_pos_i (const double& _abs_pos) throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::lock_pos_i");

  //- is the command allowed?
  std::string why_not;
  if (! this->m_state_man.command_allowed(galil::LOCK_POS_CMD, &why_not))
  {
    THROW_DEVFAILED(_CPTC("BAD_STATE"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::Axis::lock_pos_i"));
  }

  double mot_pos = USER_TO_HW_UNITS(_abs_pos - this->m_offset);

  YAT_LOG("Axis::lock_pos::locking axis position @ " << mot_pos);

  //- build Pos[] command
  yat::OSStream cmd;
  cmd << "Pos[" 
      << static_cast<int>(this->m_cfg.id) 
      << "]="
      << std::fixed
      << std::setprecision(4)
      << mot_pos
      << "\r";

  this->exec_command(cmd.str());

  //- build Cmd[] command
  cmd.str("");
  cmd << "Cmd[" 
      << static_cast<int>(this->m_cfg.id) 
      << "]=$100\r";
  
  this->exec_command(cmd.str());

  //- be sure the cmd is processed at ucode level
  this->m_state_man.wait_current_command_processed(kDEFAULT_WAIT_CMD_PROC_TMO, true);
}

// ============================================================================
// Axis::unlock_pos
// ============================================================================
void Axis::unlock_pos () throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::unlock_pos");

    //- check <AxisPositionLocker> instance
  if (! this->m_axis_pos_locker)
  {
    THROW_DEVFAILED(_CPTC("INTERNAL_ERROR"),
                    _CPTC("the <AxisPositionLocker> is not properly initialized"),
                    _CPTC("galil::Axis::unlock_pos"));
  }

  //- ask our <AxisPositionLocker> to disable the locking mecanism
  this->m_axis_pos_locker->unlock_position();
}

// ============================================================================
// Axis::stop
// ============================================================================
void Axis::stop () throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::stop");

  //- notify our <AxisPositionLocker>
  if (this->m_axis_pos_locker && this->m_axis_pos_locker->locking_enabled())
    this->m_axis_pos_locker->stop_requested();

  //- is the command allowed?
  std::string why_not;
  if (! this->m_state_man.command_allowed(galil::STOP_CMD, &why_not))
  {
    THROW_DEVFAILED(_CPTC("BAD_STATE"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::Axis::stop"));
  }

  //- exec <STOP> command
  this->stop_i();
}

// ============================================================================
// Axis::stop_i
// ============================================================================
void Axis::stop_i () throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::stop_i");

  //- build Cmd[] command
  yat::OSStream cmd;
  cmd << "Cmd[" 
      << static_cast<int>(this->m_cfg.id) 
      << "]=$1\r";
  
  this->exec_command(cmd.str());

  //- be sure the cmd is processed at ucode level
  this->m_state_man.wait_current_command_processed(kDEFAULT_WAIT_CMD_PROC_TMO, true);

  //- be sure the motor is stopped
  this->m_state_man.wait_motor_stopped(4 * kDEFAULT_WAIT_CMD_PROC_TMO);
}

// ============================================================================
// Axis::init_reference_position
// ============================================================================
void Axis::init_reference_position () throw (Tango::DevFailed)
{
 //- no init. ref. pos. for absolute encoders 
 if (m_cfg.encoder_type == galil::Axis::ENC_ABSOLUTE)
 {
    THROW_DEVFAILED(_CPTC("DATA_OUT_OF_RANGE"),
                    _CPTC("incompatible encoder type - no init. ref. pos. for absolute encoders"),
                    _CPTC("galil::Axis::init_reference_position"));
 }

 //- make sure a init. ref. pos. strategy is defined in the InitType property 
 if (this->m_cfg.init_type == 0)
 {
    THROW_DEVFAILED(_CPTC("DATA_OUT_OF_RANGE"),
                    _CPTC("no reference position strategy defined in InitType property [use DefinePosition instead]"),
                    _CPTC("galil::Axis::init_reference_position"));
 }

 this->init_ref_pos_i (this->m_cfg.init_pos, this->m_cfg.init_type);
}

// ============================================================================
// Axis::define_position 
// ============================================================================
void Axis::define_position (double _pos) throw (Tango::DevFailed)
{
	//- force init_type to 0 : DP
	this->init_ref_pos_i (_pos, 0);
}

// ============================================================================
// Axis::init_ref_pos_i
// ============================================================================
void Axis::init_ref_pos_i (double _init_pos, unsigned long _init_type)
 throw (Tango::DevFailed)
{
  //- be sure the REF_POS_CMD command is currently allowed
  std::string why_not;
  if (! this->m_state_man.command_allowed(galil::REF_POS_CMD, &why_not))
  {
    THROW_DEVFAILED(_CPTC("BAD_STATE"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::Axis::init_ref_pos_i"));
  }
  
  //- compute the init pos in encoder unit
  double actual_init_pos = 0.;

  if (_init_type == 0)
  {
    INFO_STREAM << "Axis::init_ref_pos_i DefinePosition call -> use offset to compute hardware position" << std::endl;
    actual_init_pos = USER_TO_HW_UNITS(_init_pos - this->m_offset);
  }
  else
  {
    //- bug 16437 : InitReferencePosition must be independant from offset
    INFO_STREAM << "Axis::init_ref_pos_i InitializeReferencePosition call -> use hardware position regardless offset" << std::endl;
    actual_init_pos = USER_TO_HW_UNITS(_init_pos);
  }

	//- specify the initial position to the controller
  yat::OSStream cmd;
  cmd << "IniPos[" 
      << static_cast<int>(this->m_cfg.id) 
      << "]="
      << std::fixed
      << std::setprecision(4)
			<< actual_init_pos 
      << "\r" ;
  this->exec_command(cmd.str());

	//- specify the initialization velocity to the controller
  cmd.str("");
  cmd << "IniSpeed[" 
      << static_cast<int>(this->m_cfg.id) 
      << "]="
      << std::fixed
      << std::setprecision(4)
			<< ::fabs(USER_TO_HW_UNITS(this->m_cfg.init_velocity))
      << "\r" ;
  this->exec_command(cmd.str());
  
  //- specify the positionning strategy
  cmd.str("");
  cmd << "IniTyp[" 
      << static_cast<int>(this->m_cfg.id) 
      << "]="
			<< _init_type
      << "\r" ;
  this->exec_command(cmd.str());

	//- exec REF_POS_CMD 
  cmd.str("");
  cmd << "Cmd[" 
      << static_cast<int>(this->m_cfg.id) 
      << "]=$10\r" ;
  this->exec_command(cmd.str());

  //- be sure the cmd is processed at ucode level
  this->m_state_man.wait_current_command_processed(kDEFAULT_WAIT_CMD_PROC_TMO);
}

// ============================================================================
// Axis::acceleration
// ============================================================================
void Axis::acceleration (const double& _val) throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::acceleration");

  //- is the command allowed?
  std::string why_not;
  if (! this->m_state_man.command_allowed(galil::ACCEL_WRITE, &why_not))
  {
    THROW_DEVFAILED(_CPTC("BAD_STATE"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::Axis::acceleration"));
  }

  if (_val < kMIN_ACC || _val > kMAX_ACC)
  {
    yat::OSStream err_desc;
    err_desc << "the specified acceleration is out of range - must be in [" 
             << kMIN_ACC
             << ","
             << kMAX_ACC
             << "]";
    THROW_DEVFAILED(_CPTC("INVALID_ARGUMENT"),
                    _CPTC(err_desc.str().c_str()),
                    _CPTC("galil::Axis::acceleration"));
  }

  YAT_LOG("Axis::acceleration::setting axis acceleration to " << _val);

  //- build command (convert to encoder unit)
  yat::OSStream cmd;
  cmd << "AC" 
      << Axis::axis_letter[this->m_cfg.id] 
      << "=" 
      << std::fixed
      << std::setprecision(4)
      << ::fabs(USER_TO_HW_UNITS(_val))
      << "\r";

  this->exec_command(cmd.str());
}

// ============================================================================
// Axis::acceleration
// ============================================================================
double Axis::acceleration () throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::acceleration");

  //- build command
  yat::OSStream cmd;
  cmd << "AC" 
      << Axis::axis_letter[this->m_cfg.id]  
      << "=?\r";

  //- Axis::get_parameter will convert from encoder to user unit
  return ::fabs(this->get_parameter(cmd.str()));
}

// ============================================================================
// Axis::deceleration
// ============================================================================
void Axis::deceleration (const double& _val) throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::deceleration");

  //- is the command allowed?
  std::string why_not;
  if (! this->m_state_man.command_allowed(galil::DECEL_WRITE, &why_not))
  {
    THROW_DEVFAILED(_CPTC("BAD_STATE"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::Axis::deceleration"));
  }

  if (_val < kMIN_DEC || _val > kMAX_DEC)
  {
    yat::OSStream err_desc;
    err_desc << "the specified deceleration is out of range - must be in [" 
             << kMIN_DEC 
             << ","
             << kMAX_DEC
             << "]";
    THROW_DEVFAILED(_CPTC("INVALID_ARGUMENT"),
                    _CPTC(err_desc.str().c_str()),
                    _CPTC("galil::Axis::deceleration"));
  }

  YAT_LOG("Axis::deceleration::setting axis deceleration to " << _val);

  //- build command (convert to encoder unit)
  yat::OSStream cmd;
  cmd << "DC" 
      << Axis::axis_letter[this->m_cfg.id] 
      << "=" 
      << std::fixed
      << std::setprecision(4)
      << ::fabs(USER_TO_HW_UNITS(_val))
      << "\r";

  this->exec_command(cmd.str());
}

// ============================================================================
// Axis::deceleration
// ============================================================================
double Axis::deceleration () throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::deceleration");

  //- build command
  yat::OSStream cmd;
  cmd << "DC" 
      << Axis::axis_letter[this->m_cfg.id] 
      << "=?\r";

  //- Axis::get_parameter will convert from encoder to user unit
  return ::fabs(this->get_parameter(cmd.str()));
}

// ============================================================================
// Axis::velocity
// ============================================================================
void Axis::velocity (const double& _val) throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::velocity");
	
  //- is the command allowed?
  std::string why_not;
  if (! this->m_state_man.command_allowed(galil::VELOCITY_WRITE, &why_not))
  {
    THROW_DEVFAILED(_CPTC("BAD_STATE"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::Axis::velocity"));
  }
  
  
  //- workaround for a GALIL firmware bug : do not modify SP when axis is gearing slave
//  if (m_state_man.extended_axis_status().ucas.axis_slave())
//		{
//			THROW_DEVFAILED(_CPTC("NOT_ALLOWED"),
//											_CPTC("command not allowed on slit-slave axis"),
//											_CPTC("galil::Axis::velocity"));
//		}   

  if (_val < kMIN_SPD || _val > kMAX_SPD || ::fabs(USER_TO_HW_UNITS(_val)) < this->m_sp_mini)
  {
    yat::OSStream err_desc;
    err_desc << "the specified velocity is out of range - must be in [" 
             << kMIN_SPD 
             << ","
             << kMAX_SPD
             << "] and over "
             << HW_TO_USER_UNITS (this->m_sp_mini);
    THROW_DEVFAILED(_CPTC("INVALID_ARGUMENT"),
                    _CPTC(err_desc.str().c_str()),
                    _CPTC("galil::Axis::velocity"));
  }

  YAT_LOG("Axis::velocity::setting axis velocity to " << _val);

  //- build command (convert to encoder unit)
  yat::OSStream cmd;
  cmd << "SP" 
      << Axis::axis_letter[this->m_cfg.id] 
      << "=" 
      << std::fixed
      << std::setprecision(4)
      << ::fabs(USER_TO_HW_UNITS(_val))
      << "\r";

  this->exec_command(cmd.str());
}

// ============================================================================
// Axis::velocity
// ============================================================================
double Axis::velocity () throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::velocity");

  //- build command
  yat::OSStream cmd;
  cmd << "SP" 
      << Axis::axis_letter[this->m_cfg.id] 
      << "=?\r";

  //- Axis::get_parameter will convert from encoder to user unit
  return ::fabs(this->get_parameter(cmd.str()));
}

// ============================================================================
// Axis::accuracy
// ============================================================================
void Axis::accuracy (const double& _val) throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::accuracy");

  //- is the command allowed?
  std::string why_not;
  if (! this->m_state_man.command_allowed(galil::DEADBAND_WRITE, &why_not))
  {
    THROW_DEVFAILED(_CPTC("BAD_STATE"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::Axis::accuracy"));
  }

  if (_val < kMIN_DBW || _val > kMAX_DBW)
  {
    yat::OSStream err_desc;
    err_desc << "the specified accuracy is out of range - must be in [" 
             << kMIN_DBW 
             << ","
             << kMAX_DBW
             << "]";
    THROW_DEVFAILED(_CPTC("INVALID_ARGUMENT"),
                    _CPTC(err_desc.str().c_str()),
                    _CPTC("galil::Axis::accuracy"));
  }

  YAT_LOG("Axis::accuracy::setting axis accuracy to " << _val);

  //- build command (convert to encoder unit)
  yat::OSStream cmd;
  cmd << "Dbw[" 
      << static_cast<int>(this->m_cfg.id) 
      << "]=" 
      << std::fixed
      << std::setprecision(4)
      << ::fabs(USER_TO_HW_UNITS(_val))
      << "\r";

  this->exec_command(cmd.str());;
}

// ============================================================================
// Axis::accuracy
// ============================================================================
double Axis::accuracy () throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::accuracy");

  //- build command
  yat::OSStream cmd;
  cmd << "Dbw[" 
      << static_cast<int>(this->m_cfg.id) 
      << "]=?\r";

  //- Axis::get_parameter will convert from encoder to user unit
  return ::fabs(this->get_parameter(cmd.str()));
}

// ============================================================================
// Axis::backlash
// ============================================================================
void Axis::backlash (const double& _val) throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::backlash");

  //- is the command allowed?
  std::string why_not;
  if (! this->m_state_man.command_allowed(galil::BACKLASH_WRITE, &why_not))
  {
    THROW_DEVFAILED(_CPTC("BAD_STATE"),
                    _CPTC(why_not.c_str()),
                    _CPTC("galil::Axis::backlash"));
  }

  double bck_min = (kMIN_BCK <= kMAX_BCK) ? kMIN_BCK : kMAX_BCK;
  double bck_max = (kMIN_BCK <= kMAX_BCK) ? kMAX_BCK : kMIN_BCK;

  if (_val < bck_min || _val > bck_max)
  {
    yat::OSStream err_desc;
    err_desc << "the specified backlash is out of range - must be in [" 
             << bck_min
             << ","
             << bck_max
             << "]";
    THROW_DEVFAILED(_CPTC("INVALID_ARGUMENT"),
                    _CPTC(err_desc.str().c_str()),
                    _CPTC("galil::Axis::backlash"));
  }

  YAT_LOG("Axis::backlash::setting axis backlash to " << _val);

  //- actual backlash
  double actual_backlash = USER_TO_HW_UNITS(_val);
  //- if (kUSER_TO_HW_UNIT_RATIO < 0.)
  //-  actual_backlash *= -1.;

  //- build command (convert to encoder unit)
  yat::OSStream cmd;
  cmd << "Bck[" 
      << static_cast<int>(this->m_cfg.id) 
      << "]=" 
      << std::fixed
      << std::setprecision(4)
      << actual_backlash
      << "\r";

  this->exec_command(cmd.str());
}

// ============================================================================
// Axis::backlash
// ============================================================================
double Axis::backlash () throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::backlash");

  //- build command
  yat::OSStream cmd;
  cmd << "Bck[" 
      << static_cast<int>(this->m_cfg.id) 
      << "]=?\r";

  //- Axis::get_parameter will convert from encoder to user unit
  double actual_backlash = this->get_parameter(cmd.str());

  //- if (kUSER_TO_HW_UNIT_RATIO < 0.)
  //-   actual_backlash *= -1.;

  return actual_backlash;
}

// ============================================================================
// Axis::offset
// ============================================================================
void Axis::offset (const double& _val) throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::offset");

  YAT_LOG("Axis::offset::setting axis offset to " << _val);

  this->m_offset = _val;
}

// ============================================================================
// Axis::offset
// ============================================================================
double Axis::offset () throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::offset");

  return this->m_offset;
}

// ============================================================================
// Axis::get_parameter
// ============================================================================
double Axis::get_parameter (const std::string& _cmd, bool _convert_to_uu) 
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::get_parameter");

  YAT_LOG("Axis::get_parameter::writting " << _cmd << " on the I/O port");

  double parameter_value = 0.;

  std::string hw_out;

  try 
  {
    //- write the command on hw I/O port
    SHARED_HW_IO->inout(_cmd, hw_out);
  }
  catch (Tango::DevFailed & df)
  {
    try
    {
      //- try to update the status (in order to get the actual error)
			this->m_state_man.extended_axis_status(true);
    }
    catch (...)
    {
      //- ignore error
    }
    RETHROW_DEVFAILED(df,
                      _CPTC("COMMUNICATION_ERROR"),
                      _CPTC("failed to obtain the required parameter from Axis"),
                      _CPTC("Axis::get_parameter"));
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught"),
                    _CPTC("Axis::get_parameter"));
  }

  YAT_LOG("Axis::get_parameter::hw reply: " << hw_out);

  try
  {
    //- convert to user unit
    parameter_value = yat::XString<double>::to_num(hw_out);
  }

  catch (yat::Exception & ye)
  {
    _YAT_TO_TANGO_EXCEPTION(ye, te);
    RETHROW_DEVFAILED(te,
                      _CPTC("SOFTWARE_ERROR"),
                      _CPTC("yat::Exception caught while trying to extract Axis parameter value from std::tring"),
                      _CPTC("Axis::get_parameter"));
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught while trying to extract Axis parameter value from std::tring"),
                    _CPTC("Axis::get_parameter"));
  }
  
  if (_convert_to_uu)
  {
    parameter_value = HW_TO_USER_UNITS(parameter_value);
  }

  YAT_LOG("Axis::get_parameter::current param. value is " << parameter_value);
  return parameter_value;
}

// ============================================================================
// Axis::exec_command
// ============================================================================
void Axis::exec_command (const std::string& _cmd) 
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Axis::exec_command");

  YAT_LOG("Axis::exec_command::writting " << _cmd << " on I/O port");

  try 
  {
    //- write the command on hw I/O port
    SHARED_HW_IO->in(_cmd, IOCheckError);
  }
  catch (...)
  {
    try
    {
      //- try to update the status (in order to get the actual error)
#if defined(_DEBUG) || defined(DEBUG)
      const ExtendedAxisStatus & eas = this->m_state_man.extended_axis_status(true);
      YAT_LOG("galil::Axis::exec_command::dumping hw status...");
      eas.hws.dump();
#else
			this->m_state_man.extended_axis_status(true);
#endif
    }
    catch (...)
    {
      //- ignore error
    }
    throw;
  }

  YAT_LOG("Axis::exec_command::cmd successfully executed");
}

// ============================================================================
// Axis::state
// ============================================================================
AxisState Axis::state ()
{
  //- usefull only if motor stepper and encoder present
  if (
       this->m_cfg.motor_type == Axis::MOT_STEPPER
         &&
       this->m_cfg.encoder_type != galil::Axis::ENC_NONE
     )
  {
    if (! m_ya_defined || ! m_yb_defined || ! m_yc_defined)
      return AXIS_ERROR;
  }

  return this->m_state_man.state();
}

// ============================================================================
// Axis::status
// ============================================================================
void Axis::status (AxisStatus & es) throw (Tango::DevFailed)
{
  //- get status from state manager
  this->m_state_man.status(es);

  //- usefull only if motor stepper and encoder present
  if (
       this->m_cfg.motor_type == Axis::MOT_STEPPER
         &&
       this->m_cfg.encoder_type != galil::Axis::ENC_NONE
     )
  {
    //- check YA, YB & YC 
    if (! m_ya_defined)
    {
      es.status = kSTATUS_SEP + "ERROR: YA is not properly defined [invalid motorist parameter]";
    }
    if (! m_yb_defined)
    {
      es.status += kSTATUS_SEP + "ERROR: YB is not properly defined [invalid motorist parameter]";
    }
    if (! m_yc_defined)
    {
      es.status += kSTATUS_SEP + "ERROR: YC is not properly defined [invalid motorist parameter]";
    }
  }

  //- get extended axis status
  const ExtendedAxisStatus & eas = this->m_state_man.extended_axis_status(false);

  //- no init. pos. status if encoder is ABSOLUTE
  if (m_cfg.encoder_type != galil::Axis::ENC_ABSOLUTE && !eas.ucas.ref_pos_done())
    es.status += kSTATUS_SEP + kAXIS_INIT_POS_ERROR_STATUS;

  //- get current status of the shared connection to the controller

  std::string shared_hwio_status;

  const galil::HwIOState shared_hwio_state = SHARED_HW_IO->state();

  switch (shared_hwio_state)
  {
    case galil::HWIO_DISCONNECTED:
      shared_hwio_status = "shared com. port status: disconnected";
      break;
    case galil::HWIO_DISCONNECTED_ON_COM_ERROR:
      shared_hwio_status = "shared com. port status: disconnected on error";
      break;
    case galil::HWIO_CONNECTING:
      shared_hwio_status = "shared com. port status: connecting";
      break;
    case galil::HWIO_CONNECTED:
      shared_hwio_status = "shared com. port status: connected";
      break;
    case galil::HWIO_CONNECTION_FAILED:
      shared_hwio_status = "shared com. port status: connection failed";
      break;
    case galil::HWIO_FAULT:
      shared_hwio_status = "shared com. port status: fault";
      break;
    default:
      shared_hwio_status = "shared com. port status: unknown";
      break;
  }

  es.status += kSTATUS_SEP + shared_hwio_status;
}

// ============================================================================
// Axis::string_to_axis_id
// ============================================================================
AxisIdentifier Axis::string_to_axis_id (const std::string& _str)   throw (Tango::DevFailed)
{
  AxisIdentifier aid = galil::MIN_AXIS;

  //- axis config: get axis identifier from _str first alpha/num char in _str

  std::string::size_type pos;
  pos = _str.find_first_of("01234567ABCDEFGHXYZW");
	if (pos == std::string::npos)
  {
    THROW_DEVFAILED(_CPTC("INVALID_PARAMETER"),
                    _CPTC("invalid axis name or identifier specified"),
                    _CPTC("galil::Axis::string_to_axis_id"));
	}

	switch (static_cast<int>(_str[pos]))
	{
    case int('0'):
    case int('A'):
    case int('X'):
      aid = galil::AXIS_A;
		  break;
		case int('1'):
		case int('B'):
		case int('Y'):
			aid = galil::AXIS_B;
			break;
		case int('2'):
		case int('C'):
		case int('Z'):
			aid = galil::AXIS_C;
			break;
		case int('3'):
		case int('D'):
		case int('W'):
			aid = galil::AXIS_D;
			break;
#if ! defined(DMC_4143)
		case int('4'):
		case int('E'):
		  aid = galil::AXIS_E;
		  break;
		case int('5'):
		case int('F'):
		  aid = galil::AXIS_F;
		  break;
		case int('6'):
		case int('G'):
		  aid = galil::AXIS_G;
		  break;
		case int('7'):
		case int('H'):
		  aid = galil::AXIS_H;
		  break;
#endif
    default:
      THROW_DEVFAILED(_CPTC("INVALID_PARAMETER"),
                      _CPTC("invalid axis name or identifier specified"),
                      _CPTC("galil::Axis::string_to_axis_id"));
      break;
  }

  return aid;
}

// ============================================================================
// Axis::axis_id_to_string
// ============================================================================
std::string Axis::axis_id_to_string (const AxisIdentifier& _aid)   throw (Tango::DevFailed)
{
  yat::OSStream axis_name;
  axis_name << ( (_aid > MIN_AXIS && _aid < MAX_AXIS) ? Axis::axis_letter[_aid] : '?' ); 
  return axis_name.str();
}

// ============================================================================
// Axis::arm_latch
// ============================================================================
void Axis::arm_latch () throw (Tango::DevFailed)
{
  try
  {
    yat::OSStream cmd;
    cmd << "AL"
        << Axis::axis_letter[this->m_cfg.id]  
        << "\r";
    SHARED_HW_IO->in(cmd.str(), IOCheckError);
  }
  catch (Tango::DevFailed & df)
  {
    RETHROW_DEVFAILED(df,
                      _CPTC("COMMUNICATION_ERROR"),
                      _CPTC("could not arm latch [Tango exception caught]"),
                      _CPTC("galil::Axis::arm_latch"));
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("could not arm latch [unknown exception caught]"),
                    _CPTC("galil::Axis::arm_latch"));
  }
}

// ============================================================================
// Axis::latch_armed
// ============================================================================
bool Axis::latch_armed () throw (Tango::DevFailed)
{
  //- get extended axis status
  const ExtendedAxisStatus & eas = this->m_state_man.extended_axis_status(false);
  return eas.fas.latch_armed();
}

// ============================================================================
// Axis::latch_occured
// ============================================================================
bool Axis::latch_occured () throw (Tango::DevFailed)
{
  //- get extended axis status
  const ExtendedAxisStatus & eas = this->m_state_man.extended_axis_status(false);
  return eas.fas.latch_occured();
}

// ============================================================================
// Axis::get_latched_encoder_position
// ============================================================================
long Axis::get_latched_encoder_position () throw (Tango::DevFailed)
{
  if (! this->latch_occured())
  {
    THROW_DEVFAILED(_CPTC("OPERATION_NOT_ALLOWED"),
                    _CPTC("no latch event detected [no latched position available]"),
                    _CPTC("galil::Axis::get_latched_encoder_position"));
  }

  //- get motor ratio: get YA
  yat::OSStream cmd;
  cmd << "RL"
      << Axis::axis_letter[this->m_cfg.id]  
      << "\r";
  std::string latched_pos_str;
  SHARED_HW_IO->inout(cmd.str(), latched_pos_str);
  
  long val = 0;
  
  try
  {
    val = yat::XString<long>::to_num(latched_pos_str);
  }
  catch (yat::Exception & ye)
  {
    _YAT_TO_TANGO_EXCEPTION(ye, te);
    RETHROW_DEVFAILED(te,
                      _CPTC("SOFTWARE_ERROR"),
                      _CPTC("yat::Exception caught while trying to extract <latched pos> from std::tring"),
                      _CPTC("get_latched_encoder_position"));
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught while trying to extract <latched pos> from std::tring"),
                    _CPTC("get_latched_encoder_position"));
  }
  
  return val;
}

// ============================================================================
// Axis::::correction_history
// ============================================================================
const LockedPositionHistory & Axis::correction_history ()
  throw (Tango::DevFailed)
{
  yat::MutexLock guard(this->m_locked_pos_history_mutex);

  size_t max_x = this->m_locked_pos_history.m_w;
  size_t max_y = this->m_locked_pos_history.m_h;

  double * data = this->m_locked_pos_history.m_data;

  for (size_t x = 0; x < max_x; x++)
  {
    double * p = 0;

    switch (x)
    {
      case LOCKED_POSITION_HISTORY:
        p = this->m_locked_pos_history_pos.ordered_data().base();
        break;
      case TIMESTAMP_HISTORY:
        p = this->m_locked_pos_history_tms.ordered_data().base();
        break;
      case DERIVATION_HISTORY:
        p = this->m_locked_pos_history_drv.ordered_data().base();
        break;
      default:
        continue;
        break;
    }

    for (size_t y = 0; y < max_y; y++)
      *(data + (y * max_x) + x) = *(p + y);
  }
  
  return this->m_locked_pos_history;
}

} // namespace galil
