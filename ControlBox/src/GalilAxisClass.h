//=============================================================================
//
// file :         GalilAxisClass.h
//
// description :  Include for the GalilAxisClass root class.
//                This class is represents the singleton class for
//                the GalilAxis device class.
//                It contains all properties and methods which the 
//                GalilAxis requires only once e.g. the commands.
//			
// project :      TANGO Device Server
//
// $Author: buteau $
//
// $Revision: 1.10 $
//
// $Log: GalilAxisClass.h,v $
// Revision 1.10  2011/07/15 09:47:42  buteau
// MANTIS 19368
//
// Revision 1.8  2010/06/30 10:10:35  leclercq
// "pos. locking" impl.
//
//
// copyleft :     European Synchrotron Radiation Facility
//                BP 220, Grenoble 38043
//                FRANCE
//
//=============================================================================
//
//  		This file is generated by POGO
//	(Program Obviously used to Generate tango Object)
//
//         (c) - Software Engineering Group - ESRF
//=============================================================================

#ifndef _GALILAXISCLASS_H
#define _GALILAXISCLASS_H

#include <tango.h>
#include <GalilAxis.h>


namespace GalilAxis_ns
{
//=====================================
//	Define classes for attributes
//=====================================
class lockedPositionHistoryAttrib: public Tango::ImageAttr
{
public:
	lockedPositionHistoryAttrib():ImageAttr("lockedPositionHistory", Tango::DEV_DOUBLE, Tango::READ, 128, 1000000) {};
	~lockedPositionHistoryAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<GalilAxis *>(dev))->read_lockedPositionHistory(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<GalilAxis *>(dev))->is_lockedPositionHistory_allowed(ty);}
};

class velocityAttrib: public Tango::Attr
{
public:
	velocityAttrib():Attr("velocity", Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~velocityAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<GalilAxis *>(dev))->read_velocity(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<GalilAxis *>(dev))->write_velocity(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<GalilAxis *>(dev))->is_velocity_allowed(ty);}
};

class positionLockedAttrib: public Tango::Attr
{
public:
	positionLockedAttrib():Attr("positionLocked", Tango::DEV_BOOLEAN, Tango::READ_WRITE) {};
	~positionLockedAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<GalilAxis *>(dev))->read_positionLocked(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<GalilAxis *>(dev))->write_positionLocked(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<GalilAxis *>(dev))->is_positionLocked_allowed(ty);}
};

class positionAttrib: public Tango::Attr
{
public:
	positionAttrib():Attr("position", Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~positionAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<GalilAxis *>(dev))->read_position(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<GalilAxis *>(dev))->write_position(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<GalilAxis *>(dev))->is_position_allowed(ty);}
};

class offsetAttrib: public Tango::Attr
{
public:
	offsetAttrib():Attr("offset", Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~offsetAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<GalilAxis *>(dev))->read_offset(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<GalilAxis *>(dev))->write_offset(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<GalilAxis *>(dev))->is_offset_allowed(ty);}
};

class forwardLimitSwitchAttrib: public Tango::Attr
{
public:
	forwardLimitSwitchAttrib():Attr("forwardLimitSwitch", Tango::DEV_BOOLEAN, Tango::READ) {};
	~forwardLimitSwitchAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<GalilAxis *>(dev))->read_forwardLimitSwitch(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<GalilAxis *>(dev))->is_forwardLimitSwitch_allowed(ty);}
};

class decelerationAttrib: public Tango::Attr
{
public:
	decelerationAttrib():Attr("deceleration", Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~decelerationAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<GalilAxis *>(dev))->read_deceleration(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<GalilAxis *>(dev))->write_deceleration(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<GalilAxis *>(dev))->is_deceleration_allowed(ty);}
};

class backwardLimitSwitchAttrib: public Tango::Attr
{
public:
	backwardLimitSwitchAttrib():Attr("backwardLimitSwitch", Tango::DEV_BOOLEAN, Tango::READ) {};
	~backwardLimitSwitchAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<GalilAxis *>(dev))->read_backwardLimitSwitch(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<GalilAxis *>(dev))->is_backwardLimitSwitch_allowed(ty);}
};

class backlashAttrib: public Tango::Attr
{
public:
	backlashAttrib():Attr("backlash", Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~backlashAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<GalilAxis *>(dev))->read_backlash(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<GalilAxis *>(dev))->write_backlash(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<GalilAxis *>(dev))->is_backlash_allowed(ty);}
};

