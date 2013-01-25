//=============================================================================
// HwStatusProxy.cpp
//=============================================================================
// abstraction.......Galil global status manager abstraction class
// class.............HwStatusProxy
// original author...N.Leclercq - SOLEIL
//=============================================================================

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <tango.h>
#include <yat/utils/XString.h>
#include "HwStatusProxy.h"

// ============================================================================
// SOME CONSTs
// ============================================================================
#if defined(_USE_ASYNCH_DR_)
# define kEXPECTED_QR_SIZE_IN_BYTES 268 //- DR(ISTABCDEFGH) = 268 + end-char
#else
# if defined(DMC_4143)
#  define kEXPECTED_QR_SIZE_IN_BYTES 201 //- DR(IABCD)   = 200 + end-char
# else
#  define kEXPECTED_QR_SIZE_IN_BYTES 345 //- DR(IABCDEFGH)   = 252 + end-char
# endif
#endif
//-----------------------------------------------------------------------------
#if ! defined(_USE_ASYNCH_DR_)
# if defined(DMC_4143)
#  define UPDATE_DR						"QRIABCD\r"
# else
#  define UPDATE_DR 					"QRIABCDEFGH\r"
# endif
#endif
#define GET_TC_ERROR				  "TC1\r"
#define UPDATE_ALL_AXIS_STAT	"QU Stat[],,,1\r"  
#define UPDATE_ALL_AXIS_CMD 	"QU Cmd[],,,1\r"   
//-----------------------------------------------------------------------------
#define GALIL_LAST_ANS_CHAR 	':'  
//-----------------------------------------------------------------------------
#define kMIN_TC_CODE_INDEX			static_cast<unsigned char>(0)
#define kMAX_TC_CODE_INDEX			static_cast<unsigned char>(133)
#define kUNKNOWN_TC_CODE_INDEX	kMAX_TC_CODE_INDEX + 1  
//-----------------------------------------------------------------------------
#define kDEFAULT_PERIODIC_UPDATE_MASK \
		UPDATE_QR_STATUS | UPDATE_UC_AXIS_STATUS | UPDATE_HW_STATUS | UPDATE_UC_STATUS
//-----------------------------------------------------------------------------
#define kDEFAULT_CONTROLLER_ADDR "0.0.0.0"
#define kDEFAULT_CONTROLLER_PORT 0
//-----------------------------------------------------------------------------

// ============================================================================
// SOME USER DEFINED MESSAGES
// ============================================================================
#define kFORCE_STATUS_UPDATE (yat::FIRST_USER_MSG + 1000)
#define kSUSPEND             (yat::FIRST_USER_MSG + 1001)
#define kRESUME              (yat::FIRST_USER_MSG + 1002)
//-----------------------------------------------------------------------------

