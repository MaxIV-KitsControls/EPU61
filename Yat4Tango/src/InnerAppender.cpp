//----------------------------------------------------------------------------
// YAT4Tango LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2010  The Tango Community
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
 * \authors N.Leclercq, J.Malik - Synchrotron SOLEIL
 */

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <iomanip>
#include <time.h>
#include <yat/threading/Mutex.h>
#include <yat4tango/InnerAppender.h>

namespace yat4tango
{
// ============================================================================
// InnerAppender static members
// ============================================================================
yat::Mutex InnerAppender::m_rep_lock;
InnerAppender::InnerAppenderRepository InnerAppender::m_rep;

// ============================================================================
// InnerAppender::instanciate
// ============================================================================
void InnerAppender::initialize (Tango::DeviceImpl * hd, size_t bd)
  throw (Tango::DevFailed)
{
  yat::MutexLock guard(InnerAppender::m_rep_lock);

  if (! hd)
  {
    THROW_DEVFAILED("DEVICE_ERROR",
                    "unexpected null pointer to Tango device!",
                    "yat4tango::InnerAppender::initialize");
  }

  if (! bd)
    bd = DEFAULT_LOG_BUFFER_DEPTH;

  //- do we already have an InnerAppender registered for the specified device?
  InnerAppenderIterator it = InnerAppender::m_rep.find(hd);
  if (it != InnerAppender::m_rep.end())
  {
    THROW_DEVFAILED("DEVICE_ERROR",
                    "an InnerAppender is already associated to the specified device",
                    "InnerAppender::initialize");
  }

  InnerAppender * ia = new (std::nothrow) InnerAppender;
  if (! ia)
  {
    THROW_DEVFAILED("DEVICE_ERROR",
                    "yat4tango::InnerAppender instanciation failed!",
                    "yat4tango::InnerAppender::initialize");
  }

  ia->initialize_i(hd, bd);

  //- insert the InnerAppender into the local repository
  std::pair<InnerAppenderIterator, bool> insertion_result;
  insertion_result = InnerAppender::m_rep.insert(InnerAppenderEntry(hd, ia));
  if (insertion_result.second == false)
  {
    THROW_DEVFAILED("DEVICE_ERROR",
                    "failed to insert the InnerAppender into the local repository",
                    "yat4tango::InnerAppender::initialize");
  } 
}

// ============================================================================
// InnerAppender::release
// ============================================================================
void InnerAppender::release (Tango::DeviceImpl * hd)
  throw (Tango::DevFailed)
{
  yat::MutexLock guard(InnerAppender::m_rep_lock);

  if (! hd)
  {
    THROW_DEVFAILED("DEVICE_ERROR",
                    "unexpected null pointer to Tango device!",
                    "yat4tango::InnerAppender::release");
  }

  //- do we have an InnerAppender registered for the specified device?
  InnerAppenderIterator it = InnerAppender::m_rep.find(hd);
  if (it == InnerAppender::m_rep.end())
    return;

  //- release the InnerAppender
  it->second->release_i();

  //- remove the InnerAppender from the local repository
  InnerAppender::m_rep.erase(it);
}

// ============================================================================
// InnerAppender::InnerAppender
// ============================================================================
InnerAppender::InnerAppender ()
  : log4tango::Appender ("inner-appender"), 
    m_dev (0), 
    m_dam (0), 
    m_log_buffer_changed (true),
    m_max_log_buffer_depth (DEFAULT_LOG_BUFFER_DEPTH)
{
  //- noop 
}

// ============================================================================
// InnerAppender::~InnerAppender
// ============================================================================
InnerAppender::~InnerAppender ()
{
  DEBUG_ASSERT(m_dam == 0);
}

// ============================================================================
// InnerAppender::initialize_i
// ============================================================================
void InnerAppender::initialize_i (Tango::DeviceImpl * hd, size_t bd)
  throw (Tango::DevFailed)
{
  //- check input
  if (! hd)
  {
    THROW_DEVFAILED("DEVICE_ERROR",
                    "unexpected null pointer to Tango device!",
                    "yat4tango::InnerAppender::init");
  }
  //- store host device 
  this->m_dev = hd;

  //- store max log buffer depth
  this->m_max_log_buffer_depth = bd;

  //- instanciate the dynamic attribute manager
  this->m_dam = new (std::nothrow) DynamicAttributeManager(hd);
  if (! this->m_dam)
  {
    THROW_DEVFAILED("DEVICE_ERROR",
                    "yat4tango::DynamicAttributeManager instanciation failed!",
                    "yat4tango::InnerAppender::init");
  }

  //- describe the "log" dynamic attribute
  DynamicAttributeInfo dai;
  dai.dev = hd;
  dai.tai.name = "log";
  dai.tai.data_format = Tango::SPECTRUM;
  dai.tai.data_type = Tango::DEV_STRING;
  dai.tai.writable = Tango::READ;
  dai.tai.disp_level = Tango::OPERATOR;
  dai.rcb = DynamicAttributeReadCallback::instanciate(*this, &InnerAppender::read_callback);

  //- add it to the device interface
  this->m_dam->add_attribute(dai);

  //- self register to device logger
  this->m_dev->get_logger()->add_appender(this);
}

// ============================================================================
// InnerAppender::release_i
// ============================================================================
void InnerAppender::release_i ()
  throw (Tango::DevFailed)
{
  yat::MutexLock guard(this->m_lock);

  try
  {
    //- this will remove the "log" dynamic attribute from the device interface
    delete this->m_dam;
    this->m_dam = 0;
  }
  catch (...)
  {
    //- ignore error
  }

  //- self unregister to device logger (will generate a call to the destructor)
  this->m_dev->get_logger()->remove_appender(this);
}

// ============================================================================
// InnerAppender::close
// ============================================================================
void InnerAppender::close ()
{
  //- noop
}

// ============================================================================
// InnerAppender::read_callback (yat4tango::DynamicAttributeReadCallback impl)
// ============================================================================
void InnerAppender::read_callback (DynamicAttributeReadCallbackData& d)
  throw (Tango::DevFailed)
{
  static Tango::DevVarStringArray __log_array__;

  if (! d.tga)
  {
    THROW_DEVFAILED("DEVICE_ERROR",
                    "unexpected null pointer to Tango::Attribute [invalid yat4tango::DynamicAttributeReadCallbackData]",
                    "yat4tango::InnerAppender::read_callback");
  }

  {
    yat::MutexLock guard(this->m_lock);

    if (this->m_log_buffer_changed)
    {
      size_t l = this->m_log_buffer.size();

      __log_array__.length(l);
      
      for (size_t i = 0; i < l; i++)
        __log_array__[i] = this->m_log_buffer[i].c_str();

      this->m_log_buffer_changed = false;
    }

    d.tga->set_value(__log_array__.get_buffer(), __log_array__.length());
  }
}

// ============================================================================
// InnerAppender::
// ============================================================================
int InnerAppender::_append (const log4tango::LoggingEvent& event)
{
  //------------------------------------------------------------
  //- DO NOT LOG FROM THIS METHOD !!!
  //------------------------------------------------------------

  static const size_t max_time_str_len = 32;
  
  try
  {
    //- reformat the incomming LoggingEvent
    time_t raw_time;
    ::time(&raw_time);
    char c_date_string[max_time_str_len];
    struct tm * time_info = ::localtime(&raw_time);
    ::strftime (c_date_string, max_time_str_len, "%d:%m:%y@%H:%M:%S", time_info);
  
    std::ostringstream oss;
    oss << c_date_string
        << ": ["
        << log4tango::Level::get_name(event.level)
        << "] "
        << event.message;
        
    //- push the log into the log buffer
    {
      yat::MutexLock guard(this->m_lock);
      //- push
      this->m_log_buffer.push_back(oss.str());
      //- pop oldest log
      if (this->m_log_buffer.size() > this->m_max_log_buffer_depth)
        this->m_log_buffer.pop_front();
      //- mark buffer content as changed
      this->m_log_buffer_changed = true;
    }
  }
  catch (...) 
  {
    return -1;
  }
  return 0;
} 

// ============================================================================
// InnerAppender::requires_layout
// ============================================================================
bool InnerAppender::requires_layout () const
{
  return false; 
}

// ============================================================================
// InnerAppender::set_layout
// ============================================================================
void InnerAppender::set_layout (log4tango::Layout* l)
{
  delete l;
}

} // namespace 

