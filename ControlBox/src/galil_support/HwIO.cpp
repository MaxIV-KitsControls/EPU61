//=============================================================================
// HwIO.cpp
//=============================================================================
// abstraction.......I/O com.with Galil-Box abstraction class
// class.............HwIO
// original author...N.Leclercq - J.Coquet - SOLEIL
//=============================================================================

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include "math.h"
#include "HwIO.h"

#ifndef _HW_IO_CPP_
#define _HW_IO_CPP_

// ============================================================================
// SOME CONSTs
// ============================================================================
#define kCLOSE_ALL_TCP_HANDLES_BUT_THIS "IHT=>-2\r"
#define kCLOSE_ALL_UDP_HANDLES_BUT_THIS "IHT=>-1\r"
#define kCLOSE_THIS_COM_HANDLE          "IHS=>-1\r"
//-----------------------------------------------------------------------------
#define kCONNECTION_TMO_MSECS  30000
#define kDEFAULT_TMO_MSECS      1000
//-----------------------------------------------------------------------------
#define kMAX_RETRY 5
//-----------------------------------------------------------------------------
// ============================================================================
// SHORTCUTs
// ============================================================================
#define RECONNECTION_REQUIRED \
  (this->m_socket == 0 || (this->m_state != HWIO_CONNECTED && this->m_state != HWIO_DISCONNECTING))
  
#define CHECK_CONNECTION_PENDING(O) \
  if (this->m_state == HWIO_CONNECTING) \
  { \
    THROW_DEVFAILED(_CPTC("BAD_STATE"), \
                    _CPTC("can't perform the requested action - a connection to the controller is in progress"), \
                    _CPTC(O));  \
  }

namespace galil
{

//=============================================================================
// Standalone template function: generates a random <T> in the range [0, max]
//=============================================================================
template<typename T> inline T randomize (T max)
{
  if (max > 0) 
  {
	  int r = ((r = ::rand()) < 0) ? -r : r;
	  return (T)(r % (int)max);
  }
  return 0;
}

// ============================================================================
// HwIO<L>::HwIO
// ============================================================================
template <typename L> 
HwIO<L>::HwIO ()
  : m_cfg (), 
    m_socket (0),
    m_state (HWIO_DISCONNECTED)
{
	YAT_TRACE("galil::HwIO<L>::HwIO [default ctor]");
	
	_GET_TIME(this->m_health.startup_ts);
}

// ============================================================================
// HwIO<L>::HwIO
// ============================================================================
template <typename L> 
HwIO<L>::HwIO (const std::string& _ip_addr, size_t _port, HwIOProtocol _prot)
  : m_cfg (), 
    m_socket (0),
    m_state (HWIO_DISCONNECTED)
{
	YAT_TRACE("galil::HwIO<L>::HwIO");

  m_cfg.ip_addr = _ip_addr;
  m_cfg.port = _port;
  m_cfg.protocol = _prot;
  
  _GET_TIME(this->m_health.startup_ts);
}

// ============================================================================
// HwIO<L>::HwIO
// ============================================================================
template <typename L> 
HwIO<L>::HwIO (const Config& _cfg)
  : m_cfg (_cfg), 
    m_socket (0),
    m_state (HWIO_DISCONNECTED)
{
	YAT_TRACE("galil::HwIO<L>::HwIO");
	
	_GET_TIME(this->m_health.startup_ts);
}

// ============================================================================
// HwIO<L>::~HwIO
// ============================================================================
template <typename L> 
HwIO<L>::~HwIO (void)
{
	YAT_TRACE("galil::HwIO<L>::~HwIO");

  //- close connection to the Galil-HwIO
  try
  {
    this->close();
  }
  catch( ...)
  {
   //- ignore any error
  }
}

// ============================================================================
// HwIO<L>::open
// ============================================================================
template <typename L> 
void HwIO<L>::open (void)
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::HwIO<L>::open");

  //- enter critical section
  yat::AutoMutex<L> guard(this->m_lock);