namespace galil  
{

// ============================================================================
// SINGLETON
// ============================================================================
HwStatusProxy * HwStatusProxy::singleton = 0;

// ============================================================================
// STATIC MEMBERS
// ============================================================================
const yat::Endianness::ByteOrder 
HwStatusProxy::galil_bytes_order = yat::Endianness::BO_LITTLE_ENDIAN;

// ============================================================================
// HwStatusProxy::Config::Config
// ============================================================================
HwStatusProxy::Config::Config () 
 : galilbox_ip_addr (""), 
	 galilbox_port (0),
	 status_update_period_ms (kDEFAULT_HWSP_PERIOD),
	 startup_timeout_ms (kDEFAULT_HWSP_STARTUP_TMO),
	 status_expiration_timeout_ms (kDEFAULT_STATUS_EXPIRATION_TMO)
{
	//- noop ctor
}

// ============================================================================
// HwStatusProxy::Config::operator=
// ============================================================================
void HwStatusProxy::Config::operator= (const Config& src)
{
	galilbox_ip_addr = src.galilbox_ip_addr;
	galilbox_port = src.galilbox_port;
	status_update_period_ms = src.status_update_period_ms;
	startup_timeout_ms = src.startup_timeout_ms;
	status_expiration_timeout_ms = src.status_expiration_timeout_ms;
}

// ============================================================================
// TC ERROR STRING REPOSITORY
// ============================================================================
const char * tc_error_txt[] = 
{
	"no error",
	"unrecognized command",
	"command only valid from program",
	"command not valid in program",
	"operand error",
	"input buffer full",
	"number out of range",
	"command not valid while running",
	"command not valid when not running",
	"variable error",
	"empty program line or undefined label",
	"invalid label or line number",
	"subroutine more than 16 deep",
	"JG only valid when running in jog mode",
	"EEPROM check sum error",
	"EEPROM write error",
	"IP incorrect sign during position mode or IP given during forced deceleration",
	"ED, BN and DL not valid while program running",
	"Command not valid when contouring",
	"application strand already executing",
	"begin not valid with motor off",
	"begin not valid while running",
	"begin not possible due to Limit Switch",
	"unknown error [invalid error code]",
	"begin not valid because no sequence defined",
	"variable not given in IN command",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"S operand not valid",
	"not valid during coordinated move",
	"sequence segment too short",
	"total move distance in a sequence > 2 billion",
	"more than 511 segments in a sequence",
	"VP or CR commands cannot be mixed with LI commands",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"contouring record range error",
	"contour data being sent too slowly",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"gear axis both master and follower",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"not enough fields",
	"question mark not valid",
	"missing \" or string too long",
	"error in {}",
	"question mark part of string",
	"missing [ or []",
	"array index invalid or out of range",
	"bad function or array",
	"bad command response (i.e._GNX)",
	"mismatched parentheses",
	"download error - line too long or too many lines",
	"duplicate or bad label",
	"too many labels",
	"IF statement without ENDIF",
	"unknown error [invalid error code]",
	"IN command must have a comma",
	"array space full",
	"too many arrays or variables",
	"not valid from USB Port",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"IN only valid in task #0",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"record mode already running",
	"no array or source specified",
	"undefined Array",
	"not a valid number",
	"too many elements",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"only A B C D valid operand",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"SM jumper needs to be installed for stepper motor operation",
	"bad binary command format",
	"binary commands not valid in application program",
	"bad binary command number",
	"not valid when running ECAM",
	"improper index into ET (must be 0-256)",
	"no master axis defined for ECAM",
	"master axis modulus greater than 256.EP value",
	"not valid when axis performing ECAM",
	"EB1 command must be given first",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"must be made brushless by BA command",
	"must be made brushless by BA command",
	"BZ command timeout",
	"movement in BZ command",
	"BZ command runaway",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"unknown error [invalid error code]",
	"controller has GL1600 not GL1800",
	"unknown error [invalid error code]",
	"bad Ethernet transmit",
	"bad Ethernet packet received",
	"ethernet input buffer overrun",
	"TCP lost sync",
	"ethernet handle already in use",
	"no ARP response from IP address",
	"closed Ethernet Handle",
	"illegal Modbus Function Code",
	"IP address not valid",
	"unknown error [invalid error code]",
	"illegal IOC command",
	"timeout On Serial Port",
	"analog inputs not present",
	"handle must be UDP",
	"unknown error [error code out of range]"
};

// ============================================================================
// HwStatusProxy::init <STATIC MEMBER>
// ============================================================================
void HwStatusProxy::init ()
	throw (Tango::DevFailed)
{
	YAT_TRACE_STATIC("galil::HwStatusProxy::init");

	//- already instanciated?
	if (HwStatusProxy::singleton)
		return;

	try
	{
		//- instanciate
		HwStatusProxy::singleton = new HwStatusProxy();
		if (HwStatusProxy::singleton == 0)
			throw std::bad_alloc();
	}
	catch (const std::bad_alloc&)
	{
		THROW_DEVFAILED(_CPTC("OUT_OF_MEMORY"),
										_CPTC("galil::HwStatusProxy::singleton allocation failed"),
										_CPTC("galil::HwStatusProxy::init")); 
	}
	catch (Tango::DevFailed& df)
	{
		RETHROW_DEVFAILED(df,
											_CPTC("SOFTWARE_ERROR"),
											_CPTC("galil::HwStatusProxy::singleton initialisation failed"),
											_CPTC("galil::HwStatusProxy::init")); 
	}
	catch (const yat::Exception& ye)
	{		
	  _YAT_TO_TANGO_EXCEPTION(ye, te);
	  throw te;  
	}
	catch (...)
	{
		THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
										_CPTC("galil::HwStatusProxy::singleton initialisation failed [unknown exception]"),
										_CPTC("galil::HwStatusProxy::init")); 
	}
}

// ============================================================================
// HwStatusProxy::close  <STATIC MEMBER>
// ============================================================================
void HwStatusProxy::close ()
	throw (Tango::DevFailed)
{
	YAT_TRACE_STATIC("galil::HwStatusProxy::close");
  
	try
	{
		//- check
		if (! HwStatusProxy::singleton)
			return;
 
		//- ask the underlying yat::Task to exit
		HwStatusProxy::singleton->exit();

		//- reset singleton 
		HwStatusProxy::singleton = 0; 

		//- NEVER CALL DELETE ON <HwStatusProxy::singleton>
		//- see yat::Task impl for more info...
	}
	_HANDLE_YAT_EXCEPTION("yat::Task::exit", "HwStatusProxy::close"); 
}

