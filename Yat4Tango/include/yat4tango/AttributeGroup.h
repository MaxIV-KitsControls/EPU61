//---------------------------------------------------------------------------
// YAT4Tango LIBRARY
//---------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
/*!
 * \authors See AUTHORS file
 */

#ifndef _ATTRIBUTE_GROUP_H_
#define _ATTRIBUTE_GROUP_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <tango.h>
#include <yat4tango/LogHelper.h>
#include <yat4tango/ExceptionHelper.h>

namespace yat4tango
{

// ============================================================================
// class: AttributeGroupReply
// ============================================================================
/**
 * @brief The 'reply' abstraction of the AttributeGroup class.
 **/
class AttributeGroupReply
{
  friend class AttributeGroup;

public:
  /**
   * @brief Exceptions enabler/disabler.
   * 
   * Enables or disables exceptions. Provides a mechanism similar to the one 
   * implemented by the Tango::DeviceAttribute class. By default, the 
   * AttributeGroupReply exceptions are disabled. Note that this method is
   * static. It consequently has an impact on ALL AttributeGroupReply instances.   
   * 
   * @param enable true means "enable exceptions", false means "disable exceptions". 
   * @return previous mode
   **/
  static bool enable_exception (bool enable = true);
  
  /**
   * @brief Default constructor.
   *
   * Each AttributeGroupReply member is initialized using its own default constructor.
   **/
  AttributeGroupReply ();
  
  /**
   * @brief Copy constructor.
   * 
   * Due to the presence of the underlying Tango::DeviceAttribute member, be sure 
   * to understand how the Tango::DeviceAttribute::operator= behaves before using 
   * this constructor (i.e. be sure to understand how the Tango::DeviceAttribute 
   * manages memory).
   * 
   * @param src the AttributeGroupReply instance to copy.
   *
   * @see operator=()
   **/
  AttributeGroupReply (const AttributeGroupReply& src);
  
  /**
   * Destructor
   **/
  virtual ~AttributeGroupReply ();
  
  /**
   * @brief Assignment operator. 
   * 
   * Due to the presence of the underlying Tango::DeviceAttribute member, be sure 
   * to understand how the Tango::DeviceAttribute::operator= behaves before using 
   * this constructor (i.e. be sure to understand how the Tango::DeviceAttribute 
   * manages memory).
   * 
   * @param src the AttributeGroupReply instance to copy. 
   **/
  const AttributeGroupReply& operator= (const AttributeGroupReply& src);
  
  /**
   * @brief Device name accessor. 
   * 
   * @return the associated device name.
   **/
  inline const std::string& dev_name () const 
  {
    return dev_name_m;
  }
  
  /**
   * @brief Attribute name accessor. 
   * 
   * @return the associated attribute name.
   **/
  inline const std::string& attr_name () const 
  {
    return attr_name_m;
  }
  
  /**
   * @brief Error status. 
   * 
   * Unless exceptions are enabled, be sure to always check the has_failed() status 
   * of an AttributeGroupReply before trying to access its underlying Tango data or 
   * exception. 
   *
   * In case has_failed()returns false, it is guaranteed that get_data() will return 
   * a valid Tango::DeviceAttribute. If true is returned, the content of the Tango::
   * DeviceAttribute returned by get_data() is undefined.
   * 
   * @return returns true if an error occurred, returns false otherwise.
   * 
   * @see get_data()
   * @see get_exception()
   **/
  inline bool has_failed () const 
  { 
    return has_failed_m;
  }
  
  /**
   * @brief Data accessor.
   * 
   * Returns the attribute data. Might throw an exception in case an error occurred (while 
   * reading the associated attribute) AND the AttributeGroupReply exceptions are enabled. 
   * In case exceptions are disabled, be sure to check the has_failed() status before trying 
   * to access the Tango::DeviceAttribute. In case has_failed() returns false, it is guaranteed 
   * that get_data() will return a valid DeviceAttribute. Note that 'valid' means 'got a valid 
   * reply from device' (i.e. no error). However, the returned DeviceAttribute could be empty 
   * or could even contain an unexpected data type (see the Tango::DeviceAttribute documentation 
   * for details).
   * 
   * @return returns the associated Tango::DeviceAttribute.
   * 
   * @see has_failed()
   * @see get_exception()
   * @see enable_exception()
   **/
  inline Tango::DeviceAttribute & get_data () 
    throw (Tango::DevFailed) 
  {
    if ( has_failed_m && exception_enabled ) 
    {
      throw exception_m;
    }
    std::bitset<Tango::DeviceAttribute::numFlags> bs;
    data_m.exceptions(exception_enabled ? bs.set() : bs.reset());
    return data_m; 
  }
  
