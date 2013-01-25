//=============================================================================
// Axis.h
//=============================================================================
// abstraction.......Galil Axis abstraction class
// class.............Axis
// original author...N.Leclercq - J.Coquet - SOLEIL
//=============================================================================

#ifndef _AXIS_H_
#define _AXIS_H_

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include <yat/time/Timer.h>
#include <yat/memory/DataBuffer.h>
#include <yat4tango/DeviceTask.h>
#include <yat4tango/LogHelper.h>
#include "HwIO.h"
#include "AxisStateManager.h"

//=============================================================================
// CONSTs
//=============================================================================
#define kNUM_SERVO_ID   2
#define kNUM_STEPPER_ID 4

namespace galil
{

// ============================================================================
// HISTORY BUFFERS
// ============================================================================
//- define what is an history buffer
typedef yat::CircularBuffer<double, yat::NullMutex> HistoryBuffer;
//- chronologically ordered history data 
typedef yat::Buffer<double> OrderedHistoryBuffer;
//- history matrix - contains: locked-pos, pos. correction timestamp & derivation 
struct LockedPositionHistory
{
  //----------------------------------------------
  LockedPositionHistory ()
   : m_w(0), m_h(0), m_data(0) 
  {
    
  }
  //----------------------------------------------
  ~LockedPositionHistory ()
  {
    delete [] m_data;
  }
  //----------------------------------------------
  void resize (size_t n_rows, size_t n_cols)
    throw(Tango::DevFailed)
  {
    try
    {
      m_w = 0;
      m_h = 0;
      delete [] m_data;
      m_data = new double[n_rows * n_cols];
      if (! m_data)
        throw std::bad_alloc();
      m_w = n_rows;
      m_h = n_cols;
    }
    catch (...)
    {
      Tango::Except::throw_exception(_CPTC("MEMORY_ERROR"),
                                     _CPTC("LockedPositionHistory allocation failed"),
                                     _CPTC("LockedPositionHistory::resize"));
    }
  }
  //----------------------------------------------
  void fill (double v)
  {
    if (m_data)
      for (size_t x = 0; x < m_w; x++)
        for (size_t y = 0; y < m_h; y++)
          *(m_data + x * m_h + y) = v;  
  }
  //----------------------------------------------
  size_t m_w;
  size_t m_h;
  double * m_data;
};

// ============================================================================
// HISTORY IDENTIFIERS
// ============================================================================
enum
{
  LOCKED_POSITION_HISTORY = 0,
  TIMESTAMP_HISTORY,
  DERIVATION_HISTORY
};

//------------------------------------------------------------------------------
//- CLASS: Axis (forward declaration)
//------------------------------------------------------------------------------
class Axis;

//------------------------------------------------------------------------------
//- CLASS: AxisPositionLocker
//------------------------------------------------------------------------------
class AxisPositionLocker : public yat4tango::DeviceTask
{
  //- ugly but necessary cross referencing!
  friend class Axis;

public:
  //- ctor
  AxisPositionLocker(Axis * axis, 
                     double position_constraint, 
                     size_t ckecking_period_secs,
                     size_t averaging_window_width,
                     Tango::DeviceImpl * host_device);

  //- dtor
  virtual ~AxisPositionLocker ();

  //- locking enabled?
  inline bool locking_enabled () const {
    return this->m_locking_enabled;
  }

  //- locks current axis position 
  void lock_position (const double& target_position)
    throw (Tango::DevFailed);

  //- unlocks axis position 
  void unlock_position ()
    throw (Tango::DevFailed);

  //- motion request notification (i.e. new target/locked position)   
  void motion_requested (const double& target_position)
    throw (Tango::DevFailed);

  //- stop request notification
  void stop_requested ()
    throw (Tango::DevFailed);