// ============================================================================
// HwStatusProxy::HwStatusProxy
// ============================================================================
HwStatusProxy::HwStatusProxy ()
	: m_cfg (), 
		m_synch_hw_io (0),
    m_status_expired_error_counter (0),
		m_updated_once (false),
		m_periodic_update_mask (kDEFAULT_PERIODIC_UPDATE_MASK),
		m_suspended (false)
{
	YAT_TRACE("galil::HwStatusProxy::HwStatusProxy");

	//- configure optional msg handling
	this->enable_timeout_msg(false);
	//- enable periodic msgs
	this->enable_periodic_msg(false);
	
	//- create the hw I/O port
	HwIO<yat::NullMutex>::Config synch_hwio_cfg;
	synch_hwio_cfg.ip_addr = kDEFAULT_CONTROLLER_ADDR;
  synch_hwio_cfg.port = kDEFAULT_CONTROLLER_PORT; 
	synch_hwio_cfg.protocol = HWIO_SLOW_SAFE;
	synch_hwio_cfg.blocking = true;
	synch_hwio_cfg.close_on_error = true;
	this->m_synch_hw_io = new HwIO<yat::NullMutex>(synch_hwio_cfg);
	if (this->m_synch_hw_io == 0)
	{
		  THROW_DEVFAILED(_CPTC("OUT_OF_MEMORY"),
										  _CPTC("galil::HwIO instanciation failed"),
										  _CPTC("galil::HwStatusProxy::HwStatusProxy"));	  
	}
	
#if defined (_USE_ASYNCH_DR_)
	//- create the data-record hw I/O port
	HwIO<yat::NullMutex>::Config asynch_hwio_cfg;
	asynch_hwio_cfg.ip_addr = kDEFAULT_CONTROLLER_ADDR;
  asynch_hwio_cfg.port = kDEFAULT_CONTROLLER_PORT + 1;  
	asynch_hwio_cfg.protocol = HWIO_FAST_UNSAFE;
	asynch_hwio_cfg.blocking = false;
	asynch_hwio_cfg.close_on_error = true;
	this->m_asynch_hw_io = new HwIO<yat::NullMutex>(asynch_hwio_cfg);
	if (this->m_asynch_hw_io == 0)
	{
		  THROW_DEVFAILED(_CPTC("OUT_OF_MEMORY"),
										  _CPTC("galil::HwIO instanciation failed [for DR asynch com.]"),
										  _CPTC("galil::HwStatusProxy::HwStatusProxy"));	  
	}
	this->m_asynch_hw_io->register_hook(HwIOHook::instanciate(*this, &HwStatusProxy::asynch_hwio_hook)); 
#endif
}

// ============================================================================
// HwStatusProxy::~HwStatusProxy
// ============================================================================
HwStatusProxy::~HwStatusProxy ()
{
	YAT_TRACE("galil::HwStatusProxy::~HwStatusProxy");
	                
	try
	{
	  //-!!!!! DO NOT CHANGE RELEASE ORDER !!!!!
	  //----------------------------------------
	  
#if defined (_USE_ASYNCH_DR_)
		//- release DR HwI/O port
		SAFE_RELEASE(this->m_asynch_hw_io);
#endif

		//- release HwI/O port
		SAFE_RELEASE(this->m_synch_hw_io);
	}
	catch (...)
	{
		//- ignore any error
	}
}

// ============================================================================
// HwStatusProxy::configure
// ============================================================================
void HwStatusProxy::configure (const Config& _cfg) 
	throw (Tango::DevFailed)
{
	YAT_TRACE("galil::HwStatusProxy::configure");

  yat::Thread::State state; 
  
  { //- enter critical section
	  
	  yat::AutoMutex<yat::Mutex> guard(this->m_lock);

    //- get thread state under critical section
    state = this->state(); 
    
	  //- copy configuration
	  this->m_cfg = _cfg; 

	  //- enable periodic msgs
	  this->enable_periodic_msg(true); 
	  this->set_periodic_msg_period(this->m_cfg.status_update_period_ms);

	  //- be sure <m_cfg.status_expiration_tmo_ms> is set to a <correct> value
	  if (this->m_cfg.status_expiration_timeout_ms < 2 * this->m_cfg.status_update_period_ms)
		  this->m_cfg.status_expiration_timeout_ms = 2 * this->m_cfg.status_update_period_ms;

		//- (re)open connection to peer
	  try
	  {
		  this->m_synch_hw_io->reopen(_cfg.galilbox_ip_addr, _cfg.galilbox_port);

#if defined (_USE_ASYNCH_DR_)
      this->m_asynch_hw_io->reopen(_cfg.galilbox_ip_addr, _cfg.galilbox_port + 1);
#endif
	  }
	  catch (const std::bad_alloc&)
	  {
		  THROW_DEVFAILED(_CPTC("OUT_OF_MEMORY"),
										  _CPTC("failed to connect to the controller [HWIO instanciation failed]"),
										  _CPTC("galil::HwStatusProxy::configure"));	
	  }
	  catch (Tango::DevFailed& df)
	  {
		  RETHROW_DEVFAILED(df,
											  _CPTC("SOFTWARE_MEMORY"),
											  _CPTC("failed to connect to the controller [Tango exception caught]"),
											  _CPTC("galil::HwStatusProxy::configure"));	
	  }
	  catch (...)
	  {
		  THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
										  _CPTC("failed to connect to the controller [unknown exception caught]"),
										  _CPTC("galil::HwStatusProxy::configure"));
	  }

    //- force update (if this succeeds we are sure that the HWSP is running)
	  this->update_all_i();
	  
	  //- init of the last full status update timestamp
	  _GET_TIME(this->m_last_update_ts);
  } //- end of critical section
  
	//- start the task if not running (must be done outside critical section)
	if (state != yat::Thread::STATE_RUNNING)
  {

    try
    {
      //- start the underlying task
		  this->go(_cfg.startup_timeout_ms);
		}
    _HANDLE_YAT_EXCEPTION("yat::Task::go","HwStatusProxy::configure");
  }
}

