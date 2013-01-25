//=============================================================================
// Box.cpp
//=============================================================================
// abstraction.......Galil Box abstraction class
// class.............Box
// original author...N.Leclercq - J.Coquet - SOLEIL
//=============================================================================

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <tango.h>
#include <yat/utils/XString.h>
#include "HwStatusProxy.h"
#if defined(_UNSOLICITED_MSG_READER_SUPPORT_)
# include "UnsolicitedMsgReader.h"
#endif
#include "AxisFactory.h" 
#include "Box.h"
#include "ObjectManager.h"  

// ============================================================================
// IMPL OPTION
// ============================================================================
#define _CLEANUP_CONTROLLER_HANDLES_

// ============================================================================
// SOME CONSTs
// ============================================================================
#define kUNKNOWN_STATUS "unknown status" 
//-----------------------------------------------------------------------------
 
namespace galil 
{ 
 
// ============================================================================
// SINGLETON
// ============================================================================
Box * Box::singleton = 0; 
 
// ============================================================================
// Box::Config::Config
// ============================================================================
Box::Config::Config ()  
  : galilbox_ip_addr (""),  
    galilbox_port (0), 
    status_update_period_ms (kDEFAULT_HWSP_PERIOD),
    status_expiration_timeout_ms (kDEFAULT_STATUS_EXPIRATION_TMO),
    startup_timeout_ms (kDEFAULT_HWSP_STARTUP_TMO),
    host_device(0)
{
  //- noop ctor
}

// ============================================================================
// Box::Config::operator=
// ============================================================================
void Box::Config::operator= (const Config& src)
{
  galilbox_ip_addr = src.galilbox_ip_addr;
  galilbox_port = src.galilbox_port;
  status_update_period_ms = src.status_update_period_ms;
  status_expiration_timeout_ms = src.status_expiration_timeout_ms;
  startup_timeout_ms = src.startup_timeout_ms;
  host_device = src.host_device;
}

// ============================================================================
// Box::Status::Status
// ============================================================================
Box::Status::Status ()
 : box_state(BOX_UNKNOWN),
   com_state (COM_DISCONNECTED),
   status (kUNKNOWN_STATUS)
{
  //- noop ctor
}

// ============================================================================
// Box::Status::operator=
// ============================================================================
void Box::Status::operator= (const Status& src)
{
  this->box_state = src.box_state;
  this->com_state = src.com_state;
  this->status = src.status;
}

// ============================================================================
// Box::init <STATIC MEMBER>
// ============================================================================
void Box::init (const Config& _cfg) 
	throw (Tango::DevFailed)
{
  YAT_TRACE_STATIC("Box::init");
  
  //- instanciate the Box::singleton 
  try
  {
  	//- actual instanciation
    Box::singleton = new Box(_cfg);
    if (Box::singleton == 0)
      throw std::bad_alloc();
  }
  catch (const std::bad_alloc&)
  {
    THROW_DEVFAILED(_CPTC("OUT_OF_MEMORY"),
                    _CPTC("Box::singleton instanciation failed"),
                    _CPTC("galil::Box::init"));	
  }
  catch (Tango::DevFailed& df)
  {
    RETHROW_DEVFAILED(df,
                      _CPTC("SOFTWARE_ERROR"),
                      _CPTC("Tango exception caught while while trying to initialize the Box::singleton"),
                      _CPTC("galil::Box::init"));
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught while while trying to initialize the Box::singleton"),
                    _CPTC("galil::Box::init"));
  }
  