  //- jog request notification
  void jog_requested ()
    throw (Tango::DevFailed);
  
protected:
  //- the yat::Message handler
  virtual void process_message (yat::Message& msg)
    throw (Tango::DevFailed);

private:
  //- periodic activity (locking management)
  void manage_position_locking ()
    throw (Tango::DevFailed);
  //- the associated Axis
  Axis * m_axis;
  //- the associated Axis
  AxisIdentifier m_axis_id;
  //- position locked flag
  bool m_locking_enabled;
  //- unlock pending flag
  bool m_unlock_pending;
  //- the locked/targeted position
  double m_target_position;
  //- the locking constraint (pos threshold for correction)
  double m_position_constraint;
  //- ckecking period in seconds
  size_t m_ckecking_period_secs;
  //- axis pos averaging windows witdth
  size_t m_locked_pos_averaging_window;   
  //- axis pos averaging buffer
  HistoryBuffer m_locked_pos_averaging_buffer;   
  //- axis motion request tmo (prevent locking management before tmo expiration)
  yat::Timeout m_tmo;
};

// ============================================================================
//- CLASS: Axis
// ============================================================================
class Axis : public yat::SharedObject, public yat4tango::TangoLogAdapter
{
  friend class AxisFactory;

  //- ugly but necessary cross referencing!
  friend class AxisPositionLocker;

  //- shortcut to mother class
  typedef yat::SharedObject Inherited;
  
public:
  //- a dedicated type for motor type
  //-----------------------------------------
  typedef enum
  {
    MOT_UNKNOWN,
    MOT_STEPPER,
    MOT_SERVO_OR_PIEZO
  } MotorType;

  //- a dedicated type for encoder type
  //-----------------------------------------
  typedef enum
  {
    ENC_NONE,
    ENC_INCREMENTAL,
    ENC_ABSOLUTE
  } EncoderType;

  //- a dedicated type for gearing role
  //-----------------------------------------
  typedef enum
  {
    NO_GEARING_AXIS,
    MASTER_AXIS,
    SLAVE_AXIS,
  } GearingMode;

  //- a dedicated type for init type
  //-----------------------------------------
  typedef unsigned long InitType;

  //- a dedicated type for Axis configuration
  //-----------------------------------------
  typedef struct Config
  {
    //- axis ID
    AxisIdentifier id;
    //- motor type (stepper, ...)
    MotorType motor_type;
    //- encoder type
    EncoderType encoder_type;
    //- maintenance mode status
    bool enable_maintenance_mode;
    //- error correction mode status
    bool enable_error_correction;
    //- Allow Remote manual control with RemoteCBox
    bool allow_remote_cbox;
    //- encoder ratio
    double encoder_ratio;
    //- motor ratio
    double motor_ratio;
    //- initialization type
    InitType init_type; 
    //- initial position
    double init_pos; 
    //- initialization speed
    double init_velocity;
    //- duty cycle mode
    bool duty_cycle_enabled;
    //- percent
    double percent;
    //- retry
    unsigned long retry;
    //- settling time
    unsigned long settling_time;
    //- position locking constraint 
    double position_locking_constraint;
    //- position locking checking period (in ms)
    size_t position_locking_checking_period_secs;
    //- position locking history depth
    size_t position_locking_history_depth;
    //- position locking avaraging window width
    size_t position_locking_avaraging_window_width;
    //- host device
    Tango::DeviceImpl * host_device;
    //- ctor -----------------------
    Config ();
    //- operator= ------------------
    void operator= (const Config& src);
  } Config;

  //- a dedicated type for Gearing configuration
  //--------------------------------------------
  typedef struct GearingConfig
  {
    //- members --------------------
    //- mode 
    GearingMode mode;
    //- associated axis
    AxisIdentifier associated_axis;
    //- master/slave ratio
    double ms_ratio;
    //- enable gearing mode
    bool gearing_enabled;
    //- ctor -----------------------
    GearingConfig ();
    //- operator= ------------------
    void operator= (const GearingConfig& src);
  } GearingConfig;

  //- Axis (re)configuration ------------------
  void configure (const Config& cfg)
    throw (Tango::DevFailed);

  //- Set Axis Gearing config -----------------
  void configure_gearing (const GearingConfig& gcfg)
    throw (Tango::DevFailed);

  //- Get Axis Gearing config -----------------
  inline const GearingConfig& gearing_configuration (void) const
  {
    return m_gearing_cfg;
  }

  //- Enables the Gearing mode ----------------
  void enable_gearing (void)
    throw (Tango::DevFailed);

  //- Disables the Gearing mode ---------------
  void disable_gearing (void)
    throw (Tango::DevFailed);

