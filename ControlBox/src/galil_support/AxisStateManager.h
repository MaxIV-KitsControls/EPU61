//=============================================================================
// AxisStateManager.h
//=============================================================================
// abstraction.......Galil AxisStateManager abstraction class
// class.............AxisStateManager
// original author...N.Leclercq - SOLEIL
//=============================================================================

#ifndef _AXIS_STATE_MANAGER_H_
#define _AXIS_STATE_MANAGER_H_

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include <yat4tango/LogHelper.h>
#include "HwStatusProxy.h"

namespace galil
{

//------------------------------------------------------------------------------
//- TYPEDEF: AxisState
//------------------------------------------------------------------------------
typedef enum 
{
  AXIS_OFF,
  AXIS_MOVING, 
  AXIS_STANDBY,
  AXIS_ALARM,
  AXIS_ERROR,
  AXIS_CRTL_DISABLED,
  AXIS_UNKNOWN
} AxisState;

//------------------------------------------------------------------------------
//- TYPEDEF: CmdAllowedCriteria 
//------------------------------------------------------------------------------
typedef enum
{
  kUCODE_RUNNING = 0,
  kUCODE_NO_REF_POS_IN_PROGRESS,
  kAXIS_NOT_MOVING,
  kAXIS_MOTOR_ON,
  kAXIS_POWER_CTRL_ALLOWED,
  kAXIS_MOTOR_CTRL_ALLOWED,
  kAXIS_NO_REF_POS_IN_PROGRESS,
  kAXIS_NO_FOLLOW_ERROR,
  kAXIS_NOT_GEARING_MASTER,
  kAXIS_NOT_GEARING_SLAVE,
  kAXIS_POS_NOT_LOCKED,
  kAXIS_NO_ACTIVE_LSW,
  kAXIS_NO_CMD_IN_PROGRESS,
  kUNKNOWN_COMMAND,
  kUNKNOWN_CRITERIA
} CmdAllowedCriteria;

//------------------------------------------------------------------------------
//- TYPEDEF: AxisStatus - a dedicated type for Axis status
//------------------------------------------------------------------------------
//- a dedicated type for Axis status
//-----------------------------------------
typedef struct AxisStatus
{
  //- members -------------------
  //- the associated Tango state
  AxisState state;
  //- the human readable status
  std::string status;
  //- hardware error code
  unsigned short tc_error;
  //- hardware error string
  std::string tc_error_str;
  //- ctor ----------------------
  AxisStatus ();
  //- operator= -----------------
  void operator= (const AxisStatus& src);
} AxisStatus;

//------------------------------------------------------------------------------
//- CLASS: AxisStateManager
//------------------------------------------------------------------------------
class AxisStateManager : public yat4tango::TangoLogAdapter
{
  friend class Axis;
  friend bool eval_criteria (AxisStateManager*, int);

public:

  //- a dedicated type for Axis configuration
  //-----------------------------------------
  typedef struct AxisConfig
  {
    //- members --------------------
      //- axis ID
      AxisIdentifier id;
      //- lsw logic
      bool has_inverted_lsw;
      //- duty cycle mode
      bool duty_cycle_enabled;
      //- host device
      Tango::DeviceImpl * host_device;
    //- ctor -----------------------
    AxisConfig ();
    //- ctor -----------------------
    AxisConfig (const AxisConfig& src);
    //- operator= ------------------
    void operator= (const AxisConfig& src);
  } AxisConfig;

  //---------------------------------------------------------
  // AxisStateManager::command_allowed
  //---------------------------------------------------------
  // Giving the current status of the associated axis, this 
  // method checks whether or not the specified command is 
  // allowed. In case the command is refused and the <why> is 
  // not NULL, the reason is returned in the provided string.
  //---------------------------------------------------------
  bool command_allowed (CommandIdentifier cid, 
                        std::string * why_str = 0,
                        CmdAllowedCriteria * why_id = 0)
    throw (Tango::DevFailed);

  //---------------------------------------------------------
  // AxisStateManager::any_cmd_in_progress
  //---------------------------------------------------------
  //- Returns <true> if a command is in progress for the 
  //- associated axis, returns <false> otherwise.
  //---------------------------------------------------------
  inline bool any_command_in_progress ()
    throw (Tango::DevFailed)
  {
    //- update status
    this->update_ext_axis_status_i();
    //- delegate to ExtendedAxisStatus::UCodeAxisCommandState
    return this->m_eas.ucacs.any_command_in_progress();
  }

