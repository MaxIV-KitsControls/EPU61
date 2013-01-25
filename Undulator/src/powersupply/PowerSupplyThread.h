

#ifndef POWERSUPPLYTHREAD_H_
#define POWERSUPPLYTHREAD_H_


#include "tango.h"

namespace Undulator_ns
{

/**
 * This PowerSupply class is to provide an interface to actual power supply device.
 */
class PowerSupplyThread : public omni_thread
{
public:
    /**
     * Constructs a PowerSupply object
     */
    PowerSupplyThread(string opcProxy, vector<string> &attributes);

    /**
     * Starts the thread.
     */
    void go();

    /**
     * Sets new supply current.
     *
     * @param val desired current in ampers
     */
    void set_current(double val, uint index);

    /**
     * Reads the current from the power supply.
     *
     * @return the current on the output
     */
    double get_current(uint index);

    /**
     * Destructor.
     */
    ~PowerSupplyThread();

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
     * Returns state of the thread.
     *
     * @return Thread running.
     */
    bool isRunning();

    /**
     * If error occurred the message can be retrieved.
     *
     * @return Error message.
     */
    string getError();


private:

    Tango::DeviceProxy *opcRead; //!< Proxy to OPC device server for reading.
    Tango::DeviceProxy *opcWrite; //!< Proxy to OPC device server for writing.
    vector<string> attributeNames; //!< Name of powersupply's representing attribute.
    vector<double> currents; //!< Buffer for currents read from OPC.
    omni_mutex currentLock; //!< Lock for buffer.
    volatile bool run; //!< Thread flag.
    stringstream errorMsg; //!< If error occurred, this will hold the reason.

};

}








#endif /* POWERSUPPLYTHREAD_H_ */