  try
  {
    //- (re)open the shared HW I/O port
    SHARED_HW_IO->reopen(_cfg.galilbox_ip_addr, _cfg.galilbox_port);

#if defined (_CLEANUP_CONTROLLER_HANDLES_) || defined (_USE_ASYNCH_DR_)
    //- suspend periodic hw status update (about to close its connection to the controller)
    HW_STATUS_PROXY->suspend_hw_status_update();
   
    //- if starting up, then cleanup all connections to the control
    SHARED_HW_IO->cleanup();
#endif
  }
  catch (Tango::DevFailed& df)
  {
#if defined (_CLEANUP_CONTROLLER_HANDLES_) || defined (_USE_ASYNCH_DR_)
    //- resume periodic hw status update
    HW_STATUS_PROXY->resume_hw_status_update();
#endif
    RETHROW_DEVFAILED(df,
                      _CPTC("SOFTWARE_ERROR"),
                      _CPTC("Tango exception caught during initialization sequence"),
                      _CPTC("galil::Box::init"));
  }
  catch (...)
  {
#if defined (_CLEANUP_CONTROLLER_HANDLES_) || defined (_USE_ASYNCH_DR_)
    //- resume periodic hw status update
    HW_STATUS_PROXY->resume_hw_status_update();
#endif
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught during initialization sequence"),
                    _CPTC("galil::Box::init"));
  }

  //- (re)configure the HwStatusProxy::singleton
  try
  {
    //- setup the <HwStatusProxy> singleton config
    HwStatusProxy::Config sm_cfg;
    sm_cfg.galilbox_ip_addr = _cfg.galilbox_ip_addr;
    sm_cfg.galilbox_port = _cfg.galilbox_port + 1;
    sm_cfg.status_update_period_ms = _cfg.status_update_period_ms;
    sm_cfg.status_expiration_timeout_ms = _cfg.status_expiration_timeout_ms;
    sm_cfg.startup_timeout_ms = _cfg.startup_timeout_ms;
    
    //- (re)configure the <HwStatusProxy>
    HW_STATUS_PROXY->configure(sm_cfg);
    
#if defined (_CLEANUP_CONTROLLER_HANDLES_) || defined (_USE_ASYNCH_DR_)
    //- resume  periodic hw status update
    HW_STATUS_PROXY->resume_hw_status_update();
#endif
  }
  catch (Tango::DevFailed& df)
  {
    RETHROW_DEVFAILED(df,
                      _CPTC("SOFTWARE_ERROR"),
                      _CPTC("Tango exception caught while while trying to setup the HwStatusProxy"),
                      _CPTC("galil::Box::init"));
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught while while trying to setup the HwStatusProxy"),
                    _CPTC("galil::Box::init"));
  }

#if defined(_UNSOLICITED_MSG_READER_SUPPORT_)
  //- (re)configure the UnsolicitedMsgReader::singleton
  try
  {
    //- setup the <UnsolicitedMsgReader> singleton config
    UnsolicitedMsgReader::Config usmr_cfg;
    usmr_cfg.galilbox_ip_addr = _cfg.galilbox_ip_addr;
    usmr_cfg.galilbox_port = _cfg.galilbox_port + 2;
    UNSOLICITED_MSG_READER->configure(usmr_cfg);
  }
  catch (Tango::DevFailed& df)
  {
    RETHROW_DEVFAILED(df,
                      _CPTC("SOFTWARE_ERROR"),
                      _CPTC("Tango exception caught while while trying to setup the UnsolicitedMsgReader"),
                      _CPTC("galil::Box::init"));
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught while while trying to setup the UnsolicitedMsgReader"),
                    _CPTC("galil::Box::init"));
  }
#endif //- _UNSOLICITED_MSG_READER_SUPPORT_
}

// ============================================================================
// Box::close  <STATIC MEMBER>
// ============================================================================
void Box::close ()
	throw (Tango::DevFailed)
{
  YAT_TRACE_STATIC("Box::close");

  try
  {
    //- release the Box::singleton
    SAFE_DELETE_PTR(Box::singleton);
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                    _CPTC("an error occured while trying to shutdown the Box::singleton"),
                    _CPTC("galil::Box::close"));
  }
}

// ============================================================================
// Box::Box
// ============================================================================
Box::Box (const Config& _cfg)
  : Tango::LogAdapter(_cfg.host_device), 
    m_cfg (_cfg)
{
	YAT_TRACE("galil::Box::Box");
}

// ============================================================================
// Box::~Box
// ============================================================================
Box::~Box (void)
{
	YAT_TRACE("galil::Box::~Box");
}

// ============================================================================
// Box::reset
// ============================================================================
void Box::reset () throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Box::reset");

  try
  {
    //- write RS on the IO port
    SHARED_HW_IO->in(std::string("RS\r"), IOCheckError);
  }
  catch (Tango::DevFailed& df)
  {
    ERROR_STREAM << df << std::endl;
    RETHROW_DEVFAILED(df,
                      _CPTC("COMMUNICATION_FAILED"),
                      _CPTC("could not reset the <Galil-Box> controller "),
                      _CPTC("galil::Box::reset"));
  }
  catch (...)
  {
    ERROR_STREAM << "Box::reset::unknown exception caught" << std::endl;
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught while trying to reset the <Galil-Box> controller"),
                    _CPTC("galil::Box::reset")); 
  }
}

