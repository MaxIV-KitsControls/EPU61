//=============================================================================
// UnsolicitedMsgReader.cpp
//=============================================================================
// abstraction.......Unsolicited msg reader (sse Galil doc)
// class.............UnsolicitedMsgReader
// original author...N.Leclercq - SOLEIL
//=============================================================================

#if defined(_UNSOLICITED_MSG_READER_SUPPORT_)

#define YAT_ENABLE_LOG 

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <tango.h>
#include <yat/XString.h>
#include "UnsolicitedMsgReader.h"

// ============================================================================
// SOME CONSTs
// ============================================================================
#define kDEFAULT_CONTROLLER_ADDR "0.0.0.0"
#define kDEFAULT_CONTROLLER_PORT 0
//-----------------------------------------------------------------------------

namespace galil
{

// ============================================================================
// SINGLETON
// ============================================================================
UnsolicitedMsgReader * UnsolicitedMsgReader::singleton = 0;

// ============================================================================
// UnsolicitedMsgReader::Config::Config
// ============================================================================
UnsolicitedMsgReader::Config::Config () 
 : galilbox_ip_addr (""), 
   galilbox_port (0), 
   startup_timeout_ms (kDEFAULT_USMR_STARTUP_TMO),
   read_timeout_ms (kDEFAULT_USMR_TMO) 
{
  //- noop ctor
}

// ============================================================================
// UnsolicitedMsgReader::Config::operator=
// ============================================================================
void UnsolicitedMsgReader::Config::operator= (const Config& src)
{
  galilbox_ip_addr = src.galilbox_ip_addr;
  galilbox_port = src.galilbox_port;
  startup_timeout_ms = src.startup_timeout_ms;
  read_timeout_ms = src.read_timeout_ms;
}

// ============================================================================
// UnsolicitedMsgReader::init <STATIC MEMBER>
// ============================================================================
void UnsolicitedMsgReader::init ()
	throw (Tango::DevFailed)
{
  YAT_TRACE("galil::UnsolicitedMsgReader::init");

  //- already instanciated?
  if (UnsolicitedMsgReader::singleton)
    return;

  try
  {
  	//- instanciate
    UnsolicitedMsgReader::singleton = new UnsolicitedMsgReader();
    if (UnsolicitedMsgReader::singleton == 0)
      throw std::bad_alloc();
  }
  catch (const std::bad_alloc&)
  {
    THROW_DEVFAILED(_CPTC("OUT_OF_MEMORY"),
                    _CPTC("galil::UnsolicitedMsgReader::singleton allocation failed"),
                    _CPTC("galil::UnsolicitedMsgReader::init"));	
  }
  catch (Tango::DevFailed& df)
  {
    RETHROW_DEVFAILED(df,
                      _CPTC("SOFTWARE_ERROR"),
                      _CPTC("galil::UnsolicitedMsgReader::singleton initialisation failed"),
                      _CPTC("galil::UnsolicitedMsgReader::init"));	
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("galil::UnsolicitedMsgReader::singleton initialisation failed [unknown exception]"),
                    _CPTC("galil::UnsolicitedMsgReader::init"));	
  }
}

// ============================================================================
// UnsolicitedMsgReader::close  <STATIC MEMBER>
// ============================================================================
void UnsolicitedMsgReader::close ()
	throw (Tango::DevFailed)
{
  YAT_TRACE("galil::UnsolicitedMsgReader::close");

  try
  {
  	//- check
  	if (! UnsolicitedMsgReader::singleton)
  	  return;

    //- be sure the task will not receive a periodic message...   
    UnsolicitedMsgReader::singleton->set_periodic_msg_period(0xFFFFFFFF);

    //- finally, release the UnsolicitedMsgReader::singleton
    UnsolicitedMsgReader::singleton->exit();

    //- reset singleton 
    UnsolicitedMsgReader::singleton = 0;

    //- NEVER CALL DELETE ON <UnsolicitedMsgReader::singleton>
    //- see yat::Task impl for more info...
  }
  catch (Tango::DevFailed& df)
  {
    RETHROW_DEVFAILED(df,
                      _CPTC("SOFTWARE_ERROR"),
                      _CPTC("Tango exception caught while calling UnsolicitedMsgReader::abort"),
                      _CPTC("galil::UnsolicitedMsgReader::close"));
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught while calling UnsolicitedMsgReader::abort"),
                    _CPTC("galil::UnsolicitedMsgReader::close"));	
  } 
}

