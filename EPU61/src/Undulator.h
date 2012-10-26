/*----- PROTECTED REGION ID(Undulator.h) ENABLED START -----*/
//=============================================================================
//
// file :        Undulator.h
//
// description : Include for the Undulator class.
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


#ifndef UNDULATOR_H
#define UNDULATOR_H


#include <tango.h>
#include <string>
#include <vector>
#include "GalilCommunicator.h"
#include "correction/DataItem.h"
#include "powersupply/PowerSupplyThread.h"

/*----- PROTECTED REGION END -----*/


/**
 *	Undulator class Description:
 *	Device server for controling movement of undulator axes. 
 *	It features gap movement and phase movement in 4 modes.
 *	In expert mode, user can configure the behaviour of axes and restrict certain actions.
 */

namespace Undulator_ns
{
	/*----- PROTECTED REGION ID(Undulator::Additional Class Declarations) ENABLED START -----*/

    class UpdaterThread;
    class PollerThread;

	/*----- PROTECTED REGION END -----*/	//	Undulator::Additional Class Declarations


class Undulator : public Tango::Device_4Impl
{


	/*----- PROTECTED REGION ID(Undulator::Data Members) ENABLED START -----*/
    friend class UpdaterThread;
    friend class PollerThread;

public:

protected:
    omni_mutex deviceLock; //!< Lock for shared variables.


private:

    double desiredGap; //!< Target gap in user units.
    double desiredTaper; //!< Target taper in user units.
    double desiredCenter; //!< Target center in user units.
    double desiredOffset; //!< Desired phase offset.
    short desiredPhase; //!< Selected phase mode.

    vector<double> offsetsGapVector; //!< Holds all gap axes offsets.
    vector<double> offsetsPhaseVector; //!< Holds all phase axes offsets.

    UpdaterThread *stateUpdater; //!< Pointer to updater thread.

    bool engineeringLock; //!< Engineering mode lock.

    double gap; //!< Holds the actual gap in user units.
    double center; //!< Center line in user units. Default 0
    double taper; //!< Holds the actual taper in user units.
    double phaseOffset; //!< Current phase offset in user units.
    short phase; //!< Current phase mode.

    double taperSpeed; //!< Speed used when adjusting taper.
    double gapSpeed; //!< Speed used when moving gap.
    double phaseSpeed; //!< Speed used when moving to phase offset.

    double engineeringGapAxesSpeed; //!< Axes moving gap maximum speed in engineering mode.
    double engineeringPhaseAxesSpeed; //!< Axes moving phase maximum speed in engineering mode.

    double engineeringGapAxesAcceleration; //!< Axes moving gap acceleration and deceleration in engineering mode.
    double engineeringPhaseAxesAcceleration; //!< Axes moving phase acceleration and deceleration in engineering mode.

    double phaseAcc; //!< Phase movement acceleration and deceleration.
    double gapAcc; //!< Gap movement acceleration and deceleration.
    double taperAcc; //! Taper movement acceleration and deceleration.

    double positionRatioGap; //!< Counts to micrometers ratio for gap motors.
    double positionRatioPhase; //!< Counts to micrometers ratio for phase motors.

    double maxGap; //!< Maximal gap allowed.
    double minGap; //!< Minimal gap allowed.
    double maxOpPhase; //!< Maximal phase offset allowed.

    bool limitsDisabled; //!< Limits disabled flag.
    bool stopAll; //!< Stop all flag.
    bool gapMoving; //!< Gap axes are moving.
    bool phaseMoving; //!< Phase axes are moving.

    bool interlock; //!< Interlock signal.

    Tango::DeviceData devdata; //!< Device data when sending command and receiving response.

    GalilCommunicator *galilFunctions; //!< Pointer to class dealing with direct DMC commands.

    Tango::Group *phaseAxesGroup; //!< Group with phase axes.
    Tango::Group *gapAxesGroup; //!< Group with gap axes.

    double currentCoil1; //!< Current for first coil.
    double currentCoil2; //!< Current for second coil.
    double currentCoil3; //!< Current for third coil.
    double currentCoil4; //!< CUrrent for forth coil.

    vector<double> desiredCurrent; //!< Desired current for 4 coils. Manual mode.

    bool coilCorrectionEnabled; //!< Switch for automatic and manual coil correction. (true=automatic)
    bool dataFileLoaded; //!< Data file for automatic compensation loaded.

