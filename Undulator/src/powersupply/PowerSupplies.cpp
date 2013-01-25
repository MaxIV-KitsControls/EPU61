#include "PowerSupplies.h"

#include <Util.h>
#include <math.h>

namespace Undulator_ns
{

PowerSupplies::PowerSupplies(Tango::DeviceImpl *dev, const string &opcProxy, const vector<string> &attrs)
	: Tango::LogAdapter(dev), opcDev(0), cachedValues(4,0), prevTime(4,0)
{
    cout << "Initializing PowerSupplies" << endl;

	opcDev = new Tango::DeviceProxy(opcProxy.c_str());

	// Construct attributes
	for (size_t i=0;i<4;i++)
	{
		attributes.push_back(Tango::DeviceAttribute(attrs[i].c_str(),(double)0.0f));
	}


    
    cout << "PowerSupplies Initialized" << endl;
}

void PowerSupplies::set_current(double val, uint index)
{
	if (!opcDev)
	{
		// Fail silently, initialization error will be thrown in construction if opcDev was not created
		return;
	}

	if (index>=cachedValues.size())
		return;

	bool doSend = true;

	// Check if the value changed considerably
	if (fabs(cachedValues[index]-val) < MINIMUM_CHANGE)
		doSend = false;

	// Get current time
	u_int64_t now = get_miliseconds();
	u_int64_t timediff = now - prevTime[index];

	if (timediff<MINIMUM_UPDATE_TIME_MS)
		doSend = false;

	if (doSend)
	{
		cout << "Correcting coil " << index << " with: " << val << endl;

		// Write value to OPC
		attributes[index] << val;
		opcDev->write_attribute_asynch(attributes[index]);

		// Record this time
		prevTime[index] = now;
		// Record this value
		cachedValues[index] = val;
	}
}

PowerSupplies::~PowerSupplies()
{
    if(opcDev)
    {
    	delete opcDev;
    	opcDev = 0;
    }
}

}