  /**
   * @brief Data extractor. Extracts the attribute value as a "T".
   * 
   * @return true if the attribute value could be successfully extracted, 
   * returns false otherwise. 
   *
   * An exception might be thrown in case the data extraction failed AND the 
   * AttributeGroupReply exceptions are enabled.
   *
   * @see has_failed()
   * @see get_data()
   * @see enable_exception()
   **/
  template <typename T> bool operator>> (T& dest) 
    throw (Tango::DevFailed);
  
  /**
   * @brief Tango exception accessor.
   * 
   * @return the associated Tango exception.
   *
   * The returned exception is only valid in case has_failed() returns true.
   * Its content is undefined otherwise.
   * 
   * @see has_failed()
   **/
  inline const Tango::DevFailed& get_exception () const 
  {
    return exception_m;
  }

  /**
   * @brief Tango error stack accessor.
   * 
   * @return the Tango error stack
   * 
   * The returned error stack is only valid in case has_failed() returns true.
   * Its content is undefined otherwise.
   * 
   * @see has_failed()
   **/
  inline const Tango::DevErrorList& get_err_stack () const 
  {
    return exception_m.errors;
  }
  
private:
  AttributeGroupReply (const std::string& dev_name, 
                       const std::string& attr_name);
  
  AttributeGroupReply (const std::string& dev_name, 
                       const std::string& attr_name,
                       const Tango::DevFailed& exception);

  // exceptions flag (enabled/disabled)
  static bool exception_enabled;
  
  // the device name
  std::string dev_name_m;
  
  // attribute name
  std::string attr_name_m;
  
  // error flag: true is an error occurred, false otherwise
  bool has_failed_m;
  
  // data: valid if GroupReply::has_failed returns false
  Tango::DeviceAttribute data_m;
  
  // exception: valid if has_failed returns true
  Tango::DevFailed exception_m;
};

// ============================================================================
// AttributeGroupReply::operator>>
// ============================================================================
template <typename T> 
bool AttributeGroupReply::operator>> (T& dest) 
  throw (Tango::DevFailed) 
{
  bool result = true;
  if ( has_failed_m ) 
  {
    if ( AttributeGroupReply::exception_enabled )
      throw exception_m;
    result = false;
  }
  else 
  {
    std::bitset<Tango::DeviceAttribute::numFlags> bs;
    data_m.exceptions(AttributeGroupReply::exception_enabled ? bs.set() : bs.reset()); 
    try 
    {
      result = data_m >> dest;
    }
    catch (const Tango::DevFailed& df) 
    {
      exception_m = df;
      if ( AttributeGroupReply::exception_enabled ) 
        throw exception_m;
      result = false;
    }
    catch (...) 
    {
      if ( AttributeGroupReply::exception_enabled ) 
      {
        Tango::DevErrorList errors(1);
	      errors.length(1);
	      errors[0].severity = Tango::ERR;
	      errors[0].desc = CORBA::string_dup("unknown exception caught");
	      errors[0].reason = CORBA::string_dup("an error occured while trying to extract data");
	      errors[0].origin = CORBA::string_dup("AttributeGroupReply::operator>>");
        Tango::DevFailed df(errors);
        exception_m = df;
        throw exception_m;
      }
      result = false;
    }
  }
  return result;
}

//=============================================================================
// class: GroupReplyList
//=============================================================================
/**
 * @brief An AttributeGroupReply list (inherits from std::vector).
 **/
class AttributeGroupReplyList : public std::vector<AttributeGroupReply>
{
  typedef std::vector<AttributeGroupReply> Inherited;

  friend class AttributeGroup;

public:
  /**
   * @brief Constructor.
   * 
   * @param n intial size of the list .
   **/
  AttributeGroupReplyList (size_t n = 0);
  
  /**
   * @brief Destructor.
   **/
  virtual ~AttributeGroupReplyList ();
  
