static const char *ClassId    = "$Id: AxisRawDataReaderClass.cpp,v 1.12 2011/07/15 09:47:42 buteau Exp $";
static const char *CvsPath    = "$Source: /usr/local/CVS/DeviceServer/Generic/Motorisation/ControlBoxV2/src/AxisRawDataReaderClass.cpp,v $";
static const char *SvnPath    = "$HeadURL: $";
static const char *RcsId     = "$Header: /usr/local/CVS/DeviceServer/Generic/Motorisation/ControlBoxV2/src/AxisRawDataReaderClass.cpp,v 1.12 2011/07/15 09:47:42 buteau Exp $";
static const char *TagName   = "$Name: release_2_5_4 $";
static const char *HttpServer= "http://www.esrf.fr/computing/cs/tango/tango_doc/ds_doc/";
//+=============================================================================
//
// file :        AxisRawDataReaderClass.cpp
//
// description : C++ source for the AxisRawDataReaderClass. A singleton
//               class derived from DeviceClass. It implements the
//               command list and all properties and methods required
//               by the AxisRawDataReader once per process.
//
// project :     TANGO Device Server
//
// $Author: buteau $
//
// $Revision: 1.12 $
//
// $Log: AxisRawDataReaderClass.cpp,v $
// Revision 1.12  2011/07/15 09:47:42  buteau
// MANTIS 19368
//
// Revision 1.11  2011/07/15 09:40:53  buteau
// MANTIS 19368
//
// Revision 1.10  2011/05/27 13:45:28  coquet
// added attribute positionError to AxisRawDataDeader
// TO BE TESTED
//
// Revision 1.9  2010/03/26 09:17:39  maven
// "Migration_Tango7_Part2"
//
// Revision 1.8  2008/10/17 07:06:15  coquet
// added attribute velocity to AxosRawDataReader
// to be tested
//
// Revision 1.7  2008/09/25 14:29:11  coquet
// Attribute AxisNumber is now memorized Write on Hardware at Init
// (was simply memorized before)
//
// Revision 1.6  2007/06/22 15:46:52  leclercq
// Commit for switch back to release 2.2.3
//
// Revision 1.5  2007/06/04 12:28:57  coquet
// AxisRawDataReader :
// added latch encoder position support
// added 2 attributes
// latchIsArmed, RO, bool, reports that latch feature is ready to capture encoder position
// latchOccured reports that latch occured since last latch arming
// added 2 commands
// ArmLatch arms the latch feature
// ReportLatchedPosition returns the last lached position ( exception if latch did not occured)
//
// Revision 1.4  2007/04/19 14:04:08  coquet
// Axis::State and Axis::Status :
// when motor is servo or when stepper without encoder : no complains about YA,YB,YC
// AxisRawDataReader :
// minor change (axisLetter is now just belox axisNumer)
// Slit : no real change
//
// Revision 1.3  2007/04/19 08:26:23  coquet
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
// Revision 1.2  2007/01/08 14:45:18  coquet
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


//+----------------------------------------------------------------------------
/**
 *	Create AxisRawDataReaderClass singleton and return it in a C function for Python usage
 */
//+----------------------------------------------------------------------------
extern "C" {
#ifdef WIN32

__declspec(dllexport)

#endif

	Tango::DeviceClass *_create_AxisRawDataReader_class(const char *name) {
		return AxisRawDataReader_ns::AxisRawDataReaderClass::init(name);
	}
}


