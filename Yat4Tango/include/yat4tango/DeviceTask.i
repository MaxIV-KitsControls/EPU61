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

namespace yat4tango
{

// ============================================================================
// DeviceTask::state
// ============================================================================
YAT_INLINE yat::Thread::State DeviceTask::state () 
{
  yat::Thread::State s;
  
  try
  {
    s = this->yt()->state();
  }
  catch (...)
  {
    s = yat::Thread::STATE_NEW;
  }
  
  return s;
}

// ============================================================================
// DeviceTask::post
// ============================================================================
YAT_INLINE void DeviceTask::post (yat::Message * _msg, size_t _tmo_msecs)
  throw (Tango::DevFailed)
{
  try
  {
    this->yt()->post(_msg, _tmo_msecs);
  }
  catch (const yat::Exception& ye)
  {
    throw YATDevFailed(ye);
  }
}

// ============================================================================
// DeviceTask::post
// ============================================================================
YAT_INLINE void DeviceTask::post (size_t msg_type, size_t tmo_msecs)
  throw (Tango::DevFailed)
{
  try
  {
    this->yt()->post(msg_type, tmo_msecs);
  }
  catch (const yat::Exception& ye)
  {
    throw YATDevFailed(ye);
  }
}

// ============================================================================
// DeviceTask::wait_msg_handled
// ============================================================================
YAT_INLINE void DeviceTask::wait_msg_handled (yat::Message * _msg, size_t _tmo_msecs)
  throw (Tango::DevFailed)
{
  try
  {
    this->yt()->wait_msg_handled(_msg, _tmo_msecs);
  }
  catch (const yat::Exception& ye)
  {
    throw YATDevFailed(ye);
  }
}

// ============================================================================
// DeviceTask::wait_msg_handled
// ============================================================================
YAT_INLINE void DeviceTask::wait_msg_handled (size_t msg_type, size_t tmo_msecs)
  throw (Tango::DevFailed)
{
  try
  {
    this->yt()->wait_msg_handled(msg_type, tmo_msecs);
  }
  catch (const yat::Exception& ye)
  {
    throw YATDevFailed(ye);
  }
}

// ============================================================================
// DeviceTask::
// ============================================================================
YAT_INLINE void DeviceTask::set_timeout_msg_period (size_t p)
{ 
  this->yt()->set_timeout_msg_period(p); 
} 

// ============================================================================
// DeviceTask::
// ============================================================================
YAT_INLINE size_t DeviceTask::get_timeout_msg_period () const
{ 
  return this->yt()->get_timeout_msg_period(); 
} 

// ============================================================================
// DeviceTask::
// ============================================================================
YAT_INLINE void DeviceTask::enable_timeout_msg (bool p)
{ 
  this->yt()->enable_timeout_msg(p); 
} 

// ============================================================================
// DeviceTask::
// ============================================================================
YAT_INLINE bool DeviceTask::timeout_msg_enabled () const
{ 
  return this->yt()->timeout_msg_enabled(); 
} 

// ============================================================================
// DeviceTask::
// ============================================================================
YAT_INLINE void DeviceTask::set_periodic_msg_period (size_t p)
{ 
  this->yt()->set_periodic_msg_period(p); 
} 

// ============================================================================
// DeviceTask::
// ============================================================================
YAT_INLINE size_t DeviceTask::get_periodic_msg_period () const
{ 
  return this->yt()->get_periodic_msg_period(); 
} 

// ============================================================================
// DeviceTask::
// ============================================================================
YAT_INLINE void DeviceTask::enable_periodic_msg (bool p)
{ 
  this->yt()->enable_periodic_msg(p); 
} 

// ============================================================================
// DeviceTask::
// ============================================================================
YAT_INLINE bool DeviceTask::periodic_msg_enabled () const
{ 
  return this->yt()->periodic_msg_enabled(); 
} 

// ============================================================================
// DeviceTask::
// ============================================================================
YAT_INLINE void DeviceTask::msgq_wm_unit (yat::MessageQ::WmUnit p)
{ 
  this->yt()->msgq_wm_unit(p); 
} 

// ============================================================================
// DeviceTask::
// ============================================================================
YAT_INLINE yat::MessageQ::WmUnit DeviceTask::msgq_wm_unit () const
{ 
  return this->yt()->msgq_wm_unit(); 
} 

// ============================================================================
// DeviceTask::
// ============================================================================
YAT_INLINE void DeviceTask::msgq_lo_wm (size_t p)
{ 
  this->yt()->msgq_lo_wm(p); 
} 

// ============================================================================
// DeviceTask::
// ============================================================================
YAT_INLINE size_t DeviceTask::msgq_lo_wm () const
{ 
  return this->yt()->msgq_lo_wm(); 
} 

// ============================================================================
// DeviceTask::
// ============================================================================
YAT_INLINE void DeviceTask::msgq_hi_wm (size_t p)
{ 
  this->yt()->msgq_hi_wm(p); 
} 

// ============================================================================
// DeviceTask::
// ============================================================================
YAT_INLINE size_t DeviceTask::msgq_hi_wm () const
{ 
  return this->yt()->msgq_hi_wm(); 
} 

// ============================================================================
// DeviceTask::
// ============================================================================
YAT_INLINE const yat::MessageQ::Statistics& DeviceTask::msgq_statistics ()
{ 
  return this->yt()->msgq_statistics();
} 

// ============================================================================
// DeviceTask::yt
// ============================================================================
YAT_INLINE yat::Task * DeviceTask::yt () const
  throw (Tango::DevFailed)
{
  if (! yt_)
  {
    THROW_DEVFAILED("DEVICE_ERROR",
                    "this yat4tango::DeviceTask is not properly initialized [invalid yat::Task pointer]",
                    "DeviceTask::yt");
  }
  return yt_;
}
  
} //- namespace