  //- do nothing if already connected
  if (this->m_state == HWIO_CONNECTED)
    return;

  //- open connection (under critical section)
  this->open_i();
}

// ============================================================================
// HwIO<L>::open
// ============================================================================
template <typename L> 
void HwIO<L>::open (std::string _ip_addr, size_t _port)
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::HwIO<L>::open");

  //- enter critical section
  yat::AutoMutex<L> guard(this->m_lock);

  //- check state
  CHECK_CONNECTION_PENDING("HwIO<L>::open");

  //- do we really have to open the connection?
  if (
      this->m_state == HWIO_CONNECTED
        &&
      this->m_cfg.ip_addr == _ip_addr
        &&
      this->m_cfg.port == _port
     )
  {
    //- do nothing
    return;
  }
   
  //- store both addr & port 
  this->m_cfg.ip_addr = _ip_addr;
  this->m_cfg.port = _port;
     
  //- may to close the connection (i.e. reopen)
  this->close_i();
     
  //- open connection (under critical section)
  this->open_i();
}

// ============================================================================
// HwIO<L>::open_i
// ============================================================================
template <typename L> 
void HwIO<L>::open_i (void)
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::HwIO<L>::open_i");

  //- since we are supposed to be in critical section (m_lock locked/owned 
  //- by the calling thread), we can safely test the internal state (m_state)

  //- we should never get here with the following not verified
  DEBUG_ASSERT(this->m_socket == 0);
  DEBUG_ASSERT(this->m_state != HWIO_CONNECTED);
  DEBUG_ASSERT(this->m_state != HWIO_CONNECTING);

  try
  {
    YAT_LOG("HwIO<L>::open::instanciating underlying socket");

    //- change state to "connecting"
    this->m_state = HWIO_CONNECTING;
		
    //- select socket protocol
    yat::Socket::Protocol sp = yat::Socket::TCP_PROTOCOL;
		if (m_cfg.protocol != HWIO_SLOW_SAFE)
		  sp = yat::Socket::UDP_PROTOCOL;  
		  
		//- instanciate the ClientSocket
    this->m_socket = new yat::ClientSocket(sp);
    if (this->m_socket == 0)
      throw std::bad_alloc();

    //- bind - required for 2 way communication (peer-to-peer) and to use setsockopt
    this->m_socket->bind();
    
    //- set socket options for TCP 
    if (m_cfg.protocol == HWIO_SLOW_SAFE)
      this->m_socket->set_option(yat::Socket::SOCK_OPT_NO_DELAY, 1);

    //- set timeout
    this->m_socket->set_option(yat::Socket::SOCK_OPT_OTIMEOUT, kCONNECTION_TMO_MSECS);
    this->m_socket->set_option(yat::Socket::SOCK_OPT_ITIMEOUT, kCONNECTION_TMO_MSECS);

    //- instanciate the network address
    yat::Address addr(this->m_cfg.ip_addr, this->m_cfg.port);

    //- are we handling a com error?
    bool handling_com_error = _IS_VALID_TIMESTAMP(this->m_health.cfailure_ts);

    //- inc. connection tries counter
    if (handling_com_error)
      ++this->m_health.retries; 
    
    //- connect to peer (TCP) or configure socket (UDP)
    this->m_socket->connect(addr);  
    
    //- set timeout
    this->m_socket->set_option(yat::Socket::SOCK_OPT_OTIMEOUT, kDEFAULT_TMO_MSECS);
    this->m_socket->set_option(yat::Socket::SOCK_OPT_ITIMEOUT, kDEFAULT_TMO_MSECS);

    //- timestamp the successfull connection
    _GET_TIME(this->m_health.lsuccess_ts);

    //- if we were handling a com. error, then...
    if (handling_com_error)
    {
      //- compute the disconnection time (i.e. duration)
      this->m_health.dt = 
         _ELAPSED_SEC(this->m_health.cfailure_ts, this->m_health.lsuccess_ts);
      //- copy current failure ts to last failure ts
      _COPY_TIMESTAMP(this->m_health.cfailure_ts, this->m_health.lfailure_ts);
    }
    else if (! this->m_health.retries)
    {
      //- inc. connection tries counter
      ++this->m_health.retries; 
    }
    
    //- reset com failure timestamp
    _RESET_TIMESTAMP(this->m_health.cfailure_ts);

    //- this is a com. success!
    ++this->m_health.com_success;

    //- change state to "connected"
    this->m_state = HWIO_CONNECTED;
    
    YAT_LOG("HwIO<L>::open::socket connected"); 
    
    //- which connection handle are we using? [use What Handle (i.e. WH cmd)]
    YAT_LOG("galil::HwIO<L>::open::indentifying connection handle...");
    std::string out;
    std::string cmd("WH\r");
    //- UDP: we need to be sure that we get the handle identifier
    if (m_cfg.protocol != HWIO_SLOW_SAFE)
    {
    	size_t snd_count = 0;
      do
      {
        this->m_socket->send(cmd);
        size_t rcv_count = 0;
        do 
        {
          bool throw_timeout = (++rcv_count < kMAX_RETRY) ? false : true;
          if (this->m_socket->wait_input_data(kDEFAULT_TMO_MSECS, throw_timeout))
          {
            this->m_socket->receive(out);
            break;
          }
        }
        while (1);
     }
     while (++snd_count < kMAX_RETRY);
   }
   //- TCP : "no problem"
   else
   {
     this->m_socket->send(cmd);
     this->m_socket->wait_input_data(kDEFAULT_TMO_MSECS);
     this->m_socket->receive(out);
   }
   const std::string delims("IH\r\n");
   std::string::size_type pos = out.find_first_not_of(delims); 
   this->m_health.com_handle = out.substr(pos, 1);
   YAT_LOG("galil::HwIO<L>::open::we are using handle: " << this->m_health.com_handle);

    //- set blocking mode
    if (m_cfg.blocking) 
      this->m_socket->set_blocking_mode(); 
    else
      this->m_socket->set_non_blocking_mode();
      
    //- call client hook
    if (! this->m_hook.is_empty())
    {
      this->m_hook(HWIO_OPEN);
    }
  }
  catch (const std::bad_alloc&)
  {
    this->m_state = HWIO_FAULT;
    YAT_LOG("HwIO<L>::open::caught std::bad_alloc");
    THROW_DEVFAILED(_CPTC("OUT_OF_MEMORY"),
                    _CPTC("socket instanciation failed"),
                    _CPTC("galil::HwIO<L>::open"));
  }
  catch (const yat::SocketException& se)
  {
    YAT_LOG("HwIO<L>::open::caught yat::SocketException");
    se.dump();
    this->close_i(true);
    THROW_YAT_TO_TANGO_EXCEPTION(se);
  }
  catch (const yat::Exception& ye)
  {
    this->close_i(true);
    YAT_LOG("HwIO<L>::open::caught yat::Exception");
    THROW_YAT_TO_TANGO_EXCEPTION(ye);
  }
  catch (...)
  {
    this->close_i(true);
    YAT_LOG("HwIO<L>::open::unknown exception caught");
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught while connecting to the controller"),
                    _CPTC("galil::HwIO<L>::open")); 
  }
}

