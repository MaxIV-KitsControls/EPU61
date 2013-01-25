//=============================================================================
// HwStatusProxy.h
//=============================================================================
// abstraction.......Galil global status manager abstraction class
// class.............HardwareStatusProxy
// original author...N.Leclercq - SOLEIL
//=============================================================================

#ifndef _HW_STATUS_PROXY_H_
#define _HW_STATUS_PROXY_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/Task.h>
#include <yat4tango/ExceptionHelper.h>
#include "BitsRecords.h"
#include "HwIO.h"

// ============================================================================
// SHORTCUT TO THE <HwStatusProxy> SINGLETON
// ============================================================================
#define HW_STATUS_PROXY galil::HwStatusProxy::instance()

// ============================================================================
// SHORTCUT TO TC ERROR STRINGS
// ============================================================================
#define GALIL_ERR_STR(TC)  HW_STATUS->tc_code_error_str(TC) 

// ============================================================================
// CONSTs
// ============================================================================
#define kDEFAULT_HWSP_PERIOD            100
#define kDEFAULT_HWSP_STARTUP_TMO       3000
#define kDEFAULT_STATUS_EXPIRATION_TMO  4 * kDEFAULT_HWSP_PERIOD
//-----------------------------------------------------------------------------
#define kDEFAULT_WMH_TMO                975
//-----------------------------------------------------------------------------

namespace galil
{

// ============================================================================
// class: HwStatusProxy <SINGLETON>
// ============================================================================
class HwStatusProxy : public yat::Task
{
  friend class ObjectManager;

public:

  typedef unsigned long UpdateMask;

  //- update what bit mask (use when update is forced)
  //---------------------------------------------------------
  enum
  {
    //- select status info to be updated --
    UPDATE_QR_STATUS          = 0x0001,
    UPDATE_HW_STATUS          = 0x0002,
    UPDATE_UC_STATUS          = 0x0004,
    UPDATE_UC_AXIS_STATUS     = 0x0008,
    UPDATE_UC_AXIS_CMD_STATUS = 0x0010,
    UPDATE_ALL                = 0x0020,
    //- select axis -----------------------
    UPDATE_AXIS_A             = 0x0100,
    UPDATE_AXIS_B             = 0x0200,
    UPDATE_AXIS_C             = 0x0400,
    UPDATE_AXIS_D             = 0x0800,
    UPDATE_AXIS_E             = 0x1000,
    UPDATE_AXIS_F             = 0x2000,
    UPDATE_AXIS_G             = 0x4000,
    UPDATE_AXIS_H             = 0x8000
  };

  //- create a dedicated type for HwStatusProxy configuration
  //---------------------------------------------------------
  typedef struct Config
  {
    //- members --------------------
    std::string galilbox_ip_addr;
    size_t galilbox_port;
    size_t status_update_period_ms;
    size_t startup_timeout_ms;
    size_t status_expiration_timeout_ms;
    //- ctor -----------------------
    Config ();
    //- operator= ------------------
    void operator= (const Config& src);
  } Config;

  //----------------------------------------
  // SINGLETON ACCESSOR (SEE HW_STATUS MACRO)
  //----------------------------------------
  static inline HwStatusProxy * instance ()
		throw (Tango::DevFailed)
  {
    if (! HwStatusProxy::singleton)
        THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                        _CPTC("unexpected NULL pointer [HwStatusProxy singleton not properly initialized]"),
                        _CPTC("HwStatusProxy::instance")); 
    return HwStatusProxy::singleton;
  }

  //- The following members are thread safe
  //----------------------------------------

  //- singleton configuration --------------
  void configure (const Config& cfg)
		throw (Tango::DevFailed);

  //- Galil QR block accessor (all) --------
  inline void qr_block (QRBlock& gb)
  //----------------------------------------
  {
    //- enter critical section
    yat::AutoMutex<> guard(this->Task::m_lock);
    //- check data expiration
    status_data_expired_i();
    //- copy data
    gb = this->m_qr_block;
  }