#if defined (_USE_ASYNCH_DR_)
// ============================================================================
// HwStatusProxy::asynch_hwio_hook
// ============================================================================
void HwStatusProxy::asynch_hwio_hook (HwIOHookOp _op)
{	   
	static bool asynch_com_configured = false;
	     
  switch (_op)
  {
    case HWIO_OPEN:
      { 
        try
        {
           if (asynch_com_configured)
             return;
           YAT_LOG("HwStatusProxy::asynch_hwio_hook::called on OPEN");
           //- start asynch DR
           yat::OSStream oss;
           oss << "DR"
               << this->m_cfg.status_update_period_ms
               << ","
               << this->m_asynch_hw_io->com_handle_identifier()
               << "\r";
           this->m_synch_hw_io->in(oss.str(), IOCheckError);
           asynch_com_configured = true;
         }
         catch (...)
         {
           throw;
         }
      }
      break;
      
    case HWIO_CLOSE:
      {
        try
        {
         if (! asynch_com_configured)
             return;
          YAT_LOG("HwStatusProxy::asynch_hwio_hook::called on CLOSE");
          //- stop asynch DR
          yat::OSStream oss;
          oss << "DR0\r";
          this->m_synch_hw_io->in(oss.str(), IOCheckError);
          asynch_com_configured = false;
          //- empty the UDP socket input buffer
          BinaryData bd(kEXPECTED_QR_SIZE_IN_BYTES);
          this->read_dr_i(bd);
        }
        catch (...)
        {
          //- ignore error
        }
      }
      break;;
      
    default:
      break;
  }
}
#endif

// ============================================================================
// HwStatusProxy::handle_message
// ============================================================================
void HwStatusProxy::handle_message (yat::Message& _msg)
	throw (yat::Exception)
{
	//- YAT_TRACE("galil::HwStatusProxy::handle_message"); 

  //- enter critical section
  yat::AutoMutex<yat::Mutex> guard(this->m_lock);

	//- YAT_LOG("HwStatusProxy::handle_message::receiving msg " << _msg.to_string());

	//-static yat::Timestamp last, now;
	//-_GET_TIME(now);
	//-if (_IS_VALID_TIMESTAMP(last))
	//-	YAT_LOG("HwStatusProxy::handle_message::DT since last exec is " << _ELAPSED_MSEC(last, now) << " msecs");

  try
  {
	  //- handle msg
	  switch (_msg.type())
	  {
		  //- TASK_INIT ----------------------
		  case yat::TASK_INIT:
			  {
				  //- "initialization" code goes here
				  YAT_LOG("HwStatusProxy::handle_message::THREAD_INIT::HwStatusProxy-task is starting up");
			  } 
			  break;
		  //- TASK_EXIT ----------------------
		  case yat::TASK_EXIT:
			  {
				  //- "release" code goes here
				  YAT_LOG("HwStatusProxy::handle_message::THREAD_EXIT::HwStatusProxy-task is quitting");
			  }
			  break;
		  //- TASK_PERIODIC ------------------
		  case yat::TASK_PERIODIC:
			  {
			    if (! m_suspended)
			    {
				    //- code relative to the task's periodic job goes here
				    YAT_LOG("HwStatusProxy::handle_message::handling THREAD_PERIODIC msg");

				    //- perform "selective" status update
				    this->selective_update_i(this->m_periodic_update_mask);

            //- timestamp last status update
	          _GET_TIME(this->m_last_update_ts);
	        }
			  }
			  break;
		  //- USER DEFINED MSG: kFORCE_STATUS_UPDATE ---
		  case kFORCE_STATUS_UPDATE:
			  {
				  YAT_LOG("HwStatusProxy::handle_message::handling FORCE_STATUS_UPDATE msg");
          
          //- get update mask from message
				  UpdateMask * mask = 0;
				  _msg.detach_data(mask);
				  try
				  {
            //- update requested data
					  if (! mask)
						  this->update_all_i();
					  else
						  this->selective_update_i(*mask);

            //- timestamp last status update
	          _GET_TIME(this->m_last_update_ts);
				  }
				  catch (...)
				  {
					  SAFE_DELETE_PTR(mask);
					  throw;
				  }
				  SAFE_DELETE_PTR(mask);
			  }
			  break;
		  //- USER DEFINED MSG: kSUSPEND -----
		  case kSUSPEND:
			  {
				  YAT_LOG("HwStatusProxy::handle_message::handling SUSPEND msg");
					this->m_suspended = true;
#if defined (_USE_ASYNCH_DR_)
          this->asynch_hwio_hook(HWIO_CLOSE);
#endif
			  }
			  break;
			  
		  //- USER DEFINED MSG: kRESUME ------
		  case kRESUME:
			  {
				  YAT_LOG("HwStatusProxy::handle_message::handling RESUME msg");
#if defined (_USE_ASYNCH_DR_) 
          this->asynch_hwio_hook(HWIO_OPEN);
#endif
					this->m_suspended = false; 
			  }
			  break;
			//- default ------------------------
		  default:
			  YAT_LOG("HwStatusProxy::handle_message::unhandled msg type received");
			  break;
	  }
	}
	catch (Tango::DevFailed& te)
	{
	  YAT_LOG("HwStatusProxy::handle_message::TANGO exception caught");
	  _TANGO_TO_YAT_EXCEPTION(te, ye);
#if defined (YAT_ENABLE_LOG)
	  ye.dump();
#endif
    //- GET_TIME(last);
	  throw ye;  
	}
	catch (...)
	{		
	  YAT_LOG("HwStatusProxy::handle_message::UNKNOWN exception caught");
	  //- _GET_TIME(last);
	  throw;  
	}
	
  //-_GET_TIME(last);
		
	//- YAT_LOG("HwStatusProxy::handle_message::message_handler:msg " << _msg.to_string() << " successfully handled");
}

