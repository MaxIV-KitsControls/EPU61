#include "PollerThread.h"

#include "Undulator.h"
#include "DataItem.h"

#include <string>
#include <math.h>
#include <limits>

using namespace std;

namespace Undulator_ns
{

PollerThread::PollerThread(Undulator &c, string controlBoxGapProxy, string controlBoxPhaseProxy)
    : omni_thread(),
      Tango::LogAdapter(&c),
      undulator(c),
      run(true),
      compensate(false),
	  compensate_prev(false),
	  compensate_cond(&compensate_mutex)
{
    galil = 0;
	galil = new GalilCommunicator(controlBoxGapProxy, controlBoxPhaseProxy);
}

PollerThread::~PollerThread()
{
    if (galil) delete galil;
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
	double gap, phaseOffset, phaseMode;

	DataItem active;
	coilData data;

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
		    // Get gap, phase offset and phase mode from DMC.
		    galil->get_coil_correction_data(data);

			active.gap = data.gap;
			active.phase = data.phaseOffset;
			active.mode = (DataItem::PhaseMode)data.phaseMode;

			active.normalize(); // puts all dimension values to 0 - 1.0

			apply_correction(active);
		}
		catch(Tango::DevFailed& err)
		{
			ERROR_STREAM << __PRETTY_FUNCTION__ << " failure: " << err.errors[0].reason << " origin:"
					<< err.errors[0].origin;

			cout << err.errors[0].reason << endl;

			undulator.set_state(Tango::FAULT);
			break;
		}
		catch(...)
		{
			ERROR_STREAM << __PRETTY_FUNCTION__ << " unknown exception in the PollerThread!";
			cout << "UNKNOWN" << endl;
			undulator.set_state(Tango::FAULT);
			break;
		}
		usleep(20000);

	}

	run = false;

	return 0;
}


void PollerThread::apply_correction(const DataItem &active)
{
	int minItem=-1; 							  // nearest item position in the table
	double minDist=numeric_limits<double>::max(); // minimum distance from the nearest item

	// find nearest item in the table
	for (size_t i=0;i<undulator.data.size();i++)
	{
		const DataItem &di = undulator.data[i];
		if (di.mode!=active.mode) continue;

		double dist = sqrt(pow(di.gap-active.gap,2) + pow(di.phase-active.phase,2));
		if (dist<minDist)
		{
			minDist=dist;
			minItem=i;
		}
	}

	// ToDo: Should we silently ignore this ?
	/*
	if (minItem==-1)
		Tango::Except::throw_exception("No neighbors found in the interpolation table","",__PRETTY_FUNCTION__);
	 */
	// Apply the correction on the devices
	for (int i=0;i<4;i++)
	    undulator.supplies[i].set_current( undulator.data[minItem].coils[i] );
}

}