    PowerSupplyThread *supplies; //!< Pointer to class controlling power supplies.

    vector<DataItem> data;  //!< Interpolation table, straight entries
    PollerThread *pollerThread; //!< Pointer to pooling thread

	/*----- PROTECTED REGION END -----*/	//	Undulator::Data Members


//	Device property data members
public:		//	ControlBoxGapProxy:	Proxy to ControlBox with gap DMC
	string	controlBoxGapProxy;
	//	GapAxes:	Proxy to axes that control gap movement. Order: Z1,Z2,Z3,Z4
	vector<string>	gapAxes;
	//	PhaseAxes:	Proxy to axes that control phase movement.  Order: X1,X2,X3,X4
	vector<string>	phaseAxes;
	//	ControlBoxPhaseProxy:	Proxy to controlbox with phase DMC
	string	controlBoxPhaseProxy;
	//	GearedAxes:	Proxy to GalilGearedAxes that control upper and lower girder. Order: Upper girder, Lower girder
	vector<string>	gearedAxes;
	//	DataFile:	Filename containing interpolation data.
	string	dataFile;
	//	PowerSupplyProxy:	Proxy server where power supply will read and write current.
	string	powerSupplyProxy;
	//	PowerSupplyAttributeNames:	Attribute name for each power supply to read from and write to.
	vector<string>	powerSupplyAttributeNames;
	

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
	Undulator(Tango::DeviceClass *cl,string &s);
	/**
	 * Constructs a newly allocated Command object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device Name
	 */
	Undulator(Tango::DeviceClass *cl,const char *s);
	/**
	 * Constructs a newly allocated Command object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device name
	 *	@param d	Device description.
	 */
	Undulator(Tango::DeviceClass *cl,const char *s,const char *d);
	/**
	 * The object destructor.
	 */	
	~Undulator() {delete_device();};



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


//	Attribute methods
public:
	/**
	 *	Method      : Undulator::read_attr_hardware()
	 *	Description : Hardware acquisition for attributes.
	 */
	virtual void read_attr_hardware(vector<long> &attr_list);


	/**
	 *	Gap attribute related methods.
	 *	Description: Read-back and set-point for current gap. 
 *	             Writing to this attribute commences movement for the gap, 
 *	             using the current operator center offset and taper.
	 *
	 *	Data type:	Tango::DevDouble
	 *	Attr type:	Scalar 
	 */
	virtual void read_Gap(Tango::Attribute &attr);
	virtual void write_Gap(Tango::WAttribute &attr);
	virtual bool is_Gap_allowed(Tango::AttReqType type);



	/**
	 *	EngineeringLock attribute related methods.
	 *	Description: Setting this attribute to TRUE disables operator composite movements. 
 *	             Engineering movements to be done directly on the ControlBox axis devices.
	 *
	 *	Data type:	Tango::DevBoolean
	 *	Attr type:	Scalar 
	 */
	virtual void read_EngineeringLock(Tango::Attribute &attr);
	virtual void write_EngineeringLock(Tango::WAttribute &attr);
	virtual bool is_EngineeringLock_allowed(Tango::AttReqType type);



	/**
	 *	GapAcceleration attribute related methods.
	 *	Description: Gap movement average acceleration in um/s^2.
	 *
	 *	Data type:	Tango::DevDouble
	 *	Attr type:	Scalar 
	 */
	virtual void read_GapAcceleration(Tango::Attribute &attr);
	virtual void write_GapAcceleration(Tango::WAttribute &attr);
	virtual bool is_GapAcceleration_allowed(Tango::AttReqType type);



	/**
	 *	GapSpeed attribute related methods.
	 *	Description: Final profile velocity in um/s for gap movements.
	 *
	 *	Data type:	Tango::DevDouble
	 *	Attr type:	Scalar 
	 */
	virtual void read_GapSpeed(Tango::Attribute &attr);
	virtual void write_GapSpeed(Tango::WAttribute &attr);
	virtual bool is_GapSpeed_allowed(Tango::AttReqType type);



	/**
	 *	PhaseAcceleration attribute related methods.
	 *	Description: Phase movement average acceleration in um/s^2.
	 *
	 *	Data type:	Tango::DevDouble
	 *	Attr type:	Scalar 
	 */
	virtual void read_PhaseAcceleration(Tango::Attribute &attr);
	virtual void write_PhaseAcceleration(Tango::WAttribute &attr);
	virtual bool is_PhaseAcceleration_allowed(Tango::AttReqType type);



