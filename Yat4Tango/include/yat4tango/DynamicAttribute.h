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
// Dynamic Attribute (Quick & Dirty) Example
// ============================================================================
//    
//   class MyClass : public Tango::LogAdapter
//   {
//     public:
//       //------------------------------------------------------------------ 
//       MyClass (Tango::DeviceImpl * d)
//         : Tango::LogAdapter(d), m_dam (d)
//       {
//          this->init_dynamic_attributes();
//       }
//       //------------------------------------------------------------------ 
//       ~MyClass ()
//       {
//          this->remove_dynamic_attributes();
//       }
//       //------------------------------------------------------------------ 
//       void init_dynamic_attributes ()
//         throw (Tango::DevFailed)
//        {
//          //- this method must be executed by a Tango thread!
//          //-------------------------------------------------
//          yat4tango::DynamicAttributeInfo dai;
//          dai.dev = a pointer to the Tango::DeviceImpl to which the dyn. attr belongs;
//          dai.tai = a Tango::AttributeInfo: attr. name, format, type, ...;
//          dai.rcb = yat4tango::DynamicAttributeReadCallback::instanciate(*this, 
//                                                                         &MyClass::read_callback);
//          dai.wcb = yat4tango::DynamicAttributeWriteCallback::instanciate(*this, 
//                                                                         &MyClass::write_callback);
//          this->m_dam.add_attribute(dai);
//        }
//       //------------------------------------------------------------------ 
//       void remove_dynamic_attributes ()
//         throw (Tango::DevFailed)
//        {
//          //- this method must be executed by a Tango thread!
//          //-------------------------------------------------
//          this->m_dam.remove_attributes();
//        }
//       //------------------------------------------------------------------ 
//       void read_callback (DynamicAttributeReadCallbackData& cbd)
//        {
//          DEBUG_STREAM << "read request for dynamic attribute: " 
//                       << cbd.dya->get_name()
//                       << std::endl;
//          cbd.tga->set_value(...);
//        }
//       //------------------------------------------------------------------ 
//       void write_callback (DynamicAttributeWriteCallbackData& cbd)
//        {
//          DEBUG_STREAM << "write request for dynamic attribute: " 
//                       << cbd.dya->get_name()
//                       << std::endl;
//          cbd.tga->get_write_value(...);
//        }
//
//     private:
//        yat4tango::DynamicAttributeManager m_dam;
//   };
// ============================================================================

#ifndef _DYNAMIC_ATTRIBUTE_H_
#define _DYNAMIC_ATTRIBUTE_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/any/Any.h>
#include <yat/utils/Singleton.h>
#include <yat/utils/Callback.h>

#include <yat4tango/Export.h>
#include <yat4tango/Types.h>
#include <yat4tango/LogHelper.h>
#include <yat4tango/ExceptionHelper.h>

namespace yat4tango
{

//- used as default value for 'local' attribute names
extern const std::string KeepOriginalAttributeName;

// ============================================================================
// class: DynamicAttribute (forward declaration)
// ============================================================================
class DynamicAttribute;

// ============================================================================
// struct: DynamicAttributeCallbackData (read & write callback argument)
// ============================================================================
struct DynamicAttributeCallbackData
{
  //- the Tango device to which <dya> is attached
  Tango::DeviceImpl * dev;

  //- the dynamic attribute for which the callback is called 
  DynamicAttribute * dya;

  //- ctor
  DynamicAttributeCallbackData ()
    : dev (0), dya(0)
  {}

  //- copy ctor
  DynamicAttributeCallbackData (const DynamicAttributeCallbackData& src)
    : dev(src.dev), dya(src.dya)
  {}

  //- dtor
  ~DynamicAttributeCallbackData ()
  {}
};

// ============================================================================
// struct: DynamicAttributeReadCallbackData (read callback argument)
// ============================================================================
struct DynamicAttributeReadCallbackData : public DynamicAttributeCallbackData
{
  //- the Tango::Attribute on which a <set_value> is supposed to be done
  Tango::Attribute * tga;