// ============================================================================
// Box::start_ucode
// ============================================================================
void Box::start_ucode () throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Box::start_ucode");

  try
  {
    //- write <XQ#AUTO> on the IO port
    SHARED_HW_IO->in(std::string("XQ#AUTO\r"), IOCheckError);
  }
  catch (Tango::DevFailed& df)
  {
    yat::OSStream err_desc;
    err_desc << "could not start uCode on the <Galil-Box> controller ";
    try 
    {
      galil::GenIO gio;
      HW_STATUS_PROXY->genio(gio);
      if (gio.program_running())
        err_desc << "[uCode already running]";
      else
        err_desc << "[unknown reason]";
    }
    catch (...)
    {
       err_desc << "[unknown reason - too many errors]";
    }
    ERROR_STREAM << df << std::endl;
    THROW_DEVFAILED(_CPTC("COMMAND_FAILED"),
                    _CPTC(err_desc.str().c_str()),
                    _CPTC("galil::Box::start_ucode"));
  }
  catch (...)
  {
    ERROR_STREAM << "Box::start_ucode::unknown exception caught" << std::endl;
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught while trying to start uCode on the <Galil-Box> controller"),
                    _CPTC("galil::Box::start_ucode")); 
  }
}

// ============================================================================
// Box::ucode_version
// ============================================================================
const std::string Box::ucode_version () throw (Tango::DevFailed)
{
	YAT_TRACE("galil::Box::ucode_version");
 
	//- new style (V2.2 and later) �code variable
  try
  {
		std::string gen_rev;
		std::string spe_rev;
		
    //- get the McRev 
    SHARED_HW_IO->inout(std::string("McRevGen=?\r"), gen_rev);
    SHARED_HW_IO->inout(std::string("McRevSpe=?\r"), spe_rev);
    
		YAT_LOG("galil::Box::ucode_version::controller returned (as string): " 
			      << "\n generic revision  : " << gen_rev
			      << "\n specific revision : " << spe_rev);

		unsigned long ucode_gen_rev = yat::XString<unsigned long>::to_num(gen_rev);
		unsigned long ucode_spe_rev = yat::XString<unsigned long>::to_num(spe_rev);
  
		YAT_LOG("galil::Box::ucode_version::controller returned (as ulong): " 
			      << "\n generic revision  : " << ucode_gen_rev
			      << "\n specific revision : " << ucode_spe_rev);

		unsigned long generic_rev       = (ucode_gen_rev & 0xFF000000) / 0x1000000; 
		unsigned long generic_evo       = (ucode_gen_rev & 0x00FF0000) / 0x0010000;
		unsigned long generic_evo_minor = (ucode_gen_rev & 0x0000FF00) / 0x0000100;
		unsigned long generic_evo_min_dev = (ucode_gen_rev & 0x000000FF);

		unsigned long specific_rev       = (ucode_spe_rev & 0xFF000000) / 0x1000000;
		unsigned long specific_evo       = (ucode_spe_rev & 0x00FF0000) / 0x0010000;
		unsigned long specific_evo_minor = (ucode_spe_rev & 0x0000FF00) / 0x0000100;
		unsigned long specific_evo_min_dev = (ucode_spe_rev & 0x000000FF);

		yat::OSStream txt;
		txt << "Generic uCode : " 
				<< generic_rev 
				<< "." 
				<< generic_evo 
				<< "."
				<< generic_evo_minor 
				<< "."
        << generic_evo_min_dev
				<< std::endl
				<< "Specific uCode : " 
				<< specific_rev 
				<< "." 
				<< specific_evo
				<< "." 
				<< specific_evo_minor
				<< "." 
				<< specific_evo_min_dev;

		return txt.str();  
	}
	catch (yat::Exception & ye)
  {
    _YAT_TO_TANGO_EXCEPTION(ye, te);
    ERROR_STREAM << te << std::endl;
    RETHROW_DEVFAILED(te,
                      _CPTC("SOFTWARE_ERROR"),
                      _CPTC("yat::Exception caught while trying to obtain Box::ucode_version"),
                      _CPTC("Box::ucode_version"));
  }
  catch (Tango::DevFailed& df)
  {
    ERROR_STREAM << "Box::�code_version trying to get �code versions McRevGen and McRevSpe catched DevFailed : \n" 
			           << df 
								 << std::endl;
		//- do not rethrow, let a chance to old McRev variable
  }
  catch (...)
  {
    ERROR_STREAM << "trying to get �code versions McRevGen and McRevSpe catched DevFailed : " 
			           << "Box::ucode_version::unknown exception caught" << std::endl;
		//- do not throw, let a chance to old McRev variable
  }

	//- old style (V2.1 and before) �code variable
  std::string ucode_rev_str;
  try
  {
    //- get the McRev 
    SHARED_HW_IO->inout(std::string("McRev=?\r"), ucode_rev_str);
  }
  catch (Tango::DevFailed& df)
  {
    ERROR_STREAM << df << std::endl;
    RETHROW_DEVFAILED(df,
                      _CPTC("COMMUNICATION_FAILED"),
                      _CPTC("could not obtain the uCode version number"),
                      _CPTC("galil::Box::ucode_version"));
  }
  catch (...)
  {
    ERROR_STREAM << "Box::ucode_version::unknown exception caught" << std::endl;
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught while trying to obtain the uCode version number"),
                    _CPTC("galil::Box::ucode_version")); 
  }

  YAT_LOG("galil::Box::ucode_version::controller returned (as string): " << ucode_rev_str);

  unsigned long ucode_rev = 0; 
  try
  {
    ucode_rev = yat::XString<unsigned long>::to_num(ucode_rev_str);
  }
  catch (yat::Exception & ye)
  {
    _YAT_TO_TANGO_EXCEPTION(ye, te);
    ERROR_STREAM << te << std::endl;
    RETHROW_DEVFAILED(te,
                      _CPTC("SOFTWARE_ERROR"),
                      _CPTC("yat::Exception caught while trying to obtain Box::ucode_version"),
                      _CPTC("Box::ucode_version"));
  }
  
  YAT_LOG("galil::Box::ucode_version::controller returned (as ulong): " << ucode_rev);

  unsigned long generic_rev = (ucode_rev & 0xFF000000) / 0x1000000; 
	unsigned long generic_evo = (ucode_rev & 0x00FF0000) / 0x0010000;

  unsigned long specific_rev = (ucode_rev & 0x0000FF00) / 0x0000100;
  unsigned long specific_evo = ucode_rev & 0x000000FF;

	yat::OSStream txt;
  txt << "Generic uCode: " 
      << generic_rev 
      << "." 
      << generic_evo 
      << " - "
      << "Specific uCode: " 
      << specific_rev 
      << "." 
      << specific_evo;

  return txt.str();
}

