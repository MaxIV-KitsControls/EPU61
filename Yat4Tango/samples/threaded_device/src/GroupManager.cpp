static const char *RcsId = "$Header: /usr/local/CVS/Libraries/YAT4tango/samples/threaded_device/GroupManager.cpp,v 1.1 2007/07/18 13:09:43 leclercq Exp $";
//+=============================================================================
//
// file :         GroupManager.cpp
//
// description :  C++ source for the GroupManager and its commands. 
//                The class is derived from Device. It represents the
//                CORBA servant object which will be accessed from the
//                network. All commands which can be executed on the
//                GroupManager are implemented in this file.
//
// project :      TANGO Device Server
//
// $Author: leclercq $
//
// $Revision: 1.1 $
//
// $Log: GroupManager.cpp,v $
// Revision 1.1  2007/07/18 13:09:43  leclercq
// no message
//
// Revision 1.3  2007/07/18 13:02:50  leclercq
// Added a threaded device example
//
// Revision 1.1  2007/07/18 12:57:05  leclercq
// Added a threaded device example
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
//	The following table gives the correspondence
//	between commands and method name.
//
//  Command name|  Method name
//	----------------------------------------
//  State   |  dev_state()
//  Status  |  dev_status()
//  Start   |  start()
//  Stop    |  stop()
//
//===================================================================

#include <tango.h>
#include <GroupManager.h>
#include <GroupManagerClass.h>

// ============================================================================
// DEVICE TASK ACTIVITY PERIOD IN MILLISECS
// ============================================================================
//- the following timeout set the frequency at which the task generates its data
#define kTASK_PERIOD_MS 1000