  //- ctor
  DynamicAttributeReadCallbackData ()
    : DynamicAttributeCallbackData(), tga(0)
  {}

  //- copy ctor
  DynamicAttributeReadCallbackData (const DynamicAttributeReadCallbackData& src)
    : DynamicAttributeCallbackData(src), tga(src.tga)
  {}

  //- dtor
  ~DynamicAttributeReadCallbackData ()
  {}
};

// ============================================================================
// struct: DynamicAttributeWriteCallbackData (write callback argument)
// ============================================================================
struct DynamicAttributeWriteCallbackData : public DynamicAttributeCallbackData
{
  //- the Tango::WAttribute on which <get_write_value> is supposed to be called
  Tango::WAttribute * tga;

  //- ctor
  DynamicAttributeWriteCallbackData ()
    : DynamicAttributeCallbackData(), tga(0)
  {}

  //- copy ctor
  DynamicAttributeWriteCallbackData (const DynamicAttributeWriteCallbackData& src)
    : DynamicAttributeCallbackData(src), tga(src.tga)
  {}

  //- dtor
  ~DynamicAttributeWriteCallbackData ()
  {}
};

// ============================================================================
// class: DynamicAttributeReadCallback
// ============================================================================
YAT_DEFINE_CALLBACK(DynamicAttributeReadCallback, 
                    DynamicAttributeReadCallbackData &);

// ============================================================================
// class: DynamicAttributeWriteCallback
// ============================================================================
YAT_DEFINE_CALLBACK(DynamicAttributeWriteCallback, 
                    DynamicAttributeWriteCallbackData &);

// ============================================================================
// struct: DynamicAttributeInfo
// ============================================================================
struct DynamicAttributeInfo
{
  //- default ctor
  DynamicAttributeInfo ();

  //- copy ctor
  DynamicAttributeInfo (const DynamicAttributeInfo& s);

  //- assignment operator 
  const DynamicAttributeInfo& operator= (const DynamicAttributeInfo& s);

  //- set the associated user data
  template <typename T> void set_user_data (T* d)
  {
    uad = d;
  } 

  //- returns the associated user data
  template <typename T> T * get_user_data ()
  {
    try
    {
      return yat::any_cast<T*>(uad);
    }
    catch (const yat::Exception&)
    {
      return 0;
    }
  } 

  //- tango data type accessor (as humanly readable string) 
  const char * get_tango_data_type_str () const;

  //- tango access (i.e. read/write) accessor (as humanly readable string) 
  const char * get_tango_data_access_str () const;

  //- tango data format accessor (as humanly readable string) 
  const char * get_tango_data_format_str () const;

  //- dump this instance to std::cout
  void dump () /*const*/;

  //- the Tango device to which the dynamic attribute belongs to
  //- defaults to NULL
  Tango::DeviceImpl * dev;
  
  //- the 'local' attribute name (defaults to yat4tango::KeepOriginalAttributeName)
  //- for a simple dynamic attribute: always equals the name specified by 'tai.name' (Tango::AttributeInfo member)
  //- for a forwarded attribute: provides a way to specify the local name of the forwarded (or remote) attribute
  std::string lan;
  
  //- the Tango::Attribute info 
  Tango::AttributeInfo tai;
  
  //- read callback
  DynamicAttributeReadCallback rcb;
  
  //- write callback 
  DynamicAttributeWriteCallback wcb;

  //- should we cleanup the attribute properties from Tango db when removing this dynamic attr?
  //- defaults to true
  bool cdb;
  
private:
  friend class DynamicAttributeManager;
  friend class DynamicAttributeFactory;
  
  //- user 'abstract' data
  yat::Any uad;
}; 

// ============================================================================
// struct: ForwardedAttributeInfo (dynamic attribute instanciation - see below)
// ============================================================================
struct ForwardedAttributeInfo
{
  //- default ctor
  ForwardedAttributeInfo ();
  
