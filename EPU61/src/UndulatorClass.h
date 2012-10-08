/*----- PROTECTED REGION ID(UndulatorClass.h) ENABLED START -----*/
//=============================================================================
//
// file :        UndulatorClass.h
//
// description : Include for the UndulatorClass root class.
//               This class is the singleton class for.
//               the Undulator device class..
//               It contains all properties and methods which the .
//               Undulator requires only once e.g. the commands.
//
// project :     .
//
// $Author:  $
//
// $Revision:  $
// $Date:  $
//
// SVN only:
// $HeadURL:  $
//
// CVS only:
// $Source:  $
// $Log:  $
//
//=============================================================================
//                This file is generated by POGO
//        (Program Obviously used to Generate tango Object)
//=============================================================================


#ifndef UNDULATORCLASS_H
#define UNDULATORCLASS_H

#include <tango.h>
#include <Undulator.h>

/*----- PROTECTED REGION END -----*/

namespace Undulator_ns
{
	/*----- PROTECTED REGION ID(Undulator::classes for dynamic creation) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::classes for dynamic creation



//=========================================
//	Define classes for attributes
//=========================================
//	Attribute GapReadback class definition
class GapReadbackAttrib: public Tango::Attr
{
public:
	GapReadbackAttrib():Attr("GapReadback",
	                   Tango::DEV_DOUBLE, Tango::READ) {};
	~GapReadbackAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_GapReadback(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_GapReadback_allowed(ty);}
};

//	Attribute GapSetpoint class definition
class GapSetpointAttrib: public Tango::Attr
{
public:
	GapSetpointAttrib():Attr("GapSetpoint",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~GapSetpointAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_GapSetpoint(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_GapSetpoint(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_GapSetpoint_allowed(ty);}
};

//	Attribute EngineeringLock class definition
class EngineeringLockAttrib: public Tango::Attr
{
public:
	EngineeringLockAttrib():Attr("EngineeringLock",
	                   Tango::DEV_BOOLEAN, Tango::READ_WRITE) {};
	~EngineeringLockAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_EngineeringLock(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_EngineeringLock(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_EngineeringLock_allowed(ty);}
};

//	Attribute GapAcceleration class definition
class GapAccelerationAttrib: public Tango::Attr
{
public:
	GapAccelerationAttrib():Attr("GapAcceleration",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~GapAccelerationAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_GapAcceleration(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_GapAcceleration(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_GapAcceleration_allowed(ty);}
};

//	Attribute GapSpeed class definition
class GapSpeedAttrib: public Tango::Attr
{
public:
	GapSpeedAttrib():Attr("GapSpeed",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~GapSpeedAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_GapSpeed(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_GapSpeed(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_GapSpeed_allowed(ty);}
};

//	Attribute OffsetReadback class definition
class OffsetReadbackAttrib: public Tango::Attr
{
public:
	OffsetReadbackAttrib():Attr("OffsetReadback",
	                   Tango::DEV_DOUBLE, Tango::READ) {};
	~OffsetReadbackAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_OffsetReadback(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_OffsetReadback_allowed(ty);}
};

//	Attribute OffsetSetpoint class definition
class OffsetSetpointAttrib: public Tango::Attr
{
public:
	OffsetSetpointAttrib():Attr("OffsetSetpoint",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~OffsetSetpointAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_OffsetSetpoint(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_OffsetSetpoint(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_OffsetSetpoint_allowed(ty);}
};

//	Attribute PhaseAcceleration class definition
class PhaseAccelerationAttrib: public Tango::Attr
{
public:
	PhaseAccelerationAttrib():Attr("PhaseAcceleration",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~PhaseAccelerationAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_PhaseAcceleration(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_PhaseAcceleration(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_PhaseAcceleration_allowed(ty);}
};

//	Attribute PhaseModeReadback class definition
class PhaseModeReadbackAttrib: public Tango::Attr
{
public:
	PhaseModeReadbackAttrib():Attr("PhaseModeReadback",
	                   Tango::DEV_SHORT, Tango::READ) {};
	~PhaseModeReadbackAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_PhaseModeReadback(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_PhaseModeReadback_allowed(ty);}
};

//	Attribute PhaseModeSetpoint class definition
class PhaseModeSetpointAttrib: public Tango::Attr
{
public:
	PhaseModeSetpointAttrib():Attr("PhaseModeSetpoint",
	                   Tango::DEV_SHORT, Tango::READ_WRITE) {};
	~PhaseModeSetpointAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_PhaseModeSetpoint(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_PhaseModeSetpoint(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_PhaseModeSetpoint_allowed(ty);}
};

//	Attribute PhaseReadback class definition
class PhaseReadbackAttrib: public Tango::Attr
{
public:
	PhaseReadbackAttrib():Attr("PhaseReadback",
	                   Tango::DEV_DOUBLE, Tango::READ) {};
	~PhaseReadbackAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_PhaseReadback(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_PhaseReadback_allowed(ty);}
};

//	Attribute PhaseSetpoint class definition
class PhaseSetpointAttrib: public Tango::Attr
{
public:
	PhaseSetpointAttrib():Attr("PhaseSetpoint",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~PhaseSetpointAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_PhaseSetpoint(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_PhaseSetpoint(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_PhaseSetpoint_allowed(ty);}
};

//	Attribute PhaseSpeed class definition
class PhaseSpeedAttrib: public Tango::Attr
{
public:
	PhaseSpeedAttrib():Attr("PhaseSpeed",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~PhaseSpeedAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_PhaseSpeed(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_PhaseSpeed(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_PhaseSpeed_allowed(ty);}
};

//	Attribute TaperAcceleration class definition
class TaperAccelerationAttrib: public Tango::Attr
{
public:
	TaperAccelerationAttrib():Attr("TaperAcceleration",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~TaperAccelerationAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_TaperAcceleration(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_TaperAcceleration(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_TaperAcceleration_allowed(ty);}
};

//	Attribute TaperReadback class definition
class TaperReadbackAttrib: public Tango::Attr
{
public:
	TaperReadbackAttrib():Attr("TaperReadback",
	                   Tango::DEV_DOUBLE, Tango::READ) {};
	~TaperReadbackAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_TaperReadback(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_TaperReadback_allowed(ty);}
};

//	Attribute TaperSetpoint class definition
class TaperSetpointAttrib: public Tango::Attr
{
public:
	TaperSetpointAttrib():Attr("TaperSetpoint",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~TaperSetpointAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_TaperSetpoint(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_TaperSetpoint(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_TaperSetpoint_allowed(ty);}
};

//	Attribute TaperSpeed class definition
class TaperSpeedAttrib: public Tango::Attr
{
public:
	TaperSpeedAttrib():Attr("TaperSpeed",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~TaperSpeedAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_TaperSpeed(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_TaperSpeed(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_TaperSpeed_allowed(ty);}
};

//	Attribute LimitsDisabled class definition
class LimitsDisabledAttrib: public Tango::Attr
{
public:
	LimitsDisabledAttrib():Attr("LimitsDisabled",
	                   Tango::DEV_BOOLEAN, Tango::READ) {};
	~LimitsDisabledAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_LimitsDisabled(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_LimitsDisabled_allowed(ty);}
};

//	Attribute StopAll class definition
class StopAllAttrib: public Tango::Attr
{
public:
	StopAllAttrib():Attr("StopAll",
	                   Tango::DEV_BOOLEAN, Tango::READ) {};
	~StopAllAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_StopAll(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_StopAll_allowed(ty);}
};

//	Attribute GapMoving class definition
class GapMovingAttrib: public Tango::Attr
{
public:
	GapMovingAttrib():Attr("GapMoving",
	                   Tango::DEV_BOOLEAN, Tango::READ) {};
	~GapMovingAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_GapMoving(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_GapMoving_allowed(ty);}
};

//	Attribute PhaseMoving class definition
class PhaseMovingAttrib: public Tango::Attr
{
public:
	PhaseMovingAttrib():Attr("PhaseMoving",
	                   Tango::DEV_BOOLEAN, Tango::READ) {};
	~PhaseMovingAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_PhaseMoving(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_PhaseMoving_allowed(ty);}
};

//	Attribute EngineeringGapSpeed class definition
class EngineeringGapSpeedAttrib: public Tango::Attr
{
public:
	EngineeringGapSpeedAttrib():Attr("EngineeringGapSpeed",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~EngineeringGapSpeedAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_EngineeringGapSpeed(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_EngineeringGapSpeed(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_EngineeringGapSpeed_allowed(ty);}
};

//	Attribute EngineeringPhaseSpeed class definition
class EngineeringPhaseSpeedAttrib: public Tango::Attr
{
public:
	EngineeringPhaseSpeedAttrib():Attr("EngineeringPhaseSpeed",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~EngineeringPhaseSpeedAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_EngineeringPhaseSpeed(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_EngineeringPhaseSpeed(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_EngineeringPhaseSpeed_allowed(ty);}
};

//	Attribute EngineeringGapAcceleration class definition
class EngineeringGapAccelerationAttrib: public Tango::Attr
{
public:
	EngineeringGapAccelerationAttrib():Attr("EngineeringGapAcceleration",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~EngineeringGapAccelerationAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_EngineeringGapAcceleration(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_EngineeringGapAcceleration(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_EngineeringGapAcceleration_allowed(ty);}
};

//	Attribute EngineeringPhaseAcceleration class definition
class EngineeringPhaseAccelerationAttrib: public Tango::Attr
{
public:
	EngineeringPhaseAccelerationAttrib():Attr("EngineeringPhaseAcceleration",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~EngineeringPhaseAccelerationAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_EngineeringPhaseAcceleration(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_EngineeringPhaseAcceleration(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_EngineeringPhaseAcceleration_allowed(ty);}
};

//	Attribute Interlock class definition
class InterlockAttrib: public Tango::Attr
{
public:
	InterlockAttrib():Attr("Interlock",
	                   Tango::DEV_BOOLEAN, Tango::READ) {};
	~InterlockAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_Interlock(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_Interlock_allowed(ty);}
};

//	Attribute Coil1 class definition
class Coil1Attrib: public Tango::Attr
{
public:
	Coil1Attrib():Attr("Coil1",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~Coil1Attrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_Coil1(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_Coil1(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_Coil1_allowed(ty);}
};

//	Attribute Coil2 class definition
class Coil2Attrib: public Tango::Attr
{
public:
	Coil2Attrib():Attr("Coil2",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~Coil2Attrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_Coil2(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_Coil2(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_Coil2_allowed(ty);}
};

//	Attribute Coil3 class definition
class Coil3Attrib: public Tango::Attr
{
public:
	Coil3Attrib():Attr("Coil3",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~Coil3Attrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_Coil3(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_Coil3(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_Coil3_allowed(ty);}
};

//	Attribute Coil4 class definition
class Coil4Attrib: public Tango::Attr
{
public:
	Coil4Attrib():Attr("Coil4",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~Coil4Attrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_Coil4(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_Coil4(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_Coil4_allowed(ty);}
};

//	Attribute CorrectionEnabled class definition
class CorrectionEnabledAttrib: public Tango::Attr
{
public:
	CorrectionEnabledAttrib():Attr("CorrectionEnabled",
	                   Tango::DEV_BOOLEAN, Tango::READ_WRITE) {};
	~CorrectionEnabledAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<Undulator *>(dev))->read_CorrectionEnabled(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<Undulator *>(dev))->write_CorrectionEnabled(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<Undulator *>(dev))->is_CorrectionEnabled_allowed(ty);}
};





//=========================================
//	Define classes for commands
//=========================================
//	Command StopGap class definition
class StopGapClass : public Tango::Command
{
public:
	StopGapClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	StopGapClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~StopGapClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<Undulator *>(dev))->is_StopGap_allowed(any);}
};

//	Command ToggleStopAll class definition
class ToggleStopAllClass : public Tango::Command
{
public:
	ToggleStopAllClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	ToggleStopAllClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~ToggleStopAllClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<Undulator *>(dev))->is_ToggleStopAll_allowed(any);}
};

//	Command DisableLimits class definition
class DisableLimitsClass : public Tango::Command
{
public:
	DisableLimitsClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	DisableLimitsClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~DisableLimitsClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<Undulator *>(dev))->is_DisableLimits_allowed(any);}
};

//	Command StopPhase class definition
class StopPhaseClass : public Tango::Command
{
public:
	StopPhaseClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	StopPhaseClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~StopPhaseClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<Undulator *>(dev))->is_StopPhase_allowed(any);}
};

//	Command CalibrateGap class definition
class CalibrateGapClass : public Tango::Command
{
public:
	CalibrateGapClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	CalibrateGapClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~CalibrateGapClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<Undulator *>(dev))->is_CalibrateGap_allowed(any);}
};

//	Command CalibratePhase class definition
class CalibratePhaseClass : public Tango::Command
{
public:
	CalibratePhaseClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	CalibratePhaseClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~CalibratePhaseClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<Undulator *>(dev))->is_CalibratePhase_allowed(any);}
};

//	Command LoadCorrectionData class definition
class LoadCorrectionDataClass : public Tango::Command
{
public:
	LoadCorrectionDataClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	LoadCorrectionDataClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~LoadCorrectionDataClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<Undulator *>(dev))->is_LoadCorrectionData_allowed(any);}
};





/**
 *	The TemplateDevServClass singleton definition
 */

class
#ifdef _TG_WINDOWS_
	__declspec(dllexport)
#endif
	UndulatorClass : public Tango::DeviceClass
{
	/*----- PROTECTED REGION ID(Undulator::Additionnal DServer data members) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::Additionnal DServer data members



public:
//	write class properties data members
	Tango::DbData	cl_prop;
	Tango::DbData	cl_def_prop;
	Tango::DbData	dev_def_prop;

//	Method prototypes
	static UndulatorClass *init(const char *);
	static UndulatorClass *instance();
	~UndulatorClass();
	Tango::DbDatum	get_class_property(string &);
	Tango::DbDatum	get_default_device_property(string &);
	Tango::DbDatum	get_default_class_property(string &);
	
protected:
	UndulatorClass(string &);
	static UndulatorClass *_instance;
	void command_factory();
	void attribute_factory(vector<Tango::Attr *> &);
	void write_class_property();
	void set_default_property();
	void get_class_property();
	string get_cvstag();
	string get_cvsroot();

private:
	void device_factory(const Tango::DevVarStringArray *);
	void create_static_attribute_list(vector<Tango::Attr *> &);
	void erase_dynamic_attributes(const Tango::DevVarStringArray *,vector<Tango::Attr *> &);
	vector<string>	defaultAttList;


};

}	//	namespace

#endif	//	UNDULATORCLASS_H

