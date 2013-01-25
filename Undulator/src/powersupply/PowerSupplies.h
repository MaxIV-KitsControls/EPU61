

#ifndef POWERSUPPLIES_H_
#define POWERSUPPLIES_H_


#include "tango.h"

namespace Undulator_ns
{

/**
 * Concrete class implementing functionality to set coil power supplies currents via Tango attributes.
 */
class PowerSupplies
	: public Tango::LogAdapter
{
	/**
	 * Specifies the minimum time that must pass between updates
	 */
	const static unsigned int MINIMUM_UPDATE_TIME_MS = 30;

	/**
	 * Specifies the minimum change in output before generating an update request.
	 */
	const static double MINIMUM_CHANGE = 0.005;
public:
    /**
     * Constructs a PowerSupply object
     *
     * @param dev Device implementation class used for logging
     * @param opcProxy Name of the device containing the power supply attributes
     * @param attributes Attribute names for four power supplies
     */
    PowerSupplies(Tango::DeviceImpl *dev,const string &opcProxy, const vector<string> &attributes);

    /**
     * Destructor.
     */
    virtual ~PowerSupplies();

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

private:
    Tango::DeviceProxy *opcDev; 				//!< Proxy to OPC device server for writing.
    vector<Tango::DeviceAttribute> attributes; 	//!< Attributes

    vector<double> 		cachedValues;	//!< Contains previously applied value. To prevent needless updates.
    vector<u_int64_t>	prevTime; 		//!< Contains the time of last update in ms
};

}

#endif /* POWERSUPPLIES_H_ */