// ============================================================================
// Box::firmware_version
// ============================================================================
const std::string Box::firmware_version () throw (Tango::DevFailed)
{
	YAT_TRACE("galil::Box::firmware_version");
 
  std::string firmware_rev_str;

  try
  {
    //- build binary command: ^R ^V
    BinaryData firmware_request_cmd(3);
    firmware_request_cmd[0] = 18;
    firmware_request_cmd[1] = 22;
    firmware_request_cmd[2] = '\r';
    firmware_request_cmd.force_length(3);

    //- write binary command on the IO port
    SHARED_HW_IO->inout(firmware_request_cmd, firmware_rev_str);
  }
  catch (Tango::DevFailed& df)
  {
    ERROR_STREAM << df << std::endl;
    RETHROW_DEVFAILED(df,
                      _CPTC("COMMUNICATION_FAILED"),
                      _CPTC("could not obtain the firmware version number"),
                      _CPTC("galil::Box::firmware_version"));
  }
  catch (...)
  {
    ERROR_STREAM << "Box::ucode_version::unknown exception caught" << std::endl;
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught while trying to obtain the firmware version number"),
                    _CPTC("galil::Box::firmware_version")); 
  }

  return firmware_rev_str;
}

// ============================================================================
// Box::status
// ============================================================================
void Box::status (Box::Status & status_) throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Box::status");

  std::string last_tc_error("no error reported");

  //- get current status of the shared connection to the controller

  std::string shared_hwio_status;

  const galil::HwIOState shared_hwio_state = SHARED_HW_IO->state();

  status_.box_state = BOX_UNKNOWN;

  switch (shared_hwio_state)
  {
    case galil::HWIO_DISCONNECTED:
      status_.box_state = BOX_CLOSE;
      status_.com_state = COM_DISCONNECTED;
      shared_hwio_status = "shared com. status: disconnected";
      break;
    case galil::HWIO_DISCONNECTED_ON_COM_ERROR:
      status_.box_state = BOX_WARNING;
      status_.com_state = COM_DISCONNECTED_ON_COM_ERROR;
      shared_hwio_status = "shared com. status: disconnected on error";
      break;
    case galil::HWIO_CONNECTING:
      status_.box_state = BOX_CLOSE;
      status_.com_state = COM_CONNECTING;
      shared_hwio_status = "shared com. status: connecting";
      break;
    case galil::HWIO_CONNECTED:
      status_.box_state = BOX_OPEN;
      status_.com_state = COM_CONNECTED;
      shared_hwio_status = "shared com. status: connected";
      break;
    case galil::HWIO_DISCONNECTING:
      status_.box_state = BOX_OPEN;
      status_.com_state = COM_CONNECTED;
      shared_hwio_status = "shared com. status: disconnecting";
      break;
    case galil::HWIO_CONNECTION_FAILED:
      status_.box_state = BOX_ERROR;
      status_.com_state = COM_CONNECTION_FAILED;
      shared_hwio_status = "shared com. status: connection failed";
      break;
    case galil::HWIO_FAULT:
    default:
      status_.box_state = BOX_ERROR;
      status_.com_state = COM_FAULT;
      shared_hwio_status = "shared com. status: unknown";
      break;
  }
  
  //- get current health of the shared connection to the controller
  std::string shared_hwio_health_status("");

  const galil::HwIOHealth & shared_hwio_health = SHARED_HW_IO->health();
  
  shared_hwio_health_status = "shared com. port health:\n";

  std::string hwio_startup_str;
  _TIMESTAMP_TO_DATE(shared_hwio_health.startup_ts, hwio_startup_str);
  shared_hwio_health_status += " * started on: "
                            + hwio_startup_str
                            + std::string("\n");
                            
  shared_hwio_health_status += " * com. handle: IH"
                            + shared_hwio_health.com_handle
                            + std::string("\n");
                            
  shared_hwio_health_status += " * last successfull connection: "
                            + shared_hwio_health.success_ts_str
                            + std::string("\n");
  
  shared_hwio_health_status += " * last connection failure: "
                            + shared_hwio_health.failure_ts_str
                            + std::string("\n"); 
                            
  yat::OSStream oss;
  oss << " * com. errors: "
      << shared_hwio_health.com_errors
      << "\n";                         
  shared_hwio_health_status += oss.str();
      
  oss.str("");                                  
  oss << " * com success: "
      << shared_hwio_health.com_success
      << "\n";                                                       
  shared_hwio_health_status += oss.str(); 

  if (shared_hwio_state == galil::HWIO_CONNECTED)   
  {
    shared_hwio_health_status += " * stayed disconnected: "
       + yat::XString<long>::to_string(static_cast<long>(shared_hwio_health.dt))
       + std::string(" seconds\n");
       
    shared_hwio_health_status += " * connection tries: "
       + yat::XString<unsigned long>::to_string(shared_hwio_health.retries);
  }
  else 
  {
    shared_hwio_health_status += " * disconnection time:  "
       + yat::XString<double>::to_string(shared_hwio_health.dt)
       + std::string(" seconds\n");
       
    shared_hwio_health_status += " * connection retries: "
       + yat::XString<unsigned long>::to_string(shared_hwio_health.retries);
  } 
  
  //- get current status of the HWStatusProxy connection to the controller
  std::string synch_hwsp_hwio_status;

  const galil::HwIOState synch_hwsp_hwio_state = HW_STATUS_PROXY->synch_hw_io_state();

  switch (synch_hwsp_hwio_state)
  {
    case galil::HWIO_DISCONNECTED:
      synch_hwsp_hwio_status = "hwsp synch. com. status: disconnected";
      break;
    case galil::HWIO_DISCONNECTED_ON_COM_ERROR:
      synch_hwsp_hwio_status = "hwsp synch. com. status: disconnected on error";
      break;
    case galil::HWIO_CONNECTING:
      synch_hwsp_hwio_status = "hwsp synch. com. status: connecting";
      break;
    case galil::HWIO_CONNECTED:
      synch_hwsp_hwio_status = "hwsp synch. com. status: connected";
      break;
    case galil::HWIO_DISCONNECTING:
       synch_hwsp_hwio_status = "hwsp synch. com. status: disconnecting";
      break;
    case galil::HWIO_CONNECTION_FAILED:
      synch_hwsp_hwio_status = "hwsp synch. com. status: connection failed";
      break;
    case galil::HWIO_FAULT:
      synch_hwsp_hwio_status = "hwsp synch. com. status: fault";
      break;
    default:
      synch_hwsp_hwio_status = "hwsp synch. com. status: unknown";
      break;
  }

  //- get current health of the HWStatusProxy connection to the controller
  std::string synch_hwsp_hwio_health_status("");

  try
  {
    const galil::HwIOHealth & synch_hwsp_hwio_health = HW_STATUS_PROXY->synch_com_health();

    synch_hwsp_hwio_health_status = "hwsp synch. com. port health:\n";

    _TIMESTAMP_TO_DATE(synch_hwsp_hwio_health.startup_ts, hwio_startup_str);
    synch_hwsp_hwio_health_status += " * started on: "
                            + hwio_startup_str
                            + std::string("\n");
                            
    synch_hwsp_hwio_health_status += " * com. handle: IH"
                            + synch_hwsp_hwio_health.com_handle
                            + std::string("\n");
                            
    synch_hwsp_hwio_health_status += " * last successfull connection: "
                            + synch_hwsp_hwio_health.success_ts_str
                            + std::string("\n");
  
    synch_hwsp_hwio_health_status += " * last connection failure: "
                            + synch_hwsp_hwio_health.failure_ts_str
                            + std::string("\n"); 
    
    oss.str(""); 
    oss << " * com. errors: "
        << synch_hwsp_hwio_health.com_errors
        << "\n";                         
    synch_hwsp_hwio_health_status += oss.str();
      
    oss.str("");                                  
    oss << " * com success: "
        << synch_hwsp_hwio_health.com_success
        << "\n";                                                       
    synch_hwsp_hwio_health_status += oss.str();    
                            
    last_tc_error = "TC error: " + synch_hwsp_hwio_health.tc_error_txt;                                               

    if (synch_hwsp_hwio_state == galil::HWIO_CONNECTED)   
    {
      synch_hwsp_hwio_health_status += " * stayed disconnected: "
         + yat::XString<long>::to_string(static_cast<long>(synch_hwsp_hwio_health.dt))
         + std::string(" seconds\n");
         
      synch_hwsp_hwio_health_status += " * connection tries: "
         + yat::XString<unsigned long>::to_string(synch_hwsp_hwio_health.retries);

      synch_hwsp_hwio_health_status += "\n";
    }
    else 
    {
      synch_hwsp_hwio_health_status += " * disconnection time:  "
         + yat::XString<double>::to_string(synch_hwsp_hwio_health.dt)
         + std::string(" seconds\n");

      synch_hwsp_hwio_health_status += " * connection retries: "
         + yat::XString<unsigned long>::to_string(synch_hwsp_hwio_health.retries);

      synch_hwsp_hwio_health_status += "\n";
    } 

    yat::Timestamp lupdt_ts;
    double lupdt_elapsed_secs;
    HW_STATUS_PROXY->last_update_timestamp(lupdt_ts, lupdt_elapsed_secs);
    std::string update_ts_str = " * status updated on ";
    _TIMESTAMP_TO_DATE(lupdt_ts, update_ts_str); 

    oss.str("");  
    oss << " * status updated on "
        << update_ts_str
        << " [" 
        << lupdt_elapsed_secs 
        << " secs ago]"
        << "\n";                                                       
    synch_hwsp_hwio_health_status += oss.str();  
    
    oss.str("");  
    oss << " * status expired errors: "
        << HW_STATUS_PROXY->status_expired_error_counter()
        << "\n"; 
    synch_hwsp_hwio_health_status += oss.str(); 
  }
  catch (...)
  {
    synch_hwsp_hwio_health_status = "there is currently no hwsp synch. com. health info\n";
  }

