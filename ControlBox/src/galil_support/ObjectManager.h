//=============================================================================
// ObjectManager.h
//=============================================================================
// abstraction.......Galil boostraping helper class
// class.............ObjectManager
// original author...N.Leclercq - SOLEIL
//=============================================================================

#ifndef _OBJECT_MANAGER_H_
#define _OBJECT_MANAGER_H_

//-=============================================================================
//- DEPENDENCIES
//-=============================================================================
#include <tango.h>
#include "HwIO.h"

namespace galil
{

// ============================================================================
// SHORTCUT TO THE SHARED HWI/O
// ============================================================================
#define SHARED_HW_IO galil::ObjectManager::shared_hw_io()

//------------------------------------------------------------------------------
//- CLASS: ObjectManager
//------------------------------------------------------------------------------
class ObjectManager
{
  //- make some compilers happy
  friend class Dummy;

public:
  //- shared objects instanciation ---------
  static void init ()
		throw (Tango::DevFailed);

  //- release shared objects----------------
  static void close ()
		throw (Tango::DevFailed);

  //- shared I/O port to the <Galil-Box>
  static inline ThreadSafeHwIO * shared_hw_io ()
  {
    return ObjectManager::m_shared_hw_io;
  }
    
private:
  //- shared I/O port to the <Galil-Box>
  //- this instance is shared between box & all axis
  static ThreadSafeHwIO * m_shared_hw_io;

  //- unimplemented/unsupported members
  //-----------------------------------
  ObjectManager ();
  virtual ~ObjectManager ();
  ObjectManager (const ObjectManager &);
  ObjectManager & operator= (const ObjectManager &);
};

} //-  namespace galil

#endif //-- _OBJECT_MANAGER_H_