  //- Is the Gearing mode enabled -------------
  bool gearing_enabled (void)
    throw (Tango::DevFailed);

  //- Set the sign of the gearing ratio -------
  void sign_of_gearing_ratio (int sign_adapter)
    throw (Tango::DevFailed);

	//- Is axis gearing master?
  inline bool is_gearing_master ()
    throw (Tango::DevFailed)
  {
    return this->m_state_man.is_gearing_master();
  }

	//- Is axis gearing slave? 
  inline bool is_gearing_slave ()
    throw (Tango::DevFailed)
  {
    return this->m_state_man.is_gearing_slave();
  }

	//- Is axis position locked? 
  inline bool is_position_locked ()
    throw (Tango::DevFailed)
  {
    bool locked_at_sw_level = this->m_axis_pos_locker 
                            ? this->m_axis_pos_locker->locking_enabled()
                            : false;

    bool locked_at_hw_level = this->m_state_man.is_position_locked();

    return locked_at_sw_level || locked_at_hw_level;
  }

  //- Axis composite ExtendedStatus accessor --
  void status (AxisStatus & es)
    throw (Tango::DevFailed);

  //- Axis state accessor ---------------------
  AxisState state ();

  //- Axis status str accessor ----------------
  inline void status (std::string & status_str)
    throw (Tango::DevFailed)
  {
    AxisStatus es;
    this->status(es);
    status_str = es.status;
  }

  //- Axis extended status --------------------
  inline const ExtendedAxisStatus & extended_axis_status (bool force_update = false)
    throw (Tango::DevFailed)
  {
  	return this->m_state_man.extended_axis_status(force_update);
  }

  //- Motor-on cmd. request -------------------
  void on ()
    throw (Tango::DevFailed); 

  //- Motor-off cmd. request ------------------
  void off ()
    throw (Tango::DevFailed); 

  //- Forward cmd. request --------------------
  void forward ()
    throw (Tango::DevFailed); 

  //- Backward cmd. request -------------------
  void backward ()
    throw (Tango::DevFailed); 

  //- Init Ref. Pos cmd. request --------------
  void init_reference_position ()
    throw (Tango::DevFailed); 

	//- Define position -------------------------
  void define_position (double init_pos)
    throw (Tango::DevFailed);

  //- Abs.Pos cmd. request --------------------
  void abs_pos (const double& abs_pos)
    throw (Tango::DevFailed); 

  //- Abs.Pos cmd. accessor -------------------
  double abs_pos ()
    throw (Tango::DevFailed); 

  //- Lock pos. cmd. request ------------------
  void lock_pos (const double& abs_pos)
    throw (Tango::DevFailed); 

  //- Unlock pos. cmd. request ----------------
  void unlock_pos ()
    throw (Tango::DevFailed); 

  //- Stop cmd. request -----------------------
  void stop ()
    throw (Tango::DevFailed); 

  //- acceleration mutator --------------------
  void acceleration (const double& a)
    throw (Tango::DevFailed); 

  //- acceleration accessor -------------------
  double acceleration ()
    throw (Tango::DevFailed);

  //- deceleration mutator -------------------
  void deceleration (const double& d)
    throw (Tango::DevFailed); 

  //- deceleration accessor ------------------
  double deceleration ()
    throw (Tango::DevFailed);

  //- velocity mutator ------------------------
  void velocity (const double& v)
    throw (Tango::DevFailed); 

  //- velocity accessor -----------------------
  double velocity ()
    throw (Tango::DevFailed);

  //- accuracy mutator ------------------------
  void accuracy (const double& a)
    throw (Tango::DevFailed); 

  //- accuracy accessor -----------------------
  double accuracy ()
    throw (Tango::DevFailed);

  //- backlash mutator ------------------------
  void backlash (const double& a)
    throw (Tango::DevFailed); 

  //- backlash accessor -----------------------
  double backlash ()
    throw (Tango::DevFailed);

  //- offset mutator --------------------------
  void offset (const double& a)
    throw (Tango::DevFailed); 

  //- offset accessor -------------------------
  double offset ()
    throw (Tango::DevFailed);