// ============================================================================
// UnsolicitedMsgReader::UnsolicitedMsgReader
// ============================================================================
UnsolicitedMsgReader::UnsolicitedMsgReader ()
  : yat::Task(),
    m_cfg (), 
    m_hw_io (0)
{
	YAT_TRACE("galil::UnsolicitedMsgReader::UnsolicitedMsgReader [default ctor]");

  //- configure optional msg handling
  this->enable_timeout_msg(false);
  //- enable periodic msgs
  this->enable_periodic_msg(false); 
  
	//- create the hw I/O port
	HwIO<yat::NullMutex>::Config hwio_cfg;
	hwio_cfg.ip_addr = kDEFAULT_CONTROLLER_ADDR;
  hwio_cfg.port = kDEFAULT_CONTROLLER_PORT;
	hwio_cfg.protocol = HWIO_SLOW_SAFE;

  //- create the hw I/O port
  this->m_hw_io = new HwIO<yat::NullMutex>(hwio_cfg);
	if (this->m_hw_io == 0)
	{
	  THROW_DEVFAILED(_CPTC("OUT_OF_MEMORY"),
										_CPTC("galil::HwIO instanciation failed"),
										_CPTC("galil::UnsolicitedMsgReader::UnsolicitedMsgReader"));	  
	}
}

// ============================================================================
// UnsolicitedMsgReader::configure
// ============================================================================
void UnsolicitedMsgReader::configure (const Config& _cfg)
		throw (Tango::DevFailed)
{
	YAT_TRACE("galil::UnsolicitedMsgReader::UnsolicitedMsgReader");

  //- enter critical section
  yat::AutoMutex<yat::Mutex> guard(this->Task::m_lock);

  //- copy configuration
  this->m_cfg = _cfg;

  //- enable periodic msgs
  this->enable_periodic_msg(true); 
  this->set_periodic_msg_period(kDEFAULT_USMR_PERIOD);

  //- (re)open connection to peer
	try
	{
    this->m_hw_io->reopen(_cfg.galilbox_ip_addr, _cfg.galilbox_port);
	}
  catch (const std::bad_alloc&)
  {
    THROW_DEVFAILED(_CPTC("OUT_OF_MEMORY"),
                    _CPTC("failed to connect to the controller [HWIO instanciation failed]"),
                    _CPTC("galil::UnsolicitedMsgReader::configure"));	
  }
  catch (Tango::DevFailed& df)
  {
    RETHROW_DEVFAILED(df,
                      _CPTC("SOFTWARE_MEMORY"),
                      _CPTC("failed to connect to the controller [Tango exception caught]"),
                      _CPTC("galil::UnsolicitedMsgReader::configure"));	
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("failed to connect to the controller [unknown exception caught]"),
                    _CPTC("galil::UnsolicitedMsgReader::configure"));	
  }

  try 
  {
    //- disable unsolicited char "marking" [see CW cmd doc]
    yat::OSStream cmd;
    cmd << "CW2\r";
    this->m_hw_io->in(cmd.str(), IOCheckError);
    
    //- redirect unsolicited msg to the connection handle we are using [using CF cmd]
    cmd.str("");
    cmd << "CF" << this->m_hw_io->com_handle() << "\r";
    this->m_hw_io->in(cmd.str(), IOCheckError);

    YAT_LOG("UnsolicitedMsgReader::configure::unsolicited msgs successfully redirected");
  }
  catch (Tango::DevFailed& df)
  {
    RETHROW_DEVFAILED(df,
                      _CPTC("SOFTWARE_MEMORY"),
                      _CPTC("failed to redirect unsolicited messages [Tango exception caught]"),
                      _CPTC("galil::UnsolicitedMsgReader::configure"));	
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("failed to redirect unsolicited messages [unknown exception caught]"),
                    _CPTC("galil::UnsolicitedMsgReader::configure"));	
  }

  //- start the task if not running
  if (this->state() != yat::Thread::STATE_RUNNING)
    this->go(_cfg.startup_timeout_ms);
}

