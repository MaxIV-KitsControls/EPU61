// ============================================================================
//
// = CONTEXT
//		DeviceTask example
//
// = File
//		GroupManagerTask.h
//
// = AUTHOR
//    N.Leclercq - SOLEIL & JC.Pret - APSIDE
//
// ============================================================================

#ifndef _MY_DEVICE_TASK_H_
#define _MY_DEVICE_TASK_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <tango.h>

#include <yat/any/GenericContainer.h>
#include <yat/memory/DataBuffer.h>
#include <yat4tango/DeviceTask.h>
#include <yat4tango/DynamicAttributeManager.h>

namespace GroupManager_ns
{

// ============================================================================
// class: GroupManagerTask
// ============================================================================
class GroupManagerTask : public yat4tango::DeviceTask
{
public:

  //- the task's configuration data struct
  typedef struct Config 
  {
    //-default ctor
    Config ()
      : task_activity_period_ms(1000), 
        host_device(0)
    {}
    //- copy ctor
    Config (const Config & src)
    { 
      *this = src;
    }
    //- operator=
    const Config & operator= (const Config & src)
    { 
      if (&src == this)
        return *this;
      managed_devices = src.managed_devices;
      excluded_devices = src.excluded_devices;
      monitored_attributes= src.monitored_attributes;
      task_activity_period_ms = src.task_activity_period_ms;
      host_device = src.host_device;
      return *this;
    }
    //- managed devices 
    std::vector<std::string> managed_devices;
    //- excluded devices 
    std::vector<std::string> excluded_devices;
    //- monitored attributes 
    std::vector<std::string> monitored_attributes;
    //- period of task's periodic activity in msecs 
    size_t task_activity_period_ms;
    //- the Tango device hoting this task
    Tango::DeviceImpl * host_device;
  } Config;

	//- ctor
	GroupManagerTask (const Config & cfg);

	//- dtor
	virtual ~GroupManagerTask ();

	//- specialization of yat4tango::DeviceTask::go
  //---------------------------------------------------------------------------------------
  //- TANGO THREADING CONSTRAINT: THIS METHOD IS EXPECTED TO BE CALLED FROM <INIT_DEVICE>
  //- OR (AT LEAST) FROM A TANGO DEVICE THREAD (SEE THE TANGO DEVICE MONITOR FOR DETAILS)
  //---------------------------------------------------------------------------------------
  virtual void go (size_t tmo_ms = kDEFAULT_MSG_TMO_MSECS)
    throw (Tango::DevFailed);

	//- specialization of yat4tango::DeviceTask::exit
  //---------------------------------------------------------------------------------------
  //- TANGO THREADING CONSTRAINT: THIS METHOD IS EXPECTED TO BE CALLED FROM <INIT_DEVICE>
  //- OR (AT LEAST) FROM A TANGO DEVICE THREAD (SEE THE TANGO DEVICE MONITOR FOR DETAILS)
  //---------------------------------------------------------------------------------------
  virtual void exit ()
    throw (Tango::DevFailed);

  //- starts task's periodic activity (a <tmo_ms> of 0 means asynchronous exec)
  void start_periodic_activity (size_t tmo_ms = 0)
    throw (Tango::DevFailed);

  //- stops task's periodic activity (a <tmo_ms> of 0 means asynchronous exec)
  void stop_periodic_activity (size_t tmo_ms = 0)
    throw (Tango::DevFailed);

  //- post some data to the task (a <tmo_ms> of 0 means asynchronous exec)
  void post_some_data_to_the_task (double the_data, size_t tmo_ms = 0)
    throw (Tango::DevFailed); 

protected:
	//- process_message (implements yat4tango::DeviceTask pure virtual method)
	virtual void process_message (yat::Message& msg)
    throw (Tango::DevFailed);

private:
  //- encapsulates the periodic activity
  void periodic_job_i ()
    throw (Tango::DevFailed);

  //- if not already present, adds <State> at the end of the monitored attributes list
  void add_state_to_monitored_attributes_list_i ()
    throw (Tango::DevFailed);

  //- this method tries to obtain the Tango::AttributeInfo of all monitored attributes
  //- an exception is thrown in case:
  //-   -> none of the managed devices currently running (can't obtain Tango::AttributeInfo in this case)
  //-   -> (at least) one of managed devices doesn't support (at least) one of monitored attributes
  //-   -> attempt to obtain the Tango::AttributeInfoList failed on all managed devices
  void get_attributes_info_i (Tango::AttributeInfoList& attr_info)
    throw (Tango::DevFailed);

  //- generic read callback for dynamic attributes (yat4tango example)
  void generic_rd_cb (yat4tango::DynamicAttributeReadCallbackData& d)
    throw (Tango::DevFailed);

  //- generic write callback for dynamic attributes (yat4tango example)
  void generic_wt_cb (yat4tango::DynamicAttributeWriteCallbackData& d)
    throw (Tango::DevFailed);

  //- a specific read callback for some of our dynamic attributes (yat4tango example)
  void specific_rd_cb (yat4tango::DynamicAttributeReadCallbackData& d)
    throw (Tango::DevFailed);

  //- specific write callback for some of our dynamic attributes (yat4tango example)
  void specific_wt_cb (yat4tango::DynamicAttributeWriteCallbackData& d)
    throw (Tango::DevFailed);

  //- the task's configuration 
  Config m_cfg;

  //- the dynamic attribute manager
  yat4tango::DynamicAttributeManager m_dam;

  //- the managed Tango::Group
  Tango::Group m_grp;
};

} // namespace GroupManager_ns

#endif // _MY_DEVICE_TASK_H_
