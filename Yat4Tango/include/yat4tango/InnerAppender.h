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

#ifndef _INNER_APPENDER_H_
#define _INNER_APPENDER_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <deque>
#include <map>
#include <tango.h>
#include <yat/threading/Mutex.h>
#include <yat4tango/DynamicAttributeManager.h>

namespace yat4tango
{

// ============================================================================
// Default log buffer depth
// ============================================================================
#define DEFAULT_LOG_BUFFER_DEPTH 512

// ============================================================================
// class: InnerAppender
// ============================================================================
class InnerAppender : public log4tango::Appender
{
public:
  /**
   * Initialization
   **/
   static void initialize (Tango::DeviceImpl* associated_device, 
                           size_t max_log_buffer_depth = DEFAULT_LOG_BUFFER_DEPTH)
     throw (Tango::DevFailed);

  /**
   * Termination
   **/
   static void release (Tango::DeviceImpl* associated_device)
     throw (Tango::DevFailed);

  /**
   * Always returns false! (log4tango::Appender virtual interface)
   **/
  virtual bool requires_layout () const;
      
  /**
   * Does nothing else than deleting the specified layout! (log4tango::Appender virtual interface)
   **/
  virtual void set_layout (log4tango::Layout* layout);

  /**
    * Does nothing! (log4tango::Appender virtual interface)
    **/
  virtual void close ();
  
protected:
  /**
    * Log appender method
    **/
  virtual int _append (const log4tango::LoggingEvent& event); 

private:
  //- dedicate a type to the log buffer
  typedef std::deque<std::string> LogBuffer;

  //- InnerAppender repository 
  typedef std::map<Tango::DeviceImpl*, InnerAppender*> InnerAppenderRepository;
  typedef InnerAppenderRepository::value_type InnerAppenderEntry;
  typedef InnerAppenderRepository::iterator InnerAppenderIterator;
  typedef InnerAppenderRepository::const_iterator InnerAppenderConstIterator;

  //- provide the yat4tango::DynamicAttributeReadCallback with access to read_callback 
  friend class DynamicAttributeReadCallback;

  //- provide the log4tango::Logger with access to the dtor 
  friend class log4tango::Logger;

  /**
   * Ctor
   **/
   InnerAppender ();

  /**
   * Dtor
   **/
  virtual ~InnerAppender ();

  /**
   * Initialization
   **/
  void initialize_i (Tango::DeviceImpl* host_device, size_t max_log_buffer_depth)
    throw (Tango::DevFailed);

  /**
   * Termination
   **/
  void release_i ()
    throw (Tango::DevFailed);

  /**
    * Read callback of the underlying dynamic attribute
    **/
  void read_callback (DynamicAttributeReadCallbackData& cbd)
    throw (Tango::DevFailed);

  //- the host device
  Tango::DeviceImpl * m_dev;

  //- dynamic attribute manager
  DynamicAttributeManager * m_dam;

  //- thread safety
  yat::Mutex m_lock;

  //- log buffer
  LogBuffer m_log_buffer;

  //- log buffer content changed flag
  bool m_log_buffer_changed;

  //- max log buffer depth
  size_t m_max_log_buffer_depth;

  //- InnerAppender repository
  static InnerAppenderRepository m_rep;

  //- thread safety
  static yat::Mutex m_rep_lock;

  //- disabled methods
  InnerAppender (const InnerAppender&);
  const InnerAppender& operator= (const InnerAppender&);
};

} // namespace

#endif // _INNER_APPENDER_H_
