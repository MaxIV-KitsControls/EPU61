/*----- PROTECTED REGION ID(AttributesForwarderClass.cpp) ENABLED START -----*/
static const char *RcsId      = "$Id:  $";
static const char *TagName    = "$Name:  $";
static const char *CvsPath    = "$Source:  $";
static const char *SvnPath    = "$HeadURL:  $";
static const char *HttpServer = "http://www.esrf.eu/computing/cs/tango/tango_doc/ds_doc/";
//=============================================================================
//
// file :        AttributesForwarderClass.cpp
//
// description : C++ source for the AttributesForwarderClass. A singleton
//               class derived from DeviceClass. It implements the
//               command list and all properties and methods required
//               by the �name� once per process.
//
// project :     AttributesForwarder.
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


#include <AttributesForwarderClass.h>

/*----- PROTECTED REGION END -----*/

//-------------------------------------------------------------------
/**
 *	Create AttributesForwarderClass singleton and
 *	return it in a C function for Python usage
 */
//-------------------------------------------------------------------
extern "C" {
#ifdef _TG_WINDOWS_

__declspec(dllexport)

#endif

	Tango::DeviceClass *_create_AttributesForwarder_class(const char *name) {
		return AttributesForwarder_ns::AttributesForwarderClass::init(name);
	}
}


