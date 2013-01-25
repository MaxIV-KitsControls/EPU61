#ifndef CORRECTIONCOILS_H_
#define CORRECTIONCOILS_H_

#include <tango.h>
#include <insertion/PollerThread.h>
#include <powersupply/PowerSupplies.h>

class DataItem;

namespace Undulator_ns
{

class Communicator;

/**
 * Handles loading of correction coil data and applying corrections to attributes representing power supplies.
 * Also updates insertion device positions (gap, center, taper, phase offset, phase mode) on the Undulator device implementation object.
 */
class CorrectionCoils : public omni_thread, public Tango::LogAdapter, public PollerThread::Processor
{
public:
	const static unsigned long MAX_SIGNAL_WAIT = 10; //!< Maximum number of seconds to wait for compensate command to be applied
public:
	/**
	 * Constructs the poller thread
	 *
	 * @param und Undulator device implementation
	 * @param opcProxy Name of the OPC device containing the power supply tags
	 * @param supplies Attribute names for each of the power supplies
	 */
	CorrectionCoils(Undulator &und, const std::string &opcProxy, const std::vector<std::string> &supplies);

	/**
	 * Destructor
	 */
	virtual ~CorrectionCoils ();


	virtual void process(Communicator *comm);

	/**
	 * If c is true the thread will do compensation on the power supply.
	 * If false and it is compensating it will wait for compensation to end before returning.
	 *
	 * Thread-safe.
	 *
	 * @param c new value for compensate flag
	 */
	void set_compensate(bool c);


	/**
	 * Returns true if automatic compensation is running
	 *
	 * Thread-safe.
	 */
	bool is_compensating() { return compensate; }


	/**
	 * Loads the Coil Correction table data from a local CSV file
	 *
	 * If compensation is running at the moment, stops compensating before loading the data.
	 * Thread-safe.
	 *
	 * @param filePath	device server local file-name with the CSV data
	 */
	void load_correction_data(const char *filePath);

private:
	/**
	 * Applies new correction currents given current conditions are as in active parameter.
	 *
	 * Uses nearest neighbor interpolation method.
	 *
	 * @param active object characterizing current gap , phase mode and phase offset as parameter
	 */
	void apply_correction(const DataItem &active);

private:
    volatile bool compensate; 		//!< Indicates whether the power supply should compensate according to the table.
    volatile bool compensateReq; 	//!< Previous value of compensate, used to detect transition

    omni_mutex	   condMutex;		//!< Mutex used by the waitSignal condition
    omni_condition waitSignal;		//!< Signal that is used when toggling compensation

    vector<DataItem> data[4];  	//!< Interpolation table, straight entries
    bool dataFileLoaded; 		//!< Data file for automatic compensation loaded.

    PowerSupplies *supplies; 	//!< Pointer to class controlling power supplies.

    Undulator &und;				//!< Reference to Undulator device implementation object
};

}

#endif /* CORRECTIONCOILS_H_ */