  /**
   * @brief Error status. 
   *
   * @return this method returns true if at least one attribute failed, returns false
   * otherwise.
   **/
  inline bool has_failed () const 
  { 
    return has_failed_m;
  }
  
  /**
   * @brief Resets/clears the whole list. 
   *
   * This makes the list 'reusable' in the code block in which it's defined.
   * The underlying std::vector size is reseted to 0. 
   **/
  inline void reset () 
  { 
    clear(); 
    has_failed_m = false;
  };
  
  /**
   * @brief Overwrites std::vector::push_back. 
   *
   * Sets the has_failed_m member to false (if required) then calls std::vector::push_back.
   **/
  void push_back (const AttributeGroupReply& agr) 
  {
    if ( agr.has_failed() )
      has_failed_m = true;
    Inherited::push_back(agr);
  }
  
private:
  // has_failed_m: true if at least one error occurred, false otherwise
  bool has_failed_m;
};

// ============================================================================
// class: AttributeGroup
// ============================================================================
/**
 * @brief An attribute group with "a la Tango::Group" interface.
 * 
 * Attributes belonging to the same Tango device are guaranteed to be read and 
 * written in a single call to Tango::DeviceProxy::[read/write]_attributes. 
 *
 * As for the Tango::Group, the attributes registration order matters! When 
 * reading the attributes, the replies will always be returned in this the 
 * registration order. In the same way, 'write values' must be specified in 
 * the registration order. 
 * 
 * The implementation is NOT thread-safe. Don't make the assumption that a
 * simple method like AttributeGroup::device_proxy_list is thread safe!
 * Maunipulating an AttributeGroup from different threads requires each t
 * thread to access the shared AttributeGroup under critical section. 
 * 
 * This first implementation only supports SCALAR attributes of the same data 
 * type.
 **/
class AttributeGroup : public Tango::LogAdapter
{
public:
  /**
   * @brief Constructor.
   *  
   * Constructs an empty AttributeGroup.
   *
   * @param dev host Tango device (for logging purpose)
   **/
  AttributeGroup (Tango::DeviceImpl* dev = 0);

  /**
   * @brief Destructor.
   **/
  virtual ~AttributeGroup ();

  /**
   * @brief Attribute registration.
   *  
   * Adds the specified attribute to the group.
   *
   * @param fqn the fully qualified attribute name: [tdb-host:tdb-port/]the/dev/name/attr-name.
   **/
  void register_attribute (const std::string& fqn)
    throw (Tango::DevFailed);
    
  /**
   * @brief Attributes registration.
   *  
   * Adds the specified attributes to the group.
   *
   * @param fqns the fully qualified attribute names: [tdb-host:tdb-port/]the/dev/name/attr-name.
   **/
  void register_attributes (const std::vector<std::string>& fqns)
    throw (Tango::DevFailed);

  /**
   * @brief Registrered attributes list accessor.
   *  
   * @return the currently registered attributes (as a std::vector of fully qualified attribute names).
   **/
  const std::vector<std::string>& attribute_list () const;

  /**
   * @brief Device-proxy list accessor.
   *  
   * Returns le underlying Tango device proxies. The returned vector does not guarantee any special order. 
   * A device whith several attributes registered in the group only appears once in this list.
   * DOT NOT DELETE ANY DEVICE PROXY STORED IN THE RETURN STD::VECTOR.  
   *
   * @return the unordered list of currently registered devices.
   **/
  std::vector<Tango::DeviceProxy*> device_proxy_list () /*const*/;

  /**
   * @brief Device-proxy list accessor.
   *  
   * Returns le underlying Tango device proxies. The returned proxies are ordered in the 'registration order'.
   * It means that a device with 'n' attributes registered in the group will appear 'n' times in the returned list.
   * DOT NOT DELETE ANY DEVICE PROXY STORED IN THE RETURN STD::VECTOR.  
   *
   * @return the ordered list of currently registered devices.
   **/
  const std::vector<Tango::DeviceProxy*> & ordered_device_proxy_list () const;