  //---------------------------------------------------------
  // AxisStateManager::cmd_in_progress
  //---------------------------------------------------------
  //- Returns <true> if a command is in progress for the 
  //- associated axis, returns <false> otherwise. The cmd
  //- identifier and name are returned through the passed
  //- references. In case there is no cmd in progress, <cmd_id>
  //- is set to CommandIdentifier:NO_COMMAND and the cmd name 
  //- to <Unknown Galil Command>.
  //---------------------------------------------------------
  inline bool command_in_progress (CommandIdentifier& cmd_id, std::string& cmd_name)
    throw (Tango::DevFailed)
  {
    //- update status
    this->update_ext_axis_status_i();
    //- get info from ExtendedAxisStatus::UCodeAxisCommandState
    bool cip = this->m_eas.ucacs.command_in_progress(cmd_id);
    cmd_name = this->m_eas.ucacs.command_name(cmd_id);
    return cip;
  }

  //---------------------------------------------------------
  // AxisStateManager::extended_axis_status
  //---------------------------------------------------------
  //- Returns the current (i.e. most recently acquire) axis
  //- status informations. The caller can access any status 
  //- info from the <ExtendedAxisStatus> public members.  
  //---------------------------------------------------------
  inline const ExtendedAxisStatus & extended_axis_status (bool force_update = false)
    throw (Tango::DevFailed)
  {
  	return this->update_ext_axis_status_i(force_update);
  }

  //---------------------------------------------------------
  // AxisStateManager::dump
  //---------------------------------------------------------
  inline void dump ()
  {
    this->m_eas.hws.dump();
    this->m_eas.fas.dump();
    this->m_eas.ucs.dump();
    this->m_eas.ucas.dump();
    this->m_eas.ucacs.dump();

  }

  //---------------------------------------------------------
  // AxisStateManager::wait_current_command_processed
  //---------------------------------------------------------
  void wait_current_command_processed (size_t tmo_ms, bool update_state = false)
    throw (Tango::DevFailed);

  //---------------------------------------------------------
  // AxisStateManager::wait_motor_stopped 
  //---------------------------------------------------------
  void wait_motor_stopped (size_t _tmo_ms)
    throw (Tango::DevFailed);

  //---------------------------------------------------------
  // AxisStateManager::attached_axis
  //---------------------------------------------------------
  inline void attached_axis (const AxisConfig& _acfg)
  {
    this->m_acfg = _acfg;
    this->host(this->m_acfg.host_device);
  }

  //---------------------------------------------------------
  // AxisStateManager::backward_lsw_detected
  //---------------------------------------------------------
  bool backward_lsw_detected ()
	  throw (Tango::DevFailed);

  //---------------------------------------------------------
  // AxisStateManager::forward_lsw_detected
  //---------------------------------------------------------
  bool forward_lsw_detected ()
	  throw (Tango::DevFailed);

  //---------------------------------------------------------
  // AxisStateManager::is_gearing_master
  //---------------------------------------------------------
  bool is_gearing_master ()
	  throw (Tango::DevFailed);

  //---------------------------------------------------------
  // AxisStateManager::is_gearing_slave
  //---------------------------------------------------------
  bool is_gearing_slave ()
	  throw (Tango::DevFailed);

  //---------------------------------------------------------
  // AxisStateManager::is_position_locked
  //---------------------------------------------------------
  bool is_position_locked ()
	  throw (Tango::DevFailed);

private:
  //- ctor -------------
  AxisStateManager ();

  //- ctor -------------
  AxisStateManager (const AxisConfig& _acfg);

  //- dtor -------------
  virtual ~AxisStateManager ();

  //- returns the AxisState for the associated axis
  AxisState state ();

  //- returns the so called "AxisStatus" for the associated axis
  void status (AxisStatus & status_);

  //- gets current axis status from the <HwStatusProxy>
  const ExtendedAxisStatus & update_ext_axis_status_i (bool force_update = false)
    throw (Tango::DevFailed);

  //- searches the <first> criteria which failed for a given command
  CmdAllowedCriteria why_cmd_refused (CommandIdentifier _cid) const;

  //- evaluates the criteria which identifier is specified
  bool eval_criteria (int criteria) const;

  //- the <ExtendedAxisStatus> of the associated axis
  ExtendedAxisStatus m_eas;

  //- the attached axis config
  AxisConfig m_acfg;

  //- unimplemented/unsupported members
  //-----------------------------------
  AxisStateManager (const AxisStateManager &);
  AxisStateManager & operator= (const AxisStateManager &);
};

} //-  namespace galil

#endif //-- _AXIS_STATE_MANAGER_H_
