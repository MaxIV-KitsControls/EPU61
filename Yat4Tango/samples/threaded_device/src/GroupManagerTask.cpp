// ============================================================================
//
// = CONTEXT
//		TANGO Project - DeviceTask example
//
// = File
//		GroupManagerTask.cpp
//
// = AUTHOR
//		N.Leclercq - SOLEIL & JC.Pret - APSIDE
//
// ============================================================================

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <math.h>
#include <yat/threading/Mutex.h>
#include "GroupManagerTask.h"

// ============================================================================
// SOME USER DEFINED MESSAGES
// ============================================================================
#define kSTART_PERIODIC_MSG (yat::FIRST_USER_MSG + 1000)
#define kSTOP_PERIODIC_MSG  (yat::FIRST_USER_MSG + 1001)
#define kDATA_MSG           (yat::FIRST_USER_MSG + 1002)

namespace GroupManager_ns
{

// ============================================================================
// GroupManagerTask::GroupManagerTask
// ============================================================================
GroupManagerTask::GroupManagerTask (const Config & cfg)
  : yat4tango::DeviceTask(cfg.host_device), 
    m_cfg(cfg), 
    m_dam(cfg.host_device),
    m_grp("managed_group")
{
  //- configure optional msg handling
  this->enable_timeout_msg(false);
  this->enable_periodic_msg(false);
  this->set_periodic_msg_period(cfg.task_activity_period_ms);
}

// ============================================================================
// GroupManagerTask::~GroupManagerTask
// ============================================================================
GroupManagerTask::~GroupManagerTask ()
{
  //- noop
}

// ============================================================================
// GroupManagerTask::process_message
// ============================================================================
void GroupManagerTask::process_message (yat::Message& msg)
  throw (Tango::DevFailed)
{
  //- handle msg
  switch (msg.type())
  {
    //- THREAD_INIT ----------------------
    case yat::TASK_INIT:
      {
	      DEBUG_STREAM << "GroupManagerTask::handle_message::THREAD_INIT::thread is starting up" << std::endl;
      } 
	    break;
      
	  //- THREAD_EXIT ----------------------
	  case yat::TASK_EXIT:
	    {
			  DEBUG_STREAM << "GroupManagerTask::handle_message::THREAD_EXIT::thread is quitting" << std::endl;
      }
		  break;
      
	  //- THREAD_PERIODIC ------------------
	  case yat::TASK_PERIODIC:
	    {
		    this->periodic_job_i();
      }
	    break;
      
	  //- THREAD_TIMEOUT -------------------
	  case yat::TASK_TIMEOUT:
	    {
        //- not used in this example
      }
      break;

    //- kSTART_PERIODIC_MSG --------------
    case kSTART_PERIODIC_MSG:
      {
    	  DEBUG_STREAM << "GroupManagerTask::handle_message::kSTART_PERIODIC_MSG" << std::endl;
        this->enable_periodic_msg(true);
      }
      break;

    //- kSTOP_PERIODIC_MSG ---------------
    case kSTOP_PERIODIC_MSG:
      {
    	  DEBUG_STREAM << "GroupManagerTask::handle_message::kSTOP_PERIODIC_MSG" << std::endl;
        this->enable_periodic_msg(false);
      }
      break;

    //- kDATA_MSG ------------------------
    case kDATA_MSG:
      {
        //- expecting a double (might throw an exception in case the msg encapsulates the wrong data type)
        try
        {
          double & d = msg.get_data<double>();
    	    DEBUG_STREAM << "GroupManagerTask::handle_message::kDATA_MSG::received value: " <<  d << std::endl;
        }
        catch (yat::Exception& ye)
        {
          _YAT_TO_TANGO_EXCEPTION(ye, te);
          ERROR_STREAM << te << std::endl;
        }
      }
      break;

    //- UNHANDLED MSG --------------------
		default:
		  DEBUG_STREAM << "GroupManagerTask::handle_message::unhandled msg type received" << std::endl;
			break;
  }
}

// ============================================================================
// GroupManagerTask::start_periodic_activity
// ============================================================================
void GroupManagerTask::start_periodic_activity (size_t tmo_ms)
    throw (Tango::DevFailed)
{
  try
  {
    //- synchronous approach: "post then wait for the message to be handled"
    if (tmo_ms)
      this->wait_msg_handled(kSTART_PERIODIC_MSG, tmo_ms);
    //- asynchronous approach: "post the data then returm immediately"
    else
      this->post(kSTART_PERIODIC_MSG);
  }
  catch (const Tango::DevFailed&)
  {
    //- an exception could be thrown if the task msgQ is full (high water mark reached)
    //- in the synchronous case we could also caught an exception thrown by the code 
    //- handling the message 
    throw;
  }
}

// ============================================================================
// GroupManagerTask::stop_periodic_activity
// ============================================================================
void GroupManagerTask::stop_periodic_activity (size_t tmo_ms)
    throw (Tango::DevFailed)
{
  try
  {
    //- synchronous approach: "post then wait for the message to be handled"
    if (tmo_ms)
      this->wait_msg_handled(kSTOP_PERIODIC_MSG, tmo_ms);
    //- asynchronous approach: "post the data then returm immediately"
    else
      this->post(kSTOP_PERIODIC_MSG);
  }
  catch (const Tango::DevFailed&)
  {
    //- an exception could be thrown if the task msgQ is full (high water mark reached)
    //- in the synchronous case we could also caught an exception thrown by the code 
    //- handling the message 
    throw;
  }
}

// ============================================================================
// GroupManagerTask::post_some_data_to_the_task
// ============================================================================
void GroupManagerTask::post_some_data_to_the_task (double the_data, size_t tmo_ms)
  throw (Tango::DevFailed)
{
  try
  {
    //- synchronous approach: "post the data then wait for the message to be handled"
    if (tmo_ms)
      this->wait_msg_handled(kDATA_MSG, the_data, tmo_ms);
    //- asynchronous approach: "post the data then returm immediately"
    else
      this->post(kDATA_MSG, the_data);
  }
  catch (const Tango::DevFailed&)
  {
    //- an exception could be thrown if the task msgQ is full (high water mark reached)
    //- in the synchronous case we could also caught an exception thrown by the code 
    //- handling the message 
    throw;
  }
} 

// ============================================================================
// GroupManagerTask::periodic_job_i
// ============================================================================
void GroupManagerTask::periodic_job_i ()
  throw (Tango::DevFailed)
{
  //- TODO
} 

// ============================================================================
// GroupManagerTask::go
// ============================================================================
void GroupManagerTask::go (size_t tmo_ms)
    throw (Tango::DevFailed)
{
  //- a timer
  yat::Timer t;

  //- be sure we have something to do...
  if (! this->m_cfg.managed_devices.size())
  {
    THROW_DEVFAILED(_CPTC("DEVICE_ERROR"),
                    _CPTC("the managed devices list is empty - check associated property"),
                    _CPTC("GroupManagerTask::go"));
  }

  //- instanciate the dynamic attributes
  try
  {
    //- populate the group of managed devices
    this->m_grp.add(this->m_cfg.managed_devices);

    //- remove excluded devices
    if (this->m_cfg.excluded_devices.size())
     this->m_grp.remove(this->m_cfg.excluded_devices);

    DEBUG_STREAM << "GroupManagerTask::go::group contains " 
                 << this->m_grp.get_size()
                 << " devices"
                 << std::endl;

    //- in order to be able to instanciate the proper dynamic attribute (in terms of data type), 
    //- we need to obtain the Tango::AttributeInfo of each monitored attribute. so...
    Tango::AttributeInfoList ail;
    this->get_attributes_info_i(ail);

    //- can now instanciate the dynamic attributes...
    std::vector<yat4tango::DynamicAttributeInfo> dai(ail.size());
    for (size_t i = 0; i < ail.size(); i++)
    {
      //- the Tango device to which tha dynamic attribute will be attached to
      dai[i].dev = this->m_cfg.host_device;

      //- copy the Tango::AttributeInfo info
      dai[i].tai = ail[i];

      //- let's decide (for example purpose) that we want all dynamic attributes to be associated 
      //- to the same "generic" read and write callblack, except for the images, which will be
      //- connected to a "specific" callback...
      if (dai[i].tai.data_format != Tango::IMAGE)
      {
        dai[i].rcb = yat4tango::DynamicAttributeReadCallback::instanciate(*this, 
                                                                          &GroupManagerTask::generic_rd_cb);
        //- instanciate the <write callback> (can be useless but not a problem)
        dai[i].wcb = yat4tango::DynamicAttributeWriteCallback::instanciate(*this, 
                                                                           &GroupManagerTask::generic_wt_cb);
      }
      else
      {
        dai[i].rcb = yat4tango::DynamicAttributeReadCallback::instanciate(*this, 
                                                                          &GroupManagerTask::specific_rd_cb);
        //- instanciate the <write callback> (can be useless but not a problem)
        dai[i].wcb = yat4tango::DynamicAttributeWriteCallback::instanciate(*this, 
                                                                           &GroupManagerTask::specific_wt_cb);
      }
    }
    //- add all dynamic attributes to the device interface
    this->m_dam.add_attributes(dai);

    //- add a forwarded attribute
    this->m_dam.add_attribute("tgvbox:10000/sys/tg_test/1/boolean_scalar", "test_lan");
    this->m_dam.add_attribute("sys/tg_test/1/uchar_scalar", "my_uchar_scalar"); 
    this->m_dam.add_attribute("sys/tg_test/1/short_scalar");
    this->m_dam.add_attribute("sys/tg_test/1/ushort_scalar");
    this->m_dam.add_attribute("sys/tg_test/1/long_scalar");
    this->m_dam.add_attribute("sys/tg_test/1/ulong_scalar");
    this->m_dam.add_attribute("sys/tg_test/1/long64_scalar");
    this->m_dam.add_attribute("sys/tg_test/1/ulong64_scalar");
    this->m_dam.add_attribute("sys/tg_test/1/float_scalar");
    this->m_dam.add_attribute("sys/tg_test/1/double_scalar");
    this->m_dam.add_attribute("sys/tg_test/1/string_scalar");
    
    this->m_dam.add_attribute("sys/tg_test/1/boolean_spectrum_ro");
    this->m_dam.add_attribute("sys/tg_test/1/uchar_spectrum_ro");
    this->m_dam.add_attribute("sys/tg_test/1/short_spectrum_ro");
    this->m_dam.add_attribute("sys/tg_test/1/ushort_spectrum_ro");
    this->m_dam.add_attribute("sys/tg_test/1/long_spectrum_ro");
    this->m_dam.add_attribute("sys/tg_test/1/ulong_spectrum_ro");
    this->m_dam.add_attribute("sys/tg_test/1/long64_spectrum_ro");
    this->m_dam.add_attribute("sys/tg_test/1/ulong64_spectrum_ro");
    this->m_dam.add_attribute("sys/tg_test/1/float_spectrum_ro");
    this->m_dam.add_attribute("sys/tg_test/1/double_spectrum_ro");
    this->m_dam.add_attribute("sys/tg_test/1/string_spectrum_ro");
    
    this->m_dam.add_attribute("sys/tg_test/1/boolean_image_ro");
    this->m_dam.add_attribute("sys/tg_test/1/uchar_image_ro");
    this->m_dam.add_attribute("sys/tg_test/1/short_image_ro");
    this->m_dam.add_attribute("sys/tg_test/1/ushort_image_ro");
    this->m_dam.add_attribute("sys/tg_test/1/long_image_ro");
    this->m_dam.add_attribute("sys/tg_test/1/ulong_image_ro");
    this->m_dam.add_attribute("sys/tg_test/1/long64_image_ro");
    this->m_dam.add_attribute("sys/tg_test/1/ulong64_image_ro");
    this->m_dam.add_attribute("sys/tg_test/1/float_image_ro");
    this->m_dam.add_attribute("sys/tg_test/1/double_image_ro");
    this->m_dam.add_attribute("sys/tg_test/1/string_image_ro");
    
    
    this->m_dam.add_attribute("sys/tg_test/1/uchar_spectrum");
    this->m_dam.add_attribute("sys/tg_test/1/short_spectrum");
    this->m_dam.add_attribute("sys/tg_test/1/ushort_spectrum");
    this->m_dam.add_attribute("sys/tg_test/1/long_spectrum");
    this->m_dam.add_attribute("sys/tg_test/1/float_spectrum");
    this->m_dam.add_attribute("sys/tg_test/1/double_spectrum");
    this->m_dam.add_attribute("sys/tg_test/1/string_spectrum");
    
    this->m_dam.add_attribute("sys/tg_test/1/uchar_image");
    this->m_dam.add_attribute("sys/tg_test/1/short_image");
    this->m_dam.add_attribute("sys/tg_test/1/ushort_image");
    this->m_dam.add_attribute("sys/tg_test/1/long_image");
    this->m_dam.add_attribute("sys/tg_test/1/float_image");
    this->m_dam.add_attribute("sys/tg_test/1/double_image");
    this->m_dam.add_attribute("sys/tg_test/1/string_image");
    
    //- add "State" to the monitored attributes list (minimum required)
    this->add_state_to_monitored_attributes_list_i(); 
  }
  catch (Tango::DevFailed & df)
  {
    RETHROW_DEVFAILED(df,
                      _CPTC("DEVICE_ERROR"),
                      _CPTC("dynamic attributes initialization failed [Tango exception caught]"),
                      _CPTC("GroupManager::go"));
  }
  catch (...)
  {
    THROW_DEVFAILED(_CPTC("DEVICE_ERROR"),
                    _CPTC("dynamic attributes initialization failed [unknown exception caught]"),
                    _CPTC("GroupManager::go"));
  }

  //- finally call yat4tango::DeviceTask::go
  long remaining_tmo = static_cast<long>(tmo_ms) - static_cast<long>(t.elapsed_msec());
  this->yat4tango::DeviceTask::go(remaining_tmo > 1 ? remaining_tmo : 1);
}

// ============================================================================
// GroupManagerTask::exit
// ============================================================================
void GroupManagerTask::exit ()
    throw (Tango::DevFailed)
{
  //- stop the periodic activity
  try
  {
    this->stop_periodic_activity(3000);
  }
  catch (...)
  {
    //- ignore errors cause we need to exit!
  }

  //- removes all dynamic attributes from de device interface
  try
  {
    this->m_dam.remove_attributes();
  }
  catch (...)
  {
    //- ignore errors cause we need to exit!
  }

  //- finally call call yat4tango::DeviceTask::exit
  this->yat4tango::DeviceTask::exit();
}

// ============================================================================
// GroupManagerTask::generic_rd_cb (yat4tango::DynamicAttributeReadCallback impl)
// ============================================================================
void GroupManagerTask::generic_rd_cb (yat4tango::DynamicAttributeReadCallbackData& d)
  throw (Tango::DevFailed)
{
  if (d.dya)
  {
    d.dya->dump();
  }
}

// ============================================================================
// GroupManagerTask::specific_rd_cb (yat4tango::DynamicAttributeReadCallback impl)
// ============================================================================
void GroupManagerTask::specific_rd_cb (yat4tango::DynamicAttributeReadCallbackData& d)
  throw (Tango::DevFailed)
{
  if (d.dya)
  {
    d.dya->dump();
  }
}

// ============================================================================
// GroupManagerTask::generic_wt_cb (yat4tango::DynamicAttributeWriteCallback impl)
// ============================================================================
void GroupManagerTask::generic_wt_cb (yat4tango::DynamicAttributeWriteCallbackData& d)
  throw (Tango::DevFailed)
{
  if (d.dya)
  {
    d.dya->dump();
  }
}

// ============================================================================
// GroupManagerTask::specific_wt_cb (yat4tango::DynamicAttributeWriteCallback impl)
// ============================================================================
void GroupManagerTask::specific_wt_cb (yat4tango::DynamicAttributeWriteCallbackData& d)
  throw (Tango::DevFailed)
{
  if (d.dya)
  {
    d.dya->dump();
  }
}

// ============================================================================
// GroupManagerTask::get_attributes_info_i
// ============================================================================
void GroupManagerTask::get_attributes_info_i (Tango::AttributeInfoList& attr_info)
    throw (Tango::DevFailed)
{

  //- this method tries to obtain the Tango::AttributeInfo of all monitored attributes. 
  //- in our case, the Tango::AttributeInfo is required to instanciate a dynamic attribute. 
  //- this is purely application specific cause, in this example, the dynamic attribute caracteristics
  //- (format, data type, ...) are obtained from a "remote" attribute
  //-
  //- an exception is thrown in case:
  //-   -> none of the managed devices currently running (can't obtain Tango::AttributeInfo in this case)
  //-   -> (at least) one of managed devices doesn't support (at least) one of monitored attributes
  //-   -> attempt to obtain the Tango::AttributeInfoList failed on all managed devices

  //- clear the returned vector
  attr_info.clear();

  //- get group size (i.e. num of devices in the group)
  size_t g_size = this->m_grp.get_size();

  //- get any running device from the group 
  //- then obtain its AttributeInfoList
  Tango::DeviceProxy * dp = 0;
  Tango::AttributeInfoList * ail = 0;

  //- warning: Tango::Group index is in [1, num of devices in the group]  
  for (size_t i = 1; i <= g_size; i++)
  {
    dp = this->m_grp.get_device(i);
    if (dp)
    {
      DEBUG_STREAM << "GroupManagerTask::get_attributes_info::trying " 
                   << dp->dev_name()
                   << std::endl;
      //- be sure the device is running 
      try {
        dp->ping();
        ail = dp->attribute_list_query();
        break;
      }
      catch (...) {
        delete ail;
      }
    }
  }
  if (! dp || ! ail)
  {
  	ERROR_STREAM << "failed to obtain attributes description from one of the <managed device>" << std::endl;
    ERROR_STREAM << "be sure that (at least) one of the managed devices is running" << std::endl;
    ERROR_STREAM << "otherwise, check porperties providing the managed devices excluded devices list" << std::endl;
    THROW_DEVFAILED(_CPTC("DEVICE_ERROR"),
                    _CPTC("failed to obtain attributes description from one of the <managed device>"),
                    _CPTC("GroupManagerTask::get_attributes_info_i"));
  }

  //- use <dp> as our "reference device" - must support all monitored attributes
  for (size_t i = 0; i < this->m_cfg.monitored_attributes.size(); i++)
  {
    std::string & attr_name = this->m_cfg.monitored_attributes[i];
    //- search <attr_name> in <ail>
    bool attr_found = false;
    size_t j = 0;
    for (; j < ail->size(); j++)
    {
      if (attr_name == (*ail)[j].name)
      {
        attr_found = true;
        break;
      }
    }
    if (! attr_found)
    {
      delete ail;
      attr_info.clear();
  	  ERROR_STREAM << "(at least) one of the managed devices does support the monitored attribute <" << attr_name << ">" << std::endl;
      ERROR_STREAM << "be sure that ALL managed devices support ALL monitored attributes" << std::endl;
      ERROR_STREAM << "Tango attribute names are case sensistive - check the associated property" << std::endl;
      THROW_DEVFAILED(_CPTC("DEVICE_ERROR"),
                      _CPTC("(at least) one of the managed devices does support one of the monitored attributes"),
                      _CPTC("GroupManagerTask::get_attributes_info"));
    }
    //- ok, ail[j] is monitoredAttributes[i]
    DEBUG_STREAM << "GroupManagerTask::get_attributes_info::successfully obtained attribute info for " 
                 << attr_name
                 << std::endl;
    attr_info.push_back((*ail)[j]);
  }
  //- avoid memory leaks...
  delete ail;
}

// ============================================================================
// GroupManagerTask::add_state_to_monitored_attributes_list_i
// ============================================================================
void GroupManagerTask::add_state_to_monitored_attributes_list_i ()
  throw (Tango::DevFailed)
{
  const std::string & state_attr_name = "State";

  bool state_attr_found = false;

  for (size_t i = 0; i < this->m_cfg.monitored_attributes.size(); i++)
  {
    const std::string & attr_name = this->m_cfg.monitored_attributes[i];
    if (attr_name == state_attr_name)
    {
      state_attr_found = true;
      break;
    }
  }

  if (! state_attr_found)
    this->m_cfg.monitored_attributes.push_back("State");
}

} // namespace
