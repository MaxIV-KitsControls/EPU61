//=============================================================================
// AbstractSlit.cpp
//=============================================================================
// abstraction.......Galil axis abstraction class
// class.............AbstractSlit
// original author...N.Leclercq - J.Coquet - SOLEIL
//=============================================================================

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <tango.h>
#include "AxisFactory.h"
#include "AbstractSlit.h"

namespace galil
{

// ============================================================================
// AbstractSlitStatus::AbstractSlitStatus
// ============================================================================
SlitStatus::SlitStatus ()
{
  //- noop ctor
}

// ============================================================================
// AbstractSlitStatus::operator=
// ============================================================================
void SlitStatus::operator= (const SlitStatus& src)
{
  if (&src == this)
    return;

  this->slave_status  = src.slave_status;
  this->master_status = src.master_status;
	this->slit_status   = src.slit_status;
}

// ============================================================================
// AbstractSlit::Config::Config
// ============================================================================
AbstractSlit::Config::Config ()
 : master (MIN_AXIS),
   slave (MIN_AXIS),
   invert_pos_dir (false),
   host_device (0)
{
  //- noop ctor
}

// ============================================================================
// AbstractSlit::Config::operator=
// ============================================================================
void AbstractSlit::Config::operator= (const Config& src)
{
  this->master = src.master;
  this->slave = src.slave; 
  this->invert_pos_dir = src.invert_pos_dir;
  this->host_device = src.host_device;
}

// ============================================================================
// AbstractSlit::AbstractSlit
// ============================================================================
AbstractSlit::AbstractSlit ()
 : yat4tango::TangoLogAdapter (0),
   m_cfg (),
   m_master_axis(0),
   m_slave_axis(0)
{
	YAT_TRACE("galil::AbstractSlit::AbstractSlit");
}

// ============================================================================
// AbstractSlit::~AbstractSlit
// ============================================================================
AbstractSlit::~AbstractSlit (void)
{
  YAT_TRACE("galil::AbstractSlit::~AbstractSlit");

  //- release reference to the salve Axis
  SAFE_RELEASE(this->m_slave_axis);

  //- release reference to the master Axis
  SAFE_RELEASE(this->m_master_axis);
}

// ============================================================================
// AbstractSlit::configure
// ============================================================================
void AbstractSlit::configure (const Config& _cfg) throw (Tango::DevFailed)
{
	YAT_TRACE("galil::AbstractSlit::configure");

  //- release reference to the slave Axis
  SAFE_RELEASE(this->m_slave_axis);

  //- release reference to the master Axis
  SAFE_RELEASE(this->m_master_axis);

  //- copy the config locally
  this->m_cfg = _cfg;

  //- set the host device
  this->host(this->m_cfg.host_device);

  //- get a ref. master axis
  try 
  {
    this->m_master_axis = AXIS_FACTORY->get_axis(this->m_cfg.master);
  }
  catch (Tango::DevFailed& df)
  {
    ERROR_STREAM << df << std::endl;
    yat::OSStream err_desc;
    err_desc << "master axis error - axis " 
             << Axis::axis_id_to_string(this->m_cfg.master)
             << " does not exist - should be previously instanciated";
    THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                    _CPTC(err_desc.str().c_str()),
                    _CPTC("galil::AbstractSlit::configure"));
  }

  //- get a ref. to slave axis
  try 
  {
    this->m_slave_axis = AXIS_FACTORY->get_axis(this->m_cfg.slave);
  }
  catch (Tango::DevFailed& df)
  {
    ERROR_STREAM << df << std::endl;
    yat::OSStream err_desc;
    err_desc << "slave axis error - axis " 
             << Axis::axis_id_to_string(this->m_cfg.slave)
             << " does not exist - should be previously instanciated";
    THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                    _CPTC(err_desc.str().c_str()),
                    _CPTC("galil::AbstractSlit::configure"));
  }

  //- compute m/s ratio - avoid divide by 0!
  double ms_ratio = 0.;
  try
  {
    ms_ratio = this->m_master_axis->global_ratio() / this->m_slave_axis->global_ratio();
    INFO_STREAM << "- master_ratio...." << this->m_master_axis->global_ratio()
                << "- slave ratio....." << this->m_slave_axis->global_ratio()
                << "- gearing ratio..." << ms_ratio
                << std::endl;
  }
  catch (...)
  {
    yat::OSStream err_desc;
    err_desc << " Slit configuration error - the slave axis motor ratio is 0"
             << " [ask your motorist to setup axis properly]";
    THROW_DEVFAILED(_CPTC("SOFTWARE_ERROR"),
                    _CPTC(err_desc.str().c_str()),
                    _CPTC("galil::AbstractSlit::configure"));
  }

  //- set master Axis gearing configuration
  Axis::GearingConfig master_config;
  master_config.mode = Axis::MASTER_AXIS;
  master_config.associated_axis = this->m_cfg.slave;
  master_config.ms_ratio = ms_ratio;
  this->m_master_axis->configure_gearing(master_config);

  //- set slave Axis gearing configuration
  Axis::GearingConfig slave_config;
  slave_config.mode = Axis::SLAVE_AXIS;
  slave_config.associated_axis = this->m_cfg.master;
  slave_config.ms_ratio = ms_ratio;
  this->m_slave_axis->configure_gearing(slave_config);

  //- do not enable gearing at startup...
  //- reference position may need to be initialized on each axis !
}

// ============================================================================
// AbstractSlit::gap mutator
// pure virtual method derivate classes must implement
// ============================================================================
// void AbstractSlit::gap (double _g) throw (Tango::DevFailed)
// {
// }