// ============================================================================
// HwIO<L>::close
// ============================================================================
template <typename L> 
void HwIO<L>::close (void)
		throw (Tango::DevFailed)
{
  YAT_TRACE("galil::HwIO<L>::close");

  //- enter critical section
  yat::AutoMutex<L> guard(this->m_lock);

  this->close_i();
}

// ============================================================================
// HwIO<L>::close_i
// ============================================================================
template <typename L> 
void HwIO<L>::close_i (bool _close_on_com_error)
		throw (Tango::DevFailed)
{
  YAT_TRACE("galil::HwIO<L>::close_i");

  //- since we are supposed to be in critical section (m_lock locked/owned 
  //- by the calling thread), we can safely test the internal state (m_state)

  //- nothing to do here if we are not connected or no valid socket exists
  //- nothing to do here if we are trying to connected to peer
  if (this->m_socket == 0 || this->m_state == HWIO_DISCONNECTING)
    return;

  //- nothing to do here if don't want to close on error
  if (! m_cfg.close_on_error && _close_on_com_error)
    return;
    
  //- store current state
  HwIOState previous_state = this->m_state;

  //- change state to "disconnecting"
  this->m_state = HWIO_DISCONNECTING;

  //- call client hook
  if (! this->m_hook.is_empty())
  {
    try
    {
      this->m_hook(HWIO_CLOSE);
    }
    catch (...)
    {
      //- ignore error
    }
  }

  //- have to close UDP connection 
  if (m_cfg.protocol == HWIO_FAST_UNSAFE && previous_state == HWIO_CONNECTED)
  {
     try
     {
       *this << kCLOSE_THIS_COM_HANDLE;
     }
     catch (...)
 	   {
 	     //- ignore error
     }
  }

  //- if we are closing the com. port on error then...
  if (_close_on_com_error)
  {
    //- this is a com. error!
    ++this->m_health.com_errors;
    //- if we are not alredy handling a com. error then ...
    if (! _IS_VALID_TIMESTAMP(this->m_health.cfailure_ts))
    {
      //- reset the "retry counter"
      this->m_health.retries = 0;
      //- timestamp the com. failure
      _GET_TIME(this->m_health.cfailure_ts);
    } 
  }
  
  //- release the underlying socket (close connection)
  try
  {
    YAT_LOG("HwIO<L>::close::closing underlying socket...");
  
    SAFE_DELETE_PTR(this->m_socket);

    YAT_LOG("HwIO<L>::close::socket closed");
  }
  catch (const yat::SocketException& se)
  {
    this->m_state = HWIO_FAULT;

    YAT_LOG("galil::HwIO<L>::close::caught yat::SocketException");
    
    se.dump();
    
    THROW_YAT_TO_TANGO_EXCEPTION(se);
  }
  catch (const yat::Exception& ye)
  {
    this->m_state = HWIO_FAULT;

    YAT_LOG("galil::HwIO<L>::close::caught yat::Exception");
    
    ye.dump();

    THROW_YAT_TO_TANGO_EXCEPTION(ye);
  }
  catch (...)
  {
    this->m_state = HWIO_FAULT;
    
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught while trying to delete the socket"),
                    _CPTC("galil::HwIO<L>::close")); 
  }
      
  //- state transition
  switch (previous_state)
  {
    case HWIO_CONNECTING:
      this->m_state = HWIO_CONNECTION_FAILED;
      break;
    case HWIO_CONNECTED:
      if (_close_on_com_error)
        this->m_state = HWIO_DISCONNECTED_ON_COM_ERROR;
      else
        this->m_state = HWIO_DISCONNECTED;
      break;
    default:
      this->m_state = HWIO_DISCONNECTED;
      break;
  }
}

