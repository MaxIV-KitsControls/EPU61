static const char *RcsId = "$Header: /usr/local/CVS/DeviceServer/Generic/Motorisation/ControlBoxV2/src/GalilGearedAxes.cpp,v 1.5 2011/07/15 09:47:43 buteau Exp $";
//+=============================================================================
//
// file :         GalilGearedAxes.cpp
//
// description :  C++ source for the GalilGearedAxes and its commands. 
//                The class is derived from Device. It represents the
//                CORBA servant object which will be accessed from the
//                network. All commands which can be executed on the
//                GalilGearedAxes are implemented in this file.
//
// project :      TANGO Device Server
//
// $Author: buteau $
//
// $Revision: 1.5 $
//
// $Log: GalilGearedAxes.cpp,v $
// Revision 1.5  2011/07/15 09:47:43  buteau
// MANTIS 19368
//
// Revision 1.3  2011/05/19 10:52:44  coquet
// some include path updated
// changes yat dependency order
//
//
//
// copyleft :     European Synchrotron Radiation Facility
//                BP 220, Grenoble 38043
//                FRANCE
//
//-=============================================================================
//
//  		This file is generated by POGO
//	(Program Obviously used to Generate tango Object)
//
//         (c) - Software Engineering Group - ESRF
//=============================================================================



//===================================================================
//
//	The following table gives the correspondance
//	between commands and method's name.
//
//  Command's name      |  Method's name
//	----------------------------------------
//  State               |  dev_state()
//  Status              |  dev_status()
//  Stop                |  stop()
//  SetIndependantMode  |  set_independant_mode()
//  SetGearedMode       |  set_geared_mode()
//
//===================================================================


#include <tango.h>
#include <GalilGearedAxes.h>
#include <GalilGearedAxesClass.h>
#include <PogoHelper.h>

