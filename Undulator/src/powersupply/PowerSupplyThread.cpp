#include "PowerSupplyThread.h"

namespace Undulator_ns
{

PowerSupplyThread::PowerSupplyThread(string opcProxy, vector<string> &attributes) : omni_thread()
{
    opcRead = new Tango::DeviceProxy(opcProxy);
    opcWrite = new Tango::DeviceProxy(opcProxy);
    for(uint i = 0; i < attributes.size(); i++)
    {
        currents.push_back(0);
        attributeNames.push_back(attributes[i]);
    }
    run = true;
}

void PowerSupplyThread::go ()
{
    start_undetached();
}

void PowerSupplyThread::abort()
{
    run = false;
}

void* PowerSupplyThread::run_undetached (void *arg)
{
    uint size = currents.size();
    short tempCurrent[size];

    while (run)
    {
        try
        {
            for(uint i = 0; i < size; i++)
            {
                opcRead->read_attribute(attributeNames[i]) >> tempCurrent[i];
            }

            {   // CRITICAL SECTION
                omni_mutex_lock guard(currentLock);
                for(uint i = 0; i < size; i++)
                    currents[i] = tempCurrent[i];
            }
            sleep(1);
        }
        catch(Tango::DevFailed& msg)
        {
            errorMsg << "Tango exception in OPC server communication: " << msg.errors[0].reason << endl;
            run = false;
        }
        catch(std::exception& e)
        {
            errorMsg << "STD exception in OPC communication: " << e.what() << endl;
            run = false;
        }
        catch(...)
        {
            errorMsg << "Unknown exception in OPC communication occurred." << endl;
            run = false;
        }
    }

    return 0;
}

double PowerSupplyThread::get_current(uint index)
{
    // Lock and get value if possible.
    omni_mutex_lock guard(currentLock);
    if (index < currents.size())
        return currents[index];
    else
        return 0;
}

void PowerSupplyThread::set_current(double val, uint index)
{
    // Write value to OPC
    Tango::DeviceAttribute devattr(attributeNames[index], val);
    opcWrite->write_attribute_asynch(devattr);
}

bool PowerSupplyThread::isRunning()
{
    return run;
}

string PowerSupplyThread::getError()
{
    return errorMsg.str();
}

PowerSupplyThread::~PowerSupplyThread()
{
    if(opcRead) delete opcRead;
    if(opcWrite) delete opcWrite;
}

}
