/*----- PROTECTED REGION ID(AttributesForwarder.h) ENABLED START -----*/
//=============================================================================
//
// file :        AttributesForwarder.h
//
// description : Include for the AttributesForwarder class.
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


#ifndef ATTRIBUTESFORWARDER_H
#define ATTRIBUTESFORWARDER_H


#include <tango.h>
#include <yat4tango/DynamicAttributeManager.h>

/*----- PROTECTED REGION END -----*/

/**
 *	AttributesForwarder class Description:
 *	Forwards the attributes specified by the ForwardedAttributes property.
 *	Each forwarded attribute becomes a local dynamic attribute of the AttributesForwarder device.
 *	ForwardedAttributes property is an array of strings (one entry per attribute to forward).
 *	Each entry should at least contains the fully qualified name of the attribute to be forwarded.
 *	An optional 'local attribute name' can also be specified in case you need/want to change the name under which the attribute is forwarded.
 *	Each ForwardedAttributes entry must have the following syntax: [tdb-host:tdb-port/]the/dev/name/attr-name[::attr-local-name].
 *	
 */

namespace AttributesForwarder_ns
{
	/*----- PROTECTED REGION ID(AttributesForwarder::Additional Class Declarations) ENABLED START -----*/

		//		Additional Class Declarations

	/*----- PROTECTED REGION END -----*/	//	AttributesForwarder::Additional Class Declarations


class AttributesForwarder : public Tango::Device_4Impl
{
	/*----- PROTECTED REGION ID(AttributesForwarder::Data Members) ENABLED START -----*/

	//		Add your own data members

	/*----- PROTECTED REGION END -----*/	//	AttributesForwarder::Data Members

//	Device property data members
public:		//	ForwardedAttributes:	An array of strings with the followinng syntax: [tdb-host:tdb-port/]the/dev/name/attr-name[:local name]
	vector<string>	forwardedAttributes;
  Tango::DevShort attr1_val;
  Tango::DevShort attr2_val;
  Tango::DevShort attr3_val;

//	Attribute data members
public:



//	Constructors and destructors
public:
	/**
	 * Constructs a newly allocated Command object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device Name
	 */
	AttributesForwarder(Tango::DeviceClass *cl,string &s);
	/**
	 * Constructs a newly allocated Command object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device Name
	 */
	AttributesForwarder(Tango::DeviceClass *cl,const char *s);
	/**
	 * Constructs a newly allocated Command object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device name
	 *	@param d	Device description.
	 */
	AttributesForwarder(Tango::DeviceClass *cl,const char *s,const char *d);
	/**
	 * The object destructor.
	 */	
	~AttributesForwarder() {delete_device();};



//	Miscellaneous methods
public:
	/**
	 *	will be called at device destruction or at init command.
	 */
	void delete_device();
	/**
	 *	Initialize the device
	 */
	virtual void init_device();
	/**
	 *	Read the device properties from database
	 */
	 void get_device_property();
	/**
	 *	Always executed method before execution command method.
	 */
	virtual void always_executed_hook();


	/**
	 *	Method      : AttributesForwarder::add_dynamic_attributes()
	 *	Description : Add dynamic attributes if any.
	 */ 
	void add_dynamic_attributes();

//	Command related methods
public: 

	/*----- PROTECTED REGION ID(AttributesForwarder::Additional Method prototypes) ENABLED START -----*/

	//	Additional Method prototypes

private:
  //- the dyn. attrs. read callback
  void read_callback (yat4tango::DynamicAttributeReadCallbackData& cbd);

  //- the dyn. attrs. write callback
  void write_callback (yat4tango::DynamicAttributeWriteCallbackData& cbd);

  //- the dynamic attribute manager
  yat4tango::DynamicAttributeManager m_dam;
  
	/*----- PROTECTED REGION END -----*/	//	AttributesForwarder::Additional Method prototypes
};

	/*----- PROTECTED REGION ID(AttributesForwarder::Additional Classes Definitions) ENABLED START -----*/

	//	Additional Classes definitions

	/*----- PROTECTED REGION END -----*/	//	AttributesForwarder::Additional Classes Definitions

} //	namespace

#endif	//	ATTRIBUTESFORWARDER_H