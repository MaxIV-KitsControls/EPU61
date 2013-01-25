//=============================================================================
// AxisFactory.h
//=============================================================================
// abstraction.......Galil axis factory abstraction class
// class.............AxisFactory
// original author...N.Leclercq - J.Coquet - SOLEIL
//=============================================================================

#ifndef _AXIS_FACTORY_H_
#define _AXIS_FACTORY_H_

//-=============================================================================
//- DEPENDENCIES
//-=============================================================================
#include "Axis.h"

// ============================================================================
// SHORTCUT TO THE <Box> SINGLETON
// ============================================================================
#define AXIS_FACTORY galil::AxisFactory::instance()

namespace galil
{

//------------------------------------------------------------------------------
//- CLASS: AxisFactory
//------------------------------------------------------------------------------
class AxisFactory
{
  typedef std::vector<Axis *> AxisRepository;

  friend class ObjectManager;

public:
  //----------------------------------------
  // SINGLETON ACCESSOR (SEE GALIL_BOX MACRO)
  //----------------------------------------
  static inline AxisFactory * instance ()
		throw (Tango::DevFailed)
  {
    if (! AxisFactory::singleton)
        THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                        _CPTC("unexpected NULL pointer [AxisFactory singleton not properly initialized]"),
                        _CPTC("AxisFactory::instance")); 
    return AxisFactory::singleton;
  }

  //- singleton instanciation --------------
  static void init ()
		throw (Tango::DevFailed);

  //- singleton release --------------------
  static void close ()
		throw (Tango::DevFailed);

  //- attach a new axis to the box ---------
  Axis * attach_axis (const Axis::Config& axis_cfg)
		throw (Tango::DevFailed);

  //- return a ref.to an existing Axis  ----
  Axis * get_axis (const AxisIdentifier& axis_id)
		throw (Tango::DevFailed);

private:
  //- ctor -------------
  AxisFactory ();

  //- dtor -------------
  virtual ~AxisFactory ();

  //- the singleton
  static AxisFactory * singleton;

  //- axis repository
  AxisRepository m_axis;

  //- unimplemented/unsupported members
  //-----------------------------------
  AxisFactory (const AxisFactory &);
  AxisFactory & operator= (const AxisFactory &);
};

} //-  namespace galil

#endif //-- _AXIS_FACTORY_H_
