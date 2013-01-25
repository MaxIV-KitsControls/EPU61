//----------------------------------------------------------------------------
// YAT4Tango LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2011 The Tango Community
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

#ifndef _YAT4TANGO_TYPES_H_
#define _YAT4TANGO_TYPES_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat4tango/CommonHeader.h>

namespace yat4tango
{

// ============================================================================
// struct: TangoTraits [deals with genericity]
// ============================================================================
template <typename T>
struct TangoTraits
{
};

// ============================================================================
// TangoTraits specializations [deals with genericity]
// ============================================================================
# define TANGO_TRAITS(TangoDataType, TangoDataTypeID, DevAttrMemberType, DevAttrMemberName) \
  template<> struct TangoTraits<TangoDataType> \
  { \
    static const int type_id = TangoDataTypeID; \
    \
    typedef DevAttrMemberType DevVarArray_var; \
    static DevVarArray_var& corba_buffer (Tango::DeviceAttribute& dev_attr) \
    { \
      return dev_attr.DevAttrMemberName; \
    } \
  }

// ============================================================================
// TangoTraits specializations [deals with genericity]
// ============================================================================
TANGO_TRAITS(Tango::DevBoolean, Tango::DEV_BOOLEAN, Tango::DevVarBooleanArray_var, BooleanSeq);
TANGO_TRAITS(Tango::DevUChar,   Tango::DEV_UCHAR,   Tango::DevVarCharArray_var,    UCharSeq);
TANGO_TRAITS(Tango::DevUShort,  Tango::DEV_USHORT,  Tango::DevVarUShortArray_var,  UShortSeq);
TANGO_TRAITS(Tango::DevShort,   Tango::DEV_SHORT,   Tango::DevVarShortArray_var,   ShortSeq);
TANGO_TRAITS(Tango::DevULong,   Tango::DEV_ULONG,   Tango::DevVarULongArray_var,   ext->ULongSeq);
TANGO_TRAITS(Tango::DevLong,    Tango::DEV_LONG,    Tango::DevVarLongArray_var,    LongSeq);
TANGO_TRAITS(Tango::DevULong64, Tango::DEV_ULONG64, Tango::DevVarULong64Array_var, ext->ULong64Seq);
TANGO_TRAITS(Tango::DevLong64,  Tango::DEV_LONG64,  Tango::DevVarLong64Array_var,  ext->Long64Seq);
TANGO_TRAITS(Tango::DevFloat,   Tango::DEV_FLOAT,   Tango::DevVarFloatArray_var,   FloatSeq);
TANGO_TRAITS(Tango::DevDouble,  Tango::DEV_DOUBLE,  Tango::DevVarDoubleArray_var,  DoubleSeq);
TANGO_TRAITS(Tango::DevString,  Tango::DEV_STRING,  Tango::DevVarStringArray_var,  StringSeq);
TANGO_TRAITS(Tango::DevState,   Tango::DEV_STATE,   Tango::DevVarStateArray_var,   ext->StateSeq);
TANGO_TRAITS(Tango::DevEncoded, Tango::DEV_ENCODED, Tango::DevVarEncodedArray_var, ext->EncodedSeq);
  
} //- namespace

#endif //- _YAT4TANGO_TYPES_H_ 