// ============================================================================
// UnsolicitedMsgReader::~UnsolicitedMsgReader
// ============================================================================
UnsolicitedMsgReader::~UnsolicitedMsgReader ()
{
	YAT_TRACE("galil::UnsolicitedMsgReader::~UnsolicitedMsgReader");

  try
  {
		//- release HwI/O port
		SAFE_RELEASE(this->m_hw_io);
  }
  catch (...)
  {
    //- ignore any error
  }
}

// ============================================================================
// UnsolicitedMsgReader::handle_message
// ============================================================================
void UnsolicitedMsgReader::handle_message (yat::Message& _msg)
	throw (yat::Exception)
{
  YAT_TRACE("galil::UnsolicitedMsgReader::handle_message"); 

  //- The Task's lock_ is locked
	//-------------------------------------------------------------------

	//- YAT_LOG("UnsolicitedMsgReader::handle_message::receiving msg " << _msg.to_string());

	//- handle msg
  switch (_msg.type())
	{

	  //- THREAD_INIT ----------------------
	  case yat::TASK_INIT:
	    {
  	    //- "initialization" code goes here
  	    YAT_LOG("UnsolicitedMsgReader::handle_message::THREAD_INIT::thread is starting up");
      } 
		  break;

		//- THREAD_EXIT ----------------------
		case yat::TASK_EXIT:
		  {
  			//- "release" code goes here
  			YAT_LOG("UnsolicitedMsgReader::handle_message::THREAD_EXIT::thread is quitting");
      }
			break;

		//- THREAD_PERIODIC ------------------
		case yat::TASK_PERIODIC:
		  {
		    //- enter critical section
		    yat::AutoMutex<yat::Mutex> guard(this->Task::m_lock);
		    
		    //- get unsolicited msg
		    try
		    {
  		    //- code relative to the task's periodic job goes here
  		    //- YAT_LOG("UnsolicitedMsgReader::handle_message::handling THREAD_PERIODIC msg");
          std::string unsolicited_msg;
          this->hw_io()->out(unsolicited_msg);
          if (unsolicited_msg.size())
          {
            YAT_LOG("***************************************************************************");
            YAT_LOG("* UnsolicitedMsgReader::got the following unsolicited msg from controller *");
            YAT_LOG("***************************************************************************");
            YAT_LOG("unsolicited msg length..." << unsolicited_msg.size());
            YAT_LOG("unsolicited msg.........." << unsolicited_msg);
            YAT_LOG("***************************************************************************");
          }
        }
        catch (Tango::DevFailed& te)
	      {
	        YAT_LOG("UnsolicitedMsgReader::handle_message::TANGO exception caught");
	        _TANGO_TO_YAT_EXCEPTION(te, ye);
	        throw ye;  
	      }
	      catch (...)
	      {		
	        YAT_LOG("UnsolicitedMsgReader::handle_message::UNKNOWN exception caught");
	        throw;  
	      }
      }
		  break;

  	default:
  		//- YAT_LOG("UnsolicitedMsgReader::handle_message::unhandled msg type received");
  		break;
	}

	//- YAT_LOG("UnsolicitedMsgReader::handle_message::message_handler:msg " << _msg.to_string() << " successfully handled");
}

} // namespace galil

#endif //- _UNSOLICITED_MSG_READER_SUPPORT_
