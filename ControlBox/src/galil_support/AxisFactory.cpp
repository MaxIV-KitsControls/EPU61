//=============================================================================
// AxisFactory.cpp
//=============================================================================
// abstraction.......Galil AxisFactory abstraction class
// class.............AxisFactory
// original author...N.Leclercq - J.Coquet - SOLEIL
//=============================================================================

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include "BitsRecords.h"
#include "Box.h"
#include "AxisFactory.h"
#include "ObjectManager.h"

namespace galil
{
// ============================================================================
// SINGLETON
// ============================================================================
AxisFactory * AxisFactory::singleton = 0;

// ============================================================================
// AxisFactory::init <STATIC MEMBER>
// ============================================================================
void AxisFactory::init () 
	throw (Tango::DevFailed)
{
  //- already instanciated?
  if (AxisFactory::singleton)
    return;

  try
  {
  	//- actual instanciation
    AxisFactory::singleton = new AxisFactory();
    if (AxisFactory::singleton == 0)
      throw std::bad_alloc();
  }
  catch (const std::bad_alloc&)
  {
    THROW_DEVFAILED(_CPTC("OUT_OF_MEMORY"),
                    _CPTC("AxisFactory::singleton instanciation failed"),
                    _CPTC("galil::AxisFactory::init"));	
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught while while trying to instanciate the AxisFactory::singleton"),
                    _CPTC("galil::AxisFactory::init"));
  }
}

// ============================================================================
// AxisFactory::close  <STATIC MEMBER>
// ============================================================================
void AxisFactory::close ()
	throw (Tango::DevFailed)
{
  if (! AxisFactory::singleton)
    return;

  try
  {
    //- release the <AxisFactory> singleton
    SAFE_DELETE_PTR(AxisFactory::singleton);
  }
  catch (Tango::DevFailed& df)
  {
    RETHROW_DEVFAILED(df,
                      _CPTC("SOFTWARE_ERROR"),
                      _CPTC("Tango exception caught while trying to shutdown the AxisFactory::singleton"),
                      _CPTC("galil::AxisFactory::close"));
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught while while trying to shutdown the AxisFactory::singleton"),
                    _CPTC("galil::AxisFactory::close"));
  }
}

// ============================================================================
// AxisFactory::AxisFactory
// ============================================================================
AxisFactory::AxisFactory ()
  : m_axis (MAX_AXIS)
{
	YAT_TRACE("galil::AxisFactory::AxisFactory [default ctor]");

  //- init each entry in the axis repository
  for (size_t i = 0; i < this->m_axis.size(); i++)
    this->m_axis[i] = 0;
}

// ============================================================================
// AxisFactory::~AxisFactory
// ============================================================================
AxisFactory::~AxisFactory (void)
{
	YAT_TRACE("galil::AxisFactory::~AxisFactory");

  //- release any allocated axis
  for (size_t i = 0; i < this->m_axis.size(); i++)
  {
    if (this->m_axis[i])
    {
      this->m_axis[i]->release();
      this->m_axis[i] = 0;
    }
  }
}

// ============================================================================
// AxisFactory::attach_axis
// ============================================================================
Axis * AxisFactory::attach_axis (const Axis::Config& _axis_cfg)
		throw (Tango::DevFailed)
{
  YAT_TRACE("galil::AxisFactory::attach_axis");

  //- check id
  if (_axis_cfg.id <= galil::MIN_AXIS || _axis_cfg.id >= galil::MAX_AXIS)
  {
    THROW_DEVFAILED(_CPTC("no such axis"),
                    _CPTC("invalid axis identifier specified"),
                    _CPTC("galil::AxisFactory::attach_axis"));
  }

  //- axis already instanciated?
  if (this->m_axis[_axis_cfg.id])
  {
    //- just reconfigure it...
    this->m_axis[_axis_cfg.id]->configure(_axis_cfg);
    //- then return a reference to it
    return this->m_axis[_axis_cfg.id]->duplicate();
  }

  //- allocate the axis
  try
  {
    this->m_axis[_axis_cfg.id] = new Axis();

    if (this->m_axis[_axis_cfg.id] == 0)
      throw std::bad_alloc();
  }
  catch (std::bad_alloc&)
  {
    THROW_DEVFAILED(_CPTC("OUT_OF_MEMORY"),
                    _CPTC("Axis instanciation failed"),
                    _CPTC("galil::AxisFactory::attach_axis"));
  }
  catch (Tango::DevFailed& df)
  {
    RETHROW_DEVFAILED(df,
                      _CPTC("SOFTWARE_ERROR"),
                      _CPTC("Tango exception caught while trying to instanciate an Axis"),
                      _CPTC("galil::AxisFactory::attach_axis"));
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught while trying to instanciate an Axis"),
                    _CPTC("galil::AxisFactory::attach_axis")); 
  }

  //- configure the axis
  try
  {
    this->m_axis[_axis_cfg.id]->configure(_axis_cfg);
  }
  catch (Tango::DevFailed& df)
  {
    RETHROW_DEVFAILED(df,
                      _CPTC("SOFTWARE_ERROR"),
                      _CPTC("Tango exception caught while trying to configure an Axis"),
                      _CPTC("galil::AxisFactory::attach_axis"));
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("UNKNOWN_ERROR"),
                    _CPTC("unknown exception caught while trying to configure an Axis"),
                    _CPTC("galil::AxisFactory::attach_axis")); 
  }

  return this->m_axis[_axis_cfg.id]->duplicate();
}
    
// ============================================================================
// AxisFactory::get_axis
// ============================================================================
Axis * AxisFactory::get_axis (const AxisIdentifier& _axis_id)
		throw (Tango::DevFailed)
{
  YAT_TRACE("galil::AxisFactory::get_axis");

  //- check id
  if (_axis_id <= galil::MIN_AXIS || _axis_id >= galil::MAX_AXIS)
  {
    THROW_DEVFAILED(_CPTC("no such axis"),
                    _CPTC("invalid axis identifier specified"),
                    _CPTC("galil::AxisFactory::get_axis"));
  }

  //- axis instanciated?
  if (! this->m_axis[_axis_id])
  {
    THROW_DEVFAILED(_CPTC("no such axis"),
                    _CPTC("requested Axis does not exists"),
                    _CPTC("galil::AxisFactory::get_axis"));
  }
  
  //- return a reference to the existing Axis
  return this->m_axis[_axis_id]->duplicate();
}

} // namespace galil