#if defined (_USE_ASYNCH_DR_)
  //- get current status of the HWStatusProxy connection to the controller
  std::string asynch_hwsp_hwio_status;

  const galil::HwIOState asynch_hwsp_hwio_state = HW_STATUS_PROXY->synch_hw_io_state();

  switch (asynch_hwsp_hwio_state)
  {
    case galil::HWIO_DISCONNECTED:
      asynch_hwsp_hwio_status = "hwsp asynch. com. status: disconnected";
      break;
    case galil::HWIO_DISCONNECTED_ON_COM_ERROR:
      asynch_hwsp_hwio_status = "hwsp asynch. com. status: disconnected on error";
      break;
    case galil::HWIO_CONNECTING:
      asynch_hwsp_hwio_status = "hwsp asynch. com. status: connecting";
      break;
    case galil::HWIO_CONNECTED:
      asynch_hwsp_hwio_status = "hwsp asynch. com. status: connected";
      break;
    case galil::HWIO_DISCONNECTING:
      asynch_hwsp_hwio_status = "hwsp asynch. com. status: disconnecting";
      break;
    case galil::HWIO_CONNECTION_FAILED:
      asynch_hwsp_hwio_status = "hwsp asynch. com. status: connection failed";
      break;
    case galil::HWIO_FAULT:
      asynch_hwsp_hwio_status = "hwsp asynch. com. status: fault";
      break;
    default:
      asynch_hwsp_hwio_status = "hwsp asynch. com. status: unknown";
      break;
  }
  
  //- get current health of the HWStatusProxy connection to the controller
  std::string asynch_hwsp_hwio_health_status("");
  
  try
  {
    const galil::HwIOHealth & asynch_hwsp_hwio_health = HW_STATUS_PROXY->asynch_com_health();

    asynch_hwsp_hwio_health_status = "hwsp asynch. com. port health:\n";

    _TIMESTAMP_TO_DATE(asynch_hwsp_hwio_health.startup_ts, hwio_startup_str);
    asynch_hwsp_hwio_health_status += " * started on: "
                            + hwio_startup_str
                            + std::string("\n");
                            
    asynch_hwsp_hwio_health_status += " * com. handle: IH"
                            + asynch_hwsp_hwio_health.com_handle
                            + std::string("\n");
                            
    asynch_hwsp_hwio_health_status += " * last successfull connection: "
                            + asynch_hwsp_hwio_health.success_ts_str
                            + std::string("\n");
  
    asynch_hwsp_hwio_health_status += " * last connection failure: "
                            + asynch_hwsp_hwio_health.failure_ts_str
                            + std::string("\n"); 
    
    oss.str(""); 
    oss << " * com. errors: "
        << asynch_hwsp_hwio_health.com_errors
        << "\n";                         
    asynch_hwsp_hwio_health_status += oss.str();
      
    oss.str("");                                  
    oss << " * com success: "
        << asynch_hwsp_hwio_health.com_success
        << "\n";                                                       
    asynch_hwsp_hwio_health_status += oss.str();                                                                          

    if (asynch_hwsp_hwio_state == galil::HWIO_CONNECTED)   
    {
      asynch_hwsp_hwio_health_status += " * stayed disconnected: "
         + yat::XString<long>::to_string(static_cast<long>(asynch_hwsp_hwio_health.dt))
         + std::string(" seconds\n");
         
      asynch_hwsp_hwio_health_status += " * connection tries: "
         + yat::XString<unsigned long>::to_string(asynch_hwsp_hwio_health.retries);
    }
    else 
    {
      asynch_hwsp_hwio_health_status += " * disconnection time:  "
         + yat::XString<double>::to_string(asynch_hwsp_hwio_health.dt)
         + std::string(" seconds\n");

      asynch_hwsp_hwio_health_status += " * connection retries: "
         + yat::XString<unsigned long>::to_string(asynch_hwsp_hwio_health.retries);
    } 
  }
  catch (...)
  {
    asynch_hwsp_hwio_health_status = "there is currently no hwsp asynch. com. health info\n";
  }
