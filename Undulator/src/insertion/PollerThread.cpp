#include "PollerThread.h"

#include <math.h>

#include <Undulator.h>
#include <Util.h>

#include "Communicator.h"

namespace Undulator_ns
{

PollerThread::PollerThread (Undulator& dev, ulong sleepTime, const std::string &gapProxy, const std::string &phaseProxy) :
			omni_thread(),
			dev(dev),
			Tango::LogAdapter(&dev),
			sleepTime(sleepTime)
{
    cout << "Initializing PollerThread" << endl;
    
	std::vector<std::string> communicatorParams;
	communicatorParams.push_back(gapProxy);
	communicatorParams.push_back(phaseProxy);
	comm = Communicator::create_communicator(communicatorParams);

    cycleCounter = 0;
    
    cout << "PollerThread Initialized" << endl;
}

PollerThread::~PollerThread()
{
	if (comm)
	{
		delete comm;
		comm = 0;
	}
}

void PollerThread::go ()
{
	DEBUG_STREAM << "PollerThread::go" << endl;
	run = true;
	previousTime = time(NULL);
	start_undetached();
}

// Main thread loop
void* PollerThread::run_undetached (void *arg)
{
	DEBUG_STREAM << "PollerThread::run_undetached" << endl;

	const int REPORT_EVERY = 10000;

	while (run)
	{
		cycleCounter++;
    
		if (cycleCounter%REPORT_EVERY==0)
		{
			time_t now = time(NULL);

			u_int64_t diff = now-previousTime;
			// Avg time will contain the cycle time in ms
			double avgTime = (diff * 1000.0f) / ((double) REPORT_EVERY);

			cout << "ping " << cycleCounter/REPORT_EVERY << " avg. cycle time: " << avgTime << " ms " << endl;
            dev.update_cycle_time(avgTime);

			previousTime = now;
		}

		for(size_t i=0;i<processors.size() && run;i++)
		{
			ProcessorItem &pi=processors[i];

			// Execute the appropriate processor in the correct cycle
			if (cycleCounter%pi.first==0)
			{
				// Update states and positions.
				try
				{
					pi.second->process(comm);
				}
				catch(Tango::DevFailed& msg)
				{
					stringstream ss;
					ss << "Tango exception: " << msg.errors[0].reason;
					ERROR_STREAM << ss.str();
					cerr << ss.str() << endl;

					dev.set_state(Tango::FAULT);
					abort();
				}
				catch(std::exception& e)
				{
					stringstream ss;
					ss << "STD exception: " << e.what();
					ERROR_STREAM << ss.str();
					cerr << ss.str() << endl;

					dev.set_state(Tango::FAULT);
					abort();
				}
				catch(...)
				{
					stringstream ss;

					ss << "Unknown exception occurred.";
					ERROR_STREAM << ss.str();
					cerr << ss.str() << endl;

					dev.set_state(Tango::FAULT);
					abort();
				}
			}
		}

		usleep(sleepTime);
	}

	return 0;
}

void PollerThread::abort()
{
	DEBUG_STREAM << "PollerThread::abort" << endl;
	run = false;
}

}