// ============================================================================
// HwStatusProxy::update_all_i
// ============================================================================
void HwStatusProxy::update_all_i ()
	throw (Tango::DevFailed)
{
	YAT_TRACE("galil::HwStatusProxy::update_all");

	//- yat::Timestamp before, after;
	//- _GET_TIME(before);

  //- THE UCODE AXIS STATE MUST BE READ BEFORE THE DATA-RECORD
  //- THIS IS THE ONLY WAY TO GARANTEE THAT THE MOTOR POSITION
  //- IS CORRELATED TO THE MOTOR STATE.

	//- get current ucode status for each axis
	this->update_ucode_axis_status_i();

	//- get current status from Galil-Box
	this->update_qr_status_i();

	//- get current ucode cmd for each axis
	this->update_ucode_cmd_i();

	//- update the ucode status 
	this->update_ucode_status_i();

	//- update the hardware status 
	this->update_hardware_status_i();

	//- we obtained at least one full status update
	this->m_updated_once = true;
}

// ============================================================================
// HwStatusProxy::selective_update_i
// ============================================================================
void HwStatusProxy::selective_update_i (const UpdateMask& _mask)
	throw (Tango::DevFailed)
{
	if (_mask & UPDATE_ALL)
	{
		YAT_LOG("HwStatusProxy::handle_message::masked update::performing full update");
		this->update_all_i();
	}
	else
	{
		//- dump the received value
		//- YAT_LOG("HwStatusProxy::handle_message::update mask is: " << std::ios::hex << _mask << std::ios::dec);
		
    //- THE UCODE AXIS STATE MUST BE READ BEFORE THE DATA-RECORD
    //- THIS IS THE ONLY WAY TO GARANTEE THAT THE MOTOR POSITION
    //- IS CORRELATED TO THE MOTOR STATE.

		//- what should be updated?
		if (_mask & UPDATE_UC_STATUS || _mask & UPDATE_UC_AXIS_STATUS)
			this->update_ucode_axis_status_i();

		if (_mask & UPDATE_UC_STATUS || _mask & UPDATE_QR_STATUS || _mask & UPDATE_HW_STATUS) 
			this->update_qr_status_i();

		if (_mask & UPDATE_UC_AXIS_CMD_STATUS)
			this->update_ucode_cmd_i();

		if (_mask & UPDATE_UC_STATUS)
			this->update_ucode_status_i();
	}
}

// ============================================================================
// HwStatusProxy::read_dr_i
// ============================================================================
void HwStatusProxy::read_dr_i (BinaryData& _bd)
	throw (Tango::DevFailed)
{
	YAT_TRACE("galil::HwStatusProxy::read_dr_i");
  
  //- request some data to the Galil-Box
	_bd.force_length(0);

	//- YAT_LOG("HwStatusProxy::update_qr_status_i::requesting data to the Galil-Box...");
#if defined (_USE_ASYNCH_DR_)
  do
  {
    try
    {
		  this->asynch_hw_io()->out(_bd);
		}
		catch (const yat::SocketException & se)
		{
		  YAT_LOG("HwStatusProxy::update_qr_status_i::SocketException caught while reading DR on the async com. channel");   
	    se.dump();
	    throw;
		}
  }
  while (this->asynch_hw_io()->can_read_without_blocking());
#else
	this->synch_hw_io()->inout(UPDATE_DR, _bd);
#endif
}

// ============================================================================
// HwStatusProxy::update_qr_status_i
// ============================================================================
void HwStatusProxy::update_qr_status_i (void)
	throw (Tango::DevFailed)
{
	YAT_TRACE("galil::HwStatusProxy::update_qr_status_i");

  static BinaryData bd(kEXPECTED_QR_SIZE_IN_BYTES);

	try
	{
	  //-yat::Timestamp before, after;
		//-_GET_TIME(before);

		//- YAT_LOG("HwStatusProxy::update_qr_status_i::requesting data to the Galil-Box...");
		this->read_dr_i(bd);
		//- parse the data using a magical BitsStream
		//- YAT_LOG("HwStatusProxy::parsing bits stream");

		if (bd.length() != kEXPECTED_QR_SIZE_IN_BYTES)
		{
			std::cerr << "LOOK AT ME Buffer length: " <<  bd.length() << std::endl;
			if(bd.length() > 0){

				yat::BitsStream bs(reinterpret_cast<unsigned char *>(bd.base()),
						bd.size(),
						HwStatusProxy::galil_bytes_order);
				QRBlock err_block;
				bs >> err_block;
				std::cerr << err_block << std::endl;
			}
			THROW_DEVFAILED(_CPTC("HARDWARE_ERROR"),
											_CPTC("got invalid data from Galil-Box [DR reading failed] bytes:"),
											_CPTC(" galil::HwStatusProxy::update_qr_status"));


		}
			
		yat::BitsStream bs(reinterpret_cast<unsigned char *>(bd.base()), 
											 bd.size(), 
											 HwStatusProxy::galil_bytes_order);
		bs >> this->m_qr_block;

		//std::cout << "#################QR-BLOCK###################" << std::endl; // DEBUG ONLY
		//std::cout << m_qr_block << std::endl; // DEBUG ONLY
		//std::cout << "############################################" << std::endl; // DEBUG ONLY

		//- check error
		if (this->m_qr_block.gio.err_code() != 0)
		{
        //std::string ans; // DEBUG ONLY
        //this->synch_hw_io()->inout(GET_TC_ERROR, ans); // DEBUG ONLY

			YAT_LOG (
								 "HwStatusProxy::update_status::GALIL ERROR DETECTED::[err:"
									<< (size_t)this->m_qr_block.gio.err_code() 
									<< "]"
							);
      unsigned char err = this->m_qr_block.gio.err_code();
      std::string err_txt =  tc_error_txt[(err <= kMAX_TC_CODE_INDEX) ? err : kUNKNOWN_TC_CODE_INDEX];
			this->synch_hw_io()->tc_error_detected(err, err_txt);
    } 
    
    //- _GET_TIME(after);
		//- YAT_LOG("HwStatusProxy::update_qr_status_i::update took::" << _ELAPSED_SEC(before, after) * 1.E3 << " msecs");
		
		//- YAT_LOG("HwStatusProxy::update_qr_status::Galil-Box status successfully updated");
		
	}
	catch (Tango::DevFailed& df)
	{
		RETHROW_DEVFAILED(df,
											_CPTC("SOFTWARE_ERROR"),
											_CPTC("Tango exception caught while trying to obtain the QR data"),
											_CPTC("galil::HwStatusProxy::update_qr_status"));
	}
	catch (...)
	{
		THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
										_CPTC("unknown exception caught while trying to obtain the QR data"),
										_CPTC("galil::HwStatusProxy::update_qr_status"));
	}
}