#endif //- _USE_ASYNCH_DR_

#if defined(_UNSOLICITED_MSG_READER_SUPPORT_)
  //- get current status of the UnsolicitedMsgReader connection to the controller
  std::string umr_hwio_status;
  const galil::HwIOState umr_hwio_state = UNSOLICITED_MSG_READER->hw_io_state();
  switch (umr_hwio_state)
  {
    case galil::HWIO_DISCONNECTED:
      umr_hwio_status = "umr com. status: disconnected";
      break;
    case galil::HWIO_DISCONNECTED_ON_COM_ERROR:
      umr_hwio_status = "umr com. status: disconnected on error";
      break;
    case galil::HWIO_CONNECTING:
      umr_hwio_status = "umr com. status: connecting";
      break;
    case galil::HWIO_CONNECTED:
     umr_hwio_status = "umr com. status: connected";
      break;
    case galil::HWIO_DISCONNECTING:
      umr_hwio_status = "umr com. status: disconnecting";
      break;
    case galil::HWIO_CONNECTION_FAILED:
      umr_hwio_status = "umr com. status: connection failed";
      break;
    case galil::HWIO_FAULT:
      umr_hwio_status = "umr com. status: fault";
      break;
    default:
      umr_hwio_status = "umr com. status: unknown";
      break;
  }
  
  //- get current health of the UnsolicitedMsgReader connection to the controller
  std::string umr_hwio_health_status("");

  try
  {
    const galil::HwIOHealth & umr_hwio_health = UNSOLICITED_MSG_READER->health();
  
    umr_hwio_health_status = "umr com. port health:\n";

    _TIMESTAMP_TO_DATE(umr_hwio_health.startup_ts, hwio_startup_str);
    shared_hwio_health_status += " * started on: "
                            + hwio_startup_str
                            + std::string("\n");
                            
    umr_hwio_health_status += " * com. handle: IH"
                            + umr_hwio_health.com_handle
                            + std::string("\n");
                            
    umr_hwio_health_status += " * last successfull connection: "
                            + umr_hwio_health.success_ts_str
                            + std::string("\n");
  
    umr_hwio_health_status += " * last connection failure: "
                            + umr_hwio_health.failure_ts_str
                            + std::string("\n"); 
                            
    oss.str(""); 
    oss << " * com. errors: "
        << umr_hwio_health.com_errors
        << "\n";                         
    umr_hwio_health_status += oss.str();
      
    oss.str("");                                  
    oss << " * com success: "
        << umr_hwio_health.com_success
        << "\n";                                                       
    umr_hwio_health_status += oss.str(); 
       
    if (umr_hwio_state == galil::HWIO_CONNECTED)   
    {
      umr_hwio_health_status += " * stayed disconnected: "
         + yat::XString<long>::to_string(static_cast<long>(umr_hwio_health.dt))
         + std::string(" seconds\n");
         
      umr_hwio_health_status += " * connection tries: "
         + yat::XString<unsigned long>::to_string(umr_hwio_health.retries);
    }
    else 
    {
      umr_hwio_health_status += " * disconnection time:  "
         + yat::XString<double>::to_string(umr_hwio_health.dt)
         + std::string(" seconds\n");

      umr_hwio_health_status += " * connection retries: "
         + yat::XString<unsigned long>::to_string(umr_hwio_health.retries);
    } 
  }
  catch (...)
  {
    umr_hwio_health_status = "there is currently no umr health info\n";
  }