  //- Galil Header accessor ----------------
  inline void header (Header& gh)
  //----------------------------------------
  {
    //- enter critical section
    yat::AutoMutex<> guard(this->Task::m_lock);
    //- check data expiration
    status_data_expired_i();
    //- copy data
    this->header_i(gh);
  }

  //- Galil General I/O accessor -----------
  inline void genio (GenIO& gio)
  //----------------------------------------
  {
    //- enter critical section
    yat::AutoMutex<> guard(this->Task::m_lock);
    //- check data expiration
    status_data_expired_i();
    //- copy data
    this->genio_i(gio);
  }

  //- Firmware axis state accessor ---------
  inline void firmware_axis_state (AxisIdentifier ai, FirmwareAxisState& fas)
  //----------------------------------------
  {
    //- enter critical section
    yat::AutoMutex<> guard(this->Task::m_lock);
    //- check data expiration
    status_data_expired_i();
    //- copy data
    this->firmware_axis_state_i (ai, fas);
  }

  //- Extended axis status accessor --------
  inline void extended_axis_status (AxisIdentifier ai, ExtendedAxisStatus& eas)
  //----------------------------------------
  {
    //- enter critical section
    yat::AutoMutex<> guard(this->Task::m_lock);
    //- check data expiration
    status_data_expired_i();
    //- copy data
    this->hardware_status_i(eas.hws);
    this->firmware_axis_state_i(ai, eas.fas);
    this->ucode_status_i(eas.ucs);
    this->ucode_axis_state_i(ai, eas.ucas);
    this->ucode_axis_cmd_state_i(ai, eas.ucacs);
  }

  //- uCode Status -------------------------
  inline void ucode_status (UCodeStatus& ucs)
  //----------------------------------------
  {
    //- enter critical section
    yat::AutoMutex<> guard(this->Task::m_lock);
    //- check data expiration
    status_data_expired_i();
    //- copy data
    this->ucode_status_i(ucs);
  }

  //- Hardware Status ----------------------
  inline void ucode_status (HardwareStatus& hws)
  //----------------------------------------
  {
    //- enter critical section
    yat::AutoMutex<> guard(this->Task::m_lock);
    //- check data expiration
    status_data_expired_i();
    //- copy data
    this->hardware_status_i(hws);
  }

  //- uCode Axis State ---------------------
  inline void ucode_axis_state (AxisIdentifier ai, UCodeAxisState& ucas)
  //----------------------------------------
  {
    //- enter critical section
    yat::AutoMutex<> guard(this->Task::m_lock);
    //- check data expiration
    status_data_expired_i();
    //- copy data
    this->ucode_axis_state_i(ai, ucas);
  }

  //- uCode Cmd ----------------------------
  inline void ucode_axis_cmd_state (AxisIdentifier ai, UCodeAxisCommandState& ucacs)
  //----------------------------------------
  {
    //- enter critical section
    yat::AutoMutex<> guard(this->Task::m_lock);
    //- check data expiration
    status_data_expired_i();
    //- copy data
    this->ucode_axis_cmd_state_i(ai, ucacs);
  }

  //- TC error code to tc error string ----
  const char* tc_code_error_str (unsigned char _tc_code) const;

  //- Force status update -------------------
  void force_status_update (const UpdateMask& update_mask,
                            bool _wmh = true, 
                            size_t _wmh_tmo_ms = kDEFAULT_WMH_TMO)
		throw (Tango::DevFailed);

  //- Underlying synch. HwIO state
  inline /*const*/ HwIOState synch_hw_io_state () const
  {
    if (this->m_synch_hw_io)
      return this->m_synch_hw_io->state();
    return galil::HWIO_DISCONNECTED;
  }

  //- Underlying HwIO health
  inline const HwIOHealth & synch_com_health (void)
  {
    if (this->m_synch_hw_io)
      return this->m_synch_hw_io->health();
    
    THROW_DEVFAILED(_CPTC("NO_DATA"),
                    _CPTC("the underlying HwIO is not instanciated yet"),
                    _CPTC("HwStatusProxy::synch_com_health"));
  }

