//----------------------------------------------------------------------------
// YAT4Tango LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2011 N.Leclercq & The Tango Community
//
// The YAT4Tango library is free software; you can redistribute it and/or 
// modify it under the terms of the GNU General Public License as published 
// by the Free Software Foundation; either version 2 of the License, or (at 
// your option) any later version.
//
// The YAT4Tango library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// See COPYING file for license details  
//
// Contact:
//      Nicolas Leclercq
//      Synchrotron SOLEIL
//------------------------------------------------------------------------------
/*!
 * \authors See AUTHORS file
 */

#ifndef _DEVICE_TASK_H_
#define _DEVICE_TASK_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/Task.h>
#include <yat4tango/Export.h>
#include <yat4tango/ExceptionHelper.h>

namespace yat4tango
{

// ============================================================================
// class: DeviceTask
// ============================================================================
class YAT4TANGO_DECL DeviceTask : public Tango::LogAdapter
{
public:
	//! Default ctor
	DeviceTask (Tango::DeviceImpl * dev);
  
  //! Ctor
	DeviceTask (const yat::Task::Config& cfg, Tango::DeviceImpl * dev);

	//! Dtor
	virtual ~DeviceTask ();

	//! Starts the task
  virtual void go (size_t tmo_ms = kDEFAULT_MSG_TMO_MSECS)
    throw (Tango::DevFailed);

 	//! Starts the task 
  //! An exception is thrown in case the specified message:
  //!   * is not of type yat::TASK_INIT
  //!   * is not "waitable"
  virtual void go (yat::Message * msg, size_t tmo_ms = kDEFAULT_MSG_TMO_MSECS)
    throw (Tango::DevFailed);

	//! Aborts the task (joins with the underlying thread before returning).
  virtual void exit ()
    throw (Tango::DevFailed);

  //! Posts a message to the task (asynchronous approach)
	void post (yat::Message * msg, size_t tmo_msecs = kDEFAULT_POST_MSG_TMO)
    throw (Tango::DevFailed);

  //! Posts the specified msg to the task then returns immediately (asynchronous approach)
  void post (size_t msg_type, size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS)
    throw (Tango::DevFailed);

  //! Posts the specified data to the task then returns immediately (asynchronous approach)
  template <typename T> void post (size_t msg_type, 
                                   T * data, 
                                   bool transfer_ownership = true,  
                                   size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS)
    throw (Tango::DevFailed);

  //! Posts the specified data to the task then returns immediately (asynchronous approach)
  template <typename T> void post (size_t msg_type,  
                                   const T & data,  
                                   size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS)
    throw (Tango::DevFailed);

	//! Posts a message to the task then wait for this msg to be handled (synchronous approach)
	void wait_msg_handled (yat::Message * msg, size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS)
    throw (Tango::DevFailed);

  //! Posts the specified msg to the task then waits for this message to be handled (synchronous approach)
  void wait_msg_handled (size_t msg_type, size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS)
    throw (Tango::DevFailed);

  //! Posts the specified data to the task then waits for the associated message to be handled (synchronous approach)
  template <typename T> void wait_msg_handled (size_t msg_type, 
                                               T * data, 
                                               bool transfer_ownership = true,  
                                               size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS)
    throw (Tango::DevFailed);

  //! Posts the specified data to the task then waits for the associated message to be handled (synchronous approach)
  template <typename T> void wait_msg_handled (size_t msg_type,  
                                               const T & data,  
                                               size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS)
    throw (Tango::DevFailed);

  //- timeout msg period mutator
  void set_timeout_msg_period (size_t p_msecs);
  
  //- periodic msg period accessor
  size_t get_timeout_msg_period () const;
  
  //- enable/disable timeout messages
  void enable_timeout_msg (bool enable);

  //- returns timeout messages handling status
  bool timeout_msg_enabled () const;

  //- periodic msg period mutator
  void set_periodic_msg_period (size_t p_msecs);
  