// ============================================================================
// HwIO<L>::reopen
// ============================================================================
template <typename L> 
void HwIO<L>::reopen (std::string _ip_addr, size_t _port)
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::HwIO<L>::reopen");

  //- enter critical section
  yat::AutoMutex<L> guard(this->m_lock);

  //- check state
  CHECK_CONNECTION_PENDING("HwIO<L>::reopen");
  
  //- store both addr & port 
  this->m_cfg.ip_addr = _ip_addr;
  this->m_cfg.port = _port;
     
  //- reset the com. errors counters
  this->m_health.reset_com_counters();
    
  //- close the connection
  this->close_i();
     
  //- open connection
  this->open_i();
}

// ============================================================================
// Box::cleanup
// ============================================================================
template <typename L> 
void HwIO<L>::cleanup () throw (Tango::DevFailed)
{
  YAT_TRACE("galil::HwIO<L>::cleanup");

  //- enter critical section
  yat::AutoMutex<L> guard(this->m_lock);

  try
  {
    //- close all sockets except the one sending the command
    //------------------------------------------------------
    //- do it for TCP sockets
    *this << kCLOSE_ALL_TCP_HANDLES_BUT_THIS << IOCheckError;
    
    //- do it for UDP sockets
    *this << kCLOSE_ALL_UDP_HANDLES_BUT_THIS << IOCheckError;
  }
	catch (const yat::SocketException & se)
	{     
	  se.dump();
	  close_i(true);
		THROW_YAT_TO_TANGO_EXCEPTION(se);
	}
	catch (const yat::Exception & ye)
	{  
	  ye.dump();
    close_i(true);
		THROW_YAT_TO_TANGO_EXCEPTION(ye);
	}
  catch (Tango::DevFailed& df)
  {
    close_i(true); 
    RETHROW_DEVFAILED(df,
                      _CPTC("COMMUNICATION_FAILED"),
                      _CPTC("Tango exception caught while trying to cleanup connections"),
                      _CPTC("galil::HwIO<L>::cleanup"));
  }
  catch (...)
  {
    close_i(true);
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught while trying to cleanup connections"),
                    _CPTC("galil::HwIO<L>::cleanup")); 
  }
}