  //- Timestamp of the last 'full' status update
  inline void last_update_timestamp (yat::Timestamp& ts, double& elapsed_secs)
  {
    //- enter critical section
    yat::AutoMutex<> guard(this->Task::m_lock);
    //- get both timestamp and elapsed secs
	  yat::Timestamp now;
    _COPY_TIMESTAMP(this->m_last_update_ts, ts);
	  _GET_TIME(now);
	  elapsed_secs = _ELAPSED_SEC(this->m_last_update_ts, now);
  }

  //- Returns the "status expired" error counter
  inline unsigned long status_expired_error_counter (void)
  {
    unsigned long c = 0;
    {
      //- enter critical section
      yat::AutoMutex<> guard(this->Task::m_lock);
      // get value
      c = this->m_status_expired_error_counter;
    }
	  return c;
  }

#if defined (_USE_ASYNCH_DR_)

  //- Underlying asynch. HwIO state
  inline const HwIOState asynch_hw_io_state () const
  {
    if (this->m_asynch_hw_io)
      return this->m_asynch_hw_io->state();
    return galil::HWIO_DISCONNECTED;
  }
  
  inline const HwIOHealth & asynch_com_health (void)
  {
    if (this->m_asynch_hw_io)
      return this->m_asynch_hw_io->health();
    
    THROW_DEVFAILED(_CPTC("NO_DATA"),
                    _CPTC("the underlying HwIO is not instanciated yet"),
                    _CPTC("HwStatusProxy::asynch_com_health"));
  }
#endif

  //- suspend the task activity
  void suspend_hw_status_update (void)
		throw (Tango::DevFailed);
  
  //- resume the task activity
  void resume_hw_status_update (void)
		throw (Tango::DevFailed);
  
protected:
	//- handle_message -----------------------
	virtual void handle_message (yat::Message& msg)
		throw (yat::Exception);

private:
  //- singleton instanciation --------------
  static void init ()
		throw (Tango::DevFailed);

  //- singleton release --------------------
  static void close ()
		throw (Tango::DevFailed);
	
  //- The following members are NOT thread safe
  //-------------------------------------------

  //- the singleton ------------------------
  static HwStatusProxy * singleton;

	//- ctor ---------------------------------
	HwStatusProxy ();

	//- dtor ---------------------------------
	virtual ~HwStatusProxy ();

  //- full status update
  void update_all_i ()
		throw (Tango::DevFailed);
  
  //- partial status update 
  void selective_update_i (const UpdateMask& mask)
	  throw (Tango::DevFailed);

  //- QR status update (BINARY)
  void update_qr_status_i (void)
		throw (Tango::DevFailed); 
		
  //- DR update (BINARY)
  void read_dr_i (BinaryData& _bd)
		throw (Tango::DevFailed);

  //- ucode axis status update (ACSII)
  void update_ucode_axis_status_i (void)
		throw (Tango::DevFailed); 

  //- ucode command update (ACSII)
  void update_ucode_cmd_i (void)
		throw (Tango::DevFailed); 

  //- ucode status update (LOCAL COPY)
  void update_ucode_status_i (); 

  //- hardware status update (LOCAL COPY)
  void update_hardware_status_i (); 

  //- check status data expiration 
  void status_data_expired_i ()
		throw (Tango::DevFailed);

#if defined (_USE_ASYNCH_DR_)
  //- called when the async HwIo is closed or open
  void asynch_hwio_hook (HwIOHookOp _op); 
#endif
  
  //- internal impl: galil Header accessor
  inline void header_i (Header& gh)
  {
    gh = this->m_qr_block.gh;
  }

  //- internal impl: I/O accessor
  inline void genio_i (GenIO& gio)
  {
    gio = this->m_qr_block.gio;
  }