#endif //- _UNSOLICITED_MSG_READER_SUPPORT_

  //- build status 

  std::string ucode_status;
#if defined (_USE_ASYNCH_DR_)
  if (asynch_hwsp_hwio_state == galil::HWIO_CONNECTED)
#else
  if (synch_hwsp_hwio_state == galil::HWIO_CONNECTED)
#endif
  {
    try
    {
      galil::GenIO gio;
      HW_STATUS_PROXY->genio(gio);
      ucode_status = gio.program_running() 
                   ? std::string("uCode is running") 
                   : std::string("uCode is NOT running"); 
                   
      if (! gio.program_running() && status_.box_state != BOX_ERROR)
        status_.box_state = BOX_WARNING;
    }
    catch (const Tango::DevFailed & df)
    {
      ucode_status  = "unknown uCode status - HWSP failure ["; 
      ucode_status += std::string(df.errors[0].desc);
      ucode_status += "]"; 
      status_.box_state = BOX_ERROR;
    }
  }
  else
  {
    ucode_status = "unknown uCode status - HWSP disconnected";
    status_.box_state = BOX_ERROR;
  }
  
  status_.status = ucode_status
                 + std::string("\n\n") 
                 + last_tc_error
                 + std::string("\n\n")
                 + shared_hwio_status 
                 + std::string("\n") 
                 + shared_hwio_health_status
                 + std::string("\n\n") 
                 + synch_hwsp_hwio_status
                 + std::string("\n")
                 + synch_hwsp_hwio_health_status
