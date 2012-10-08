#include "PollerThread.h"

#include "CoilCorrection.h"
#include "DataItem.h"

#include <string>
#include <math.h>
#include <limits>

using namespace std;

namespace CoilCorrection_ns
{

PollerThread::PollerThread(CoilCorrection &c, string controlBoxGapLink, string controlBoxPhaseLink)
    : omni_thread(),
      Tango::LogAdapter(&c),
      coilCorrection(c),
      run(true),
      compensate(false),
	  compensate_prev(false),
	  compensate_cond(&compensate_mutex)
{
	controlBoxGap = controlBoxPhase = 0;

	controlBoxGap = new Tango::DeviceProxy(controlBoxGapLink);
	controlBoxPhase = new Tango::DeviceProxy(controlBoxPhaseLink);
}

PollerThread::~PollerThread()
{
	if (controlBoxGap) delete controlBoxGap;
	if (controlBoxPhase) delete controlBoxPhase;
}

void PollerThread::go()
{
    start_undetached();
}

void PollerThread::abort()
{
	run = false;
}

void PollerThread::set_compensate(bool c)
{
	compensate_prev = compensate;
	compensate = c;

	// If required to stop compensation, wait for the condition to be processed
	if (compensate==false && compensate_prev==true)
	{
		compensate_cond.wait();
	}
}

void* PollerThread::run_undetached(void *)
{
	// Storage for read variables.
	double gap, taper, center, phaseOffset, phaseMode;

	// DMC delimiter for separating values.
	string delimiter;

	// Holding response from DMCs.
	Tango::DeviceData devdataGap;
	Tango::DeviceData devdataPhase;

	// Commands to send to DMCs.
	Tango::DeviceData commandGap;
	Tango::DeviceData commandPhase;

	// String commands.
	string cmdGap = "coilG=?;coilT=?;coilC=?";
	string cmdPhase = "coilPO=?;coilPM=?";

	// Command name on controlbox.
	string execute = "ExecLowLevelCmd";

	// Fill commands.
	commandGap << cmdGap;
	commandPhase << cmdPhase;

	// Used for parameter extraction.
	stringstream extractor;

	DataItem active;

	while(run)
	{
		// Signal that compensate off has been processed
		if (compensate==false && compensate_prev==true)
		{
			compensate_prev = compensate;
			compensate_cond.signal();
		}

		if (!compensate)
		{
			sleep(0,200*1000); // sleep 200 ms if compensation is not enabled
			continue;
		}

		try
		{
			// Send commands.
			devdataGap = controlBoxGap->command_inout(execute, commandGap);
			devdataPhase = controlBoxPhase->command_inout(execute, commandPhase);

			// Get string response from devdata.
			devdataGap >> delimiter;
			// Add string to stringstream for easier parameter extraction.
			extractor.str(delimiter);

			// Extract data.
			extractor >> gap;
			extractor >> delimiter;
			extractor >> taper;
			extractor >> delimiter;
			extractor >> center;

			// Get string response from devdata.
			devdataPhase >> delimiter;
			// Add string to stringstream for easier parameter extraction.
		    extractor.str(delimiter);

		    // Extract data.
		    extractor >> phaseOffset;
		    extractor >> delimiter;
		    extractor >> phaseMode;

			active.gap = gap;
			active.phase = phaseOffset;
			active.mode = (DataItem::PhaseMode)(int)phaseMode;

			active.normalize(); // puts all dimension values to 0 - 1.0

			apply_correction(active);

		}
		catch(Tango::DevFailed& err)
		{
			ERROR_STREAM << __PRETTY_FUNCTION__ << " failure: " << err.errors[0].reason << " origin:"
					<< err.errors[0].origin;

			coilCorrection.set_state(Tango::FAULT);
			break;
		}
		catch(...)
		{
			ERROR_STREAM << __PRETTY_FUNCTION__ << " unknown exception in the PollerThread!";
			coilCorrection.set_state(Tango::FAULT);
			break;
		}

	}

	run = false;

	return 0;
}


void PollerThread::apply_correction(const DataItem &active)
{
	int minItem=-1; 							  // nearest item position in the table
	double minDist=numeric_limits<double>::max(); // minimum distance from the nearest item

	// find nearest item in the table
	for (size_t i=0;i<coilCorrection.data.size();i++)
	{
		const DataItem &di = coilCorrection.data[i];
		if (di.mode!=active.mode) continue;

		double dist = sqrt(pow(di.gap-active.gap,2) + pow(di.phase-active.phase,2));
		if (dist<minDist)
		{
			minDist=dist;
			minItem=i;
		}
	}

	// ToDo: Should we silently ignore this ?
	if (minItem==-1)
		Tango::Except::throw_exception("No neighbors found in the interpolation table","",__PRETTY_FUNCTION__);

	// Apply the correction on the devices
	for (int i=0;i<4;i++)
		coilCorrection.supplies[i].set_current( coilCorrection.data[minItem].coils[i] );
}

}
