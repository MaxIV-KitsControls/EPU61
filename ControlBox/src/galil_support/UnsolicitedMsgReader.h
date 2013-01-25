//=============================================================================
// UnsolicitedMsgReader.h
//=============================================================================
// abstraction.......Unsolicited msg reader (sse Galil doc)
// class.............UnsolicitedMsgReader
// original author...N.Leclercq - SOLEIL
//=============================================================================

#if defined(_UNSOLICITED_MSG_READER_SUPPORT_)

#ifndef _UNSOLICITED_MSG_READER_H_
#define _UNSOLICITED_MSG_READER_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/Task.h>
#include <yat4tango/ExceptionHelper.h>
#include "HwIO.h"

// ============================================================================
// SHORTCUT TO THE <UnsolicitedMsgReader> SINGLETON
// ============================================================================
#define UNSOLICITED_MSG_READER galil::UnsolicitedMsgReader::instance()

// ============================================================================
// CONSTs
// ============================================================================
#define kDEFAULT_USMR_PERIOD      1000
#define kDEFAULT_USMR_STARTUP_TMO 3000
#define kDEFAULT_USMR_TMO         1000
//-----------------------------------------------------------------------------

namespace galil
{

// ============================================================================
// class: UnsolicitedMsgReader <SINGLETON>
// ============================================================================
class UnsolicitedMsgReader : public yat::Task
{
  friend class ObjectManager;

public:
  //- create a dedicated type for UnsolicitedMsgReader configuration
  //---------------------------------------------------------
  typedef struct Config
  {
    //- members --------------------
    std::string galilbox_ip_addr;
    size_t galilbox_port;
    size_t startup_timeout_ms;
    size_t read_timeout_ms;
    //- ctor -----------------------
    Config ();
    //- operator= ------------------
    void operator= (const Config& src);
  } Config;

  //----------------------------------------
  // SINGLETON ACCESSOR (SEE HW_STATUS MACRO)
  //----------------------------------------
  static inline UnsolicitedMsgReader * instance ()
		throw (Tango::DevFailed)
  {
    if (! UnsolicitedMsgReader::singleton)
        THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                        _CPTC("unexpected NULL pointer [UnsolicitedMsgReader singleton not properly initialized]"),
                        _CPTC("UnsolicitedMsgReader::instance")); 
    return UnsolicitedMsgReader::singleton;
  }

  //- The following members are thread safe
  //----------------------------------------

  //- singleton configuration --------------
  void configure (const Config& cfg)
		throw (Tango::DevFailed);

  //- Underlying HwIO state
  inline const HwIOState hw_io_state () const
  {
    if (this->m_hw_io)
      return this->m_hw_io->state();
    return galil::HWIO_DISCONNECTED;
  }
  
  //- Underlying HwIO health
  inline const HwIOHealth & health (void)
  {
    if (this->m_hw_io)
      return this->m_hw_io->health();
    
    THROW_DEVFAILED(_CPTC("NO_DATA"),
                    _CPTC("the underlying HwIO is not instanciated yet"),
                    _CPTC("UnsolicitedMsgReader::health"));
  }
  
protected:
	//- handle_message -----------------------
	virtual void handle_message (yat::Message& _msg)
	  throw (yat::Exception);
	
private:
  //- The following members are NOT thread safe
  //-------------------------------------------

  //- singleton instanciation --------------
  static void init ()
		throw (Tango::DevFailed);

  //- singleton release --------------------
  static void close ()
		throw (Tango::DevFailed);

  //- the singleton ------------------------
  static UnsolicitedMsgReader * singleton;

	//- ctor ---------------------------------
	UnsolicitedMsgReader ();

	//- dtor ---------------------------------
	virtual ~UnsolicitedMsgReader ();

  //- get ref. to the underlying HWI/O port (if valid)
  inline HwIO<yat::NullMutex> * hw_io (void)
		throw (Tango::DevFailed)
  {
    if (! m_hw_io)
    {
      THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                      _CPTC("can not access hardware [com. initialization failure]"),
                      _CPTC("UnsolicitedMsgReader::hw_io")); 
    }
    return m_hw_io;
  }

  //- task configuration
  Config m_cfg;

  //- I/O port to the <Galil-Box>
  HwIO<yat::NullMutex> * m_hw_io;

  //- unimplemented/unsupported members
  //-----------------------------------
  UnsolicitedMsgReader (const UnsolicitedMsgReader &);
  UnsolicitedMsgReader & operator= (const UnsolicitedMsgReader &);
};

} // namespace galil

#endif // _UNSOLICITED_MSG_READER_H_

#endif // _UNSOLICITED_MSG_READER_SUPPORT_