namespace AttributesForwarder_ns
{


//===================================================================
//	Initialize pointer for singleton pattern
//===================================================================
AttributesForwarderClass *AttributesForwarderClass::_instance = NULL;

//--------------------------------------------------------
/**
 * method : 		AttributesForwarderClass::AttributesForwarderClass(string &s)
 * description : 	constructor for the AttributesForwarderClass
 *
 * @param s	The class name
 */
//--------------------------------------------------------
AttributesForwarderClass::AttributesForwarderClass(string &s):DeviceClass(s)
{
	cout2 << "Entering AttributesForwarderClass constructor" << endl;
	set_default_property();
	get_class_property();
	write_class_property();

	/*----- PROTECTED REGION ID(AttributesForwarder::Class::constructor) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	AttributesForwarder::Class::constructor

	cout2 << "Leaving AttributesForwarderClass constructor" << endl;
}


//--------------------------------------------------------
/**
 * method : 		AttributesForwarderClass::~AttributesForwarderClass()
 * description : 	destructor for the AttributesForwarderClass
 */
//--------------------------------------------------------
AttributesForwarderClass::~AttributesForwarderClass()
{
	/*----- PROTECTED REGION ID(AttributesForwarder::Class::destructor) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	AttributesForwarder::Class::destructor

	_instance = NULL;
}


//--------------------------------------------------------
/**
 * method : 		AttributesForwarderClass::init
 * description : 	Create the object if not already done.
 *                  Otherwise, just return a pointer to the object
 *
 * @param	name	The class name
 */
//--------------------------------------------------------
AttributesForwarderClass *AttributesForwarderClass::init(const char *name)
{
	if (_instance == NULL)
	{
		try
		{
			string s(name);
			_instance = new AttributesForwarderClass(s);
		}
		catch (bad_alloc)
		{
			throw;
		}		
	}		
	return _instance;
}

//--------------------------------------------------------
/**
 * method : 		AttributesForwarderClass::instance
 * description : 	Check if object already created,
 *                  and return a pointer to the object
 */
//--------------------------------------------------------
AttributesForwarderClass *AttributesForwarderClass::instance()
{
	if (_instance == NULL)
	{
		cerr << "Class is not initialised !!" << endl;
		exit(-1);
	}
	return _instance;
}




//===================================================================
//	Command execution method calls
//===================================================================




//===================================================================
//	Properties management
//===================================================================

//--------------------------------------------------------
/**
 * method : 		AttributesForwarderClass::get_class_property
 * description : 	Get the class property for specified name.
 *
 * @param	name  The property name
 */
//--------------------------------------------------------
Tango::DbDatum AttributesForwarderClass::get_class_property(string &prop_name)
{
	for (unsigned int i=0 ; i<cl_prop.size() ; i++)
		if (cl_prop[i].name == prop_name)
			return cl_prop[i];
	//	if not found, returns  an empty DbDatum
	return Tango::DbDatum(prop_name);
}


//--------------------------------------------------------
/**
 *	Method      : AttributesForwarder::AttributesForwarderClass::get_default_device_property()()
 *	Description : Return the default value for device property.
 */
//--------------------------------------------------------
Tango::DbDatum AttributesForwarderClass::get_default_device_property(string &prop_name)
{
	for (unsigned int i=0 ; i<dev_def_prop.size() ; i++)
		if (dev_def_prop[i].name == prop_name)
			return dev_def_prop[i];
	//	if not found, return  an empty DbDatum
	return Tango::DbDatum(prop_name);
}


//--------------------------------------------------------
/**
 *	Method      : AttributesForwarder::AttributesForwarderClass::get_default_class_property()()
 *	Description : Return the default value for class property.
 */
//--------------------------------------------------------
Tango::DbDatum AttributesForwarderClass::get_default_class_property(string &prop_name)
{
	for (unsigned int i=0 ; i<cl_def_prop.size() ; i++)
		if (cl_def_prop[i].name == prop_name)
			return cl_def_prop[i];
	//	if not found, return  an empty DbDatum
	return Tango::DbDatum(prop_name);
}


//--------------------------------------------------------
/**
 *	Method      : AttributesForwarder::AttributesForwarderClass::get_class_property()
 *	Description : //	Add your own code to initialize
 */
//--------------------------------------------------------
void AttributesForwarderClass::get_class_property()
{
}


//--------------------------------------------------------
/**
 *	Method      : AttributesForwarder::AttributesForwarderClass::set_default_property()
 *	Description : Set default property (class and device) for wizard.
 *	              For each property, add to wizard property name and description.
 *	              If default value has been set, add it to wizard property and.
 *	              store it in a DbDatum.
 */
//--------------------------------------------------------
void AttributesForwarderClass::set_default_property()
{
	string	prop_name;
	string	prop_desc;
	string	prop_def;
	vector<string>	vect_data;
	
	//	Set Default Class Properties

	//	Set Default Device Properties

	prop_name = "ForwardedAttributes";
	prop_desc = "An array of strings with the followinng syntax: [tdb-host:tdb-port/]the/dev/name/attr-name[:local name]";
	prop_def  = "";
	vect_data.clear();
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


//--------------------------------------------------------
/**
 *	Method      : AttributesForwarder::AttributesForwarderClass::write_class_property()
 *	Description : Set class description fields as property in database
 */
//--------------------------------------------------------
void AttributesForwarderClass::write_class_property()
{
	//	First time, check if database used
	if (Tango::Util::_UseDb == false)
		return;

	Tango::DbData	data;
	string	classname = get_name();
	string	header;
	string::size_type	start, end;

	//	Put title
	Tango::DbDatum	title("ProjectTitle");
	string	str_title("AttributesForwarder");
	title << str_title;
	data.push_back(title);

	//	Put Description
	Tango::DbDatum	description("Description");
	vector<string>	str_desc;
	str_desc.push_back("Forwards the attributes specified by the ForwardedAttributes property.");
	str_desc.push_back("Each forwarded attribute becomes a local dynamic attribute of the AttributesForwarder device.");
	str_desc.push_back("ForwardedAttributes property is an array of strings (one entry per attribute to forward).");
	str_desc.push_back("Each entry should at least contains the fully qualified name of the attribute to be forwarded.");
	str_desc.push_back("An optional 'local attribute name' can also be specified in case you need/want to change the name under which the attribute is forwarded.");
	str_desc.push_back("A ForwardedAttributes has the following syntax: [tdb-host:tdb-port/]the/dev/name/attr-name[:local name].");
	str_desc.push_back("");
	description << str_desc;
	data.push_back(description);
		
	//	put cvs or svn location
	string	filename("AttributesForwarder");
	filename += "Class.cpp";
	
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
	inheritance.push_back("Device_Impl");
	inher_datum << inheritance;
	data.push_back(inher_datum);

	//	Call database and and values
	get_db_class()->put_property(data);
}




//===================================================================
//	Factory methods
//===================================================================


//--------------------------------------------------------
/**
 * method : 		AttributesForwarderClass::device_factory
 * description : 	Create the device object(s)
 *                  and store them in the device list
 *
 * @param	*devlist_ptr	The device name list
 */
//--------------------------------------------------------
void AttributesForwarderClass::device_factory(const Tango::DevVarStringArray *devlist_ptr)
{

	/*----- PROTECTED REGION ID(AttributesForwarder::Class::device_factory_before) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	AttributesForwarder::Class::device_factory_before

	//	Create devices and add it into the device list
	for (unsigned long i=0 ; i<devlist_ptr->length() ; i++)
	{
		cout4 << "Device name : " << (*devlist_ptr)[i].in() << endl;
		device_list.push_back(new AttributesForwarder(this, (*devlist_ptr)[i]));							 
	}

	//	Manage dynamic attributes if any
	//- erase_dynamic_attributes(devlist_ptr, get_class_attr()->get_attr_list());

	//	Export devices to the outside world
	for (unsigned long i=1 ; i<=devlist_ptr->length() ; i++)
	{
		//	Add dynamic attributes if any
		AttributesForwarder *dev = static_cast<AttributesForwarder *>(device_list[device_list.size()-i]);
		dev->add_dynamic_attributes();

		//	Check before if database used.
		if ((Tango::Util::_UseDb == true) && (Tango::Util::_FileDb == false))
			export_device(dev);
		else
			export_device(dev, dev->get_name().c_str());
	}

	/*----- PROTECTED REGION ID(AttributesForwarder::Class::device_factory_after) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	AttributesForwarder::Class::device_factory_after

	
}


//--------------------------------------------------------
/**
 *	Method      : AttributesForwarder::AttributesForwarderClass::attribute_factory()
 *	Description : Create the attribute object(s)
 *	              and store them in the attribute list
 */
//--------------------------------------------------------
void AttributesForwarderClass::attribute_factory(vector<Tango::Attr *> &att_list)
{
	/*----- PROTECTED REGION ID(AttributesForwarder::Class::attribute_factory_before) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	AttributesForwarder::Class::attribute_factory_before



	//	Create a list of static attributes
	create_static_attribute_list(get_class_attr()->get_attr_list());

	/*----- PROTECTED REGION ID(AttributesForwarder::Class::attribute_factory_after) ENABLED START -----*/

	//	Add your own code

	/*----- PROTECTED REGION END -----*/	//	AttributesForwarder::Class::attribute_factory_after

}


//--------------------------------------------------------
/**
 *	Method      : AttributesForwarder::AttributesForwarderClass::command_factory()
 *	Description : Create the command object(s)
 *	              and store them in the command list
 */
//--------------------------------------------------------
void AttributesForwarderClass::command_factory()
{
	/*----- PROTECTED REGION ID(AttributesForwarder::Class::command_factory_before) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	AttributesForwarder::Class::command_factory_before


	/*----- PROTECTED REGION ID(AttributesForwarder::Class::command_factory_after) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	AttributesForwarder::Class::command_factory_after

}




//===================================================================
//	Dynamic attributes related methods
//===================================================================


//--------------------------------------------------------
/**
 * method : 		AttributesForwarderClass::create_static_attribute_list
 * description : 	Create the a list of static attributes
 *
 * @param	att_list	the ceated attribute list 
 */
//--------------------------------------------------------
void AttributesForwarderClass::create_static_attribute_list(vector<Tango::Attr *> &att_list)
{
	for (unsigned long i=0 ; i<att_list.size() ; i++)
	{
		string att_name(att_list[i]->get_name());
		transform(att_name.begin(), att_name.end(), att_name.begin(), ::tolower);
		defaultAttList.push_back(att_name);
	}

	cout2 << defaultAttList.size() << " attributes in default list" << endl;


	/*----- PROTECTED REGION ID(AttributesForwarder::Class::create_static_att_list) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	AttributesForwarder::Class::create_static_att_list

}


//--------------------------------------------------------
/**
 * method : 		AttributesForwarderClass::erase_dynamic_attributes
 * description : 	delete the dynamic attributes if any.
 *
 * @param	devlist_ptr	the device list pointer
 * @param	list of all attributes
 */
//--------------------------------------------------------
void AttributesForwarderClass::erase_dynamic_attributes(const Tango::DevVarStringArray *devlist_ptr, vector<Tango::Attr *> &att_list)
{
	Tango::Util *tg = Tango::Util::instance();

	for (unsigned long i=0 ; i<devlist_ptr->length() ; i++)
	{	
		Tango::DeviceImpl *dev_impl = tg->get_device_by_name(((string)(*devlist_ptr)[i]).c_str());
		AttributesForwarder *dev = static_cast<AttributesForwarder *> (dev_impl);
		
		vector<Tango::Attribute *> &dev_att_list = dev->get_device_attr()->get_attribute_list();
		vector<Tango::Attribute *>::iterator ite_att;
		for (ite_att=dev_att_list.begin() ; ite_att != dev_att_list.end() ; ++ite_att)
		{
			string att_name((*ite_att)->get_name_lower());
			if ((att_name == "state") || (att_name == "status"))
				continue;
			vector<string>::iterator ite_str = find(defaultAttList.begin(), defaultAttList.end(), att_name);
			if (ite_str == defaultAttList.end())
			{
				cout2 << att_name << " is a UNWANTED dynamic attribute for device " << (*devlist_ptr)[i] << endl;
				Tango::Attribute &att = dev->get_device_attr()->get_attr_by_name(att_name.c_str());
				dev->remove_attribute(att_list[att.get_attr_idx()],true);
				--ite_att;
			}
		}
	}
	/*----- PROTECTED REGION ID(AttributesForwarder::Class::erase_dynamic_attributes) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	AttributesForwarder::Class::erase_dynamic_attributes

}



	/*----- PROTECTED REGION ID(AttributesForwarder::Class::Additional Methods) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	AttributesForwarder::Class::Additional Methods

} //	namespace
