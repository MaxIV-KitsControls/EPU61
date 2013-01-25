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

#ifndef _YAT4TANGO_LOGHELPER_H_
#define _YAT4TANGO_LOGHELPER_H_

// ============================================================================
// WIN32 SPECIFIC
// ============================================================================
#if defined (WIN32)
# pragma warning(disable:4786)
#endif

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include "yat/time/Timer.h"
#include "yat4tango/CommonHeader.h"

namespace yat4tango
{

// ============================================================================
// Class TangoLogAdapter
// ============================================================================
class YAT4TANGO_DECL TangoLogAdapter
{
public:

  TangoLogAdapter (Tango::DeviceImpl * _host)
    : logger_ (0)
  {
    this->host(_host);
  }

  virtual ~TangoLogAdapter () 
  {
    //- noop dtor
  } ;

  inline log4tango::Logger * get_logger () 
  {
    return logger_;
  }

  inline void host (Tango::DeviceImpl * _host) 
  {
    if (_host)
      logger_ = _host->get_logger();
    else
      logger_ = 0;
  }

private:
  log4tango::Logger* logger_;
};

// ============================================================================
// Class TraceHelper
// ============================================================================
class YAT4TANGO_DECL TraceHelper
{
public:

  TraceHelper(const char* _func_name, Tango::DeviceImpl * _this = 0)
    : m_this(_this), 
      m_func(_func_name),
      m_logger (_this ? _this->get_logger() : 0)
  { 
#if defined(WIN32)
    DEBUG_STREAM << "[this::0x" 
#else
    DEBUG_STREAM << "[this::" 
#endif
                 << std::hex 
                 << m_this 
                 << std::dec 
                 << "] "
                 << m_func 
                 << " <<"
                 << std::endl;
  };

  TraceHelper(const char* _func_name, Tango::LogAdapter * _this = 0)
    : m_this(_this), 
      m_func(_func_name),
      m_logger (_this ? _this->get_logger() : 0)
  { 
#if defined(WIN32)
    DEBUG_STREAM << "[this::0x" 
#else
    DEBUG_STREAM << "[this::" 
#endif
                 << std::hex 
                 << m_this 
                 << std::dec 
                 << "] "
                 << m_func 
                 << " <<"
                 << std::endl;
  };

  virtual ~TraceHelper()
  { 
#if defined(WIN32)
    DEBUG_STREAM << "[this::0x" 
#else
    DEBUG_STREAM << "[this::" 
#endif
                 << std::hex 
                 << m_this 
                 << std::dec 
                 << "] "
                 << m_func 
                 << " >> ["
                 << m_timer.elapsed_msec()
                 << " ms]"
                 << std::endl; 
  };

  inline log4tango::Logger * get_logger () 
  {
    return m_logger;
  }

private:
  const void * m_this;
  const char * m_func;
  log4tango::Logger * m_logger;
  yat::Timer m_timer;
};

} //- namespace yat4tango 

#endif //- _YAT4TANGO_LOGHELPER_H_