// ============================================================================
// AbstractSlit::gap accessor
// ============================================================================
double AbstractSlit::gap ()
{
	YAT_TRACE("galil::AbstractSlit::gap");

  return this->master_position() + this->slave_position();
}

// ============================================================================
// AbstractSlit::position mutator
// pure virtual method derivate classes must implement
// ============================================================================
// void AbstractSlit::position (double _p) throw (Tango::DevFailed)
// {
// }

// ============================================================================
// AbstractSlit::position accessor
// ============================================================================
double AbstractSlit::position ()
{
	YAT_TRACE("galil::AbstractSlit::position");
	double new_p;
	new_p = (this->slave_position() - this->master_position()) / 2.;
	//- invert position if needed
	if(this->m_cfg.invert_pos_dir)
  {
		new_p *= -1.;
  }
  return new_p;
}

// ============================================================================
// AbstractSlit::master_position
// pure virtual method derivate classes must implement
// ============================================================================
// void AbstractSlit::master_position (double _mp) throw (Tango::DevFailed)
// {
// }

// ============================================================================
// AbstractSlit::master_position
// ============================================================================
double AbstractSlit::master_position ()
{
	YAT_TRACE("galil::AbstractSlit::master_position");

  double current_pos;

  //- get current master pos
  try
  {
    current_pos = this->m_master_axis->abs_pos();
  }
  catch (Tango::DevFailed& df)
  {
    ERROR_STREAM << "AbstractSlit::master_position::" << df << std::endl;
    RETHROW_DEVFAILED(df,
                      _CPTC("HARDWARE_ERROR"),
                      _CPTC("could not obtain the master axis position"),
                      _CPTC("galil::AbstractSlit::master_position"));
  }

  return current_pos;
}

// ============================================================================
// AbstractSlit::slave_position
// pure virtual method derivate classes must implement
// ============================================================================
// void AbstractSlit::slave_position (double _sp) throw (Tango::DevFailed)
// {
// }

// ============================================================================
// AbstractSlit::slave_position
// ============================================================================
double AbstractSlit::slave_position ()
{
	YAT_TRACE("galil::AbstractSlit::slave_position");

  double current_pos;

  //- get current master pos
  try
  {
    current_pos = this->m_slave_axis->abs_pos();
  }
  catch (Tango::DevFailed& df)
  {
    ERROR_STREAM << "AbstractSlit::slave_position::" << df << std::endl;
    RETHROW_DEVFAILED(df,
                      _CPTC("HARDWARE_ERROR"),
                      _CPTC("could not obtain the slave axis position"),
                      _CPTC("galil::AbstractSlit::slave_position"));
  }

  return current_pos;
}

// ============================================================================
// AbstractSlit::stop 
// ============================================================================
// void AbstractSlit::stop () throw (Tango::DevFailed)
// {
// }


// ============================================================================
// AbstractSlit::off 
// ============================================================================
void AbstractSlit::off () throw (Tango::DevFailed)
{
	YAT_TRACE("galil::AbstractSlit::off");
  try
  {
    this->disable_gearing ();
    this->m_slave_axis->off ();
    this->m_master_axis->off ();
  }
  catch (Tango::DevFailed &e)
  {
    THROW_DEVFAILED(_CPTC("BAD_STATE"),
                    _CPTC(e.errors[0].desc),
                    _CPTC("galil::AbstractSlit::off"));
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("BAD_STATE"),
                    _CPTC("unknown error trying to turn off motors"),
                    _CPTC("galil::AbstractSlit::off"));
  }

}

// ============================================================================
// AbstractSlit::on 
// ============================================================================
void AbstractSlit::on () throw (Tango::DevFailed)
{
	YAT_TRACE("galil::AbstractSlit::on");
  try
  {
    this->m_slave_axis->on ();
    this->m_master_axis->on ();
  }
  catch (Tango::DevFailed &e)
  {
    THROW_DEVFAILED(_CPTC("BAD_STATE"),
                    _CPTC(e.errors[0].desc),
                    _CPTC("galil::AbstractSlit::off"));
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("BAD_STATE"),
                    _CPTC("unknown error trying to turn off motors"),
                    _CPTC("galil::AbstractSlit::off"));
  }
}

// ============================================================================
// AbstractSlit::disable_gearing 
// ============================================================================
void AbstractSlit::disable_gearing () throw (Tango::DevFailed)
{
	YAT_TRACE("galil::AbstractSlit::disable_gearing");

  //- disable gearing on slave axis (enough to disable on both axis)
  if (this->m_slave_axis->gearing_enabled())
    this->m_slave_axis->disable_gearing();
}

// ============================================================================
// AbstractSlit::state 
// pure virtual method derivate classes must implement
// ============================================================================
// AxisState AbstractSlit::state ()
// {
// }

// ============================================================================
// AbstractSlit::status 
// pure virtual method derivate classes must implement
// ============================================================================
// void AbstractSlit::status (AbstractSlitStatus & status_)  throw (Tango::DevFailed)
// {
// }

// ============================================================================
// AbstractSlit::command_allowed
// Giving the current status of the AbstractSlit, this 
// method checks whether or not the specified command is 
// allowed. In case the command is refused and the <why> is 
// not NULL, the reason is returned in the provided string.
// ============================================================================
// pure virtual method derivate classes must implement

// ============================================================================
// GearedSlit::why_cmd_refused 
// ============================================================================
// pure virtual method derivate classes must implement




} // namespace galil



