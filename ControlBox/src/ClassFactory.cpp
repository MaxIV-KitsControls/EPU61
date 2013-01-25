static const char *RcsId = "$Header: /usr/local/CVS/DeviceServer/Generic/Motorisation/ControlBoxV2/src/ClassFactory.cpp,v 1.10 2011/07/15 09:47:42 buteau Exp $";
//+=============================================================================
//
// file :        ClassFactory.cpp
//
// description : C++ source for the class_factory method of the DServer
//               device class. This method is responsible to create
//               all class singletin for a device server. It is called
//               at device server startup
//
// project :     TANGO Device Server
//
// $Author: buteau $
//
// $Revision: 1.10 $
//
// $Log: ClassFactory.cpp,v $
// Revision 1.10  2011/07/15 09:47:42  buteau
// MANTIS 19368
//
// Revision 1.8  2008/05/28 12:06:27  coquet
// added GalilGearedAxes Tango Class to add suppport for geared axes with user ratio capability
//
//
// copyleft :    European Synchrotron Radiation Facility
//               BP 220, Grenoble 38043
//               FRANCE
//
//-=============================================================================
//
//  		This file is generated by POGO
//	(Program Obviously used to Generate tango Object)
//
//         (c) - Software Engineering Group - ESRF
//=============================================================================


#include <ControlBoxClass.h>
#include <GalilAxisClass.h>
#include <GalilSlitClass.h>
#include <AxisRawDataReaderClass.h>
#include <GalilCoupledAxesClass.h>
#include <GalilGearedAxesClass.h>

/**
 *	Create ControlBoxClass singleton and store it in DServer object.
 */

void Tango::DServer::class_factory()
{
	add_class(ControlBox_ns::ControlBoxClass::init("ControlBox"));
	add_class(GalilAxis_ns::GalilAxisClass::init("GalilAxis"));
  add_class(GalilSlit_ns::GalilSlitClass::init("GalilSlit"));
	add_class(AxisRawDataReader_ns::AxisRawDataReaderClass::init("AxisRawDataReader"));
	add_class(GalilCoupledAxes_ns::GalilCoupledAxesClass::init("GalilCoupledAxes"));
	add_class(GalilGearedAxes_ns::GalilGearedAxesClass::init("GalilGearedAxes"));
}