namespace AxisRawDataReader_ns
{
//+----------------------------------------------------------------------------
//
// method : 		ArmLatchClass::execute()
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
CORBA::Any *ArmLatchClass::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{

	cout2 << "ArmLatchClass::execute(): arrived" << endl;

	((static_cast<AxisRawDataReader *>(device))->arm_latch());
	return new CORBA::Any();
}

//+----------------------------------------------------------------------------
//
// method : 		ReportLatchedPositionClass::execute()
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
CORBA::Any *ReportLatchedPositionClass::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{

	cout2 << "ReportLatchedPositionClass::execute(): arrived" << endl;

	return insert((static_cast<AxisRawDataReader *>(device))->report_latched_position());
}



//
//----------------------------------------------------------------
//	Initialize pointer for singleton pattern
//----------------------------------------------------------------
//
AxisRawDataReaderClass *AxisRawDataReaderClass::_instance = NULL;

//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReaderClass::AxisRawDataReaderClass(string &s)
// 
// description : 	constructor for the AxisRawDataReaderClass
//
// in : - s : The class name
//
//-----------------------------------------------------------------------------
AxisRawDataReaderClass::AxisRawDataReaderClass(string &s):DeviceClass(s)
{

	cout2 << "Entering AxisRawDataReaderClass constructor" << endl;
	set_default_property();
	write_class_property();
	get_class_property();
	
	cout2 << "Leaving AxisRawDataReaderClass constructor" << endl;

}
//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReaderClass::~AxisRawDataReaderClass()
// 
// description : 	destructor for the AxisRawDataReaderClass
//
//-----------------------------------------------------------------------------
AxisRawDataReaderClass::~AxisRawDataReaderClass()
{
	_instance = NULL;
}

//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReaderClass::instance
// 
// description : 	Create the object if not already done. Otherwise, just
//			return a pointer to the object
//
// in : - name : The class name
//
//-----------------------------------------------------------------------------
AxisRawDataReaderClass *AxisRawDataReaderClass::init(const char *name)
{
	if (_instance == NULL)
	{
		try
		{
			string s(name);
			_instance = new AxisRawDataReaderClass(s);
		}
		catch (bad_alloc)
		{
			throw;
		}		
	}		
	return _instance;
}

AxisRawDataReaderClass *AxisRawDataReaderClass::instance()
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
// method : 		AxisRawDataReaderClass::command_factory
// 
// description : 	Create the command object(s) and store them in the 
//			command list
//
//-----------------------------------------------------------------------------
void AxisRawDataReaderClass::command_factory()
{
	command_list.push_back(new ReportLatchedPositionClass("ReportLatchedPosition",
		Tango::DEV_VOID, Tango::DEV_LONG,
		"",
		"the encoder value latched",
		Tango::OPERATOR));
	command_list.push_back(new ArmLatchClass("ArmLatch",
		Tango::DEV_VOID, Tango::DEV_VOID,
		"",
		"",
		Tango::OPERATOR));

	//	add polling if any
	for (unsigned int i=0 ; i<command_list.size(); i++)
	{
	}
}

//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReaderClass::get_class_property
// 
// description : 	Get the class property for specified name.
//
// in :		string	name : The property name
//
//+----------------------------------------------------------------------------
Tango::DbDatum AxisRawDataReaderClass::get_class_property(string &prop_name)
{
	for (unsigned int i=0 ; i<cl_prop.size() ; i++)
		if (cl_prop[i].name == prop_name)
			return cl_prop[i];
	//	if not found, return  an empty DbDatum
	return Tango::DbDatum(prop_name);
}
//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReaderClass::get_default_device_property()
// 
// description : 	Return the default value for device property.
//
//-----------------------------------------------------------------------------
Tango::DbDatum AxisRawDataReaderClass::get_default_device_property(string &prop_name)
{
	for (unsigned int i=0 ; i<dev_def_prop.size() ; i++)
		if (dev_def_prop[i].name == prop_name)
			return dev_def_prop[i];
	//	if not found, return  an empty DbDatum
	return Tango::DbDatum(prop_name);
}

//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReaderClass::get_default_class_property()
// 
// description : 	Return the default value for class property.
//
//-----------------------------------------------------------------------------
Tango::DbDatum AxisRawDataReaderClass::get_default_class_property(string &prop_name)
{
	for (unsigned int i=0 ; i<cl_def_prop.size() ; i++)
		if (cl_def_prop[i].name == prop_name)
			return cl_def_prop[i];
	//	if not found, return  an empty DbDatum
	return Tango::DbDatum(prop_name);
}
//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReaderClass::device_factory
// 
// description : 	Create the device object(s) and store them in the 
//			device list
//
// in :		Tango::DevVarStringArray *devlist_ptr : The device name list
//
//-----------------------------------------------------------------------------
void AxisRawDataReaderClass::device_factory(const Tango::DevVarStringArray *devlist_ptr)
{

	//	Create all devices.(Automatic code generation)
	//-------------------------------------------------------------
	for (unsigned long i=0 ; i < devlist_ptr->length() ; i++)
	{
		cout4 << "Device name : " << (*devlist_ptr)[i].in() << endl;
						
		// Create devices and add it into the device list
		//----------------------------------------------------
		device_list.push_back(new AxisRawDataReader(this, (*devlist_ptr)[i]));							 

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
//	Method: AxisRawDataReaderClass::attribute_factory(vector<Tango::Attr *> &att_list)
//-----------------------------------------------------------------------------
void AxisRawDataReaderClass::attribute_factory(vector<Tango::Attr *> &att_list)
{
	//	Attribute : axisNumber
	axisNumberAttrib	*axis_number = new axisNumberAttrib();
	Tango::UserDefaultAttrProp	axis_number_prop;
	axis_number_prop.set_label("Axis Number [0..7]");
	axis_number_prop.set_unit(" ");
	axis_number_prop.set_standard_unit(" ");
	axis_number_prop.set_display_unit(" ");
	axis_number_prop.set_format("%1d");
	axis_number_prop.set_max_value("7");
	axis_number_prop.set_min_value("0");
	axis_number_prop.set_description("number of the axis to be displayed from 0 (A) to 7 (H)");
	axis_number->set_default_properties(axis_number_prop);
	axis_number->set_memorized();
	axis_number->set_memorized_init(true);
	att_list.push_back(axis_number);

	//	Attribute : axisLetter
	axisLetterAttrib	*axis_letter = new axisLetterAttrib();
	Tango::UserDefaultAttrProp	axis_letter_prop;
	axis_letter_prop.set_description("Axis letter selected by axis number");
	axis_letter->set_default_properties(axis_letter_prop);
	att_list.push_back(axis_letter);

	//	Attribute : encoder
	encoderAttrib	*encoder = new encoderAttrib();
	Tango::UserDefaultAttrProp	encoder_prop;
	encoder_prop.set_label("Encoder Position");
	encoder_prop.set_format("%8d");
	encoder_prop.set_description("Main Encoder Position id. TP");
	encoder->set_default_properties(encoder_prop);
	att_list.push_back(encoder);

	//	Attribute : auxiliary
	auxiliaryAttrib	*auxiliary = new auxiliaryAttrib();
	Tango::UserDefaultAttrProp	auxiliary_prop;
	auxiliary_prop.set_label("Motor Pos or Aux Encoder");
	auxiliary_prop.set_format("%8d");
	auxiliary_prop.set_description("displays the stepper motor position or servo second encoder \nid TD command");
	auxiliary->set_default_properties(auxiliary_prop);
	att_list.push_back(auxiliary);

	//	Attribute : commandedPos
	commandedPosAttrib	*commanded_pos = new commandedPosAttrib();
	Tango::UserDefaultAttrProp	commanded_pos_prop;
	commanded_pos_prop.set_label("Commanded Position");
	commanded_pos_prop.set_format("%8d");
	commanded_pos_prop.set_description("displays the commanded position \n(the position requested by the ControlBox) id. RP");
	commanded_pos->set_default_properties(commanded_pos_prop);
	att_list.push_back(commanded_pos);

	//	Attribute : velocity
	velocityAttrib	*velocity = new velocityAttrib();
	Tango::UserDefaultAttrProp	velocity_prop;
	velocity_prop.set_label("velocity");
	velocity_prop.set_format("%8d");
	velocity_prop.set_description("displays the instantaneous velocity\n");
	velocity->set_default_properties(velocity_prop);
	att_list.push_back(velocity);

	//	Attribute : analogInput
	analogInputAttrib	*analog_input = new analogInputAttrib();
	Tango::UserDefaultAttrProp	analog_input_prop;
	analog_input_prop.set_label("Analog Input");
	analog_input_prop.set_unit("V");
	analog_input_prop.set_standard_unit("V");
	analog_input_prop.set_display_unit("V");
	analog_input_prop.set_format("%6.4f");
	analog_input_prop.set_description("displays the analog input value id. MG @AN[n]");
	analog_input->set_default_properties(analog_input_prop);
	att_list.push_back(analog_input);

	//	Attribute : homeInput
	homeInputAttrib	*home_input = new homeInputAttrib();
	Tango::UserDefaultAttrProp	home_input_prop;
	home_input_prop.set_description("State of the Home Input");
	home_input->set_default_properties(home_input_prop);
	att_list.push_back(home_input);

	//	Attribute : latchInput
	latchInputAttrib	*latch_input = new latchInputAttrib();
	Tango::UserDefaultAttrProp	latch_input_prop;
	latch_input_prop.set_description("state of the Latch Input");
	latch_input->set_default_properties(latch_input_prop);
	att_list.push_back(latch_input);

	//	Attribute : latchOccured
	latchOccuredAttrib	*latch_occured = new latchOccuredAttrib();
	Tango::UserDefaultAttrProp	latch_occured_prop;
	latch_occured_prop.set_description("lach of encoder position occured since last latch arming");
	latch_occured->set_default_properties(latch_occured_prop);
	att_list.push_back(latch_occured);

	//	Attribute : latchIsArmed
	latchIsArmedAttrib	*latch_is_armed = new latchIsArmedAttrib();
	Tango::UserDefaultAttrProp	latch_is_armed_prop;
	latch_is_armed_prop.set_description("latch is armed, ready to latch the encoder position ");
	latch_is_armed->set_default_properties(latch_is_armed_prop);
	att_list.push_back(latch_is_armed);

	//	Attribute : positionError
	positionErrorAttrib	*position_error = new positionErrorAttrib();
	Tango::UserDefaultAttrProp	position_error_prop;
	position_error_prop.set_label("Position Error");
	position_error_prop.set_description("as result of TE cmd (but got it in the Data Record)");
	position_error->set_default_properties(position_error_prop);
	att_list.push_back(position_error);

	//	End of Automatic code generation
	//-------------------------------------------------------------
}

//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReaderClass::get_class_property()
// 
// description : 	Read the class properties from database.
//
//-----------------------------------------------------------------------------
void AxisRawDataReaderClass::get_class_property()
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
// method : 	AxisRawDataReaderClass::set_default_property
// 
// description: Set default property (class and device) for wizard.
//              For each property, add to wizard property name and description
//              If default value has been set, add it to wizard property and
//              store it in a DbDatum.
//
//-----------------------------------------------------------------------------
void AxisRawDataReaderClass::set_default_property()
{
	string	prop_name;
	string	prop_desc;
	string	prop_def;

	vector<string>	vect_data;
	//	Set Default Class Properties
	//	Set Default Device Properties
}
//+----------------------------------------------------------------------------
//
// method : 		AxisRawDataReaderClass::write_class_property
// 
// description : 	Set class description as property in database
//
//-----------------------------------------------------------------------------
void AxisRawDataReaderClass::write_class_property()
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
	string	str_title("AxisRawDataReader");
	title << str_title;
	data.push_back(title);

	//	Put Description
	Tango::DbDatum	description("Description");
	vector<string>	str_desc;
	str_desc.push_back("provides read-only raw data from the specified axis");
	description << str_desc;
	data.push_back(description);
		
	//	put cvs or svn location
	string	filename(classname);
	filename += "Class.cpp";
	
	// Create a string with the class ID to
	// get the string into the binary
	string	class_id(ClassId);
	
	// check for cvs information
	string	src_path(CvsPath);
	start = src_path.find("/");
	if (start!=string::npos)
	{
		end   = src_path.find(filename);
		if (end>start)
		{
			string	strloc = src_path.substr(start, end-start);
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
	// check for svn information
	else
	{
		string	src_path(SvnPath);
		start = src_path.find("://");
		if (start!=string::npos)
		{
			end = src_path.find(filename);
			if (end>start)
			{
				header = "$HeadURL: ";
				start = header.length();
				string	strloc = src_path.substr(start, (end-start));
				
				Tango::DbDatum	svn_loc("svn_location");
				svn_loc << strloc;
				data.push_back(svn_loc);
			}
		}
	}

	//	Get CVS or SVN revision tag
	
	// CVS tag
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
	
	// SVN tag
	string	svnpath(SvnPath);
	header = "$HeadURL: ";
	start = header.length();
	
	end   = svnpath.find(endstr);
	if (end!=string::npos && end>start)
	{
		string	strloc = svnpath.substr(start, end-start);
		
		string tagstr ("/tags/");
		start = strloc.find(tagstr);
		if ( start!=string::npos )
		{
			start = start + tagstr.length();
			end   = strloc.find(filename);
			string	strtag = strloc.substr(start, end-start-1);
			
			Tango::DbDatum	svn_tag("svn_tag");
			svn_tag << strtag;
			data.push_back(svn_tag);
		}
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
