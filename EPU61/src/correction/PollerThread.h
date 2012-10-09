#ifndef POLLERTHREAD_H_
#define POLLERTHREAD_H_

#include <tango.h>

class DataItem;

namespace Undulator_ns
{

// Forward declaration of the device server implementation class.
class Undulator;

/**
 * This thread polls the DMC via ControlBox for getting the most up-to-date values for gap,phase and phase mode
 * and sends the appropriate values to the power supplies.
 */
class PollerThread : public omni_thread, public Tango::LogAdapter
{
public:
	/**
	 * Constructs the poller thread
	 *
	 * @param cc
	 * @param controlBoxGapLink
	 * @param controlBoxPhaseLink
	 */
	PollerThread(Undulator &cc, std::string controlBoxGapLink, std::string controlBoxPhaseLink);

	/**
	 * Starts the thread.
	 */
	void go();

	/**
	 * Stops the thread.
	 */
	void abort();

	/**
	 * If c is true the thread will do compensation on the power supply.
	 *
	 * @param c
	 */
	void set_compensate(bool c);

	/**
	 * Applies new correction currents given current conditions are as in active parameter.
	 *
	 * Uses nearest neighbor interpolation method.
	 *
	 * @param active object characterizing current gap , phase mode and phase offset as parameter
	 */
	void apply_correction(const DataItem &active);

	/**
	 * Starts the thread
	 *
	 * @param arg argument passed to the thread main function
	 */
	virtual void* run_undetached(void *arg);

protected:
    ~PollerThread ();

private:
    Undulator &undulator;  //!< Reference to the coil correction device server implementation class.

    volatile bool run; 		//!< Used for running thread.
    volatile bool compensate; 		//!< Indicates whether the power supply should compensate according to the table.
    volatile bool compensate_prev;	//!< Previous value of the compensate flag

    omni_mutex compensate_mutex;	//!< Required mutex for the condition variable compensate_cond
    omni_condition compensate_cond;	//!< Condition variable signaling that compensate has been turned off

    Tango::DeviceProxy *controlBoxGap; //!< Pointer to control box moving gap.
    Tango::DeviceProxy *controlBoxPhase; //!< Pointer to control box moving phase.
};

}


#endif /* POLLERTHREAD_H_ */