namespace GroupManager_ns
{

//+----------------------------------------------------------------------------
//
// method : 		GroupManager::GroupManager(string &s)
// 
// description : 	constructor for simulated GroupManager
//
// in : - cl : Pointer to the DeviceClass object
//      - s : Device name 
//
//-----------------------------------------------------------------------------
GroupManager::GroupManager(Tango::DeviceClass *cl,string &s)
:Tango::Device_4Impl(cl,s.c_str())
{
	init_device();
}

GroupManager::GroupManager(Tango::DeviceClass *cl,const char *s)
:Tango::Device_4Impl(cl,s)
{
	init_device();
}

GroupManager::GroupManager(Tango::DeviceClass *cl,const char *s,const char *d)
 : Tango::Device_4Impl(cl,s,d)
{
	init_device();
}
//+----------------------------------------------------------------------------
//
// method : 		GroupManager::delete_device()
// 
// description : 	will be called at device destruction or at init command.
//
//-----------------------------------------------------------------------------
void GroupManager::delete_device()
{
  yat4tango::TraceHelper t("GroupManager::delete_device", this);
	
	//- release the task
	if (this->m_task)
	{
		//- ask the task to quit
	  this->m_task->exit();
    //- !!!!! never try to <delete> a yat4tango::DeviceTask, cause 
    //- it commits suicide upon return of its main function !!!!!!
	  this->m_task = 0;
	}

  //- remove the inner appender
  yat4tango::InnerAppender::release(this);
}

//+----------------------------------------------------------------------------
//
// method : 		GroupManager::init_device()
//
// description : 	will be called at device initialization.
//
//-----------------------------------------------------------------------------
void GroupManager::init_device()
{
  //- initialize the inner appender (first thing to do)
  try
  {
    yat4tango::InnerAppender::initialize(this, 512);
  }
  catch( Tango::DevFailed& df )
  {
    ERROR_STREAM << df << std::endl;
    this->set_state(Tango::FAULT);
    this->set_status( "initialization failed - could not instanciate the InnerAppender");
    return;
  }

  //- trace/profile this method
  yat4tango::TraceHelper t("GroupManager::init_device", this);

	INFO_STREAM << "initializing device " << device_name << endl;

	// Initialise variables to default values
	//--------------------------------------------
  try
  {
	  this->get_device_property();
  }
  catch (Tango::DevFailed & df)
  {
  	ERROR_STREAM << df << std::endl;
    this->set_status ("initialization failed - <get_device_property> failed [see device log for details]");
    this->set_state (Tango::FAULT);
    return;
  }
  catch (...)
  {
    ERROR_STREAM << "initialization failed - unknown <get_device_property> error" << std::endl;
    this->set_status ("initialization failed [unknown <get_device_property> error]");
    this->set_state (Tango::FAULT);
    return;
  }

  //- clear/reset all members (see also <delete_device>)
  this->m_task = 0;
  this->a_double = 0;

	try
	{
		//- task configuration
    GroupManagerTask::Config cfg;
    cfg.managed_devices = this->managedDevices;
    cfg.excluded_devices = this->excludedDevices;
    cfg.monitored_attributes = this->monitoredAttributes;
    cfg.task_activity_period_ms = kTASK_PERIOD_MS;
    cfg.host_device = this;

		//- allocate the task
		DEBUG_STREAM << "GroupManager::init_device::allocating task" << std::endl;
    this->m_task = new (std::nothrow) GroupManagerTask(cfg);
	  if (! this->m_task)
      throw std::bad_alloc();

    //- start the task
    DEBUG_STREAM << "GroupManager::init_device::starting task" << std::endl;
    this->m_task->go();
  }
  catch (const std::bad_alloc&)
  {
  	ERROR_STREAM << "initialization failed - GroupManagerTask allocation failed" << std::endl;
    this->set_status ("initialization failed - GroupManagerTask allocation failed");
    this->set_state (Tango::FAULT);
    return; 
  }
  catch (Tango::DevFailed & df)
  {
  	ERROR_STREAM << df << std::endl;
    this->set_status ("initialization failed - GroupManagerTask allocation failed [see device log for details]");
    this->set_state (Tango::FAULT);
    return;
  }
  catch (...)
  {
    ERROR_STREAM << "initialization failed - unknown error" << std::endl;
    this->set_status ("initialization failed [unknown error]");
    this->set_state (Tango::FAULT);
    return;
  }

  //- update sate and status
  this->set_state (Tango::RUNNING);
  this->set_status ("device is up and running");

  DEBUG_STREAM << "device successfully initialized" << std::endl;
}

//+----------------------------------------------------------------------------
//
// method : 		GroupManager::get_device_property()
// 
// description : 	Read the device properties from database.
//
//-----------------------------------------------------------------------------
void GroupManager::get_device_property()
{
	//	Initialize your default values here (if not done with  POGO).
	//------------------------------------------------------------------

	//	Read device properties from database.(Automatic code generation)
	//------------------------------------------------------------------
	Tango::DbData	dev_prop;
	dev_prop.push_back(Tango::DbDatum("ManagedDevices"));
	dev_prop.push_back(Tango::DbDatum("MonitoredAttributes"));
	dev_prop.push_back(Tango::DbDatum("ExcludedDevices"));

	//	Call database and extract values
	//--------------------------------------------
	if (Tango::Util::instance()->_UseDb==true)
		get_db_device()->get_property(dev_prop);
	Tango::DbDatum	def_prop, cl_prop;
	GroupManagerClass	*ds_class =
		(static_cast<GroupManagerClass *>(get_device_class()));
	int	i = -1;

	//	Try to initialize ManagedDevices from class property
	cl_prop = ds_class->get_class_property(dev_prop[++i].name);
	if (cl_prop.is_empty()==false)	cl_prop  >>  managedDevices;
	else {
		//	Try to initialize ManagedDevices from default device value
		def_prop = ds_class->get_default_device_property(dev_prop[i].name);
		if (def_prop.is_empty()==false)	def_prop  >>  managedDevices;
	}
	//	And try to extract ManagedDevices value from database
	if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  managedDevices;

	//	Try to initialize MonitoredAttributes from class property
	cl_prop = ds_class->get_class_property(dev_prop[++i].name);
	if (cl_prop.is_empty()==false)	cl_prop  >>  monitoredAttributes;
	else {
		//	Try to initialize MonitoredAttributes from default device value
		def_prop = ds_class->get_default_device_property(dev_prop[i].name);
		if (def_prop.is_empty()==false)	def_prop  >>  monitoredAttributes;
	}
	//	And try to extract MonitoredAttributes value from database
	if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  monitoredAttributes;

	//	Try to initialize ExcludedDevices from class property
	cl_prop = ds_class->get_class_property(dev_prop[++i].name);
	if (cl_prop.is_empty()==false)	cl_prop  >>  excludedDevices;
	else {
		//	Try to initialize ExcludedDevices from default device value
		def_prop = ds_class->get_default_device_property(dev_prop[i].name);
		if (def_prop.is_empty()==false)	def_prop  >>  excludedDevices;
	}
	//	And try to extract ExcludedDevices value from database
	if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  excludedDevices;

	//	End of Automatic code generation
	//------------------------------------------------------------------

}
//+----------------------------------------------------------------------------
//
// method : 		GroupManager::always_executed_hook()
// 
// description : 	method always executed before any command is executed
//
//-----------------------------------------------------------------------------
void GroupManager::always_executed_hook()
{
  //- yat4tango::TraceHelper t("GroupManager::always_executed_hook", this);
}
//+----------------------------------------------------------------------------
//
// method : 		GroupManager::read_attr_hardware
// 
// description : 	Hardware acquisition for attributes.
//
//-----------------------------------------------------------------------------
void GroupManager::read_attr_hardware(vector<long> &attr_list)
{
  //- noop
}
//+----------------------------------------------------------------------------
//
// method : 		GroupManager::read_postThisDoubleToTheTask
// 
// description : 	Extract real attribute values for postThisDoubleToTheTask acquisition result.
//
//-----------------------------------------------------------------------------
void GroupManager::read_postThisDoubleToTheTask(Tango::Attribute &attr)
{
  attr.set_value(&this->a_double);
}


//+----------------------------------------------------------------------------
//
// method : 		GroupManager::write_postThisDoubleToTheTask
// 
// description : 	Write postThisDoubleToTheTask attribute values to hardware.
//
//-----------------------------------------------------------------------------
void GroupManager::write_postThisDoubleToTheTask(Tango::WAttribute &attr)
{
  yat4tango::TraceHelper t("GroupManager::write_PostThisValueToTheTask", this);

  attr.get_write_value(this->a_double);

  if (this->m_task)
    this->m_task->post_some_data_to_the_task(this->a_double);
}

//+------------------------------------------------------------------
/**
 *	method:	GroupManager::start
 *
 *	description:	method to execute "Start"
 *	Start periodic activity
 *
 *
 */
//+------------------------------------------------------------------
void GroupManager::start()
{
  yat4tango::TraceHelper t("GroupManager::start", this);

  if (this->m_task)
    this->m_task->start_periodic_activity();
}

//+------------------------------------------------------------------
/**
 *	method:	GroupManager::stop
 *
 *	description:	method to execute "Stop"
 *	Stop periodic activity
 *
 *
 */
//+------------------------------------------------------------------
void GroupManager::stop()
{
  yat4tango::TraceHelper t("GroupManager::stop", this);

  if (this->m_task)
    this->m_task->stop_periodic_activity();
}



}	//	namespace