  /**
   * @brief Writes the specified values on the registered attributes.
   *  
   * As for the Tango::Group, the attributes registration order matters! 
   * The'write values' must be specified in the registration order so that
   * the implementation can apply the 'right value' on the 'right attribute'.
   * 
   * @param values the 'write values' (must respect registration order!).
   *
   * Might throw an exception in case an error occurred (at least one).
   *
   * @see read()
   **/
  template <typename T>
  void write (const std::vector<T>& values)
    throw (Tango::DevFailed);
    
  /**
   * @brief Reads the registered attributes.
   *  
   * As for the Tango::Group, the attributes registration order matters! 
   * The replies are guaranteed to be returned in the registration order.
   * 
   * @return the attribute values.
   *
   * Might throw an exception in case an error occurred. 
   *
   * @see write()
   * @see AttributeGroupReply
   **/
  AttributeGroupReplyList read ()
    throw (Tango::DevFailed);
    
  /**
   * @brief Clears the group (i.e. removes any registered attributes).
   * After this call, the group is empty!
   **/
  void clear ();

private:
  // fully qualified attribute name (one entry per registred attribute)
  std::vector<std::string> m_fqn_vect;
  // device name (one entry per registred attribute)
  std::vector<std::string> m_dev_vect;
  // attribute name (one entry per registred attribute)
  std::vector<std::string> m_att_vect;

  // list of devices involved in the group (one entry per registered device)
  std::vector<std::string> m_unique_dev_vect;

  // device proxies repository (one entry per registered device)
  typedef std::map<std::string, Tango::DeviceProxy*> Proxies;
  typedef Proxies::iterator ProxiesIterator;
  Proxies m_dev_proxy_map;

  // device proxies list (might have several entries per registered device)
  typedef std::vector<Tango::DeviceProxy*> ProxiesList;
  ProxiesList m_ordered_dev_proxy_list;

  // attribute lists repository (one entry per registered device)
  typedef std::vector<std::string> AttributeList;
  typedef std::map<std::string, AttributeList> AttributeLists;
  typedef AttributeLists::iterator AttributeListsIterator;
  AttributeLists m_attr_list_map;
  
  // registration ids repository (one entry per registered attribute)
  typedef std::map<std::string, size_t> Attr2RegId;
  typedef Attr2RegId::iterator Attr2RegIdIterator;
  Attr2RegId m_attr_2_regid_map;
  