	/**
	 *	PhaseMode attribute related methods.
	 *	Description: Read-back and set-point for desired phase mode. 
 *	             Will be used next time Phase is written.
	 *
	 *	Data type:	Tango::DevShort
	 *	Attr type:	Scalar 
	 */
	virtual void read_PhaseMode(Tango::Attribute &attr);
	virtual void write_PhaseMode(Tango::WAttribute &attr);
	virtual bool is_PhaseMode_allowed(Tango::AttReqType type);



	/**
	 *	Phase attribute related methods.
	 *	Description: Read-back and set-point for phase offset.
 *	             When this attribute is written new phase movement commences 
 *	             using phase mode specified in PhaseMode attribute.
	 *
	 *	Data type:	Tango::DevDouble
	 *	Attr type:	Scalar 
	 */
	virtual void read_Phase(Tango::Attribute &attr);
	virtual void write_Phase(Tango::WAttribute &attr);
	virtual bool is_Phase_allowed(Tango::AttReqType type);



	/**
	 *	PhaseSpeed attribute related methods.
	 *	Description: Final profile velocity in um/s for phase (sub-girders) movement.
	 *
	 *	Data type:	Tango::DevDouble
	 *	Attr type:	Scalar 
	 */
	virtual void read_PhaseSpeed(Tango::Attribute &attr);
	virtual void write_PhaseSpeed(Tango::WAttribute &attr);
	virtual bool is_PhaseSpeed_allowed(Tango::AttReqType type);



	/**
	 *	TaperAcceleration attribute related methods.
	 *	Description: Taper movement average acceleration in um/s^2.
	 *
	 *	Data type:	Tango::DevDouble
	 *	Attr type:	Scalar 
	 */
	virtual void read_TaperAcceleration(Tango::Attribute &attr);
	virtual void write_TaperAcceleration(Tango::WAttribute &attr);
	virtual bool is_TaperAcceleration_allowed(Tango::AttReqType type);



	/**
	 *	Taper attribute related methods.
	 *	Description: Read-back and set-point value for the current taper. 
 *	             Will be applied next time the GapSetpoint attribute is modified.
	 *
	 *	Data type:	Tango::DevDouble
	 *	Attr type:	Scalar 
	 */
	virtual void read_Taper(Tango::Attribute &attr);
	virtual void write_Taper(Tango::WAttribute &attr);
	virtual bool is_Taper_allowed(Tango::AttReqType type);



	/**
	 *	Offset attribute related methods.
	 *	Description: Read-back and set-point value for the current vertical offset. 
 *	             Will be applied next time the Gap attribute is modified.
	 *
	 *	Data type:	Tango::DevDouble
	 *	Attr type:	Scalar 
	 */
	virtual void read_Offset(Tango::Attribute &attr);
	virtual void write_Offset(Tango::WAttribute &attr);
	virtual bool is_Offset_allowed(Tango::AttReqType type);



	/**
	 *	TaperSpeed attribute related methods.
	 *	Description: Final profile velocity in um/s for taper movements.
	 *
	 *	Data type:	Tango::DevDouble
	 *	Attr type:	Scalar 
	 */
	virtual void read_TaperSpeed(Tango::Attribute &attr);
	virtual void write_TaperSpeed(Tango::WAttribute &attr);
	virtual bool is_TaperSpeed_allowed(Tango::AttReqType type);



	/**
	 *	LimitsDisabled attribute related methods.
	 *	Description: Indicates that the software limits on the DMCs are disabled. 
 *	             Useful for commissioning and engineering movements.
	 *
	 *	Data type:	Tango::DevBoolean
	 *	Attr type:	Scalar 
	 */
	virtual void read_LimitsDisabled(Tango::Attribute &attr);
	virtual bool is_LimitsDisabled_allowed(Tango::AttReqType type);



	/**
	 *	StopAll attribute related methods.
	 *	Description: Indicates that the undulator is in StopAll state inhibiting all motion.
	 *
	 *	Data type:	Tango::DevBoolean
	 *	Attr type:	Scalar 
	 */
	virtual void read_StopAll(Tango::Attribute &attr);
	virtual bool is_StopAll_allowed(Tango::AttReqType type);



	/**
	 *	GapMoving attribute related methods.
	 *	Description: Indicates that gap movement is in progress. 
 *	             Used to distinguish whether gap, phase or both are moving.
	 *
	 *	Data type:	Tango::DevBoolean
	 *	Attr type:	Scalar 
	 */
	virtual void read_GapMoving(Tango::Attribute &attr);
	virtual bool is_GapMoving_allowed(Tango::AttReqType type);