  //- copy ctor
  ForwardedAttributeInfo (const ForwardedAttributeInfo& s);

  //- assignment operator 
  const ForwardedAttributeInfo& operator= (const ForwardedAttributeInfo& s);
  
  //- the device to which the dynamic attribute belongs to
  //- defaults to NULL
  Tango::DeviceImpl * dev;
  
  //- the fully qualified name of the attribute to be forwarded
  //- defaults to empty string
  std::string url;
  
  //- the "local" attribute name used to forward <url>
  //- defaults to yat4tango::KeepOriginalAttributeName  
  std::string lan;

  //- should the attribute access be restricted to read-only?
  //- defaults to false
  bool rdo;
};

// ============================================================================
// struct: DynamicAttribute 
// ============================================================================
class DynamicAttribute : public Tango::LogAdapter
{
public:
  //---------------------------------------------------------------------------
  //- attaches the specified read callback to the dynamic attribute
  //---------------------------------------------------------------------------
  void set_read_callback (const DynamicAttributeReadCallback & rcb);

  //---------------------------------------------------------------------------
  //- attaches the specified read callback to the dynamic attribute
  //---------------------------------------------------------------------------
  void set_write_callback (const DynamicAttributeWriteCallback & wcb);

  //---------------------------------------------------------------------------
  //- returns the underlying Tango::Attr
  //---------------------------------------------------------------------------
  virtual Tango::Attr * get_tango_attribute () = 0;

  //---------------------------------------------------------------------------
  //- returns the attribuet name
  //---------------------------------------------------------------------------
  inline const std::string & get_name () const
  {
    return this->dai_.tai.name;
  }

  //---------------------------------------------------------------------------
  //- returns the attribute data format - can be one of the following:
  //-       Tango::SCALAR, Tango::SPECTRUM, Tango::IMAGE or Tango::FMT_UNKNOWN
  //---------------------------------------------------------------------------
  inline int get_tango_data_format () const
  {
    return static_cast<int>(this->dai_.tai.data_format);
  }

  //---------------------------------------------------------------------------
  //- returns the attribute data format as a humanly readable string
  //-  <i> is a Tango data format identifier
  //-  the default i value means: return <this> attribute data format
  //---------------------------------------------------------------------------
  const char * get_tango_data_format_str () const;

  //---------------------------------------------------------------------------
  //- returns the data type (see server/tango_const.h/CmdArgType for details)
  //---------------------------------------------------------------------------
  inline int get_tango_data_type () const
  {
    return this->dai_.tai.data_type;
  }

  //---------------------------------------------------------------------------
  //- returns the attribute data type as a humanly readable string
  //-  <i> is a Tango data type identifier 
  //-  the default i value means: return <this> attribute data type
  //---------------------------------------------------------------------------
  const char * get_tango_data_type_str () const;

  //---------------------------------------------------------------------------
  //- returns the R/W access - can be one of the following:
  //-   Tango::READ, Tango::READ_WRITE, Tango::READ_WITH_WRITE, or Tango::WRITE
  //---------------------------------------------------------------------------
  inline int get_tango_data_access () const
  {
    return static_cast<int>(this->dai_.tai.writable);
  }

  //---------------------------------------------------------------------------
  //- returns the attribute data access as a humanly readable string
  //-  <i> is a Tango data access identifier
  //-  the default i value means: return <this> attribute data type
  //---------------------------------------------------------------------------
  const char * get_tango_data_access_str () const;

  //---------------------------------------------------------------------------
  //- returns the associated Tango::AttributeInfo (i.e. attribute description)
  //---------------------------------------------------------------------------
  inline const Tango::AttributeInfo & get_attribute_info () const
  {
    return this->dai_.tai;
  }

  //---------------------------------------------------------------------------
  //- sets the user data
  //---------------------------------------------------------------------------
  template <typename T> void set_user_data (T* d)
  {
    this->dai_.set_user_data<T>(d);
  } 

