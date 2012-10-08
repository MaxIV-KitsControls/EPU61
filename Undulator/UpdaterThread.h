#ifndef UPDATERTHREAD_H_
#define UPDATERTHREAD_H_

#include "Undulator.h"

namespace Undulator_ns
{

/**
 * Class in a separate thread updating states and movement variables.
 */
class UpdaterThread :
	public omni_thread, public Tango::LogAdapter
{
public:
	/**
	 * Constructor of updater class.
	 * @param dev Reference to undulator device.
	 * @param sleep_time Time in milliseconds between updates.
	 * @param gapAxes Links to axes moving gap.
	 * @param phaseAxes Links to axes moving phase.
	 * @param gearedAxes Links to upper and lower girder axes.
	 */
	UpdaterThread(Undulator& dev, ulong sleep_time, vector<string> &gapAxes, vector<string> &phaseAxes,vector<string> &gearedAxes);

	/**
	 * Starts the thread.
	 */
	void go();

	/**
	 * Runs the loop until user stops it or gets destroyed.
	 *
	 * @param arg Argument passed to the thread
	 */
	virtual void* run_undetached(void *arg);

	/**
	 * Stops the thread.
	 */
	void abort();

	/**
	* Checks all axes and sets appropriate status and state.
	*/
	void check_axes_state();

	/**
	 * Displays statuses of all axes to status box.
	 */
	void display_axes_status();

	/**
	 * Reads current position from each axis and calculates all position variables.
	 */
	void read_motor_pos();


protected:
	/**
	 * Destructor for the thread object
	 */
	~UpdaterThread();


// Private variables.
private:
	volatile bool run;	//!< Flag signaling whether the thread loop should run.
	Undulator& dev;		//!< Reference to instance of the affected undulator device.
	ulong sleepTime;	//!< Sleep time between iterations of the scanning in the thread main loop.

	Tango::DeviceProxy *gapA; //!< Pointer to gap axis A.
	Tango::DeviceProxy *gapB; //!< Pointer to gap axis B.
	Tango::DeviceProxy *gapC; //!< Pointer to gap axis C.
	Tango::DeviceProxy *gapD; //!< Pointer to gap axis D.
	Tango::DeviceProxy *phaseA; //!< Pointer to phase axis A.
	Tango::DeviceProxy *phaseB; //!< Pointer to phase axis B.
	Tango::DeviceProxy *phaseC; //!< Pointer to phase axis C.
	Tango::DeviceProxy *phaseD; //!< Pointer to phase axis D.

	Tango::DeviceProxy *gearedAxesUpper; //!< Pointer to upper girder.
	Tango::DeviceProxy *gearedAxesLower; //!< Pointer to lower girder.
};

}

#endif /* UPDATERTHREAD_H_ */
