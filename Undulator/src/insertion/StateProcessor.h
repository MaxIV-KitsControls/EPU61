

#ifndef STATEPROCESSOR_H_
#define STATEPROCESSOR_H_

#include "PollerThread.h"

namespace Undulator_ns
{

class Communicator;
/**
 * A PollerThread::Processor that updates the Undulator device state based on
 * the state of the individual GalilAxis
 */
class StateProcessor :
		public PollerThread::Processor {
public:
	/**
	 * Constructs the object.
	 *
	 * @param dev A reference to the Undulator device implementation class.
	 * @param gapAxes Vector of strings naming the proxy devices for the Z1-Z4 Gap axes
	 * @param phaseAxes Vector of strings naming the proxy devices for the X1-X4 Phase axes
	 * @param gearedAxes Vector of strings naming the proxy devices for the upper and lower girder coupled axes
	 */
	StateProcessor(Undulator& dev, vector<string> &gapAxes, vector<string> &phaseAxes,vector<string> &gearedAxes);
	virtual ~StateProcessor();

	virtual void process(Communicator *comm);

private:
	/**
	* Checks all axes and sets appropriate status and state.
	*/
	void check_axes_state();

	/**
	 * Displays statuses of all axes to status box.
	 */
	void display_axes_status();

private:
	Undulator& dev; //!< Reference to instance of the affected undulator device.

	Tango::DeviceProxy *gapA; //!< Pointer to gap axis A.
	Tango::DeviceProxy *gapB; //!< Pointer to gap axis B.
	Tango::DeviceProxy *gapC; //!< Pointer to gap axis C.
	Tango::DeviceProxy *gapD; //!< Pointer to gap axis D.
	Tango::DeviceProxy *phaseA; //!< Pointer to phase axis A.
	Tango::DeviceProxy *phaseB; //!< Pointer to phase axis B.
	Tango::DeviceProxy *phaseC; //!< Pointer to phase axis C.
	Tango::DeviceProxy *phaseD; //!< Pointer to phase axis D.
};

}  /* namespace Undulator_ns */

#endif /* STATEPROCESSOR_H_ */