	/**
	 *	PhaseMoving attribute related methods.
	 *	Description: Indicates that phase movement is in progress. 
 *	             Used to distinguish whether gap, phase or both are moving.
	 *
	 *	Data type:	Tango::DevBoolean
	 *	Attr type:	Scalar 
	 */
	virtual void read_PhaseMoving(Tango::Attribute &attr);
	virtual bool is_PhaseMoving_allowed(Tango::AttReqType type);



	/**
	 *	EngineeringGapSpeed attribute related methods.
	 *	Description: Final profile velocity in um/s for gap axes engineering movement.
	 *
	 *	Data type:	Tango::DevDouble
	 *	Attr type:	Scalar 
	 */
	virtual void read_EngineeringGapSpeed(Tango::Attribute &attr);
	virtual void write_EngineeringGapSpeed(Tango::WAttribute &attr);
	virtual bool is_EngineeringGapSpeed_allowed(Tango::AttReqType type);



	/**
	 *	EngineeringPhaseSpeed attribute related methods.
	 *	Description: Final profile velocity in um/s for phase axes engineering movement.
	 *
	 *	Data type:	Tango::DevDouble
	 *	Attr type:	Scalar 
	 */
	virtual void read_EngineeringPhaseSpeed(Tango::Attribute &attr);
	virtual void write_EngineeringPhaseSpeed(Tango::WAttribute &attr);
	virtual bool is_EngineeringPhaseSpeed_allowed(Tango::AttReqType type);



	/**
	 *	EngineeringGapAcceleration attribute related methods.
	 *	Description: Gap axes engineering movement average acceleration/deceleration in um/s^2.
	 *
	 *	Data type:	Tango::DevDouble
	 *	Attr type:	Scalar 
	 */
	virtual void read_EngineeringGapAcceleration(Tango::Attribute &attr);
	virtual void write_EngineeringGapAcceleration(Tango::WAttribute &attr);
	virtual bool is_EngineeringGapAcceleration_allowed(Tango::AttReqType type);



	/**
	 *	EngineeringPhaseAcceleration attribute related methods.
	 *	Description: Phase axes engineering movement average acceleration/deceleration in um/s^2.
	 *
	 *	Data type:	Tango::DevDouble
	 *	Attr type:	Scalar 
	 */
	virtual void read_EngineeringPhaseAcceleration(Tango::Attribute &attr);
	virtual void write_EngineeringPhaseAcceleration(Tango::WAttribute &attr);
	virtual bool is_EngineeringPhaseAcceleration_allowed(Tango::AttReqType type);



	/**
	 *	Interlock attribute related methods.
	 *	Description: Indicates that interlock is in effect.
	 *
	 *	Data type:	Tango::DevBoolean
	 *	Attr type:	Scalar 
	 */
	virtual void read_Interlock(Tango::Attribute &attr);
	virtual bool is_Interlock_allowed(Tango::AttReqType type);



	/**
	 *	Coil1 attribute related methods.
	 *	Description: Coil1 Current Setpoint/Readback
	 *
	 *	Data type:	Tango::DevDouble
	 *	Attr type:	Scalar 
	 */
	virtual void read_Coil1(Tango::Attribute &attr);
	virtual void write_Coil1(Tango::WAttribute &attr);
	virtual bool is_Coil1_allowed(Tango::AttReqType type);



	/**
	 *	Coil2 attribute related methods.
	 *	Description: Coil2 Current Setpoint/Readback
	 *
	 *	Data type:	Tango::DevDouble
	 *	Attr type:	Scalar 
	 */
	virtual void read_Coil2(Tango::Attribute &attr);
	virtual void write_Coil2(Tango::WAttribute &attr);
	virtual bool is_Coil2_allowed(Tango::AttReqType type);



	/**
	 *	Coil3 attribute related methods.
	 *	Description: Coil3 Current Setpoint/Readback
	 *
	 *	Data type:	Tango::DevDouble
	 *	Attr type:	Scalar 
	 */
	virtual void read_Coil3(Tango::Attribute &attr);
	virtual void write_Coil3(Tango::WAttribute &attr);
	virtual bool is_Coil3_allowed(Tango::AttReqType type);