// ============================================================================
// HwStatusProxy::update_ucode_axis_status_i
// ============================================================================
void HwStatusProxy::update_ucode_axis_status_i (void)
	throw (Tango::DevFailed)
{
	YAT_TRACE("galil::HwStatusProxy::update_ucode_axis_status");

	try
	{
		//-yat::Timestamp before, after;
		//-_GET_TIME(before);

		//- get Stat[] array (ucode status for each axis)
		//- delim = 1 (see UPDATE_ALL_AXIS_STAT) -> ASCII data separated by comma 
		//- and end-of-text = semicolomn (see Galil doc.)
		
		std::string ucs;
		this->synch_hw_io()->inout(UPDATE_ALL_AXIS_STAT, ucs);

	  //- be sure we got some data
	  if (ucs.length() == 0)
	    return;
	    
		std::string::size_type last_ans_char_pos = ucs.find_first_of(GALIL_LAST_ANS_CHAR);
		if (last_ans_char_pos == std::string::npos)
		{
			yat::OSStream desc;
			desc << "got invalid response from Galil-Box [" 
					 << ucs
					 << "] - retry also failed"
					 << std::ends;
			THROW_DEVFAILED(_CPTC("HARDWARE_ERROR"),
											_CPTC(desc.str().c_str()),
											_CPTC("galil::HwStatusProxy::update_ucode_axis_status"));
		}

		//- YAT_LOG("HwStatusProxy::update_ucode_status::galil-box returned::" << ucs);

		//- parse the <ucs> string and get each <MicroCodeStatus> as an <yat::uint32>
		//- the idea is to build a buffer to be passed to the BitsStream constructor.

		size_t a = AXIS_A;
		yat::uint32 tmp_buf[MAX_AXIS];

		const std::string delims("\r\n, ");
		std::string::size_type beg = ucs.find_first_not_of(delims);
		std::string::size_type end;

		while (a < MAX_AXIS && beg != std::string::npos)
		{
			end = ucs.find_first_of(delims, beg);
			//- YAT_LOG("- str token::" << ucs.substr(beg, end - beg));
			tmp_buf[a] = yat::XString<yat::uint32>::to_num(ucs.substr(beg, end - beg));
			//- YAT_LOG("- long token::" << tmp_buf[a]);
			beg = ucs.find_first_not_of(delims, end);
			a++;
		}

		//- got requested number of <MicroCodeStatus>?
		if (a != MAX_AXIS)
		{
			yat::OSStream desc;
			desc << "got invalid response from Galil-Box [" 
					 << ucs
					 << "]"
					 << std::ends;
			THROW_DEVFAILED(_CPTC("HARDWARE_ERROR"),
											_CPTC(desc.str().c_str()),
											_CPTC("galil::HwStatusProxy::update_ucode_axis_status"));
		}

		//- instanciate the BitsStream... 
		yat::BitsStream bs(reinterpret_cast<unsigned char*>(tmp_buf), 
												MAX_AXIS * sizeof(yat::uint32),
												yat::Endianness::host_endianness);

		//- populate our member
		for (a = AXIS_A; a < MAX_AXIS; a++)
			bs >> this->m_uc_axis_state[a];

		//-_GET_TIME(after);
		//-YAT_LOG("HwStatusProxy::update_ucode_status::update took::" << _ELAPSED_SEC(before, after) * 1.E3 << " msecs");
	}
	catch (Tango::DevFailed& df)
	{
		RETHROW_DEVFAILED(df,
											_CPTC("SOFTWARE_ERROR"),
											_CPTC("Tango exception caught while trying to obtain the uCode Stat[] data"),
											_CPTC("galil::HwStatusProxy::update_ucode_axis_status"));
	}
	catch (...)
	{
		THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
										_CPTC("unknown exception caught while trying to obtain the uCode Stat[] data"),
										_CPTC("galil::HwStatusProxy::update_ucode_axis_status"));
	}
}