// ============================================================================
// HwIO<L>::operator<< 
// ============================================================================
template <typename L> 
HwIO<L>& HwIO<L>::operator<< (const std::string& _in_str)
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::HwIO<L>::operator<< [ascii]");

  //- YAT_LOG("galil::HwIO<L>::in::sending::" << _in_str);

  //- check state
  CHECK_CONNECTION_PENDING("HwIO<L>::operator<<");
  
	try
	{
    //- connect is not already connected
    if (RECONNECTION_REQUIRED) 
      this->open_i();

		//- try to send command 
		this->m_socket->send(_in_str);

	  //- this is a com. success!
    ++this->m_health.com_success;
	}
	catch (const yat::SocketException & se)
	{    
	  se.dump();
	  this->close_i(true);
    THROW_YAT_TO_TANGO_EXCEPTION(se);
  }
	catch (const yat::Exception & ye)
	{  
	  ye.dump();
	  this->close_i(true);
    THROW_YAT_TO_TANGO_EXCEPTION(ye);
  }
  catch (Tango::DevFailed& df)
	{  
	  this->close_i(true);
		RETHROW_DEVFAILED(df,
                      _CPTC("COMMUNICATION_FAILED"),
			                _CPTC("TANGO exception caught while trying to write on the socket"),
			                _CPTC("galil::HwIO<L>::in"));
	}
	catch (...)
	{
    this->close_i(true);
    THROW_DEVFAILED(_CPTC("COMMUNICATION_FAILED"),
                    _CPTC("unknown exception caught while trying to write on the socket"),
                    _CPTC("galil::HwIO<L>::in")); 
	}

  return *this;
}

// ============================================================================
// HwIO<L>::operator<< 
// ============================================================================
template <typename L> 
HwIO<L>& HwIO<L>::operator<< (const BinaryData& _in)
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::HwIO<L>::operator<< [binary]");

	YAT_LOG("galil::HwIO<L>::in::writing " << _in.length() << " bytes");

  //- check state
  CHECK_CONNECTION_PENDING("HwIO<L>::operator<<");
  
	try
	{
    //- connect is not already connected
    if (RECONNECTION_REQUIRED) 
      this->open_i();

		//- write data the underlying socket
		this->m_socket->send(_in);
		
		//- this is a com. success!
    ++this->m_health.com_success;
	}
	catch (const yat::SocketException & se)
	{   
	  se.dump();
	  this->close_i(true);
		THROW_YAT_TO_TANGO_EXCEPTION(se);
	}
	catch (const yat::Exception & ye)
	{
	  ye.dump();
	  this->close_i(true);
		THROW_YAT_TO_TANGO_EXCEPTION(ye);
	}
  catch (Tango::DevFailed& df)
	{
	  this->close_i(true);
		RETHROW_DEVFAILED(df,
                      _CPTC("COMMUNICATION_FAILED"),
			                _CPTC("TANGO exception caught while trying to write on the socket"),
			                _CPTC("galil::HwIO<L>::in"));
	}
	catch (...)
	{
    this->close_i(true);
    THROW_DEVFAILED(_CPTC("COMMUNICATION_FAILED"),
                    _CPTC("unknown exception caught while trying to write on the socket"),
                    _CPTC("galil::HwIO<L>::in")); 
	}

  return *this;
}