namespace GalilGearedAxes_ns
{

//+----------------------------------------------------------------------------
//
// method : 		GalilGearedAxes::GalilGearedAxes(string &s)
// 
// description : 	constructor for simulated GalilGearedAxes
//
// in : - cl : Pointer to the DeviceClass object
//      - s : Device name 
//
//-----------------------------------------------------------------------------
GalilGearedAxes::GalilGearedAxes(Tango::DeviceClass *cl,string &s)
:Tango::Device_4Impl(cl,s.c_str())
{
	init_device();
}

GalilGearedAxes::GalilGearedAxes(Tango::DeviceClass *cl,const char *s)
:Tango::Device_4Impl(cl,s)
{
	init_device();
}

GalilGearedAxes::GalilGearedAxes(Tango::DeviceClass *cl,const char *s,const char *d)
:Tango::Device_4Impl(cl,s,d)
{
	init_device();
}
//+----------------------------------------------------------------------------
//
// method : 		GalilGearedAxes::delete_device()
// 
// description : 	will be called at device destruction or at init command.
//
//-----------------------------------------------------------------------------
void GalilGearedAxes::delete_device()
{
	//	Delete device's allocated object

	// delete read attributes
	DELETE_SCALAR_ATTRIBUTE(attr_masterPosition_read);
	DELETE_SCALAR_ATTRIBUTE(attr_slavePosition_read);
	DELETE_SCALAR_ATTRIBUTE(attr_isGeared_read);

  //- release the Slit object
  if (this->m_geared_axes)
  {
    delete this->m_geared_axes;
    this->m_geared_axes = 0;
  }

}

//+----------------------------------------------------------------------------
//
// method : 		GalilGearedAxes::init_device()
// 
// description : 	will be called at device initialization.
//
//-----------------------------------------------------------------------------
void GalilGearedAxes::init_device()
{
	INFO_STREAM << "GalilGearedAxes::GalilGearedAxes() create device " << device_name << endl;

	// Initialise variables to default values
	//--------------------------------------------
	this->m_status_str = "initializing device...";
	this->m_init_device_done = false;
  this->m_geared_axes = 0;

	// create read attributes
	CREATE_SCALAR_ATTRIBUTE(attr_masterPosition_read);
	CREATE_SCALAR_ATTRIBUTE(attr_slavePosition_read);
	CREATE_SCALAR_ATTRIBUTE(attr_isGeared_read);
	
	get_device_property();
  if (this->m_properties_missing)
  {
    TangoSys_OMemStream oms;
	  oms << "One (or more) required properties is missing "
        << "[device initialization failed]" 
        << std::ends;
    ERROR_STREAM << oms.str() << std::endl;
    this->m_status_str = oms.str();
		this->m_init_device_done = false;
    this->set_state(Tango::FAULT);
    return;
  }

  try
  {
    //- instanciate the GearedAxes object
	   this->m_geared_axes = new galil::GearedAxes();

    if (this->m_geared_axes == 0)
      throw std::bad_alloc();
  }
  catch (std::bad_alloc&)
  {
  	ERROR_STREAM << "device initialization failed [OUT OF MEMORY]" << std::endl;
    this->m_init_device_done = false;
    this->m_status_str = "device initialization failed [OUT OF MEMORY]";
    this->set_state(Tango::FAULT);
    return;
  }
  catch (...)
  {
    ERROR_STREAM << "initialization failed [unknown exception caught]" << std::endl;
    this->m_status_str = "device initialization failed [unknown exception caught]";
    this->m_init_device_done = false;
    this->set_state(Tango::FAULT);
    return;
  }

  try
  {
    //- instanciate the Slit object
    galil::GearedAxes::Config ga_cfg;
    ga_cfg.master = galil::Axis::string_to_axis_id(masterAxis);
    ga_cfg.slave = galil::Axis::string_to_axis_id(slaveAxis);
    ga_cfg.user_ratio = this->ratio;
    ga_cfg.host_device = this;
    this->m_geared_axes->configure(ga_cfg);
  }
  catch (Tango::DevFailed& df)
  {
  	ERROR_STREAM << df << std::endl;
    this->m_init_device_done = false;
    this->m_status_str = "slit configuration failed";
    this->set_state(Tango::FAULT);
    return;
  }
  catch (...)
  {
    ERROR_STREAM << "Slit configuration failed [unknown exception caught]" << std::endl;
    this->m_status_str = "slit configuration failed [unknown exception caught]";
    this->m_init_device_done = false;
    this->set_state(Tango::FAULT);
    return;
  }

	this->m_init_device_done = true;
}

//+----------------------------------------------------------------------------
//
// method : 		GalilGearedAxes::get_device_property()
// 
// description : 	Read the device properties from database.
//
//-----------------------------------------------------------------------------
void GalilGearedAxes::get_device_property()
{
	//	Initialize your default values here (if not done with  POGO).
	//------------------------------------------------------------------
  this->ratio = 1.0;

	this->m_properties_missing = false;

	//	Read device properties from database.(Automatic code generation)
	//------------------------------------------------------------------
	Tango::DbData	dev_prop;
	dev_prop.push_back(Tango::DbDatum("MasterAxis"));
	dev_prop.push_back(Tango::DbDatum("SlaveAxis"));
	dev_prop.push_back(Tango::DbDatum("Ratio"));

	//	Call database and extract values
	//--------------------------------------------
	if (Tango::Util::instance()->_UseDb==true)
		get_db_device()->get_property(dev_prop);
	Tango::DbDatum	def_prop, cl_prop;
	GalilGearedAxesClass	*ds_class =
		(static_cast<GalilGearedAxesClass *>(get_device_class()));
	int	i = -1;

	//	Try to initialize MasterAxis from class property
	cl_prop = ds_class->get_class_property(dev_prop[++i].name);
	if (cl_prop.is_empty()==false)	cl_prop  >>  masterAxis;
	//	Try to initialize MasterAxis from default device value
	def_prop = ds_class->get_default_device_property(dev_prop[i].name);
	if (def_prop.is_empty()==false)	def_prop  >>  masterAxis;
	//	And try to extract MasterAxis value from database
	if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  masterAxis;

	//	Try to initialize SlaveAxis from class property
	cl_prop = ds_class->get_class_property(dev_prop[++i].name);
	if (cl_prop.is_empty()==false)	cl_prop  >>  slaveAxis;
	//	Try to initialize SlaveAxis from default device value
	def_prop = ds_class->get_default_device_property(dev_prop[i].name);
	if (def_prop.is_empty()==false)	def_prop  >>  slaveAxis;
	//	And try to extract SlaveAxis value from database
	if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  slaveAxis;

	//	Try to initialize Ratio from class property
	cl_prop = ds_class->get_class_property(dev_prop[++i].name);
	if (cl_prop.is_empty()==false)	cl_prop  >>  ratio;
	//	Try to initialize Ratio from default device value
	def_prop = ds_class->get_default_device_property(dev_prop[i].name);
	if (def_prop.is_empty()==false)	def_prop  >>  ratio;
	//	And try to extract Ratio value from database
	if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  ratio;



	//	End of Automatic code generation
	//------------------------------------------------------------------
  Tango::DbData data_put;
  if (dev_prop[0].is_empty() || masterAxis.find("must be defined") != std::string::npos)
	{
		m_properties_missing = true;
		Tango::DbDatum	property("MasterAxis");
		property << "must be defined - select the associated axis in [A..H]";
		data_put.push_back(property);
    ERROR_STREAM << "undefined critical property <MasterAxis>" << std::endl;
	}
  if (dev_prop[1].is_empty() || slaveAxis.find("must be defined") != std::string::npos)
	{
		m_properties_missing = true;
		Tango::DbDatum	property("SlaveAxis");
		property << "must be defined - select the associated axis in [A..H]";
		data_put.push_back(property);
    ERROR_STREAM << "undefined critical property <SlaveAxis>" << std::endl;
	}
  if (dev_prop[2].is_empty())
	{
		Tango::DbDatum	property("Ratio");
		property << ratio;
		data_put.push_back(property);
    INFO_STREAM << "undefined property <Ratio> created and set to default value " << ratio << std::endl;
	}
	if (! data_put.empty())
		get_db_device()->put_property(data_put);
}
//+----------------------------------------------------------------------------
//
// method : 		GalilGearedAxes::always_executed_hook()
// 
// description : 	method always executed before any command is executed
//
//-----------------------------------------------------------------------------
void GalilGearedAxes::always_executed_hook()
{
	
}
//+----------------------------------------------------------------------------
//
// method : 		GalilGearedAxes::read_attr_hardware
// 
// description : 	Hardware acquisition for attributes.
//
//-----------------------------------------------------------------------------
void GalilGearedAxes::read_attr_hardware(vector<long> &attr_list)
{
	DEBUG_STREAM << "GalilGearedAxes::read_attr_hardware(vector<long> &attr_list) entering... "<< endl;
	//	Add your own code here
}
//+----------------------------------------------------------------------------
//
// method : 		GalilGearedAxes::read_isGeared
// 
// description : 	Extract real attribute values for isGeared acquisition result.
//
//-----------------------------------------------------------------------------
void GalilGearedAxes::read_isGeared(Tango::Attribute &attr)
{
  DEBUG_STREAM << "GalilGearedAxes::read_isGeared(Tango::Attribute &attr) entering... "<< endl;
  *attr_isGeared_read = this->m_geared_axes->is_gearing_enabled();
  attr.set_value(attr_isGeared_read);

}

//+----------------------------------------------------------------------------
//
// method : 		GalilGearedAxes::read_masterPosition
// 
// description : 	Extract real attribute values for masterPosition acquisition result.
//
//-----------------------------------------------------------------------------
void GalilGearedAxes::read_masterPosition(Tango::Attribute &attr)
{
	DEBUG_STREAM << "GalilGearedAxes::read_masterPosition(Tango::Attribute &attr) entering... "<< endl;
 
  *attr_masterPosition_read = this->m_geared_axes->master_position();

  attr.set_value(attr_masterPosition_read);
}

//+----------------------------------------------------------------------------
//
// method : 		GalilGearedAxes::write_masterPosition
// 
// description : 	Write masterPosition attribute values to hardware.
//
//-----------------------------------------------------------------------------
void GalilGearedAxes::write_masterPosition(Tango::WAttribute &attr)
{
	DEBUG_STREAM << "GalilGearedAxes::write_masterPosition(Tango::WAttribute &attr) entering... "<< endl;

  attr.get_write_value(attr_masterPosition_write);

  this->m_geared_axes->master_position(attr_masterPosition_write);
}

//+----------------------------------------------------------------------------
//
// method : 		GalilGearedAxes::read_slavePosition
// 
// description : 	Extract real attribute values for slavePosition acquisition result.
//
//-----------------------------------------------------------------------------
void GalilGearedAxes::read_slavePosition(Tango::Attribute &attr)
{
	DEBUG_STREAM << "GalilGearedAxes::read_slavePosition(Tango::Attribute &attr) entering... "<< endl;
 
  *attr_slavePosition_read = this->m_geared_axes->slave_position();

  attr.set_value(attr_slavePosition_read);
}

//+----------------------------------------------------------------------------
//
// method : 		GalilGearedAxes::write_slavePosition
// 
// description : 	Write slavePosition attribute values to hardware.
//
//-----------------------------------------------------------------------------
void GalilGearedAxes::write_slavePosition(Tango::WAttribute &attr)
{
	DEBUG_STREAM << "GalilGearedAxes::write_slavePosition(Tango::WAttribute &attr) entering... "<< endl;

  attr.get_write_value(attr_slavePosition_write);

  this->m_geared_axes->slave_position(attr_slavePosition_write);
}

//+------------------------------------------------------------------
/**
 *	method:	GalilGearedAxes::stop
 *
 *	description:	method to execute "Stop"
 *	stops any movement on the slit
 *
 *
 */
//+------------------------------------------------------------------
void GalilGearedAxes::stop()
{
	DEBUG_STREAM << "GalilGearedAxes::stop(): entering... !" << endl;

	//	Add your own code to control device here

  this->m_geared_axes->stop();
}

//+------------------------------------------------------------------
/**
 *	method:	GalilGearedAxes::set_independant_mode
 *
 *	description:	method to execute "SetIndependantMode"
 *	TURNS OFF THE GEARING MODE
 *
 *
 */
//+------------------------------------------------------------------
void GalilGearedAxes::set_independant_mode()
{
	DEBUG_STREAM << "GalilGearedAxes::set_independant_mode(): entering... !" << endl;

	//	Add your own code to control device here

  this->m_geared_axes->disable_gearing();

}

//+------------------------------------------------------------------
/**
 *	method:	GalilGearedAxes::set_geared_mode
 *
 *	description:	method to execute "SetGearedMode"
 *	turns ON the GEARING MODE
 *
 *
 */
//+------------------------------------------------------------------
void GalilGearedAxes::set_geared_mode()
{
	DEBUG_STREAM << "GalilGearedAxes::set_geared_mode(): entering... !" << endl;

	//	Add your own code to control device here
  
  this->m_geared_axes->enable_gearing();

}
//+------------------------------------------------------------------
/**
* method: GalilGearedAxes::dev_state
*
* description:  method to execute "State"
* Command to read the device state.
* The method dev_state() override a DeviceImpl class virtual method.
*
* @return State Code
*
*/
//+------------------------------------------------------------------
Tango::DevState GalilGearedAxes::dev_state()
{
  DEBUG_STREAM << "GalilGearedAxes::dev_state(): entering... !" << endl;

  //  Add your own code to control device here
  Tango::DevState argout = DeviceImpl::dev_state();

	//- is device properly initialized?
	if (! m_init_device_done ||  m_properties_missing)
  {
		this->set_state(Tango::FAULT);
    return Tango::FAULT;
	}

  galil::AxisState slit_state = this->m_geared_axes->state();

  switch (slit_state)
  {
    case galil::AXIS_OFF:
	    this->set_state(Tango::OFF);
      return Tango::OFF;
      break;
    case galil::AXIS_MOVING:
	    this->set_state(Tango::MOVING);
      return Tango::MOVING;
      break;
    case galil::AXIS_STANDBY:
	    this->set_state(Tango::STANDBY);
      return Tango::STANDBY;
      break;
    case galil::AXIS_ALARM:
	    this->set_state(Tango::ALARM);
      return Tango::ALARM;
      break;
    case galil::AXIS_ERROR:
	    this->set_state(Tango::FAULT);
      return Tango::FAULT;
      break;
    case galil::AXIS_CRTL_DISABLED:
	    this->set_state(Tango::DISABLE);
      return Tango::DISABLE;
      break;
    default:
      break;
  }

  this->set_state(Tango::UNKNOWN);

  return Tango::UNKNOWN;
}

//+------------------------------------------------------------------
/**
* method: GalilGearedAxes::dev_status
*
* description:  method to execute "Status"
* Command to read the device status.
* The method dev_status() override a DeviceImpl class virtual method.
*
* @return Status descrition
*
*/
//+------------------------------------------------------------------
Tango::ConstDevString GalilGearedAxes::dev_status()
{
  DEBUG_STREAM << "GalilGearedAxes::dev_status(): entering... !" << endl;

  //  Add your own code to control device here
	if ( this->m_properties_missing ) 
	{
		return this->m_status_str.c_str();
	}

	if (! this->m_init_device_done )
	{
    return m_status_str.c_str();
	}
  // get current slit status
  galil::GearedAxesStatus css;
  this->m_geared_axes->status(css);

  this->m_status_str = "Master Axis status : \n" +
                       css.master_status.status  +
                       "\nSlave Axis Status :\n" + 
                       css.slave_status.status   + 
					   "\nCombined Geared Axes Status :\n"+
											 css.slit_status ;

  return m_status_str.c_str();
}


}	//	namespace