#if defined (_USE_ASYNCH_DR_)
                 + std::string("\n\n")
                 + asynch_hwsp_hwio_status
                 + std::string("\n")
                 + asynch_hwsp_hwio_health_status  
#endif                                                  
#if defined(_UNSOLICITED_MSG_READER_SUPPORT_)
                 + std::string("\n\n")
                 + umr_hwio_status
                 + std::string("\n")
                 + umr_hwio_health_status
#endif
                 ;
}


// ============================================================================
// Box::exec_low_level_cmd
// ============================================================================
const std::string Box::exec_low_level_cmd (const std::string& _cmd) 
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::Box::exec_low_level_cmd");

  string low_level_cmd = _cmd;
  
  if (_cmd.find_last_of("\r") == std::string::npos)
  {
    low_level_cmd += "\r"; 
  }

  std::string reply;

  try
  { 
    SHARED_HW_IO->inout(low_level_cmd, reply);
  }
  catch (Tango::DevFailed& df)
  {
    ERROR_STREAM << df << std::endl;
    RETHROW_DEVFAILED(df,
                      _CPTC("COMMUNICATION_FAILED"),
                      _CPTC("Tango exception caught while excuting a cmd on the <Galil-Box> controller"),
                      _CPTC("galil::Box::exec_low_level_cmd"));
  }
  catch (...)
  {
    ERROR_STREAM << "Box::reset::unknown exception caught" << std::endl;
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught while excuting a cmd on the <Galil-Box> controller"),
                    _CPTC("galil::Box::exec_low_level_cmd")); 
  }

  return reply;
}

} // namespace galil