  // copy constructor is disabled
  AttributeGroup( const AttributeGroup &src);
};

// ============================================================================
// class: AttributeGroup::write
// ============================================================================
template <typename T>
void AttributeGroup::write (const std::vector<T>& values) 
  throw (Tango::DevFailed)
{
  // <values> is supposed to respect the registration order
  //-------------------------------------------------------
  
  // TraceHelper t("AttributeGroup::write", this);
  
  // check input (m_dev_vect, m_att_vect, ... have all the same size)
  if ( values.size() != m_att_vect.size() )
  {
    THROW_DEVFAILED("DEVICE_ERROR",
                    "the number of attribute values doesn't match the number of registered attributes",
                    "AttributeGroup::write");
  }

  // if data is written on a single device, then write it synchronously
  bool single_device = m_unique_dev_vect.size() == 1;
  if ( single_device )
  {
    std::cout << "AttributeGroup::writing attributes on a single device [sync]"
              << std::endl;
                 
    // the device name is the only one stored into <m_unique_dev_vect>
    const std::string dev_name = m_unique_dev_vect[0];
    
    // build list of attribute values to be written
    std::vector<Tango::DeviceAttribute> attr_values;
    for (size_t a = 0; a < m_att_vect.size(); a++)
    {
      std::cout << "AttributeGroup::about to write "
                << values[a]
                << " on "
                << dev_name
                << "/"
                << m_att_vect[a]
                << std::endl;
      attr_values.push_back(Tango::DeviceAttribute(m_att_vect[a], values[a]));
    }
    
    try
    {
      // get device proxy
      Tango::DeviceProxy * dev_proxy = m_dev_proxy_map[dev_name];
      if ( ! dev_proxy )
      {
        std::ostringstream s;
        s << "unexpected null Tango::DeviceProxy found in repository for device " 
          << dev_name;
        THROW_DEVFAILED("DEVICE_ERROR",
                        s.str().c_str(),
                        "AttributeGroup::write");
      }
      
      std::cout << "AttributeGroup::writing values synchronously..."
                << std::endl;
                   
      // write values
      dev_proxy->write_attributes(attr_values);
      
      std::cout << "AttributeGroup::values successfully written"
                << std::endl;
    }
    catch (Tango::DevFailed &df)
    {
      RETHROW_DEVFAILED(df,
                        "DEVICE_ERROR",
                        "Tango exception caught while trying to write attributes",
                        "AttributeGroup::write");
    }
    catch (...)
    {
      THROW_DEVFAILED("DEVICE_ERROR",
                      "unknown exception caught while trying to write attributes",
                      "AttributeGroup::write");
    }
  }
  // if data is written on several devices, then write it asynchronously
  else 
  {
       std::cout << "AttributeGroup::writing attributes on a several devices [async]"
                 << std::endl;
                 
    // asynchronous requests identifiers
    std::vector<long> async_req_id;

    // write attributes on each registered device 
    for (size_t d = 0; d < m_unique_dev_vect.size(); d++)
    {
      // the device name 
      const std::string dev_name = m_unique_dev_vect[d];
      
      // build list of attribute values to be written
      std::vector<Tango::DeviceAttribute> attr_values;
      for (size_t v = 0; v < values.size(); v++)
      {
        // is the i-th value associated to <dev_name>?
        if ( dev_name == m_dev_vect[v] )
        {
          std::cout << "AttributeGroup::about to write "
                    << values[v]
                    << " on "
                    << dev_name
                    << "/"
                    << m_att_vect[v]
                    << std::endl;
          // the associated attribute is simply m_att_vect[v]
          attr_values.push_back(Tango::DeviceAttribute(m_att_vect[v], values[v]));
        }
      }
      
      // select device proxy associated to current device
      Tango::DeviceProxy * dev_proxy = m_dev_proxy_map[dev_name];
      if ( ! dev_proxy )
      {
        std::ostringstream s;
        s << "unexpected null Tango::DeviceProxy found in repository for device " 
          << dev_name;
        THROW_DEVFAILED("DEVICE_ERROR",
                        s.str().c_str(),
                        "AttributeGroup::write");
      }

      std::cout << "AttributeGroup::writing "
                << attr_values.size()
                << " attributes on "
                << dev_name
                << " asynchronously"
                << std::endl;
                    
      // write attributes asynchronously
      try
      {
        long ari = dev_proxy->write_attributes_asynch(attr_values);
        async_req_id.push_back(ari);
      }
      catch (Tango::DevFailed &df)
      {
        RETHROW_DEVFAILED(df,
                          "DEVICE_ERROR",
                          "Tango exception caught while trying to write attributes",
                          "AttributeGroup::write");
      }
      catch (...)
      {
        THROW_DEVFAILED("DEVICE_ERROR",
                        "unknown exception caught while trying to write attributes",
                        "AttributeGroup::write");
      }
    }

    // get write replies
    for (size_t d = 0; d < m_unique_dev_vect.size(); d++)
    {
      // the device name 
      const std::string dev_name = m_unique_dev_vect[d];
      
      // select device proxy associated to current device
      Tango::DeviceProxy * dev_proxy = m_dev_proxy_map[dev_name];
      if ( ! dev_proxy )
      {
        std::ostringstream s;
        s << "unexpected null Tango::DeviceProxy found in repository for device " 
          << dev_name;
        THROW_DEVFAILED("DEVICE_ERROR",
                        s.str().c_str(),
                        "AttributeGroup::write");
      }
      
      std::cout << "AttributeGroup::requesting async. replies for "
                << dev_name
                << std::endl;
                
      try
      {
        dev_proxy->write_attributes_reply(async_req_id[d], 2500);
      }
      catch (Tango::DevFailed &df)
      {
        RETHROW_DEVFAILED(df,
                          "DEVICE_ERROR",
                          "Tango exception caught while trying to obtain async write replies",
                          "AttributeGroup::write");
      }
      catch (...)
      {
        THROW_DEVFAILED("DEVICE_ERROR",
                        "unknown exception caught while trying to obtain async write replies",
                        "AttributeGroup::write");
      }
      
      std::cout << "AttributeGroup::values successfully written on "
                << dev_name
                << std::endl;
    }
  }
}

} // namespace 

#endif // _ATTRIBUTE_GROUP_H_
