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

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat4tango/DeviceTask.h>

#if !defined (YAT_INLINE_IMPL)
# include <yat4tango/DeviceTask.i>
#endif // YAT_INLINE_IMPL

namespace yat4tango
{

// ======================================================================
// class: Delegate
// ======================================================================
class Delegate : public yat::Task
{
public:
  DeviceTask * dt_;
  //----------------------------------------------------
  Delegate (DeviceTask * _dt)
    : yat::Task(), dt_(_dt)
  {}
  //----------------------------------------------------
  Delegate (const Task::Config& _cfg, DeviceTask * _dt)
    : yat::Task(_cfg), dt_(_dt)
  {}
  //----------------------------------------------------
  void handle_message (yat::Message& msg)
    throw (yat::Exception)
  {
    try
    {
      if (this->dt_)
        this->dt_->process_message(msg);
    }
    catch (const Tango::DevFailed& df)
    {
      throw TangoYATException(df);
    }
    catch (...)
    {
      throw yat::Exception();
    }
  } 
};

// ======================================================================
// DeviceTask::DeviceTask
// ======================================================================
DeviceTask::DeviceTask (Tango::DeviceImpl * _dev)
  : Tango::LogAdapter(_dev), yt_(0)
{
	YAT_TRACE("DeviceTask::DeviceTask");

  yt_ = new Delegate(this);
}

// ======================================================================
// DeviceTask::DeviceTask
// ======================================================================
DeviceTask::DeviceTask (const yat::Task::Config& _cfg, Tango::DeviceImpl * _dev)
  : Tango::LogAdapter(_dev), yt_(0)
{
	YAT_TRACE("DeviceTask::DeviceTask");

  yt_ = new Delegate(_cfg, this);
}

// ======================================================================
// DeviceTask::~DeviceTask
// ======================================================================
DeviceTask::~DeviceTask ()
{
	YAT_TRACE("DeviceTask::~DeviceTask");
}

// ============================================================================
// DeviceTask::go
// ============================================================================
YAT_INLINE void DeviceTask::go (size_t _tmo_msecs)
  throw (Tango::DevFailed)
{
  try
  {
    this->yt()->go(_tmo_msecs);
  }
  catch (const yat::Exception& ye)
  {
    throw YATDevFailed(ye);
  }
  catch (...)
  {
    THROW_DEVFAILED("DEVICE_ERROR",
                    "failed to instanciate/start the uderlying yat::task [unknown exception caught]",
                    "DeviceTask::go");
  }
}

// ============================================================================
// DeviceTask::go
// ============================================================================
void DeviceTask::go (yat::Message * _msg, size_t _tmo_msecs)
  throw (Tango::DevFailed)
{
  try
  {
    this->yt()->go(_msg, _tmo_msecs);
  }
  catch (const yat::Exception& ye)
  {
    throw YATDevFailed(ye);
  }
  catch (...)
  {
    THROW_DEVFAILED("DEVICE_ERROR",
                    "failed to instanciate/start the uderlying yat::task [unknown exception caught]",
                    "DeviceTask::go");
  }
}
    
// ============================================================================
// DeviceTask::exit
// ============================================================================
void DeviceTask::exit ()
  throw (Tango::DevFailed)
{
  try
  {
    this->yt()->exit();

    delete this;
  }
  catch (const yat::Exception& ye)
  {
    throw YATDevFailed(ye);
  }
  catch (...)
  {
    THROW_DEVFAILED("DEVICE_ERROR",
                    "failed to exit the uderlying yat::task [unknown exception caught]",
                    "DeviceTask::exit");
  }
}

} // namespace