// ============================================================================
// HwIO<L>::operator<< 
// ============================================================================
template <typename L> 
HwIO<L>& HwIO<L>::operator<< (const IOOperator& _op)
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::HwIO<L>::operator<< [IOOperator]");

  //- check state
  CHECK_CONNECTION_PENDING("HwIO<L>::operator<<");
    
  std::string out; 

  try
  {
    switch (_op)
    {
      //-----------------------
      case IONone:
        {
          //- YAT_LOG("galil::HwIO<L>::operator<< [IOOperator::IONone]");
        }
        break;
      //-----------------------
      case IOOpen:
        {
          //- YAT_LOG("galil::HwIO<L>::operator<< [IOOperator::IOOpen]");
          this->open_i(); 
        }
        break;
      //-----------------------
      case IOFlushInput:
      case IOCheckError:
        {
          //- YAT_LOG("galil::HwIO<L>::operator<< [IOOperator::IOFlushInput or IOCheckError]");
          if (RECONNECTION_REQUIRED)
            this->open_i();
          //- empty input buffer 
          this->m_socket->wait_input_data(kDEFAULT_TMO_MSECS);
          this->m_socket->receive(out);
          //- this is a com. success!
          ++this->m_health.com_success;
        }
        break;
      //-----------------------
      case IOClose:
        {
          YAT_LOG("galil::HwIO<L>::operator<< [IOOperator::IOClose]");
          this->close_i(); 
        }
        break;
      default:
        break;
    }
  }
	catch (const yat::SocketException & se)
	{
	  se.dump();
	  if (_op != IOClose) 
      this->close_i(true);
		THROW_YAT_TO_TANGO_EXCEPTION(se);
	}
	catch (const yat::Exception & ye)
	{
	  ye.dump();
	  if (_op != IOClose) 
      this->close_i(true);
		THROW_YAT_TO_TANGO_EXCEPTION(ye);
	}
  catch (Tango::DevFailed& df)
	{
	  if (_op != IOClose) 
      this->close_i(true);
		RETHROW_DEVFAILED(df,
                      _CPTC("COMMUNICATION_FAILED"),
			                _CPTC("TANGO exception caught while trying to write on the socket"),
			                _CPTC("galil::HwIO<L>::in"));
	}
	catch (...)
	{
    if (_op != IOClose) 
      this->close_i(true);
    THROW_DEVFAILED(_CPTC("COMMUNICATION_FAILED"),
                    _CPTC("unknown exception caught while trying to read on the socket"),
                    _CPTC("galil::HwIO<L>::in")); 
	}

  return *this;
}