class accuracyAttrib: public Tango::Attr
{
public:
	accuracyAttrib():Attr("accuracy", Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~accuracyAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<GalilAxis *>(dev))->read_accuracy(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<GalilAxis *>(dev))->write_accuracy(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<GalilAxis *>(dev))->is_accuracy_allowed(ty);}
};

class accelerationAttrib: public Tango::Attr
{
public:
	accelerationAttrib():Attr("acceleration", Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~accelerationAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<GalilAxis *>(dev))->read_acceleration(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<GalilAxis *>(dev))->write_acceleration(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<GalilAxis *>(dev))->is_acceleration_allowed(ty);}
};

//=========================================
//	Define classes for commands
//=========================================
class GetDetailOfStateCmd : public Tango::Command
{
public:
	GetDetailOfStateCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	GetDetailOfStateCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~GetDetailOfStateCmd() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<GalilAxis *>(dev))->is_GetDetailOfState_allowed(any);}
};



class OffClass : public Tango::Command
{
public:
	OffClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	OffClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~OffClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<GalilAxis *>(dev))->is_Off_allowed(any);}
};



class OnClass : public Tango::Command
{
public:
	OnClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	OnClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~OnClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<GalilAxis *>(dev))->is_On_allowed(any);}
};



class ComputeNewOffsetClass : public Tango::Command
{
public:
	ComputeNewOffsetClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	ComputeNewOffsetClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~ComputeNewOffsetClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<GalilAxis *>(dev))->is_ComputeNewOffset_allowed(any);}
};



class DefinePositionCmd : public Tango::Command
{
public:
	DefinePositionCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	DefinePositionCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~DefinePositionCmd() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<GalilAxis *>(dev))->is_DefinePosition_allowed(any);}
};



class MotorONCmd : public Tango::Command
{
public:
	MotorONCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	MotorONCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~MotorONCmd() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<GalilAxis *>(dev))->is_MotorON_allowed(any);}
};



class MotorOFFCmd : public Tango::Command
{
public:
	MotorOFFCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	MotorOFFCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~MotorOFFCmd() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<GalilAxis *>(dev))->is_MotorOFF_allowed(any);}
};



class InitializeReferencePositionCmd : public Tango::Command
{
public:
	InitializeReferencePositionCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	InitializeReferencePositionCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~InitializeReferencePositionCmd() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<GalilAxis *>(dev))->is_InitializeReferencePosition_allowed(any);}
};



class BackwardCmd : public Tango::Command
{
public:
	BackwardCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	BackwardCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~BackwardCmd() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<GalilAxis *>(dev))->is_Backward_allowed(any);}
};



class StopCmd : public Tango::Command
{
public:
	StopCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	StopCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~StopCmd() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<GalilAxis *>(dev))->is_Stop_allowed(any);}
};



class ForwardCmd : public Tango::Command
{
public:
	ForwardCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	ForwardCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~ForwardCmd() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<GalilAxis *>(dev))->is_Forward_allowed(any);}
};



//
// The GalilAxisClass singleton definition
//

class
#ifdef WIN32
	__declspec(dllexport)
#endif
	GalilAxisClass : public Tango::DeviceClass
{
public:
//	properties member data

//	add your own data members here
//------------------------------------

public:
	Tango::DbData	cl_prop;
	Tango::DbData	cl_def_prop;
	Tango::DbData	dev_def_prop;

//	Method prototypes
	static GalilAxisClass *init(const char *);
	static GalilAxisClass *instance();
	~GalilAxisClass();
	Tango::DbDatum	get_class_property(string &);
	Tango::DbDatum	get_default_device_property(string &);
	Tango::DbDatum	get_default_class_property(string &);
	
protected:
	GalilAxisClass(string &);
	static GalilAxisClass *_instance;
	void command_factory();
	void get_class_property();
	void attribute_factory(vector<Tango::Attr *> &);
	void write_class_property();
	void set_default_property();
	string get_cvstag();
	string get_cvsroot();

private:
	void device_factory(const Tango::DevVarStringArray *);
};


}	//	namespace GalilAxis_ns

#endif // _GALILAXISCLASS_H
