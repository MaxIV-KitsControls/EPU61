//=============================================================================
//
// file :         GalilSlitClass.h
//
// description :  Include for the GalilSlitClass root class.
//                This class is represents the singleton class for
//                the GalilSlit device class.
//                It contains all properties and methods which the 
//                GalilSlit requires only once e.g. the commands.
//			
// project :      TANGO Device Server
//
// $Author: buteau $
//
// $Revision: 1.7 $
//
// $Log: GalilSlitClass.h,v $
// Revision 1.7  2011/07/15 09:47:43  buteau
// MANTIS 19368
//
// Revision 1.6  2011/07/15 09:40:53  buteau
// MANTIS 19368
//
// Revision 1.5  2010/02/09 12:59:27  coquet
// GalilSlit
// added commands ComputeGapOffset ComputePositionOffset
//
// Revision 1.4  2010/01/13 09:54:31  coquet
// added commands On Off to GalilSlit
//
// Revision 1.3  2007/10/12 14:24:25  coquet
// attributes positionOffset and gapOffset are now memorized / write on HW at init
// tested under W32
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

#ifndef _GALILSLITCLASS_H
#define _GALILSLITCLASS_H

#include <tango.h>
#include <GalilSlit.h>


namespace GalilSlit_ns
{//=====================================
//	Define classes for attributes
//=====================================
class gapOffsetAttrib: public Tango::Attr
{
public:
	gapOffsetAttrib():Attr("gapOffset", Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~gapOffsetAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<GalilSlit *>(dev))->read_gapOffset(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<GalilSlit *>(dev))->write_gapOffset(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<GalilSlit *>(dev))->is_gapOffset_allowed(ty);}
};

class positionOffsetAttrib: public Tango::Attr
{
public:
	positionOffsetAttrib():Attr("positionOffset", Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~positionOffsetAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<GalilSlit *>(dev))->read_positionOffset(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<GalilSlit *>(dev))->write_positionOffset(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<GalilSlit *>(dev))->is_positionOffset_allowed(ty);}
};

class outsideDownPositionAttrib: public Tango::Attr
{
public:
	outsideDownPositionAttrib():Attr("outsideDownPosition", Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~outsideDownPositionAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<GalilSlit *>(dev))->read_outsideDownPosition(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<GalilSlit *>(dev))->write_outsideDownPosition(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<GalilSlit *>(dev))->is_outsideDownPosition_allowed(ty);}
};

class insideUpPositionAttrib: public Tango::Attr
{
public:
	insideUpPositionAttrib():Attr("insideUpPosition", Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~insideUpPositionAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<GalilSlit *>(dev))->read_insideUpPosition(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<GalilSlit *>(dev))->write_insideUpPosition(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<GalilSlit *>(dev))->is_insideUpPosition_allowed(ty);}
};

class positionAttrib: public Tango::Attr
{
public:
	positionAttrib():Attr("position", Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~positionAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<GalilSlit *>(dev))->read_position(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<GalilSlit *>(dev))->write_position(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<GalilSlit *>(dev))->is_position_allowed(ty);}
};

class gapAttrib: public Tango::Attr
{
public:
	gapAttrib():Attr("gap", Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~gapAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<GalilSlit *>(dev))->read_gap(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
	{(static_cast<GalilSlit *>(dev))->write_gap(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<GalilSlit *>(dev))->is_gap_allowed(ty);}
};

//=========================================
//	Define classes for commands
//=========================================
class ComputeNewPositionOffsetCmd : public Tango::Command
{
public:
	ComputeNewPositionOffsetCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	ComputeNewPositionOffsetCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~ComputeNewPositionOffsetCmd() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<GalilSlit *>(dev))->is_ComputeNewPositionOffset_allowed(any);}
};



class ComputeNewGapOffsetClass : public Tango::Command
{
public:
	ComputeNewGapOffsetClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	ComputeNewGapOffsetClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~ComputeNewGapOffsetClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<GalilSlit *>(dev))->is_ComputeNewGapOffset_allowed(any);}
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
	{return (static_cast<GalilSlit *>(dev))->is_On_allowed(any);}
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
	{return (static_cast<GalilSlit *>(dev))->is_Off_allowed(any);}
};



class SetIndependantModeCmd : public Tango::Command
{
public:
	SetIndependantModeCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	SetIndependantModeCmd(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~SetIndependantModeCmd() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<GalilSlit *>(dev))->is_SetIndependantMode_allowed(any);}
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
	{return (static_cast<GalilSlit *>(dev))->is_Stop_allowed(any);}
};



//
// The GalilSlitClass singleton definition
//

class GalilSlitClass : public Tango::DeviceClass
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
	static GalilSlitClass *init(const char *);
	static GalilSlitClass *instance();
	~GalilSlitClass();
	Tango::DbDatum	get_class_property(string &);
	Tango::DbDatum	get_default_device_property(string &);
	Tango::DbDatum	get_default_class_property(string &);
	
protected:
	GalilSlitClass(string &);
	static GalilSlitClass *_instance;
	void command_factory();
	void get_class_property();
	void attribute_factory(vector<Tango::Attr *> &);
	void write_class_property();
	void set_default_property();

private:
	void device_factory(const Tango::DevVarStringArray *);
};


}	//	namespace GalilSlit_ns

#endif // _GALILSLITCLASS_H