// ============================================================================
// HwStatusProxy::update_ucode_cmd_i
// ============================================================================
void HwStatusProxy::update_ucode_cmd_i (void)
	throw (Tango::DevFailed)
{
	YAT_TRACE("galil::HwStatusProxy::update_ucode_cmd");

	try
	{

		//-yat::Timestamp before, after;
		//-_GET_TIME(before);

		//- get Cmd[] array (ucode command variable for each axis)
		//- delim = 1 (see UPDATE_ALL_AXIS_CMD) -> ASCII data separated by comma 
		//- and end-of-text = semicolomn (see Galil doc.)
		
		std::string ucc;
		this->synch_hw_io()->inout(UPDATE_ALL_AXIS_CMD, ucc);

	  //- be sure we got some data
	  if (ucc.length() == 0)
	    return;
	    
		std::string::size_type last_ans_char_pos = ucc.find_first_of(GALIL_LAST_ANS_CHAR);
		if (last_ans_char_pos == std::string::npos)
		{
			yat::OSStream desc;
			desc << "got invalid response from Galil-Box [" 
					 << ucc
					 << "] - retry also failed"
					 << std::ends;
			THROW_DEVFAILED(_CPTC("HARDWARE_ERROR"),
											_CPTC(desc.str().c_str()),
											_CPTC("galil::HwStatusProxy::update_ucode_cmd")); 
		}

		YAT_LOG("HwStatusProxy::update_ucode_cmd::galil-box returned::" << ucc);

		//- parse the <ucs> string and get each <MicroCodeStatus> as an <yat::uint32>
		//- the idea is to build a buffer to be passed to the BitsStream constructor.

		size_t a = AXIS_A;
		yat::uint32 tmp_buf[MAX_AXIS];

		const std::string delims("\r\n, ");
		std::string::size_type beg = ucc.find_first_not_of(delims);
		std::string::size_type end;

		while (a < MAX_AXIS && beg != std::string::npos)
		{
			end = ucc.find_first_of(delims, beg);
			//- YAT_LOG("- str token::" << ucc.substr(beg, end - beg));
			tmp_buf[a] = yat::XString<yat::uint32>::to_num(ucc.substr(beg, end - beg));
			//- YAT_LOG("- long token::" << tmp_buf[a]);
			beg = ucc.find_first_not_of(delims, end);
			a++;
		}

		//- got requested number of elements?
		if (a != MAX_AXIS)
		{
			yat::OSStream desc;
			desc << "got invalid response from Galil-Box [" 
					 << ucc
					 << "]"
					 << std::ends;
			THROW_DEVFAILED(_CPTC("HARDWARE_ERROR"),
											_CPTC("got invalid response from Galil-Box"),
											_CPTC("galil::HwStatusProxy::update_ucode_cmd"));
		}

		//- instanciate the BitsStream... 
		yat::BitsStream bs(reinterpret_cast<unsigned char*>(tmp_buf), 
												MAX_AXIS * sizeof(yat::uint32),
												yat::Endianness::host_endianness);

		//- populate our MicroCodeStatus records
		for (a = AXIS_A; a < MAX_AXIS; a++)
			bs >> this->m_uc_axis_cmd_state[a];

		//-_GET_TIME(after);
		//-YAT_LOG("HwStatusProxy::update_ucode_cmd::update took::" << _ELAPSED_SEC(before, after) * 1.E3 << " msecs");
	}
	catch (Tango::DevFailed& df)
	{
		RETHROW_DEVFAILED(df,
											_CPTC("SOFTWARE_ERROR"),
											_CPTC("Tango exception caught while trying to obtain the uCode Cmd[] data"),
											_CPTC("galil::HwStatusProxy::update_ucode_cmd"));
	}
	catch (...)
	{
		THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
										_CPTC("unknown exception caught while trying to obtain the uCode Cmd[] data"),
										_CPTC("galil::HwStatusProxy::update_ucode_cmd"));
	}
}

// ============================================================================
// HwStatusProxy::update_ucode_status_i
// ============================================================================
void HwStatusProxy::update_ucode_status_i (void)
{
	YAT_TRACE("galil::HwStatusProxy::update_ucode_status");

	this->m_ucode_status.running = this->m_qr_block.gio.program_running;


#if	defined(DMC_4143)
	this->m_ucode_status.processing_ref_pos =
									this->m_uc_axis_state[AXIS_A].ref_pos_in_progress()
								 || this->m_uc_axis_state[AXIS_B].ref_pos_in_progress()
								 || this->m_uc_axis_state[AXIS_C].ref_pos_in_progress()
								 || this->m_uc_axis_state[AXIS_D].ref_pos_in_progress();
#else
	this->m_ucode_status.processing_ref_pos =  
									this->m_uc_axis_state[AXIS_A].ref_pos_in_progress()
								 || this->m_uc_axis_state[AXIS_B].ref_pos_in_progress()
								 || this->m_uc_axis_state[AXIS_C].ref_pos_in_progress()
								 || this->m_uc_axis_state[AXIS_D].ref_pos_in_progress()
								 || this->m_uc_axis_state[AXIS_E].ref_pos_in_progress()
								 || this->m_uc_axis_state[AXIS_F].ref_pos_in_progress()
								 || this->m_uc_axis_state[AXIS_G].ref_pos_in_progress()
								 || this->m_uc_axis_state[AXIS_H].ref_pos_in_progress();
#endif
}