// ============================================================================
// HwIO<L>::operator>>
// ============================================================================
template <typename L> 
void HwIO<L>::operator>> (std::string& out_)
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::HwIO<L>::operator>> [ascii]");

  //- check state
  CHECK_CONNECTION_PENDING("HwIO<L>::operator>>");
  
  if (RECONNECTION_REQUIRED)
     this->open_i();

  try
  {
    //- get data from peer
    this->m_socket->wait_input_data(kDEFAULT_TMO_MSECS);
    this->m_socket->receive(out_);
    
    //- this is a com. success!
    ++this->m_health.com_success;
    
    //- YAT_LOG("HwIO<L>::operator>> [ascii]::received: " << out_);
  }
	catch (const yat::SocketException & se)
	{
  	se.dump();
	  if (se.is_a(yat::SoErr_WouldBlock) || se.is_a(yat::SoErr_TimeOut))
	  {
	    out_ = "";
      return;
	  }
	  this->close_i(true);
		THROW_YAT_TO_TANGO_EXCEPTION(se);
	}
	catch (const yat::Exception & ye)
	{
	  ye.dump();
	  this->close_i(true);
		THROW_YAT_TO_TANGO_EXCEPTION(ye);
	}
  catch (Tango::DevFailed& df)
	{   
	  this->close_i(true);
		RETHROW_DEVFAILED(df,
                      _CPTC("COMMUNICATION_FAILED"),
			                _CPTC("TANGO exception caught while trying to write on the socket"),
			                _CPTC("galil::HwIO<L>::out"));
	}
	catch (...)
	{
    this->close_i(true);
    THROW_DEVFAILED(_CPTC("COMMUNICATION_FAILED"),
                    _CPTC("unknown exception caught while trying to read on the socket"),
                    _CPTC("galil::HwIO<L>::out")); 
	}

  this->check_error(out_);
}

// ============================================================================
// HwIO<L>::operator>>
// ============================================================================
template <typename L> 
void HwIO<L>::operator>> (BinaryData& out_)
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::HwIO<L>::operator>> [binary]");

  //- check state
  CHECK_CONNECTION_PENDING("HwIO<L>::operator>>");
  
  if (RECONNECTION_REQUIRED)
     this->open_i();

  try
  {
    //- get data from peer
    this->m_socket->wait_input_data(kDEFAULT_TMO_MSECS);
    this->m_socket->receive(out_);
    
    //- this is a com. success!
    ++this->m_health.com_success;
  }
	catch (const yat::SocketException & se)
	{
	  se.dump();
	  if (se.is_a(yat::SoErr_WouldBlock) || se.is_a(yat::SoErr_TimeOut))
	  {
	    out_.force_length(0);
      return;
	  }
	  this->close_i(true);
		THROW_YAT_TO_TANGO_EXCEPTION(se);
	}
	catch (const yat::Exception & ye)
	{
	  ye.dump();
	  this->close_i(true);
		THROW_YAT_TO_TANGO_EXCEPTION(ye);
	}
  catch (Tango::DevFailed& df)
	{ 
	  this->close_i(true);
		RETHROW_DEVFAILED(df,
                      _CPTC("COMMUNICATION_FAILED"),
			                _CPTC("TANGO exception caught while trying to write on the socket"),
			                _CPTC("galil::HwIO<L>::out"));
	}
	catch (...)
	{
    this->close_i(true);
    THROW_DEVFAILED(_CPTC("COMMUNICATION_FAILED"),
                    _CPTC("unknown exception caught while trying to read on the socket"),
                    _CPTC("galil::HwIO<L>::out")); 
	}
}

// ============================================================================
// HwIO<L>::operator>>
// ============================================================================
template <typename L> 
void HwIO<L>::check_error (const std::string& _cmd, const std::string& _galil_ans)
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::HwIO<L>::check_error");

  try
  {
    this->check_error(_galil_ans);
  }
  catch (...)
  {
    //-TODO: find a way to indicate which failed...
    yat::OSStream oms;
    oms << "the <Galil-Box> controller could not interpret the command "
        << "[unknown cmd or syntax error in: " 
        << _cmd
        << "]"
        << std::ends;
    THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                    _CPTC(oms.str().c_str()),
                    _CPTC("galil::HwIO<L>::check_error"));
  }
}


