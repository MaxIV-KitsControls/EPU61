//=============================================================================
//
// file :        GalilCoupledAxes.h
//
// description : Include for the GalilCoupledAxes class.
//
// project :	GalilCoupledAxes
//
// $Author: buteau $
//
// $Revision: 1.4 $
//
// $Log: GalilCoupledAxes.h,v $
// Revision 1.4  2011/07/15 09:47:42  buteau
// MANTIS 19368
//
// Revision 1.3  2011/07/15 09:40:53  buteau
// MANTIS 19368
//
// Revision 1.2  2010/03/25 18:10:22  maven
// "Migration_Tango7"
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
// copyleft :    European Synchrotron Radiation Facility
//               BP 220, Grenoble 38043
//               FRANCE
//
//=============================================================================
//
//  		This file is generated by POGO
//	(Program Obviously used to Generate tango Object)
//
//         (c) - Software Engineering Group - ESRF
//=============================================================================
#ifndef _GalilCoupledAxes_H
#define _GalilCoupledAxes_H

#include <tango.h>
#include "CoupledAxes.h"

//using namespace Tango;

/**
 * @author	$Author: buteau $
 * @version	$Revision: 1.4 $
 */

 //	Add your own constants definitions here.
 //-----------------------------------------------


namespace GalilCoupledAxes_ns
{

/**
 * Class Description:
 * This class is used to control coupled axes by controlling only the master axis.
 *	All the master-slave axis configuration are done by motorists in micro-code itself.
 *	Special commands if needed are accessed through MicrocodeDataViewer DS
 *	First need : a detector table with 2 motors
 */

/*
 *	Device States Description:
 */


class GalilCoupledAxes: public Tango::Device_4Impl
{
public :
	//	Add your own data members here
	//-----------------------------------------


	//	Here is the Start of the automatic code generation part
	//-------------------------------------------------------------	
/**
 *	@name attributes
 *	Attributs member data.
 */
//@{
		Tango::DevDouble	*attr_position_read;
		Tango::DevDouble	attr_position_write;
		Tango::DevBoolean	*attr_forwardLimitSwitch_read;
		Tango::DevBoolean	*attr_backwardLimitSwitch_read;
//@}

/**
 *	@name Device properties
 *	Device properties member data.
 */
//@{
/**
 *	letter of the master axis, from  A to H
 *	This axis is the master axis
 *	
 */
	string	master;
//@}

/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Constructs a newly allocated Command object.
 *
 *	@param cl	Class.
 *	@param s 	Device Name
 */
	GalilCoupledAxes(Tango::DeviceClass *cl,string &s);
/**
 * Constructs a newly allocated Command object.
 *
 *	@param cl	Class.
 *	@param s 	Device Name
 */
	GalilCoupledAxes(Tango::DeviceClass *cl,const char *s);
/**
 * Constructs a newly allocated Command object.
 *
 *	@param cl	Class.
 *	@param s 	Device name
 *	@param d	Device description.
 */
	GalilCoupledAxes(Tango::DeviceClass *cl,const char *s,const char *d);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class */
//@{
/**
 * The object desctructor.
 */	
	~GalilCoupledAxes() {delete_device();};
/**
 *	will be called at device destruction or at init command.
 */
	void delete_device();
//@}

	
/**@name Miscellaneous methods */
//@{
/**
 *	Initialize the device
 */
	virtual void init_device();
/**
 *	Always executed method befor execution command method.
 */
	virtual void always_executed_hook();

//@}

/**
 * @name GalilCoupledAxes methods prototypes
 */

//@{
/**
 *	Hardware acquisition for attributes.
 */
	virtual void read_attr_hardware(vector<long> &attr_list);
/**
 *	Extract real attribute values for position acquisition result.
 */
	virtual void read_position(Tango::Attribute &attr);
/**
 *	Write position attribute values to hardware.
 */
	virtual void write_position(Tango::WAttribute &attr);
/**
 *	Extract real attribute values for forwardLimitSwitch acquisition result.
 */
	virtual void read_forwardLimitSwitch(Tango::Attribute &attr);
/**
 *	Extract real attribute values for backwardLimitSwitch acquisition result.
 */
	virtual void read_backwardLimitSwitch(Tango::Attribute &attr);
/**
 *	Read/Write allowed for position attribute.
 */
	virtual bool is_position_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for forwardLimitSwitch attribute.
 */
	virtual bool is_forwardLimitSwitch_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for backwardLimitSwitch attribute.
 */
	virtual bool is_backwardLimitSwitch_allowed(Tango::AttReqType type);
/**
 *	Execution allowed for Stop command.
 */
	virtual bool is_Stop_allowed(const CORBA::Any &any);
/**
 *	Execution allowed for Forward command.
 */
	virtual bool is_Forward_allowed(const CORBA::Any &any);
/**
 *	Execution allowed for Backward command.
 */
	virtual bool is_Backward_allowed(const CORBA::Any &any);
/**
 * This command gets the device state (stored in its <i>device_state</i> data member) and returns it to the caller.
 *	@return	State Code
 *	@exception DevFailed
 */
	virtual Tango::DevState	dev_state();
/**
 * This command gets the device status (stored in its <i>device_status</i> data member) and returns it to the caller.
 *	@return	Status description
 *	@exception DevFailed
 */
	virtual Tango::ConstDevString	dev_status();
/**
 * stops the movement
 *	@exception DevFailed
 */
	void	stop();
/**
 * initiates a jog forward
 *	continuous forward movement, stopped with Stop command
 *	@exception DevFailed
 */
	void	forward();
/**
 * initiates a jog backward
 *	continuous backward movement, stopped with Stop command
 *	@exception DevFailed
 */
	void	backward();

/**
 *	Read the device properties from database
 */
	 void get_device_property();
//@}

	//	Here is the end of the automatic code generation part
	//-------------------------------------------------------------	



protected :	
	//	Add your own data members here
	//-----------------------------------------

private :
	bool m_init_device_done;
	bool m_properties_missing;
	std::string m_master_status;
	std::string m_status_str;

  std::string master_axis_letter;     // contains axis letter for Galil commands
  std::string command_concat;         // pour la manipulation de chaines de caracteres

  galil::CoupledAxes * m_coupled_axes;

};

}	// namespace_ns

#endif	// _GalilCoupledAxes_H
