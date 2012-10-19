#include "UpdaterThread.h"

#include <math.h>

namespace Undulator_ns
{
/**
 * Constructor for thread class.
 *
 * @param dev Reference to undulator device.
 * @param sleep_time Time in ms between updates.
 * @param gapAxes Proxys to axes moving gap.
 * @param phaseAxes Proxys to axes moving phase.
 * @param gearedAxes Proxys to upper and lower girder axes.
 */
UpdaterThread::UpdaterThread (Undulator& dev, ulong sleep_time, vector<string> &gapAxes,
		vector<string> &phaseAxes,vector<string> &gearedAxes) :
			omni_thread(),
			Tango::LogAdapter(&dev),
			run(true),
			dev(dev),
			sleepTime(sleep_time)
{
    flagDelay = false;

	// Set to zero
	gapA=gapB=gapC=gapD=phaseA=phaseB=phaseC=phaseD=0;
	gearedAxesUpper=gearedAxesLower=0;

	// Gap axes.
	gapA = new Tango::DeviceProxy(gapAxes[0]);
	gapB = new Tango::DeviceProxy(gapAxes[1]);
	gapC = new Tango::DeviceProxy(gapAxes[2]);
	gapD = new Tango::DeviceProxy(gapAxes[3]);

	// Phase axes.
	phaseA = new Tango::DeviceProxy(phaseAxes[0]);
	phaseB = new Tango::DeviceProxy(phaseAxes[1]);
	phaseC = new Tango::DeviceProxy(phaseAxes[2]);
	phaseD = new Tango::DeviceProxy(phaseAxes[3]);

	// Geared axes.
	gearedAxesUpper = new Tango::DeviceProxy(gearedAxes[0]);
	gearedAxesLower = new Tango::DeviceProxy(gearedAxes[1]);
}
/**
 * Destructor.
 * Deletes all device proxies if needed.
 */
UpdaterThread::~UpdaterThread()
{
	if (gapA) delete gapA;
	if (gapB) delete gapB;
	if (gapC) delete gapC;
	if (gapD) delete gapD;
	if (phaseA) delete phaseA;
	if (phaseB) delete phaseB;
	if (phaseC) delete phaseC;
	if (phaseD) delete phaseD;
	if (gearedAxesUpper) delete gearedAxesUpper;
	if (gearedAxesLower) delete gearedAxesLower;
}

/**
 * Starts the thread undetached.
 */
void UpdaterThread::go ()
{
	DEBUG_STREAM << "Updater::go" << endl;
	start_undetached();
}
/**
 * Main thread loop.
 */
void* UpdaterThread::run_undetached (void *arg)
{
	DEBUG_STREAM << "Updater::run_undetached" << endl;

	while (run)
	{
		DEBUG_STREAM << "Updater::checking axes states" << endl;

		// Update states and positions.
		try
		{
			check_axes_state();
			read_motor_pos();
		}
		catch(Tango::DevFailed& msg)
		{
			ERROR_STREAM << "Tango exception: " << msg.errors[0].reason << endl;
			dev.set_state(Tango::FAULT);
			break;
		}
		catch(std::exception& e)
		{
		    ERROR_STREAM << "STD exception: " << e.what() << endl;
		}

		catch(...)
		{
			ERROR_STREAM << "Unknown exception occured." << endl;
			dev.set_state(Tango::FAULT);
			break;
		}

		sleep(0, sleepTime * 1000000);
	}

	return 0;
}

/**
 * Stop the thread from running.
 */
void UpdaterThread::abort()
{
	DEBUG_STREAM << "Updater::abort" << endl;
	run = false;
}

/**
 * Checks the states of axes and updated Undulator device server state accordingly.
 * Sets the device to disabled if interlock or stop all are detected.
 */
void UpdaterThread::check_axes_state()
{
	// If interlock or stop all.
	if(dev.interlock || dev.stopAll)
	{
		dev.set_state(Tango::DISABLE);
	}
	else
	{
		// Read states.
		Tango::DevState stateA = gapA->state();
		Tango::DevState stateB = gapB->state();
		Tango::DevState stateC = gapC->state();
		Tango::DevState stateD = gapD->state();
		Tango::DevState stateE = phaseA->state();
		Tango::DevState stateF = phaseB->state();
		Tango::DevState stateG = phaseC->state();
		Tango::DevState stateH = phaseD->state();
		Tango::DevState stateUpper = gearedAxesUpper->state();
		Tango::DevState stateLower = gearedAxesLower->state();

		// Gearing check.
		bool upperGeared = false;
		bool lowerGeared = false;

		gearedAxesUpper->read_attribute("isGeared") >> (upperGeared);
		gearedAxesLower->read_attribute("isGeared") >> (lowerGeared);


		if(dev.state() == Tango::FAULT)
		{
			// Stay in fault.
		}
		// If all motors are in standby we set the state to standby.
		else if( (stateA == Tango::STANDBY) && (stateB == Tango::STANDBY) && (stateC == Tango::STANDBY)
			&& (stateD == Tango::STANDBY) && (stateE == Tango::STANDBY) && (stateF == Tango::STANDBY)
			&& (stateG == Tango::STANDBY) && (stateH == Tango::STANDBY))
		{
			dev.set_state(Tango::STANDBY);

			if(flagDelay)
			{
			    dev.gapMoving = false;
			    dev.phaseMoving = false;
			    flagDelay = false;
			}
			else if(dev.gapMoving || dev.phaseMoving)
			    flagDelay = true;
		}
		// If any motor is moving set moving state.
		else if( (stateA == Tango::MOVING) || (stateB == Tango::MOVING) || (stateC == Tango::MOVING)
			|| (stateD == Tango::MOVING) || (stateE == Tango::MOVING) || (stateF == Tango::MOVING)
			|| (stateG == Tango::MOVING) || (stateH == Tango::MOVING) )
		{
			dev.set_state(Tango::MOVING);
			flagDelay = true;
		}
		// In case of gearing additional check.
		else if ((upperGeared && (stateUpper == Tango::STANDBY)) || (lowerGeared && (stateLower == Tango::STANDBY)))
		{
			dev.set_state(Tango::STANDBY);
		}
		// If not moving or standby...(motor off, alarm...)
		else
		{
			dev.set_state(Tango::ALARM);
		}

	}
	// Display statuses.
	display_axes_status();
}
/**
 * Reads axes status and displays a message for user in Undulator device status.
 */
void UpdaterThread::display_axes_status()
{
	// Create status.
	stringstream cmd;
	cmd.str("");
	cmd << "AXIS A: " << gapA->status() << endl
		<< "AXIS B: " << gapB->status() << endl
		<< "AXIS C: " << gapC->status() << endl
		<< "AXIS D: " << gapD->status() << endl
		<< "AXIS E: " << phaseA->status() << endl
		<< "AXIS F: " << phaseB->status() << endl
		<< "AXIS G: " << phaseC->status() << endl
		<< "AXIS H: " << phaseD->status();

	// Set status.
	dev.set_status(cmd.str());
}
/**
 * Reads axes positions and uses that to calculate and update undulator parameters.
 */
void UpdaterThread::read_motor_pos()
{
	string attr = "position";
	Tango::DeviceAttribute devattr;
	double posArray[8];

	// Read all 8 positions.
	devattr = gapA->read_attribute(attr);
	devattr >> posArray[0];

	devattr = gapB->read_attribute(attr);
	devattr >> posArray[1];

	devattr = gapC->read_attribute(attr);
	devattr >> posArray[2];

	devattr = gapD->read_attribute(attr);
	devattr >> posArray[3];

	devattr = phaseA->read_attribute(attr);
	devattr >> posArray[4];

	devattr = phaseB->read_attribute(attr);
	devattr >> posArray[5];

	devattr = phaseC->read_attribute(attr);
	devattr >> posArray[6];

	devattr = phaseD->read_attribute(attr);
	devattr >> posArray[7];

	// Lock and update motor positions, taper, offset, gap, center.
	omni_mutex_lock guard(dev.deviceLock);

	// Calculate taper.
	dev.taper = (posArray[1] - posArray[0])+(posArray[2] - posArray[3]);

	// Calculate gap.
	dev.gap = (fabs(posArray[0] - posArray[2]) + fabs(posArray[1] - posArray[3]))/2;

	// Calculate center.
	dev.center = posArray[2] + (posArray[0] - posArray[2])/2;

	// Calculate offset.

	switch(dev.phase)
	{
	    case 0:
	            dev.phaseOffset = (posArray[5]+posArray[6])/2;
	            break;
	    case 1:
	            dev.phaseOffset = (posArray[4]+posArray[7])/2;
	            break;
	    case 2:
	            dev.phaseOffset = (posArray[5]-posArray[6])/2;
	            break;
	    case 3:
	            dev.phaseOffset = (posArray[4]-posArray[7])/2;
	            break;
	    default:
	        break;
	}

}

}
