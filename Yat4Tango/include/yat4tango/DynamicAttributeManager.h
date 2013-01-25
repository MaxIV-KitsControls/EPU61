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

#ifndef _YAT4TANGO_DYNAMIC_ATTR_MANAGER_H_
#define _YAT4TANGO_DYNAMIC_ATTR_MANAGER_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat4tango/CommonHeader.h>
#include <yat4tango/LogHelper.h>
#include <yat4tango/ExceptionHelper.h>
#include <yat4tango/DynamicAttribute.h>

namespace yat4tango
{

// ============================================================================
// class: DynamicAttributeManager
// ============================================================================
class YAT4TANGO_DECL DynamicAttributeManager : public Tango::LogAdapter
{
public:
  /**
   * Constructor. 
   * @param  _dev the device handled by the instance
   */
  DynamicAttributeManager (Tango::DeviceImpl * _dev = 0);
  
  /**
   * Destructor
   */
  ~DynamicAttributeManager();

  /**
   * set_host_device
   * @param h the Tango device instance to which the dynamic attributes are linked
   */
  void set_host_device (Tango::DeviceImpl * h)
    throw (Tango::DevFailed);

  /**
   * add_attribute
   * @param ai a yat4tango::DynamicAttributeInfo containing the attribute description
   */
  void add_attribute (const yat4tango::DynamicAttributeInfo& dai)
    throw (Tango::DevFailed);

  /**
   * add_attribute
   * @param da a yat4tango::DynamicAttribute
   */
  void add_attribute (yat4tango::DynamicAttribute* da)
    throw (Tango::DevFailed);

  /**
   * add_attribute - adds a forwarded attribute to the device interface
   * @param url a fully qualified Tango attribute name (i.e. remote/dev/name/attr)
   * @param lan local attribute name of the forwarded attribute - defaults to yat4tango::KeepOriginalAttributeName
   * @param rdo if set to true, the forwarded attribute access is locally rectricted to "read-only" - defaults to false
   */
  void add_attribute (const std::string& url, 
                      const std::string& lan = yat4tango::KeepOriginalAttributeName,
                      bool rdo = false)
    throw (Tango::DevFailed);

  /**
   * add_attribute - adds a forwarded attribute to the device interface
   * @param ai a yat4tango::ForwardedAttributeInfo containing the attribute description
   */
  void add_attribute (const yat4tango::ForwardedAttributeInfo& fai)
    throw (Tango::DevFailed);
    
  /**
   * add_attributes
   * @param ai a std::vector of Tango::AttributeInfo containing the attributes description
   */
  void add_attributes (const std::vector<DynamicAttributeInfo>& dai)
    throw (Tango::DevFailed);

  /**
   * add_attributes - adds the specified forwarded attributes to the device interface (provides a way to specify local attribute names)
   * @param ai a std::vector of Tango::ForwardedAttributeInfo containing the attributes description
   */
  void add_attributes (const std::vector<ForwardedAttributeInfo>& dai)
    throw (Tango::DevFailed);
    
  /**
   * add_attributes - adds the specified forwarded attributes to the device interface (keeps original attribute names and accesses)
   * @param urls a std::vector of fully qualified Tango attribute name (i.e. remote/dev/name/attr)
   */
  void add_attributes (const std::vector<std::string>& urls)
    throw (Tango::DevFailed);

  /**
   * remove_attribute
   * @param an the attribute name
   */
  void remove_attribute (const std::string& an)
    throw (Tango::DevFailed);

  /**
   * remove_attributes
   * Removes all the dynamic attributes registered
   */
  void remove_attributes ()
    throw (Tango::DevFailed);

  /**
   * get_attribute
   * @param an the attribute name
   * @param a reference to a DynamicAttribute 
   */
  DynamicAttribute & get_attribute (const std::string& an)
    throw (Tango::DevFailed)
  {
    DynamicAttributeIterator it = this->rep_.find(an);
	  if (it == this->rep_.end() || ! (*it).second)
	  {
	    THROW_DEVFAILED("ATTR_NOT_FOUND",
	                    "the specified dynamic attribute does not exist",
                      "DynamicAttributeManager::get_attribute");
	  }
	  return *((*it).second);
  }

private:
  typedef std::map <std::string, DynamicAttribute*> DynamicAttributeRepository;
  typedef DynamicAttributeRepository::value_type DynamicAttributeEntry;
  typedef DynamicAttributeRepository::iterator DynamicAttributeIterator;
  typedef DynamicAttributeRepository::const_iterator DynamicAttributeConstIterator;

  Tango::DeviceImpl * dev_;
  DynamicAttributeRepository rep_;

  DynamicAttributeManager(const DynamicAttributeManager&);
  DynamicAttributeManager& operator = (const DynamicAttributeManager&);
};

} // namespace

#endif
