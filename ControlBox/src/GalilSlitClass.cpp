static const char *RcsId     = "$Header: /usr/local/CVS/DeviceServer/Generic/Motorisation/ControlBoxV2/src/GalilSlitClass.cpp,v 1.12 2011/07/15 09:47:43 buteau Exp $";
static const char *TagName   = "$Name: release_2_5_4 $";
static const char *HttpServer= "http://www.esrf.fr/computing/cs/tango/tango_doc/ds_doc/";
//+=============================================================================
//
// file :        GalilSlitClass.cpp
//
// description : C++ source for the GalilSlitClass. A singleton
//               class derived from DeviceClass. It implements the
//               command list and all properties and methods required
//               by the GalilSlit once per process.
//
// project :     TANGO Device Server
//
// $Author: buteau $
//
// $Revision: 1.12 $
//
// $Log: GalilSlitClass.cpp,v $
// Revision 1.12  2011/07/15 09:47:43  buteau
// MANTIS 19368
//
// Revision 1.11  2011/07/15 09:40:53  buteau
// MANTIS 19368
//
// Revision 1.10  2010/03/26 09:17:39  maven
// "Migration_Tango7_Part2"
//
// Revision 1.9  2010/02/09 12:59:27  coquet
// GalilSlit
// added commands ComputeGapOffset ComputePositionOffset
//
// Revision 1.8  2010/01/13 09:54:31  coquet
// added commands On Off to GalilSlit
//
// Revision 1.7  2008/10/20 13:24:47  coquet
// added property IsGapPosition to GalilSlit (support for Gap and Position type slits)
//
// Revision 1.6  2007/11/30 09:57:15  coquet
// added slit support :
// now supports independant moving (not geared moving) for better positionning accuracy for "bad mecanics" slits
// very minor change to CoupledAxes class (linux compilation warning correction)
//
// Revision 1.5  2007/10/12 14:24:25  coquet
// attributes positionOffset and gapOffset are now memorized / write on HW at init
// tested under W32
//
//
// copyleft :   European Synchrotron Radiation Facility
//              BP 220, Grenoble 38043
//              FRANCE
//
//-=============================================================================
//
//  		This file is generated by POGO
//	(Program Obviously used to Generate tango Object)
//
//         (c) - Software Engineering Group - ESRF
//=============================================================================


#include <tango.h>

#include <GalilSlit.h>
#include <GalilSlitClass.h>


//+----------------------------------------------------------------------------
/**
 *	Create GalilSlitClass singleton and return it in a C function for Python usage
 */
//+----------------------------------------------------------------------------
extern "C" {
#ifdef WIN32

__declspec(dllexport)

#endif

	Tango::DeviceClass *_create_GalilSlit_class(const char *name) {
		return GalilSlit_ns::GalilSlitClass::init(name);
	}
}