  //- backward limit switch state -------------
  inline bool backward_limit_switch_detected ()
    throw (Tango::DevFailed)
  {
    return this->m_state_man.backward_lsw_detected();
  }

  //- forward limit switch state --------------
  inline bool forward_limit_switch_detected ()
    throw (Tango::DevFailed)
  {
    return this->m_state_man.forward_lsw_detected();
  }

  //- AxisIdentifier accessor -----------------
  inline AxisIdentifier identifier () const
  {
    return this->m_cfg.id;
  }

  //- Dump (i.e. printout) status info --------
  inline void dump ()
  {
    this->m_state_man.dump();
  }

  //- yat::SharedObject interface ------------
  inline Axis * duplicate ()
  {
    return static_cast<Axis*>(Inherited::duplicate());
  }

  //- returns the global ratio ---------------- 
  inline double global_ratio ()
  {
    return this->m_cfg.encoder_ratio * this->m_cfg.motor_ratio;
  }

  //- Arm latch -------------------------------
  void arm_latch ()
    throw (Tango::DevFailed); 

  //- Latch armed -----------------------------
  bool latch_armed ()
    throw (Tango::DevFailed);

  //- Latch occured ---------------------------
  bool latch_occured ()
    throw (Tango::DevFailed); 

  //- Latched pos -----------------------------
  long get_latched_encoder_position ()
    throw (Tango::DevFailed);

  //- Utility:: string to axis identifier converter
  static AxisIdentifier string_to_axis_id (const std::string& str) 
    throw (Tango::DevFailed);

  //- Utility:: axis identifier to string converter
  static std::string axis_id_to_string (const AxisIdentifier& aid) 
    throw (Tango::DevFailed);

  //- locked position history accessor
  const LockedPositionHistory & correction_history ()
    throw (Tango::DevFailed);

private:
  //- private ctor
  Axis ();

  //- private dtor
  virtual ~Axis ();

  //- read an axis parameter (_hw_in is the cmd to be passed)
  double get_parameter (const std::string& _hw_in, bool _convert_to_uu = true) 
    throw (Tango::DevFailed);

  //- write the specified <_hw_in> cmd on the HW-I/O port
  void exec_command (const std::string& _hw_in) 
    throw (Tango::DevFailed);

  //- Init Ref. Pos cmd. internal impl
	//- used by init_reference_position and define_position 
  void init_ref_pos_i (double init_pos, InitType init_type)
    throw (Tango::DevFailed);

  //- internal <lock_pos> impl
  void lock_pos_i (const double& _abs_pos)
    throw (Tango::DevFailed);

  //- internal <stop> impl
  void stop_i ()
    throw (Tango::DevFailed); 

  //- axis position locker
  AxisPositionLocker * m_axis_pos_locker;

  //- configuration
  Axis::Config m_cfg;

  //- the associated AxisStateManager
  AxisStateManager m_state_man;

  //- pos. offset value (in user unit)
  double m_offset;

  //- YA, YB & YC def. flag
  bool m_ya_defined;
  bool m_yb_defined;
  bool m_yc_defined;

  //- minimum speed retreived in the CB in var SpMini
  double m_sp_mini;

  //- Gearing configuration for this Axis
  Axis::GearingConfig m_gearing_cfg;

  //- "a la" galil axis identifier (letter)
  static const char axis_letter[MAX_AXIS];

  //- MT range for stepper motor
  static const double stepper_motor_ids[kNUM_STEPPER_ID];

  //- MT range for servo motor
  static const double servo_motor_ids[kNUM_SERVO_ID];
  
  //- signof
  static inline int signof (int i)
  { 
    return unsigned(i) >> (sizeof (int) * 8 - 1);
  } 

  //- locked position history
  yat::Mutex m_locked_pos_history_mutex;
  HistoryBuffer m_locked_pos_history_pos; //- locked axis pos.
  HistoryBuffer m_locked_pos_history_tms; //- correction timestamp
  HistoryBuffer m_locked_pos_history_drv; //- axis pos. derivation
  LockedPositionHistory m_locked_pos_history;

  //- unimplemented/unsupported members
  //-----------------------------------
  Axis (const Axis &);
  Axis & operator= (const Axis &);
};

} //-  namespace galil

#endif //-- _AXIS_H_
