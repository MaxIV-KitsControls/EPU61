//=============================================================================
// ObjectManager.h
//=============================================================================
// abstraction.......Galil boostraping helper class
// class.............ObjectManager
// original author...N.Leclercq - SOLEIL
//=============================================================================


// ============================================================================
// DEPENDENCIES
// ============================================================================
#include "ObjectManager.h"
#include "HwStatusProxy.h"
#if defined(_UNSOLICITED_MSG_READER_SUPPORT_)
# include "UnsolicitedMsgReader.h"
#endif
#include "AxisFactory.h"

namespace galil
{

// ============================================================================
// STATIC MEMBERS
// ============================================================================
ThreadSafeHwIO * ObjectManager::m_shared_hw_io = 0;

// ============================================================================
// ObjectManager::init <STATIC MEMBER>
// ============================================================================
void ObjectManager::init () 
	throw (Tango::DevFailed)
{
  YAT_TRACE_STATIC("galil::ObjectManager::init");

  //- instanciate the shared I/O port
  try
  {
  	ObjectManager::m_shared_hw_io = new ThreadSafeHwIO;

    if (ObjectManager::m_shared_hw_io == 0)
      throw std::bad_alloc();
  }
  catch (const std::bad_alloc&)
  {
    THROW_DEVFAILED(_CPTC("OUT_OF_MEMORY"),
                    _CPTC("ThreadSafeHwIO instanciation failed"),
                    _CPTC("galil::ObjectManager::init"));	
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("OUT_OF_MEMORY"),
                    _CPTC("ThreadSafeHwIO instanciation failed"),
                    _CPTC("galil::ObjectManager::init"));	
  }
  
  //- initialize the HwStatsusProxy::singleton
  HwStatusProxy::init();

#if defined(_UNSOLICITED_MSG_READER_SUPPORT_)
  //- initialize the UnsolicitedMsgReader::singleton
  UnsolicitedMsgReader::init();
#endif

  //- initialize the AxisFactory::singleton
  AxisFactory::init();
}

// ============================================================================
// ObjectManager::close  <STATIC MEMBER>
// ============================================================================
void ObjectManager::close ()
	throw (Tango::DevFailed)
{
  YAT_TRACE_STATIC("galil::ObjectManager::close");

  //- release the AxisFactory::singleton
  AxisFactory::close();

#if defined(_UNSOLICITED_MSG_READER_SUPPORT_)
  //- release the UnsolicitedMsgReader::singleton
  UnsolicitedMsgReader::close();
#endif

  //- release the HwStatsusProxy::singleton
  HwStatusProxy::close();

  //- release the shared I/O port
  try 
  {
    SAFE_RELEASE(ObjectManager::m_shared_hw_io);
  }
  catch (...)
  {
    //- ignore any error
  }  
}


} // namespace galil