namespace GalilSlit_ns
{
//+----------------------------------------------------------------------------
//
// method : 		ComputeNewPositionOffsetCmd::execute()
// 
// description : 	method to trigger the execution of the command.
//                PLEASE DO NOT MODIFY this method core without pogo   
//
// in : - device : The device on which the command must be excuted
//		- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------
CORBA::Any *ComputeNewPositionOffsetCmd::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{

	cout2 << "ComputeNewPositionOffsetCmd::execute(): arrived" << endl;

	Tango::DevDouble	argin;
	extract(in_any, argin);

	((static_cast<GalilSlit *>(device))->compute_new_position_offset(argin));
	return new CORBA::Any();
}

//+----------------------------------------------------------------------------
//
// method : 		ComputeNewGapOffsetClass::execute()
// 
// description : 	method to trigger the execution of the command.
//                PLEASE DO NOT MODIFY this method core without pogo   
//
// in : - device : The device on which the command must be excuted
//		- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------
CORBA::Any *ComputeNewGapOffsetClass::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{

	cout2 << "ComputeNewGapOffsetClass::execute(): arrived" << endl;

	Tango::DevDouble	argin;
	extract(in_any, argin);

	((static_cast<GalilSlit *>(device))->compute_new_gap_offset(argin));
	return new CORBA::Any();
}

//+----------------------------------------------------------------------------
//
// method : 		OnClass::execute()
// 
// description : 	method to trigger the execution of the command.
//                PLEASE DO NOT MODIFY this method core without pogo   
//
// in : - device : The device on which the command must be excuted
//		- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------
CORBA::Any *OnClass::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{

	cout2 << "OnClass::execute(): arrived" << endl;

	((static_cast<GalilSlit *>(device))->on());
	return new CORBA::Any();
}

//+----------------------------------------------------------------------------
//
// method : 		OffClass::execute()
// 
// description : 	method to trigger the execution of the command.
//                PLEASE DO NOT MODIFY this method core without pogo   
//
// in : - device : The device on which the command must be excuted
//		- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------
CORBA::Any *OffClass::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{

	cout2 << "OffClass::execute(): arrived" << endl;

	((static_cast<GalilSlit *>(device))->off());
	return new CORBA::Any();
}


//+----------------------------------------------------------------------------
//
// method : 		StopCmd::execute()
// 
// description : 	method to trigger the execution of the command.
//                PLEASE DO NOT MODIFY this method core without pogo   
//
// in : - device : The device on which the command must be excuted
//		- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------
CORBA::Any *StopCmd::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{

	cout2 << "StopCmd::execute(): arrived" << endl;

	((static_cast<GalilSlit *>(device))->stop());
	return new CORBA::Any();
}

//+----------------------------------------------------------------------------
//
// method : 		SetIndependantModeCmd::execute()
// 
// description : 	method to trigger the execution of the command.
//                PLEASE DO NOT MODIFY this method core without pogo   
//
// in : - device : The device on which the command must be excuted
//		- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------
CORBA::Any *SetIndependantModeCmd::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{

	cout2 << "SetIndependantModeCmd::execute(): arrived" << endl;

	((static_cast<GalilSlit *>(device))->set_independant_mode());
	return new CORBA::Any();
}


//
//----------------------------------------------------------------
//	Initialize pointer for singleton pattern
//----------------------------------------------------------------
//
GalilSlitClass *GalilSlitClass::_instance = NULL;

//+----------------------------------------------------------------------------
//
// method : 		GalilSlitClass::GalilSlitClass(string &s)
// 
// description : 	constructor for the GalilSlitClass
//
// in : - s : The class name
//
//-----------------------------------------------------------------------------
GalilSlitClass::GalilSlitClass(string &s):DeviceClass(s)
{

	cout2 << "Entering GalilSlitClass constructor" << endl;
	set_default_property();
	get_class_property();
	write_class_property();
	
	cout2 << "Leaving GalilSlitClass constructor" << endl;

}
//+----------------------------------------------------------------------------
//
// method : 		GalilSlitClass::~GalilSlitClass()
// 
// description : 	destructor for the GalilSlitClass
//
//-----------------------------------------------------------------------------
GalilSlitClass::~GalilSlitClass()
{
	_instance = NULL;
}

//+----------------------------------------------------------------------------
//
// method : 		GalilSlitClass::instance
// 
// description : 	Create the object if not already done. Otherwise, just
//			return a pointer to the object
//
// in : - name : The class name
//
//-----------------------------------------------------------------------------
GalilSlitClass *GalilSlitClass::init(const char *name)
{
	if (_instance == NULL)
	{
		try
		{
			string s(name);
			_instance = new GalilSlitClass(s);
		}
		catch (bad_alloc)
		{
			throw;
		}		
	}		
	return _instance;
}

GalilSlitClass *GalilSlitClass::instance()
{
	if (_instance == NULL)
	{
		cerr << "Class is not initialised !!" << endl;
		exit(-1);
	}
	return _instance;
}

//+----------------------------------------------------------------------------
//
// method : 		GalilSlitClass::command_factory
// 
// description : 	Create the command object(s) and store them in the 
//			command list
//
//-----------------------------------------------------------------------------
void GalilSlitClass::command_factory()
{
	command_list.push_back(new StopCmd("Stop",
		Tango::DEV_VOID, Tango::DEV_VOID,
		"",
		"",
		Tango::OPERATOR));
	command_list.push_back(new SetIndependantModeCmd("SetIndependantMode",
		Tango::DEV_VOID, Tango::DEV_VOID,
		"",
		"",
		Tango::OPERATOR));
	command_list.push_back(new OffClass("Off",
		Tango::DEV_VOID, Tango::DEV_VOID,
		"",
		"",
		Tango::OPERATOR));
	command_list.push_back(new OnClass("On",
		Tango::DEV_VOID, Tango::DEV_VOID,
		"",
		"",
		Tango::OPERATOR));
	command_list.push_back(new ComputeNewGapOffsetClass("ComputeNewGapOffset",
		Tango::DEV_DOUBLE, Tango::DEV_VOID,
		"the gap you want to have here and now",
		"",
		Tango::OPERATOR));
	command_list.push_back(new ComputeNewPositionOffsetCmd("ComputeNewPositionOffset",
		Tango::DEV_DOUBLE, Tango::DEV_VOID,
		"the position you want to have here and now",
		"",
		Tango::OPERATOR));

	//	add polling if any
	for (unsigned int i=0 ; i<command_list.size(); i++)
	{
	}
}

//+----------------------------------------------------------------------------
//
// method : 		GalilSlitClass::get_class_property
// 
// description : 	Get the class property for specified name.
//
// in :		string	name : The property name
//
//+----------------------------------------------------------------------------
Tango::DbDatum GalilSlitClass::get_class_property(string &prop_name)
{
	for (unsigned int i=0 ; i<cl_prop.size() ; i++)
		if (cl_prop[i].name == prop_name)
			return cl_prop[i];
	//	if not found, return  an empty DbDatum
	return Tango::DbDatum(prop_name);
}
//+----------------------------------------------------------------------------
//
// method : 		GalilSlitClass::get_default_device_property()
// 
// description : 	Return the default value for device property.
//
//-----------------------------------------------------------------------------
Tango::DbDatum GalilSlitClass::get_default_device_property(string &prop_name)
{
	for (unsigned int i=0 ; i<dev_def_prop.size() ; i++)
		if (dev_def_prop[i].name == prop_name)
			return dev_def_prop[i];
	//	if not found, return  an empty DbDatum
	return Tango::DbDatum(prop_name);
}

//+----------------------------------------------------------------------------
//
// method : 		GalilSlitClass::get_default_class_property()
// 
// description : 	Return the default value for class property.
//
//-----------------------------------------------------------------------------
Tango::DbDatum GalilSlitClass::get_default_class_property(string &prop_name)
{
	for (unsigned int i=0 ; i<cl_def_prop.size() ; i++)
		if (cl_def_prop[i].name == prop_name)
			return cl_def_prop[i];
	//	if not found, return  an empty DbDatum
	return Tango::DbDatum(prop_name);
}
//+----------------------------------------------------------------------------
//
// method : 		GalilSlitClass::device_factory
// 
// description : 	Create the device object(s) and store them in the 
//			device list
//
// in :		Tango::DevVarStringArray *devlist_ptr : The device name list
//
//-----------------------------------------------------------------------------
void GalilSlitClass::device_factory(const Tango::DevVarStringArray *devlist_ptr)
{

	//	Create all devices.(Automatic code generation)
	//-------------------------------------------------------------
	for (unsigned long i=0 ; i < devlist_ptr->length() ; i++)
	{
		cout4 << "Device name : " << (*devlist_ptr)[i].in() << endl;
						
		// Create devices and add it into the device list
		//----------------------------------------------------
		device_list.push_back(new GalilSlit(this, (*devlist_ptr)[i]));							 

		// Export device to the outside world
		// Check before if database used.
		//---------------------------------------------
		if ((Tango::Util::_UseDb == true) && (Tango::Util::_FileDb == false))
			export_device(device_list.back());
		else
			export_device(device_list.back(), (*devlist_ptr)[i]);
	}
	//	End of Automatic code generation
	//-------------------------------------------------------------

}
//+----------------------------------------------------------------------------
//	Method: GalilSlitClass::attribute_factory(vector<Tango::Attr *> &att_list)
//-----------------------------------------------------------------------------
void GalilSlitClass::attribute_factory(vector<Tango::Attr *> &att_list)
{
	//	Attribute : gap
	gapAttrib	*gap = new gapAttrib();
	Tango::UserDefaultAttrProp	gap_prop;
	gap_prop.set_label("Gap Value");
	gap_prop.set_format("%8.3f");
	gap_prop.set_description("read  : the current value of the gap\nwrite :  gap setpoint");
	gap->set_default_properties(gap_prop);
	att_list.push_back(gap);

	//	Attribute : position
	positionAttrib	*position = new positionAttrib();
	Tango::UserDefaultAttrProp	position_prop;
	position_prop.set_label("Position ");
	position_prop.set_format("%8.3f");
	position_prop.set_description("position of the center of the gap\n");
	position->set_default_properties(position_prop);
	att_list.push_back(position);

	//	Attribute : insideUpPosition
	insideUpPositionAttrib	*inside_up_position = new insideUpPositionAttrib();
	Tango::UserDefaultAttrProp	inside_up_position_prop;
	inside_up_position_prop.set_label("InsideUpPosition");
	inside_up_position_prop.set_format("%8.3f");
	inside_up_position_prop.set_description("position of the inside ( Up ) blade");
	inside_up_position->set_default_properties(inside_up_position_prop);
	att_list.push_back(inside_up_position);

	//	Attribute : outsideDownPosition
	outsideDownPositionAttrib	*outside_down_position = new outsideDownPositionAttrib();
	Tango::UserDefaultAttrProp	outside_down_position_prop;
	outside_down_position_prop.set_label("OutsideDownPosition");
	outside_down_position_prop.set_format("%8.3f");
	outside_down_position_prop.set_description("position of the outside ( down ) blade ");
	outside_down_position->set_default_properties(outside_down_position_prop);
	att_list.push_back(outside_down_position);

	//	Attribute : positionOffset
	positionOffsetAttrib	*position_offset = new positionOffsetAttrib();
	Tango::UserDefaultAttrProp	position_offset_prop;
	position_offset_prop.set_format("%8.3f");
	position_offset_prop.set_description("user offset added to the position of the gap");
	position_offset->set_default_properties(position_offset_prop);
	position_offset->set_disp_level(Tango::EXPERT);
	position_offset->set_memorized();
	position_offset->set_memorized_init(true);
	att_list.push_back(position_offset);

	//	Attribute : gapOffset
	gapOffsetAttrib	*gap_offset = new gapOffsetAttrib();
	Tango::UserDefaultAttrProp	gap_offset_prop;
	gap_offset_prop.set_label("GapOffset");
	gap_offset_prop.set_format("%8.3f");
	gap_offset_prop.set_description("adds an offset to the gap ");
	gap_offset->set_default_properties(gap_offset_prop);
	gap_offset->set_disp_level(Tango::EXPERT);
	gap_offset->set_memorized();
	gap_offset->set_memorized_init(true);
	att_list.push_back(gap_offset);

	//	End of Automatic code generation
	//-------------------------------------------------------------
}

//+----------------------------------------------------------------------------
//
// method : 		GalilSlitClass::get_class_property()
// 
// description : 	Read the class properties from database.
//
//-----------------------------------------------------------------------------
void GalilSlitClass::get_class_property()
{
	//	Initialize your default values here (if not done with  POGO).
	//------------------------------------------------------------------

	//	Read class properties from database.(Automatic code generation)
	//------------------------------------------------------------------

	//	Call database and extract values
	//--------------------------------------------
	if (Tango::Util::instance()->_UseDb==true)
		get_db_class()->get_property(cl_prop);
	Tango::DbDatum	def_prop;
	int	i = -1;


	//	End of Automatic code generation
	//------------------------------------------------------------------

}

//+----------------------------------------------------------------------------
//
// method : 	GalilSlitClass::set_default_property
// 
// description: Set default property (class and device) for wizard.
//              For each property, add to wizard property name and description
//              If default value has been set, add it to wizard property and
//              store it in a DbDatum.
//
//-----------------------------------------------------------------------------
void GalilSlitClass::set_default_property()
{
	string	prop_name;
	string	prop_desc;
	string	prop_def;

	vector<string>	vect_data;
	//	Set Default Class Properties
	//	Set Default Device Properties
	prop_name = "InsideUpAxis";
	prop_desc = "letter of the Inside ( Up ) axis, from  A to H\nThis axis is the master axis\n";
	prop_def  = "A";
	vect_data.clear();
	vect_data.push_back("A");
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

	prop_name = "OutsideDownAxis";
	prop_desc = "Outside ( Down ) axis of the slit, A to H\nThis axis is the slave axis\n";
	prop_def  = "B";
	vect_data.clear();
	vect_data.push_back("B");
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

	prop_name = "InvertPositionDirection";
	prop_desc = "use this property to invert the default direction of position of the slit\ntrue : the position is inverted :\ne.g. standard position of -3mm will be displayed as +3mm\npositive direction of position of a vertical slit will be up rather than down\n\n";
	prop_def  = "";
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

	prop_name = "IsGeared";
	prop_desc = "set to true if you want to run your Slit as Geared Master/Slave\nthe Slave blage follows tigthly ( same speed, etc...) the master blade\nYou can change a position with constant gap, change of gap and keep position at the same place\non the other hand blade positions can derivate when mechanics is not correct\n(ther is no position correction on the slave\n\nset to false, Slit is run as 2 separate motors without gearing.\nPositions are corrected on both axes, so gap and position are more accurate\nThe speeds can be different on the 2 axes.\nIt is possible to control motors through their own devices while the Slit device is running\n(no gear -> no DISABLE state)\n";
	prop_def  = "true";
	vect_data.clear();
	vect_data.push_back("true");
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

	prop_name = "IsGapPosition";
	prop_desc = "set it to true if your slit has 1 motor for gap / 1 motor for position\nif true IsGeared has no effect";
	prop_def  = "false";
	vect_data.clear();
	vect_data.push_back("false");
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

}
//+----------------------------------------------------------------------------
//
// method : 		GalilSlitClass::write_class_property
// 
// description : 	Set class description as property in database
//
//-----------------------------------------------------------------------------
void GalilSlitClass::write_class_property()
{
	//	First time, check if database used
	//--------------------------------------------
	if (Tango::Util::_UseDb == false)
		return;

	Tango::DbData	data;
	string	classname = get_name();
	string	header;
	string::size_type	start, end;

	//	Put title
	Tango::DbDatum	title("ProjectTitle");
	string	str_title("GalilSlit");
	title << str_title;
	data.push_back(title);

	//	Put Description
	Tango::DbDatum	description("Description");
	vector<string>	str_desc;
	str_desc.push_back("handles generic slits");
	str_desc.push_back("type independant blades");
	str_desc.push_back("type 1 motor for gap plus 1 motor for translation");
	str_desc.push_back("ETC");
	description << str_desc;
	data.push_back(description);
		
	//	put cvs location
	string	rcsId(RcsId);
	string	filename(classname);
	start = rcsId.find("/");
	if (start!=string::npos)
	{
		filename += "Class.cpp";
		end   = rcsId.find(filename);
		if (end>start)
		{
			string	strloc = rcsId.substr(start, end-start);
			//	Check if specific repository
			start = strloc.find("/cvsroot/");
			if (start!=string::npos && start>0)
			{
				string	repository = strloc.substr(0, start);
				if (repository.find("/segfs/")!=string::npos)
					strloc = "ESRF:" + strloc.substr(start, strloc.length()-start);
			}
			Tango::DbDatum	cvs_loc("cvs_location");
			cvs_loc << strloc;
			data.push_back(cvs_loc);
		}
	}

	//	Get CVS tag revision
	string	tagname(TagName);
	header = "$Name: ";
	start = header.length();
	string	endstr(" $");
	end   = tagname.find(endstr);
	if (end!=string::npos && end>start)
	{
		string	strtag = tagname.substr(start, end-start);
		Tango::DbDatum	cvs_tag("cvs_tag");
		cvs_tag << strtag;
		data.push_back(cvs_tag);
	}

	//	Get URL location
	string	httpServ(HttpServer);
	if (httpServ.length()>0)
	{
		Tango::DbDatum	db_doc_url("doc_url");
		db_doc_url << httpServ;
		data.push_back(db_doc_url);
	}

	//  Put inheritance
	Tango::DbDatum	inher_datum("InheritedFrom");
	vector<string> inheritance;
	inheritance.push_back("Device_4Impl");
	inher_datum << inheritance;
	data.push_back(inher_datum);

	//	Call database and and values
	//--------------------------------------------
	get_db_class()->put_property(data);
}

}	// namespace