	/**
	 *	Coil4 attribute related methods.
	 *	Description: Coil4 Current Setpoint/Readback
	 *
	 *	Data type:	Tango::DevDouble
	 *	Attr type:	Scalar 
	 */
	virtual void read_Coil4(Tango::Attribute &attr);
	virtual void write_Coil4(Tango::WAttribute &attr);
	virtual bool is_Coil4_allowed(Tango::AttReqType type);



	/**
	 *	CorrectionEnabled attribute related methods.
	 *	Description: Enables correction.
	 *
	 *	Data type:	Tango::DevBoolean
	 *	Attr type:	Scalar 
	 */
	virtual void read_CorrectionEnabled(Tango::Attribute &attr);
	virtual void write_CorrectionEnabled(Tango::WAttribute &attr);
	virtual bool is_CorrectionEnabled_allowed(Tango::AttReqType type);



	/**
	 *	Method      : Undulator::add_dynamic_attributes()
	 *	Description : Add dynamic attributes if any.
	 */
		void add_dynamic_attributes();

//	Command related methods
public: 


	/**
	 *	Command StopGap related methods.
	 */
	void stop_gap();
	virtual bool is_StopGap_allowed(const CORBA::Any &any);

	/**
	 *	Command ToggleStopAll related methods.
	 */
	void toggle_stop_all(Tango::DevBoolean argin);
	virtual bool is_ToggleStopAll_allowed(const CORBA::Any &any);

	/**
	 *	Command DisableLimits related methods.
	 */
	void disable_limits(Tango::DevBoolean argin);
	virtual bool is_DisableLimits_allowed(const CORBA::Any &any);

	/**
	 *	Command StopPhase related methods.
	 */
	void stop_phase();
	virtual bool is_StopPhase_allowed(const CORBA::Any &any);

	/**
	 *	Command CalibrateGap related methods.
	 */
	void calibrate_gap(const Tango::DevVarDoubleArray *argin);
	virtual bool is_CalibrateGap_allowed(const CORBA::Any &any);

	/**
	 *	Command CalibratePhase related methods.
	 */
	void calibrate_phase();
	virtual bool is_CalibratePhase_allowed(const CORBA::Any &any);

	/**
	 *	Command LoadCorrectionData related methods.
	 */
	void load_correction_data(Tango::DevString argin);
	virtual bool is_LoadCorrectionData_allowed(const CORBA::Any &any);



	/*----- PROTECTED REGION ID(Undulator::Additional Method prototypes) ENABLED START -----*/
protected:


private:

	/**
	 * Calculates final destinations of phase axes depending on desired phase.
	 *
	 * @param destinations Destinations vector to fill.
	 */
	void calculate_motor_destinations(vector<double> &destinations);


	/**
	 *  Gets offsets from all gap axes and stores them in offsetsGap array.
	 */
	void get_offsets_gap();

	/**
     *  Gets offsets from all phase axes and stores them in offsetsPhase array.
     */
    void get_offsets_phase();


	/**
	 * Takes user unit and converts it to counts.
	 *
	 * @param number User unit value to convert to counts.
	 * @param ratio Ratio to use for conversion.
	 * @return Converted user units in counts.
	 */
    double convert_to_counts(double number, double ratio);


    /**
     * Calculates software limits for gap and sets them on DMC.
     */
    void set_soft_limits_gap();

    /**
     * Calculates software limits for phase and sets them on DMC.
     */
    void set_soft_limits_phase();

    /**
     * Converts counts to micrometers using given ratio.
     *
     * @param value Value to convert to micrometers.
     * @param ratio Ratio to use for conversion.
     * @return Converted value.
     */
    double convert_to_micrometers(double value, double ratio);

    /**
     * Creates command to start desired gap movement.
     * Executes the command on DMC.
     */
    void start_gap();

    /**
     * Calculates final motor positions, sends required parameters
     * and starts phase movement on DMC.
     */
    void start_phase();

    /**
     * Switches to manual coil correction.
     */
    void start_coils_manual();

    /**
     * Switches to automatic coil correction.
     */
    void start_coils_automatic();


	/*----- PROTECTED REGION END -----*/	//	Undulator::Additional Method prototypes

};

	/*----- PROTECTED REGION ID(Undulator::Additional Classes Definitions) ENABLED START -----*/

	//	Additional Classes definitions

	/*----- PROTECTED REGION END -----*/	//	Undulator::Additional Classes Definitions

} //	namespace

#endif	//	UNDULATOR_H
