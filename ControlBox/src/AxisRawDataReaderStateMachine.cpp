static const char *RcsId = "$Header: /usr/local/CVS/DeviceServer/Generic/Motorisation/ControlBoxV2/src/AxisRawDataReaderStateMachine.cpp,v 1.8 2011/07/15 09:47:42 buteau Exp $";
//+=============================================================================
//
// file :         AxisRawDataReaderStateMachine.cpp
//
// description :  C++ source for the AxisRawDataReader and its alowed. 
//                method for commands and attributes
//
// project :      TANGO Device Server
//
// $Author: buteau $
//
// $Revision: 1.8 $
//
// $Log: AxisRawDataReaderStateMachine.cpp,v $
// Revision 1.8  2011/07/15 09:47:42  buteau
// MANTIS 19368
//
// Revision 1.7  2011/07/15 09:40:53  buteau
// MANTIS 19368
//
// Revision 1.6  2011/05/27 13:45:28  coquet
// added attribute positionError to AxisRawDataDeader
// TO BE TESTED
//
// Revision 1.5  2008/10/17 07:06:15  coquet
// added attribute velocity to AxosRawDataReader
// to be tested
//
// Revision 1.4  2007/06/04 12:28:57  coquet
// AxisRawDataReader :
// added latch encoder position support
// added 2 attributes
// latchIsArmed, RO, bool, reports that latch feature is ready to capture encoder position
// latchOccured reports that latch occured since last latch arming
// added 2 commands
// ArmLatch arms the latch feature
// ReportLatchedPosition returns the last lached position ( exception if latch did not occured)
//
// Revision 1.3  2007/04/19 08:26:24  coquet
// AxisRawDataReader :
// added 3 attributes
// * axisLetter
// * latchInput
// * homeInput
// GalilSSlit :
// * added tips in Status to come out of Limit switchs if necessary
// * added boolean property InvertPositionDirection to allow user to invert the positive direction if necessary
// GalilAxis :
// * attribute position is in ALARM if positionning failed
// * internal "EncoderMotorRatio" is set to 1.0 when motor is servo (according to bit 0 of Stat[x])  or if stepper motor has no encoder.
//
// Revision 1.2  2007/01/08 14:45:25  coquet
// AxisRawDataReader device first tests :
// bug corrections.
//
// Revision 1.1  2007/01/08 11:19:54  coquet
// added AxisRawDataReader device
// this devices offers read-only acces to raw data of an axis (selected by an attribute)
// added the files AxisRawDataReader*.*,
// modified ClassFactory, Makefile.vc, DevicesGalilV2.dsp Visual Studio project
//
//
// copyleft :  Synchrotron SOLEIL
//             L'Orme des Merisiers
//             Saint-Aubin - BP 48
//             FRANCE
//
//-=============================================================================
//
//  		This file is generated by POGO
//	(Program Obviously used to Generate tango Object)
//
//         (c) - Software Engineering Group - ESRF
//=============================================================================

#include <tango.h>
#include <AxisRawDataReader.h>
#include <AxisRawDataReaderClass.h>

/*====================================================================
 *	This file contains the methods to allow commands and attributes
 * read or write execution.
 *
 * If you wand to add your own code, add it between 
 * the "End/Re-Start of Generated Code" comments.
 *
 * If you want, you can also add your own methods.
 *====================================================================
 */

namespace AxisRawDataReader_ns
{

//=================================================
//		Attributes Allowed Methods
//=================================================


//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReader::is_axisNumber_allowed
// 
// description : 	Read/Write allowed for axisNumber attribute.
//
//-----------------------------------------------------------------------------
bool AxisRawDataReader::is_axisNumber_allowed(Tango::AttReqType type)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReader::is_encoder_allowed
// 
// description : 	Read/Write allowed for encoder attribute.
//
//-----------------------------------------------------------------------------
bool AxisRawDataReader::is_encoder_allowed(Tango::AttReqType type)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReader::is_auxiliary_allowed
// 
// description : 	Read/Write allowed for auxiliary attribute.
//
//-----------------------------------------------------------------------------
bool AxisRawDataReader::is_auxiliary_allowed(Tango::AttReqType type)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReader::is_commandedPos_allowed
// 
// description : 	Read/Write allowed for commandedPos attribute.
//
//-----------------------------------------------------------------------------
bool AxisRawDataReader::is_commandedPos_allowed(Tango::AttReqType type)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReader::is_analogInput_allowed
// 
// description : 	Read/Write allowed for analogInput attribute.
//
//-----------------------------------------------------------------------------
bool AxisRawDataReader::is_analogInput_allowed(Tango::AttReqType type)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReader::is_axisLetter_allowed
// 
// description : 	Read/Write allowed for axisLetter attribute.
//
//-----------------------------------------------------------------------------
bool AxisRawDataReader::is_axisLetter_allowed(Tango::AttReqType type)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReader::is_homeInput_allowed
// 
// description : 	Read/Write allowed for homeInput attribute.
//
//-----------------------------------------------------------------------------
bool AxisRawDataReader::is_homeInput_allowed(Tango::AttReqType type)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReader::is_latchInput_allowed
// 
// description : 	Read/Write allowed for latchInput attribute.
//
//-----------------------------------------------------------------------------
bool AxisRawDataReader::is_latchInput_allowed(Tango::AttReqType type)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReader::is_latchOccured_allowed
// 
// description : 	Read/Write allowed for latchOccured attribute.
//
//-----------------------------------------------------------------------------
bool AxisRawDataReader::is_latchOccured_allowed(Tango::AttReqType type)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReader::is_latchIsArmed_allowed
// 
// description : 	Read/Write allowed for latchIsArmed attribute.
//
//-----------------------------------------------------------------------------
bool AxisRawDataReader::is_latchIsArmed_allowed(Tango::AttReqType type)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReader::is_velocity_allowed
// 
// description : 	Read/Write allowed for velocity attribute.
//
//-----------------------------------------------------------------------------
bool AxisRawDataReader::is_velocity_allowed(Tango::AttReqType type)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReader::is_positionError_allowed
// 
// description : 	Read/Write allowed for positionError attribute.
//
//-----------------------------------------------------------------------------
bool AxisRawDataReader::is_positionError_allowed(Tango::AttReqType type)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
	return true;
}

//=================================================
//		Commands Allowed Methods
//=================================================

//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReader::is_ReportLatchedPosition_allowed
// 
// description : 	Execution allowed for ReportLatchedPosition command.
//
//-----------------------------------------------------------------------------
bool AxisRawDataReader::is_ReportLatchedPosition_allowed(const CORBA::Any &any)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReader::is_ArmLatch_allowed
// 
// description : 	Execution allowed for ArmLatch command.
//
//-----------------------------------------------------------------------------
bool AxisRawDataReader::is_ArmLatch_allowed(const CORBA::Any &any)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
	return true;
}

}	// namespace AxisRawDataReader_ns