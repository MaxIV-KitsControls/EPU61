//=============================================================================
//
// file :         GalilCoupledAxesClass.h
//
// description :  Include for the GalilCoupledAxesClass root class.
//                This class is represents the singleton class for
//                the GalilCoupledAxes device class.
//                It contains all properties and methods which the 
//                GalilCoupledAxes requires only once e.g. the commands.
//			
// project :      TANGO Device Server
//
// $Author: buteau $
//
// $Revision: 1.3 $
//
// $Log: GalilCoupledAxesClass.h,v $
// Revision 1.3  2011/07/15 09:47:43  buteau
// MANTIS 19368
//
// Revision 1.2  2011/07/15 09:40:53  buteau
// MANTIS 19368
//
// Revision 1.1  2007/11/16 09:39:31  leclercq
// no message
//
// Revision 1.1  2007/11/15 16:15:22  coquet
// added Coupled Axes support
// handeles internally coupled axes, device controls only the master position
// example : the detector table of swing with 2 synchronised motors
// the coupling is provided in the micro-code in the controlbox
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

#ifndef _GalilCoupledAxesCLASS_H
#define _GalilCoupledAxesCLASS_H

#include <tango.h>
#include <GalilCoupledAxes.h>


namespace GalilCoupledAxes_ns
{//=====================================
//	Define classes for attributes
//=====================================
class backwardLimitSwitchAttrib: public Tango::Attr
{
public:
	backwardLimitSwitchAttrib():Attr("backwardLimitSwitch", Tango::DEV_BOOLEAN, Tango::READ) {};
	~backwardLimitSwitchAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<GalilCoupledAxes *>(dev))->read_backwardLimitSwitch(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<GalilCoupledAxes *>(dev))->is_backwardLimitSwitch_allowed(ty);}
};

class forwardLimitSwitchAttrib: public Tango::Attr
{
public:
	forwardLimitSwitchAttrib():Attr("forwardLimitSwitch", Tango::DEV_BOOLEAN, Tango::READ) {};
	~forwardLimitSwitchAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<GalilCoupledAxes *>(dev))->read_forwardLimitSwitch(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<GalilCoupledAxes *>(dev))->is_forwardLimitSwitch_allowed(ty);}
};

class positionAttrib: public Tango::Attr
{
public:
	positionAttrib():Attr("position", Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~positionAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<GalilCoupledAxes *>(dev))->read_position(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<GalilCoupledAxes *>(dev))->write_position(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<GalilCoupledAxes *>(dev))->is_position_allowed(ty);}
};

//=========================================
//	Define classes for commands
//=========================================
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
	{return (static_cast<GalilCoupledAxes *>(dev))->is_Backward_allowed(any);}
};



class ForwardClass : public Tango::Command
{
public:
	ForwardClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	ForwardClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~ForwardClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<GalilCoupledAxes *>(dev))->is_Forward_allowed(any);}
};



class StopClass : public Tango::Command
{
public:
	StopClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	StopClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~StopClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<GalilCoupledAxes *>(dev))->is_Stop_allowed(any);}
};



//
// The GalilCoupledAxesClass singleton definition
//

class GalilCoupledAxesClass : public Tango::DeviceClass
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
	static GalilCoupledAxesClass *init(const char *);
	static GalilCoupledAxesClass *instance();
	~GalilCoupledAxesClass();
	Tango::DbDatum	get_class_property(string &);
	Tango::DbDatum	get_default_device_property(string &);
	Tango::DbDatum	get_default_class_property(string &);
	
protected:
	GalilCoupledAxesClass(string &);
	static GalilCoupledAxesClass *_instance;
	void command_factory();
	void get_class_property();
	void attribute_factory(vector<Tango::Attr *> &);
	void write_class_property();
	void set_default_property();

private:
	void device_factory(const Tango::DevVarStringArray *);
};


}	//	namespace GalilCoupledAxes_ns

#endif // _GalilCoupledAxesCLASS_H