  //- internal impl: firmware axis state accessor
  inline void firmware_axis_state_i (AxisIdentifier ai, FirmwareAxisState& fas)
  {
    switch (ai)
    {
      case AXIS_A:
        fas = this->m_qr_block.ab_a;
        break;
      case AXIS_B:
        fas = this->m_qr_block.ab_b;
        break;
      case AXIS_C:
        fas = this->m_qr_block.ab_c;
        break;
      case AXIS_D:
        fas = this->m_qr_block.ab_d;
        break;
#if ! defined(DMC_4143)
      case AXIS_E:
        fas = this->m_qr_block.ab_e;
        break;
      case AXIS_F:
        fas = this->m_qr_block.ab_f;
        break;
      case AXIS_G:
        fas = this->m_qr_block.ab_g;
        break;
      case AXIS_H:
        fas = this->m_qr_block.ab_h;
        break;
#endif
      default:
      	 break;
    }
  }

  //- internal impl: ucode status accessor
  inline void ucode_status_i (UCodeStatus& mcs)
  {
    mcs = m_ucode_status;
  }

  //- internal impl: hardware status accessor
  inline void hardware_status_i (HardwareStatus& hws)
  {
    hws = m_hw_status;
  }

  //- internal impl: ucode axis status accessor
  inline void ucode_axis_state_i (AxisIdentifier ai, UCodeAxisState& mcas)
  {
    mcas = m_uc_axis_state[ai];
  }

  //- internal impl: AxisCommandStatus accessor
  inline void ucode_axis_cmd_state_i (AxisIdentifier ai, UCodeAxisCommandState& ucacs) 
  {
    ucacs = m_uc_axis_cmd_state[ai];
  }

  //- get ref. to the underlying HWI/O port (if valid)
  inline HwIO<yat::NullMutex> * synch_hw_io (void)
		throw (Tango::DevFailed)
  {
    if (! m_synch_hw_io)
    {
      THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                      _CPTC("can not access hardware status information [com. initialization failure]"),
                      _CPTC("HwStatusProxy::hw_io")); 
    }
    return m_synch_hw_io;
  }

  //- task configuration
  Config m_cfg;

  //- I/O port to the <Galil-Box>
  HwIO<yat::NullMutex> * m_synch_hw_io;

#if defined (_USE_ASYNCH_DR_)
  //- UDP asynch I/O port to the <Galil-Box>
  HwIO<yat::NullMutex> * m_asynch_hw_io;
  
  //- get ref. to the underlying DR HWI/O port (if valid)
  inline HwIO<yat::NullMutex> * asynch_hw_io (void)
		throw (Tango::DevFailed)
  {
    if (! m_asynch_hw_io)
    {
      THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                      _CPTC("can not access hardware status information [com. initialization failure]"),
                      _CPTC("HwStatusProxy::hw_io")); 
    }
    return m_asynch_hw_io;
  }
#endif

  //- Galilbox data record
  QRBlock m_qr_block;

  //- uCode status for each axis
  UCodeAxisState m_uc_axis_state[MAX_AXIS];

  //- uCode command for each axis
  UCodeAxisCommandState m_uc_axis_cmd_state[MAX_AXIS];

  //- uCode status
  UCodeStatus m_ucode_status;

  //- Hardware status
  HardwareStatus m_hw_status;

  //- Elasped time since last full status update
  yat::Timestamp m_last_update_ts;

  //- A counter for the so called "status expired error"
  unsigned long m_status_expired_error_counter;

  //- Flag: true if we obtained (at least) one full status
  bool m_updated_once;

  //- periodic update mask 
  UpdateMask m_periodic_update_mask;

  //- hw status update flag
  bool m_suspended;
  
  //- the Galil-box bytes order
  static const yat::Endianness::ByteOrder galil_bytes_order;

  //- unimplemented/unsupported members
  //-----------------------------------
  HwStatusProxy (const HwStatusProxy &);
  HwStatusProxy & operator= (const HwStatusProxy &);
};

} // namespace galil

#endif // _HW_STATUS_PROXY_H_
