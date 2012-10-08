/*----- PROTECTED REGION ID(CoilCorrectionClass.h) ENABLED START -----*/
//=============================================================================
//
// file :        CoilCorrectionClass.h
//
// description : Include for the CoilCorrectionClass root class.
//               This class is the singleton class for.
//               the CoilCorrection device class..
//               It contains all properties and methods which the .
//               CoilCorrection requires only once e.g. the commands.
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


#ifndef COILCORRECTIONCLASS_H
#define COILCORRECTIONCLASS_H

#include <tango.h>
#include <CoilCorrection.h>

/*----- PROTECTED REGION END -----*/

namespace CoilCorrection_ns
{
	/*----- PROTECTED REGION ID(CoilCorrection::classes for dynamic creation) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	CoilCorrection::classes for dynamic creation



//=========================================
//	Define classes for attributes
//=========================================
//	Attribute Coil1Readback class definition
class Coil1ReadbackAttrib: public Tango::Attr
{
public:
	Coil1ReadbackAttrib():Attr("Coil1Readback",
	                   Tango::DEV_DOUBLE, Tango::READ) {};
	~Coil1ReadbackAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<CoilCorrection *>(dev))->read_Coil1Readback(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<CoilCorrection *>(dev))->is_Coil1Readback_allowed(ty);}
};

//	Attribute Coil2Readback class definition
class Coil2ReadbackAttrib: public Tango::Attr
{
public:
	Coil2ReadbackAttrib():Attr("Coil2Readback",
	                   Tango::DEV_DOUBLE, Tango::READ) {};
	~Coil2ReadbackAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<CoilCorrection *>(dev))->read_Coil2Readback(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<CoilCorrection *>(dev))->is_Coil2Readback_allowed(ty);}
};

//	Attribute Coil3Readback class definition
class Coil3ReadbackAttrib: public Tango::Attr
{
public:
	Coil3ReadbackAttrib():Attr("Coil3Readback",
	                   Tango::DEV_DOUBLE, Tango::READ) {};
	~Coil3ReadbackAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<CoilCorrection *>(dev))->read_Coil3Readback(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<CoilCorrection *>(dev))->is_Coil3Readback_allowed(ty);}
};

//	Attribute Coil4Readback class definition
class Coil4ReadbackAttrib: public Tango::Attr
{
public:
	Coil4ReadbackAttrib():Attr("Coil4Readback",
	                   Tango::DEV_DOUBLE, Tango::READ) {};
	~Coil4ReadbackAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<CoilCorrection *>(dev))->read_Coil4Readback(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<CoilCorrection *>(dev))->is_Coil4Readback_allowed(ty);}
};

//	Attribute Coil1Setpoint class definition
class Coil1SetpointAttrib: public Tango::Attr
{
public:
	Coil1SetpointAttrib():Attr("Coil1Setpoint",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~Coil1SetpointAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<CoilCorrection *>(dev))->read_Coil1Setpoint(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<CoilCorrection *>(dev))->write_Coil1Setpoint(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<CoilCorrection *>(dev))->is_Coil1Setpoint_allowed(ty);}
};

//	Attribute Coil2Setpoint class definition
class Coil2SetpointAttrib: public Tango::Attr
{
public:
	Coil2SetpointAttrib():Attr("Coil2Setpoint",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~Coil2SetpointAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<CoilCorrection *>(dev))->read_Coil2Setpoint(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<CoilCorrection *>(dev))->write_Coil2Setpoint(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<CoilCorrection *>(dev))->is_Coil2Setpoint_allowed(ty);}
};

//	Attribute Coil3Setpoint class definition
class Coil3SetpointAttrib: public Tango::Attr
{
public:
	Coil3SetpointAttrib():Attr("Coil3Setpoint",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~Coil3SetpointAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<CoilCorrection *>(dev))->read_Coil3Setpoint(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<CoilCorrection *>(dev))->write_Coil3Setpoint(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<CoilCorrection *>(dev))->is_Coil3Setpoint_allowed(ty);}
};

//	Attribute Coil4Setpoint class definition
class Coil4SetpointAttrib: public Tango::Attr
{
public:
	Coil4SetpointAttrib():Attr("Coil4Setpoint",
	                   Tango::DEV_DOUBLE, Tango::READ_WRITE) {};
	~Coil4SetpointAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
		{(static_cast<CoilCorrection *>(dev))->read_Coil4Setpoint(att);}
	virtual void write(Tango::DeviceImpl *dev,Tango::WAttribute &att)
		{(static_cast<CoilCorrection *>(dev))->write_Coil4Setpoint(att);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
		{return (static_cast<CoilCorrection *>(dev))->is_Coil4Setpoint_allowed(ty);}
};





//=========================================
//	Define classes for commands
//=========================================
//	Command On class definition
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
	{return (static_cast<CoilCorrection *>(dev))->is_On_allowed(any);}
};

//	Command Off class definition
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
	{return (static_cast<CoilCorrection *>(dev))->is_Off_allowed(any);}
};

//	Command Load class definition
class LoadClass : public Tango::Command
{
public:
	LoadClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	LoadClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~LoadClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<CoilCorrection *>(dev))->is_Load_allowed(any);}
};

//	Command Run class definition
class RunClass : public Tango::Command
{
public:
	RunClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const char        *in_desc,
				   const char        *out_desc,
				   Tango::DispLevel  level)
	:Command(name,in,out,in_desc,out_desc, level)	{};

	RunClass(const char   *name,
	               Tango::CmdArgType in,
				   Tango::CmdArgType out)
	:Command(name,in,out)	{};
	~RunClass() {};
	
	virtual CORBA::Any *execute (Tango::DeviceImpl *dev, const CORBA::Any &any);
	virtual bool is_allowed (Tango::DeviceImpl *dev, const CORBA::Any &any)
	{return (static_cast<CoilCorrection *>(dev))->is_Run_allowed(any);}
};





/**
 *	The TemplateDevServClass singleton definition
 */

class
#ifdef _TG_WINDOWS_
	__declspec(dllexport)
#endif
	CoilCorrectionClass : public Tango::DeviceClass
{
	/*----- PROTECTED REGION ID(CoilCorrection::Additionnal DServer data members) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	CoilCorrection::Additionnal DServer data members



public:
//	write class properties data members
	Tango::DbData	cl_prop;
	Tango::DbData	cl_def_prop;
	Tango::DbData	dev_def_prop;

//	Method prototypes
	static CoilCorrectionClass *init(const char *);
	static CoilCorrectionClass *instance();
	~CoilCorrectionClass();
	Tango::DbDatum	get_class_property(string &);
	Tango::DbDatum	get_default_device_property(string &);
	Tango::DbDatum	get_default_class_property(string &);
	
protected:
	CoilCorrectionClass(string &);
	static CoilCorrectionClass *_instance;
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

#endif	//	COILCORRECTIONCLASS_H