  //- periodic msg period accessor
  size_t get_periodic_msg_period () const;

  //- enable/disable periodic messages
  void enable_periodic_msg (bool enable);

  //- returns period messages handling status
  bool periodic_msg_enabled () const;

  //- MsgQ water marks unit mutator
  void msgq_wm_unit (yat::MessageQ::WmUnit _wmu);

  //- MsgQ water marks unit accessor
  yat::MessageQ::WmUnit msgq_wm_unit () const;

  //- MsgQ low water mark mutator (MsgQ unit dependent)
  void msgq_lo_wm (size_t _lo_wm);
  
  //- MsgQ low water mark accessor (MsgQ unit dependent)
  size_t msgq_lo_wm () const;
  
  //- MsgQ high water mark mutator (MsgQ unit dependent)
  void msgq_hi_wm (size_t _hi_wm);
  
  //- MsgQ high water mark accessor (MsgQ unit dependent)
  size_t msgq_hi_wm () const;
  
  //- MsgQ high water mark accessor (MsgQ unit dependent)
  const yat::MessageQ::Statistics & msgq_statistics ();

  //- Returns the underlying thread state
  //- yat::Thread::STATE_NEW: thread object exists but hasn't started yet
  //- yat::Thread::STATE_RUNNING: thread is running
  //- yat::Thread::STATE_TERMINATED: thread has terminated (waiting to be joined).
  yat::Thread::State state ();

protected:
  //! the actual yat::Message handler (must be implemented by derived classes)
  //! be sure that your implementation only throws Tango::DevFailed exceptions
  virtual void process_message (yat::Message& msg)
    throw (Tango::DevFailed) = 0;

private:
  friend class Delegate;

  //- helper member
  yat::Task * yt () const
    throw (Tango::DevFailed);

  //- delegate all activity to the underlying yat::Task;
  yat::Task * yt_;
};

// ============================================================================
// DeviceTask::post
// ============================================================================
template <typename T> void DeviceTask::post (size_t msg_type, 
                                             T * data, 
                                             bool transfer_ownership,  
                                             size_t tmo_msecs)
  throw (Tango::DevFailed)
{
  try
  {
    this->yt()->post(msg_type, data, transfer_ownership, tmo_msecs);
  }
  catch (const yat::Exception& ye)
  {
    throw YATDevFailed(ye);
  }
}

// ============================================================================
// DeviceTask::post
// ============================================================================
template <typename T> void DeviceTask::post (size_t msg_type,  
                                             const T & data,  
                                             size_t tmo_msecs)
  throw (Tango::DevFailed)
{
  try
  {
    this->yt()->post(msg_type, data, tmo_msecs);
  }
  catch (const yat::Exception& ye)
  {
    throw YATDevFailed(ye);
  }
}

// ============================================================================
// DeviceTask::wait_msg_handled
// ============================================================================
template <typename T> void DeviceTask::wait_msg_handled (size_t msg_type, 
                                                         T * data, 
                                                         bool transfer_ownership,  
                                                         size_t tmo_msecs)
  throw (Tango::DevFailed)
{
  try
  {
    this->yt()->wait_msg_handled(msg_type, data, transfer_ownership, tmo_msecs);
  }
  catch (const yat::Exception& ye)
  {
    throw YATDevFailed(ye);
  }
}

// ============================================================================
// DeviceTask::wait_msg_handled
// ============================================================================
template <typename T> void DeviceTask::wait_msg_handled (size_t msg_type,  
                                                         const T & data,  
                                                         size_t tmo_msecs)
  throw (Tango::DevFailed)
{
  try
  {
    this->yt()->wait_msg_handled(msg_type, data, tmo_msecs);
  }
  catch (const yat::Exception& ye)
  {
    throw YATDevFailed(ye);
  }
}

} // namespace

#if defined (YAT_INLINE_IMPL)
# include <yat4tango/DeviceTask.i>
#endif // YAT_INLINE_IMPL

#endif // _DEVICE_TASK_H_
