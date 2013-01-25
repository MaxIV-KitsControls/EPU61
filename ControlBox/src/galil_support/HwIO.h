//=============================================================================
// HwIO.h
//=============================================================================
// abstraction.......I/O com.with Galil-Box abstraction class
// class.............HwIO.h
// original author...N.Leclercq - J.Coquet - SOLEIL
//=============================================================================

#ifndef _HW_IO_H_
#define _HW_IO_H_

//-=============================================================================
//- DEPENDENCIES
//-=============================================================================
#include <tango.h>
#include <yat/threading/Mutex.h>
#include <yat/utils/Callback.h>
#include <yat/memory/DataBuffer.h>
#include <yat/network/ClientSocket.h>
#include <yat4tango/CommonHeader.h>
#include <yat4tango/ExceptionHelper.h>

namespace galil
{

//------------------------------------------------------------------------------
//- CLASS: IOBinaryData
//------------------------------------------------------------------------------
typedef yat::Buffer<char> BinaryData;

//------------------------------------------------------------------------------
//- TYPEDEF: HwIO::Operator
//------------------------------------------------------------------------------
typedef enum 
{ 
  IONone,
  IOOpen, 
  IOFlushInput,
  IOCheckError,
  IOClose
} IOOperator;


//------------------------------------------------------------------------------
//- TYPEDEF: HwIOState
//------------------------------------------------------------------------------
typedef enum 
{
  HWIO_DISCONNECTED,
  HWIO_DISCONNECTED_ON_COM_ERROR,
  HWIO_CONNECTING,
  HWIO_CONNECTED,
  HWIO_DISCONNECTING,
  HWIO_CONNECTION_FAILED,
  HWIO_FAULT
} HwIOState;

//------------------------------------------------------------------------------
//- TYPEDEF: HwIOProtocol
//------------------------------------------------------------------------------
typedef enum 
{
  HWIO_FAST_UNSAFE,
  HWIO_SLOW_SAFE
} HwIOProtocol;

//------------------------------------------------------------------------------
//- TYPEDEF: HwIOHealth
//------------------------------------------------------------------------------
typedef struct HwIOHealth
{
 //- members --------------------
 //- startup timestamp
 yat::Timestamp startup_ts;
 //- timestamp of last successfull connection
 yat::Timestamp lsuccess_ts;
 //- humanly readable timestamp of last successfull connection
 std::string success_ts_str;
 //- timestamp of "current" connection failure
 yat::Timestamp cfailure_ts;
 //- timestamp of connection failure
 yat::Timestamp lfailure_ts;
 //- humanly readable timestamp of last successfull connection
 std::string failure_ts_str;
 //- disconnection duration in secs
 double dt;
 //- number of connection tries since last failure
 unsigned long retries;
 //- number of communication errors since instanciation
 unsigned long com_errors;
 //- number of communication success since instanciation
 unsigned long com_success;
 //- controller com. handle identifier
 std::string com_handle;
 //- DMC TC error
 int tc_error_code;
 std::string tc_error_txt;
 
 //- ctor -----------------------
 HwIOHealth ()
 {
   this->reset();
 }
 //- ctor -----------------------
 HwIOHealth (const HwIOHealth & src)
 {
   *this = src;
 }
 //- operator= ------------------
 HwIOHealth & operator= (const HwIOHealth & src)
 {
   if (this == &src)
     return *this;
   _COPY_TIMESTAMP(src.startup_ts, startup_ts);
   _COPY_TIMESTAMP(src.lsuccess_ts, lsuccess_ts);
   _COPY_TIMESTAMP(src.lfailure_ts, lfailure_ts);
   _COPY_TIMESTAMP(src.cfailure_ts, cfailure_ts);
   dt = src.dt;
   retries = src.retries;
   com_errors = src.com_errors;
   com_success = src.com_success;
   success_ts_str = src.success_ts_str;
   failure_ts_str = src.failure_ts_str;
   com_handle = src.com_handle;
   tc_error_code = src.tc_error_code;
   tc_error_txt = src.tc_error_txt;
   return *this;
 }
 //- reset ----------------------
 void reset_com_counters (void)
 {
   com_errors = 0;
   com_success = 0;
 }
 //- reset ----------------------
 void reset (void)
 {
   dt = 0.;
   retries = 0;
   com_errors = 0;
   com_success = 0;
   com_handle = "?";
   tc_error_code = 0;
   tc_error_txt = "no controller error reported";
   _RESET_TIMESTAMP(startup_ts);
   _RESET_TIMESTAMP(lsuccess_ts);
   _RESET_TIMESTAMP(cfailure_ts);
   _RESET_TIMESTAMP(lfailure_ts);
   success_ts_str = std::string("--/--/-- --:--:--");
   failure_ts_str = std::string("--/--/-- --:--:--");
 }
} HwIOHealth;

//------------------------------------------------------------------------------
//- CLASS: HwIO Open/Close Hook
//------------------------------------------------------------------------------
typedef enum
{
  HWIO_OPEN,
  HWIO_CLOSE,
  HWIO_CLOSE_ON_ERROR
} HwIOHookOp;
//------------------------------------------------------------------------------
YAT_DEFINE_CALLBACK(HwIOHook, HwIOHookOp);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//- CLASS: HwIO
//------------------------------------------------------------------------------
template <typename LOCK> class HwIO : public yat::SharedObject
{
  typedef yat::SharedObject Inherited;

public:
  //- create a dedicated type for configuration
  //-------------------------------------------
  typedef struct Config
  {
    //- members --------------------
    std::string ip_addr;
    size_t port;
    HwIOProtocol protocol;
    bool blocking;
    bool close_on_error;
    //- ctor -----------------------
    Config () 
      : ip_addr(""), 
        port(0), 
        protocol(HWIO_SLOW_SAFE),
        blocking(true),
        close_on_error(true)
    {
      /* noop ctor */
    }
    //- operator= ------------------
    void operator= (const Config& src)
    {
      ip_addr = src.ip_addr;
      port = src.port;
      protocol = src.protocol;
      blocking = src.blocking;
      close_on_error = src.close_on_error;
    }
  } Config;

  //- ctors
  HwIO ();
  HwIO (const Config & cfg);
  HwIO (const std::string& ip_addr, size_t port, HwIOProtocol prot = HWIO_SLOW_SAFE);
   
  //- open HwIO (previously configured via ctor)
  void open (void)
		throw (Tango::DevFailed); 

  //- open HwIO
  void open (std::string ip_addr, size_t port)
		throw (Tango::DevFailed); 

  //- reopen HwIO (atomic close/open)
  void reopen (std::string ip_addr, size_t port)
		throw (Tango::DevFailed);
		
  //- close HwIO
  void close (void)
		throw (Tango::DevFailed);

  //- cleanup all connections
  void cleanup (void)
		throw (Tango::DevFailed); 

  //- NOTE: the following members should be template but some
  //- compilers (including msvc6) don't like template members
  //- nested into a template class. Anyway, let's go...

  //- atomic write on the I/O port 
  void in (const std::string& _in, const IOOperator& _op = IOCheckError)
		throw (Tango::DevFailed)
  {
    yat::AutoMutex<LOCK> guard(this->m_lock);
    *this << _in << _op;
  }

  //- atomic write on the I/O port 
  void in (const BinaryData& _in, const IOOperator& _op = IOCheckError)
		throw (Tango::DevFailed)
  {
    yat::AutoMutex<LOCK> guard(this->m_lock);
    *this << _in << _op;
  }

  //- atomic write on the I/O port
  void inout (const std::string& _in, std::string& out_)
		throw (Tango::DevFailed)
  {
    yat::AutoMutex<LOCK> guard(this->m_lock);
    *this << _in;
    *this >> out_;
  }

  //- atomic write on the I/O port
  void inout (const std::string& _in, BinaryData& out_)
		throw (Tango::DevFailed)
  {
    yat::AutoMutex<LOCK> guard(this->m_lock);
    *this << _in;
    *this >> out_;
  }

  //- atomic write on the I/O port
  void inout (const BinaryData& _in, std::string& out_)
		throw (Tango::DevFailed)
  {
    yat::AutoMutex<LOCK> guard(this->m_lock);
    *this << _in;
    *this >> out_;
  }

  //- atomic write on the I/O port
  void inout (const BinaryData& _in, BinaryData& out_)
		throw (Tango::DevFailed)
  {
    yat::AutoMutex<LOCK> guard(this->m_lock);
    *this << _in;
    *this >> out_;
  }

  //- atomic out on the I/O port
  void out (std::string& out_)
		throw (Tango::DevFailed)
  {
    yat::AutoMutex<LOCK> guard(this->m_lock);
    *this >> out_;
  }

  //- atomic out on the I/O port
  void out (BinaryData& out_)
		throw (Tango::DevFailed)
  {
    yat::AutoMutex<LOCK> guard(this->m_lock);
    *this >> out_;
  }

  //- I/O port alive?
  inline bool alive (void) const
  {
    return this->m_socket != 0;
  }

  //- yat::SharedObject interface
  inline HwIO * duplicate ()
  {
    return static_cast<HwIO*>(Inherited::duplicate());
  }

  //- connection state 
  inline const HwIOState& state () const
  {
    return m_state;
  }

  //- connection health
  const HwIOHealth & health (void);

  //- reset com. counters
  void reset_com_counters (void)
  {
    this->m_health.reset_com_counters();
  }
 
  //- returns the current configuration
  const Config & configuration (void) const
  {
    return this->m_cfg;
  }
  
  //- com. handle identifier
  const std::string & com_handle (void) const
  {
    return m_health.com_handle;
  }
  
  //- set the TC error
  void tc_error_detected (int err_code, const std::string& err_txt);
  
  //- returns the com handle identifier
  int com_handle_identifier (void) const;
  
  //- register HwIOHook
  void register_hook (const HwIOHook & _hook)
  {
    this->m_hook = _hook;
  }
  
  //- is there any pending data?
  bool can_read_without_blocking (void);
  
private:
  //- dtor (private dtor due to SharedObject inheritance)
  virtual ~HwIO ();

  //- internal impl: open the I/O port
  void open_i ()
		throw (Tango::DevFailed); 

  //- internal impl: close the I/O port
  void close_i (bool on_error = false)
		throw (Tango::DevFailed); 
		
  //- internal impl: close all com. on controller side
  void cleanup_i () 
    throw (Tango::DevFailed);

  //- write on the I/O port
  HwIO<LOCK>& operator << (const std::string& _in)
		throw (Tango::DevFailed); 

  //- write on the I/O port
  HwIO<LOCK>& operator << (const BinaryData& _in)
		throw (Tango::DevFailed);

  //- apply operator to the IO port
  HwIO<LOCK>& operator << (const IOOperator& _op)
		throw (Tango::DevFailed);

  //- write on the I/O port
  void operator >> (std::string& out_)
		throw (Tango::DevFailed); 

  //- read on the I/O port
  void operator >> (BinaryData& out_)
		throw (Tango::DevFailed);

  //- check error in Galil box a Galil-box answer
  void check_error (const std::string& _cmd, 
                    const std::string& _galil_ans)
    throw (Tango::DevFailed);

  //- check error in Galil box a Galil-box answer
  void check_error (const std::string& _galil_ans)
    throw (Tango::DevFailed);

  //- check connection state
  inline void check_connection_state (void)
    throw (Tango::DevFailed)
  {
    if (this->m_state != HWIO_CONNECTED)
    {
      THROW_DEVFAILED(_CPTC("COM_ERROR"),
                      _CPTC("there is currently no valid communication link with the hardware"),
                      _CPTC("galil::HwIO<L>::check_connection_state"));
    }
  }

  //- is the current error a UDP-WouldBlock error?
  bool is_udp_wouldblock_error (const yat::SocketException & se);

  //- configuration
  Config m_cfg;

  //- the [Dummy or Device]Mutex
  LOCK m_lock;

  //- actual HwIO to the <Galil-Box>
  yat::ClientSocket * m_socket;

  //- connection state 
  HwIOState m_state;

  //- connection health
  HwIOHealth m_health;
  
  //- the hook
  HwIOHook m_hook;

  //- unimplemented/unsupported members
  //-----------------------------------
  HwIO (HwIO &);
  HwIO & operator= (HwIO &);
};


//------------------------------------------------------------------------------
//- CLASS: ThreadSafeHwIO
//------------------------------------------------------------------------------
typedef HwIO<yat::Mutex> ThreadSafeHwIO;

} //-  namespace galil

#include "HwIO.cpp"

#endif //-- _HW_IO_H_