// ============================================================================
// HwStatusProxy::update_hardware_status_i
// ============================================================================
void HwStatusProxy::update_hardware_status_i (void)
{
	YAT_TRACE("galil::HwStatusProxy::update_hardware_status");

	this->m_hw_status.tc_error = 
					 this->m_qr_block.gio.err_code();

	this->m_hw_status.tc_error_str = 
						this->tc_code_error_str(this->m_hw_status.tc_error);

#if defined(_DEBUG) || defined (DEBUG)
	this->m_hw_status.dump();
#endif
}

// ============================================================================
// HwStatusProxy::status_data_expired_i
// ============================================================================
void HwStatusProxy::status_data_expired_i () throw (Tango::DevFailed)
{
	if (! m_updated_once)
	{
	 THROW_DEVFAILED(_CPTC("NO_DATA"),
									 _CPTC("no status data available"),
									 _CPTC("galil::HwStatusProxy::status_data_expired"));
	}

	yat::Timestamp now;
	_GET_TIME(now);
	double msecs = _ELAPSED_MSEC(this->m_last_update_ts, now);

	if (msecs >= this->m_cfg.status_expiration_timeout_ms)
	{
     this->m_status_expired_error_counter++;

		 yat::OSStream oms;
		 oms << "hardware status data expired [last successfull update "
				 << msecs / 1.E3 
				 << " secs ago]"
				 << std::ends;
		 THROW_DEVFAILED(_CPTC("HARDWARE_ERROR"),
										 _CPTC(oms.str().c_str()),
										 _CPTC("galil::HwStatusProxy::status_data_expired"));
	}
}

// ============================================================================
// HwStatusProxy::force_status_update
// ============================================================================
void HwStatusProxy::force_status_update (const UpdateMask& _mask, bool _wmh, size_t _wmh_tmo_ms)
	throw (Tango::DevFailed) 
{
  if (this->state() != yat::Thread::STATE_RUNNING)
    return;
    
	yat::Message * msg = new yat::Message(kFORCE_STATUS_UPDATE, MAX_USER_PRIORITY, _wmh);
	if (msg == 0)
	{
		THROW_DEVFAILED(_CPTC("OUT_OF_MEMORY"),
										_CPTC("yat::Message allocation failed"),
										_CPTC("HwStatusProxy::force_status_update"));
	}

	msg->attach_data(_mask);

  try
  {
  	if (_wmh)
  		this->wait_msg_handled(msg, _wmh_tmo_ms);
  	else
  		this->post(msg);
  }
  _HANDLE_YAT_EXCEPTION("yat::Task::wait_msg_handled or post", "HwStatusProxy::force_status_update"); 
}

// ============================================================================
// HwStatusProxy::suspend_hw_status_update
// ============================================================================
void HwStatusProxy::suspend_hw_status_update (void)
	throw (Tango::DevFailed) 
{
  { //- enter critical section
	  yat::AutoMutex<yat::Mutex> guard(this->m_lock);
    if (this->state_i() != yat::Thread::STATE_RUNNING)
    {
      this->m_suspended = true;
      return;
 	  }
  }

	yat::Message * msg = new yat::Message(kSUSPEND, MAX_USER_PRIORITY, true);
	if (msg == 0)
	{
		THROW_DEVFAILED(_CPTC("OUT_OF_MEMORY"),
										_CPTC("yat::Message allocation failed"),
										_CPTC("suspend_hw_status_update"));
	}

  try
  {
    this->wait_msg_handled(msg);
  }
  _HANDLE_YAT_EXCEPTION("yat::Task::wait_msg_handled", "HwStatusProxy::suspend_hw_status_update"); 
}

// ============================================================================
// HwStatusProxy::resume_hw_status_update
// ============================================================================
void HwStatusProxy::resume_hw_status_update (void)
	throw (Tango::DevFailed) 
{
  { //- enter critical section
	  yat::AutoMutex<yat::Mutex> guard(this->m_lock);
    if (this->state_i() != yat::Thread::STATE_RUNNING)
    {
      this->m_suspended = false;
      return;
 	  }
  }

	yat::Message * msg = new yat::Message(kRESUME, MAX_USER_PRIORITY, true);
	if (msg == 0)
	{
		THROW_DEVFAILED(_CPTC("OUT_OF_MEMORY"),
										_CPTC("yat::Message allocation failed"),
										_CPTC("resume_hw_status_update"));
	}

  try
  {
    this->wait_msg_handled(msg);
  }
  _HANDLE_YAT_EXCEPTION("yat::Task::wait_msg_handled", "HwStatusProxy:resume_hw_status_update"); 
}

// ============================================================================
// HwStatusProxy::tc_code_error_str
// ============================================================================
const char* HwStatusProxy::tc_code_error_str (unsigned char _tc_code) const
{
	if (_tc_code <= kMAX_TC_CODE_INDEX)
		return tc_error_txt[_tc_code];
	else 
		return tc_error_txt[kUNKNOWN_TC_CODE_INDEX]; 
}

} // namespace galil
