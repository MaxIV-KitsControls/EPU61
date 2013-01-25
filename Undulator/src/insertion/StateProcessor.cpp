/*
 * StateProcessor.cpp
 *
 *  Created on: Jan 8, 2013
 *      Author: green
 */

#include "StateProcessor.h"

#include <Undulator.h>

namespace Undulator_ns
{

StateProcessor::StateProcessor(Undulator& dev, vector<string> &gapAxes, vector<string> &phaseAxes,vector<string> &gearedAxes)
		:	dev(dev)
{
    cout << "Initializing StateProcessor" << endl;
   
	// Initialize the device proxies
	// Set to zero
	gapA=gapB=gapC=gapD=phaseA=phaseB=phaseC=phaseD=0;

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
    
    cout << "StateProcessor initialized" << endl;
}

StateProcessor::~StateProcessor() {
	if (gapA) delete gapA;
	if (gapB) delete gapB;
	if (gapC) delete gapC;
	if (gapD) delete gapD;
	if (phaseA) delete phaseA;
	if (phaseB) delete phaseB;
	if (phaseC) delete phaseC;
	if (phaseD) delete phaseD;
}

void StateProcessor::process(Communicator *comm)
{
	check_axes_state();
}

void StateProcessor::check_axes_state()
{
	if(dev.state() == Tango::FAULT) return; // Stay in fault.

	// If interlock or stop all.
	if(!dev.get_interlock() && !dev.get_stop_all())
	{
		if (dev.get_axis_flags())
		{
			// No Interlock or StopAll but axes flags is containing an error
			dev.set_state(Tango::ALARM); // Report alarm
		} else
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

			// If any motor is moving set moving state.
			if( (stateA == Tango::MOVING) || (stateB == Tango::MOVING) || (stateC == Tango::MOVING)
				|| (stateD == Tango::MOVING) || (stateE == Tango::MOVING) || (stateF == Tango::MOVING)
				|| (stateG == Tango::MOVING) || (stateH == Tango::MOVING) )
			{
				dev.set_state(Tango::MOVING);
			} else
			{
				dev.set_state(Tango::STANDBY);
			}
		}
	} else
	{
		dev.set_state(Tango::DISABLE);
	}
	// Display statuses.
	display_axes_status();
}

void StateProcessor::display_axes_status()
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

} /* namespace Undulator_ns */
