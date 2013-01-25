#ifndef EPU61DATA_H_
#define EPU61DATA_H_

namespace Undulator_ns {

/**
 * Structure that represents an insertion device position
 */
struct IDPosition
{
    double 	gap; 			//!< Current gap.
    double 	center;			//!< Current center
    double 	taper;			//!< Current taper

    short 	phaseMode; 		//!< Current phase mode.
    double 	phaseOffset; 	//!< Current phase offset.
};

/**
 * Structure that represents the insertion device state
 */
struct IDStatus
{
	bool interlock;		//!< Interlock active
	bool stopAll;		//!< StopAll active

	bool gapMoving;		//!< Gap is moving
	bool phaseMoving;	//!< Phase is moving

	/**
	 * Represents bit-encoded axis flags
	 *
	 * Currently:
	 * Least significant byte, bits 0..7 represent communication error status for motors 1 to 8 respectively
	 * More significant byte, bits 8..15 represent encoder error status for motors 1 to 8 respectively
	 * More significant byte, bits 15..23 represent drive error status for motors 1 to 8 respectively
	 *
	 */
	unsigned int axesFlags;
};

/**
 * This structure contains all parameters that are set during gap movement.
 */
struct GapParameters
{
    double gap; //!< Desired gap.
    double offset; //!< Desired offset.
    double taper; //!< Desired taper.
    double gapSpeed; //!< Desired gap speed.
    double taperSpeed; //!< Desired taper speed.
    double gapAcceleration; //!< Desired gap acceleration and deceleration.
    double taperAcceleration; //!< Desired taper acceleration and deceleration.
};

/**
 * This structure contains all parameters that are set during phase movement.
 */
struct PhaseParameters
{
    std::vector<double> finalAxisPos; //!< Final axes positions based on desired phase mode without offsets applied.
    short phaseMode; //!< Desired phase mode.
    double phaseSpeed; //!< Desired phase speed.
    double phaseAcceleration; //!< Desired phase acceleration and deceleration.
};

} /* namespace Undulator_ns */

#endif /* EPU61DATA_H_ */
