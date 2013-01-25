//=============================================================================
// Box.h
//=============================================================================
// abstraction.......Galil Box abstraction class
// class.............Box
// original author...N.Leclercq - J.Coquet - SOLEIL
//=============================================================================

#ifndef _BOX_H_
#define _BOX_H_

//-=============================================================================
//- DEPENDENCIES
//-=============================================================================
#include <yat4tango/ExceptionHelper.h>
#include "HwIO.h"

namespace galil
{

// ============================================================================
// SHORTCUT TO THE <Box> SINGLETON
// ============================================================================
#define GALIL_BOX galil::Box::instance()

//------------------------------------------------------------------------------
//- CLASS: Box
//------------------------------------------------------------------------------
class Box : public Tango::LogAdapter
{
public:

  //- create a dedicated type for box state
  //----------------------------------------
  typedef enum 
  {
    BOX_ERROR,
    BOX_WARNING, 
    BOX_OPEN,
    BOX_CLOSE,
    BOX_UNKNOWN
  } BoxState;

  //- create a dedicated type for com state
  //----------------------------------------
  typedef enum 
  {
    COM_DISCONNECTED,
    COM_DISCONNECTED_ON_COM_ERROR,
    COM_CONNECTING,
    COM_CONNECTED,
    COM_CONNECTION_FAILED,
    COM_FAULT,
  } ComState;

  //- a dedicated type for Box status
  //----------------------------------------
  typedef struct Status
  {
    //- members -------------------
    //- the boix state
    BoxState box_state;
    //- the com state
    ComState com_state;
    //- the human readable status
    std::string status;
    //- ctor ----------------------
    Status ();
    //- operator= -----------------
    void operator= (const Status& src);
  } Status;

  //- a dedicated type for Box configuration
  //----------------------------------------
  typedef struct Config
  {
    //- members --------------------
    std::string galilbox_ip_addr;
    size_t galilbox_port;
    size_t status_update_period_ms;
    size_t status_expiration_timeout_ms;
    size_t startup_timeout_ms;
    Tango::DeviceImpl * host_device;
    //- ctor -----------------------
    Config ();
    //- operator= ------------------
    void operator= (const Config& src);
  } Config;

  //----------------------------------------
  // SINGLETON ACCESSOR (SEE GALIL_BOX MACRO)
  //----------------------------------------
  static inline Box * instance ()
		throw (Tango::DevFailed)
  {
    if (! Box::singleton)
        THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                        _CPTC("unexpected NULL pointer [Box singleton not properly initialized]"),
                        _CPTC("Box::instance")); 
    return Box::singleton;
  }

  //- singleton instanciation --------------
  static void init (const Config& cfg)
		throw (Tango::DevFailed);

  //- singleton release --------------------
  static void close ()
		throw (Tango::DevFailed);

  //- Box::Status accessor -----------------
  void status (Status & s)
    throw (Tango::DevFailed);

  //- Box::Reset cmd -----------------------
  void reset () 
    throw (Tango::DevFailed);

  //- Box::Start uCode cmd -----------------
  void start_ucode () 
    throw (Tango::DevFailed);

  //- Box::Returns both generic an specific uCode version numbers
  const std::string ucode_version () 
    throw (Tango::DevFailed);

  //- Box::Returns both generic an specific firmware version numbers
  const std::string firmware_version () 
    throw (Tango::DevFailed);

  //- Box::Executes the specified low level (or expert) cmd
  const std::string exec_low_level_cmd (const std::string& cmd) 
    throw (Tango::DevFailed);

private:
  //- private ctor
  Box (const Config & cfg);

  //- private dtor
  virtual ~Box (); 

  //- the singleton
  static Box * singleton;

  //- configuration
  Box::Config m_cfg;

  //- unimplemented/unsupported members
  //-----------------------------------
  Box (const Box &);
  Box & operator= (const Box &);
};

} //-  namespace galil

#endif //-- _BOX_H_