  //---------------------------------------------------------------------------
  //- returns the user data - returns 0 if no data or in case cast to T failed
  //---------------------------------------------------------------------------
  template <typename T> T* get_user_data ()
  {
    return this->dai_.get_user_data<T>();
  } 

  //---------------------------------------------------------------------------
  //- checks type compatibility beetween <T> and actual DynamicAttribute type
  //---------------------------------------------------------------------------
  template <typename T> bool check_data_type_compatibility () const
  {
    return this->get_tango_data_type() == TangoTraits<T>::type_id;
  }

  //---------------------------------------------------------------------------
  //- returns true if the dynamic attribute is writable, returns false otherwise
  //---------------------------------------------------------------------------
  inline bool is_writable () const
  {
    return this->dai_.tai.writable == Tango::WRITE
        || this->dai_.tai.writable == Tango::READ_WRITE
        || this->dai_.tai.writable == Tango::READ_WITH_WRITE;
  }

  //---------------------------------------------------------------------------
  //- returns true if the dynamic attribute is a forwarded, returns false otherwise
  //---------------------------------------------------------------------------
  inline bool is_forwarded () const
  {
    return this->dp_ ? true : false;
  }

  //---------------------------------------------------------------------------
  //- dumps attribute info to std::cout
  //---------------------------------------------------------------------------
 void dump () /*const*/;

protected:
  //---------------------------------------------------------------------------
  //- ctor
  //---------------------------------------------------------------------------
  DynamicAttribute (const DynamicAttributeInfo & dai);

  //---------------------------------------------------------------------------
  //- dtor
  //---------------------------------------------------------------------------
  virtual ~DynamicAttribute();

  //---------------------------------------------------------------------------
  //- link_to_proxy - links a forwarded attribute to its "remote" device
  //---------------------------------------------------------------------------
  void link_to_proxy (Tango::DeviceProxy * dp);

  //- the associated attribute info
  DynamicAttributeInfo dai_;

  //- the associated device proxy  (forwarded attribute only - NULL otherwise)
  Tango::DeviceProxy * dp_;
  
  //- the associated attribute value (forwarded attribute only - NULL otherwise)
  Tango::DeviceAttribute * da_;

private:
  friend class DynamicAttributeManager;
  friend class DynamicAttributeFactory;
  
  //- default forwarded attr read callback
  void fwd_attr_read (DynamicAttributeReadCallbackData& cbd)
    throw (Tango::DevFailed);
   
  //- forwards attribute data for reading
  void fwd_attr_read_data (Tango::Attribute& ta, Tango::DeviceAttribute& da)
    throw (Tango::DevFailed);
    
  //- default forwarded attr write callback
  void fwd_attr_write (DynamicAttributeWriteCallbackData& cbd)
    throw (Tango::DevFailed);

  //- forwards attribute data for writting
  void fwd_attr_write_data (Tango::WAttribute & wa, Tango::DeviceAttribute& da)
    throw (Tango::DevFailed);
  
  //- removes the attribute properties from the Tango db
  void cleanup_properties ()
    throw (Tango::DevFailed);

  //- not allowed methods
  DynamicAttribute (DynamicAttribute&);
  DynamicAttribute&  operator= (DynamicAttribute&);
};

// ============================================================================
// class: DynamicAttributeFactory
// ============================================================================
class DynamicAttributeFactory : public yat::Singleton<DynamicAttributeFactory>
{
  friend class DynamicAttributeManager;

private:
  //- dynamic attribute factory
  //- i: the Tango attribute info (name, format, type, ...) 
  DynamicAttribute * create_attribute (const yat4tango::DynamicAttributeInfo& i)
    throw (Tango::DevFailed);

  //- dynamic attribute factory
  //- i: the forwared attribute info (host, url, ...) 
  DynamicAttribute * create_attribute (const yat4tango::ForwardedAttributeInfo& i)
    throw (Tango::DevFailed);
};

} //- namespace 

#endif //- _DYNAMIC_ATTRIBUTE_H_