// ============================================================================
// HwIO<L>::operator>>
// ============================================================================
template <typename L> 
void HwIO<L>::check_error (const std::string& _galil_ans)
  throw (Tango::DevFailed)
{
  YAT_TRACE("galil::HwIO<L>::check_error");

  YAT_LOG("galil::HwIO<L>::check_error::galil reply contains " 
          << _galil_ans.size()
          << " characters - content is [" 
          << _galil_ans
          << "]");
    
  //- did the <Galil-Box> controller understand the resquest properly?
	if (std::count(_galil_ans.begin(), _galil_ans.end(), '?'))
	{
	  YAT_LOG("HwIO<L>::check_error::error detected - no '?'- galil ans was [" 
	           << _galil_ans 
	           << "]");
	          
    //-TODO: find a way to indicate which failed...
    yat::OSStream oms;
    oms << "the <Galil-Box> controller could not interpret the command "
        << "[unknown cmd or syntax error] - hw reply was ["
        << _galil_ans
        << "]"
        << std::ends;
        
    THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                    _CPTC(oms.str().c_str()),
                    _CPTC("galil::HwIO<L>::check_error")); 
	}
}

// ============================================================================
// HwIO<L>::health
// ============================================================================
template <typename L> 
const HwIOHealth & HwIO<L>::health (void)
{
  if (_IS_VALID_TIMESTAMP(m_health.lsuccess_ts))
  {
    _TIMESTAMP_TO_DATE(m_health.lsuccess_ts, m_health.success_ts_str);
  }
  else
  {
    m_health.success_ts_str = std::string("--/--/-- --:--:--");
  }

  if (_IS_VALID_TIMESTAMP(m_health.cfailure_ts))
  {
    _TIMESTAMP_TO_DATE(m_health.cfailure_ts, m_health.failure_ts_str);
    yat::Timestamp ts;
    _GET_TIME(ts);
    this->m_health.dt = _ELAPSED_SEC(this->m_health.cfailure_ts, ts);
  }
  else if (_IS_VALID_TIMESTAMP(m_health.lfailure_ts))
  {
    _TIMESTAMP_TO_DATE(m_health.lfailure_ts, m_health.failure_ts_str);
  }
  else 
  {
    m_health.failure_ts_str = std::string("--/--/-- --:--:--");
  }

  return m_health;
}

// ============================================================================
// HwIO<L>::tc_error_detected
// ============================================================================
template <typename L> 
void HwIO<L>::tc_error_detected (int err_code, const std::string& err_txt)
{
  yat::Timestamp now;
  _GET_TIME(now);
  std::string now_str;
  _TIMESTAMP_TO_DATE(now, now_str);
  yat::OSStream tc_txt;
  tc_txt << err_code << " - " << err_txt << " [" << now_str << "]";
  this->m_health.tc_error_txt = tc_txt.str(); 
  this->m_health.tc_error_code = err_code; 
} 

// ============================================================================
// HwIO<L>::is_udp_wouldblock_error
// ============================================================================
template <typename L> 
bool HwIO<L>::is_udp_wouldblock_error (const yat::SocketException & se)
{
  return (m_cfg.protocol == HWIO_FAST_UNSAFE) && se.is_a(yat::SoErr_WouldBlock);
}

// ============================================================================
// HwIO<L>::com_handle_identifier
// ============================================================================
template <typename L> 
int HwIO<L>::com_handle_identifier (void) const
{
  if (this->m_health.com_handle == "A")
    return 0;
  
  if (this->m_health.com_handle == "B")
    return 1;
  
  if (this->m_health.com_handle == "C")
    return 2;
  
  if (this->m_health.com_handle == "D")
    return 3;
  
  if (this->m_health.com_handle == "E")
    return 4;
  
  if (this->m_health.com_handle == "F")
    return 5; 
  
  if (this->m_health.com_handle == "G")
    return 6;  
  
  if (this->m_health.com_handle == "H")
    return 7;
    
  return -1;  
}

// ============================================================================
// HwIO<L>::can_read_without_blocking
// ============================================================================
template <typename L> 
bool HwIO<L>::can_read_without_blocking (void)
{
  //- check state
  CHECK_CONNECTION_PENDING("HwIO<L>::can_read_without_blocking");
  
  if (RECONNECTION_REQUIRED)
      return false;
      
  return this->m_socket->can_read_without_blocking();
}
  
} // namespace galil

#endif // _HW_IO_CPP_
