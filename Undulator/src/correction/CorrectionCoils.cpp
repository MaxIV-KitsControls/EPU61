#include "CorrectionCoils.h"
#include <powersupply/PowerSupplies.h>
#include <insertion/Communicator.h>

#include "Undulator.h"
#include "DataItem.h"

#include <string>
#include <math.h>
#include <limits>

using namespace std;

namespace Undulator_ns
{

CorrectionCoils::CorrectionCoils(Undulator &und, const std::string &opcProxy, const std::vector<std::string> &attr_supplies)
    :	und(und),
    	omni_thread(),
		Tango::LogAdapter(&und),
		compensate(false),
		compensateReq(false),
		dataFileLoaded(false),
		waitSignal(&condMutex)
{
	supplies = 0;
	supplies = new PowerSupplies(&und, opcProxy, attr_supplies);

	cout << "CorrectionCoils constructed" << endl;
}

CorrectionCoils::~CorrectionCoils()
{
	if (supplies)
	{
		delete supplies;
		supplies=0;
	}
}

void CorrectionCoils::set_compensate(bool c)
{
	// Data must be loaded for compensation to work
	if(c && !dataFileLoaded)
		Tango::Except::throw_exception("Data not loaded.", "Correction coil data file is not loaded.", __PRETTY_FUNCTION__);

	if (compensate != c)
	{
		compensateReq = c; 								// request change in the compensate flag
		unsigned long waitSec=0, waitNSec=0;
		omni_thread::get_time(&waitSec,&waitNSec,MAX_SIGNAL_WAIT, 0);
		if (!waitSignal.timedwait(waitSec,waitNSec)) 	// wait for the other thread to process it
		{
			Tango::Except::throw_exception("Time-out while signaling correction coil thread.","Is the EPU in FAULT state?",__PRETTY_FUNCTION__);
		}
	}
}

void CorrectionCoils::process(Communicator *comm)
{
	IDPosition data;
	comm->get_id_position(data);

	if (compensate)
	{
		DataItem active;
		active.gap 	 = data.gap;
		active.phase = data.phaseOffset;
		active.mode  = (DataItem::PhaseMode)data.phaseMode;

		active.normalize(); // puts all dimension values to 0 - 1.0

		apply_correction(active);
	}

	// Change compensation flag and signal that it has been processed
	if (compensate!=compensateReq)
	{
		compensate=compensateReq;
		waitSignal.signal();
	}

	und.update_position(data);
}


void CorrectionCoils::apply_correction(const DataItem &active)
{
	int minItem=-1; 							  // nearest item position in the table
	double minDist=numeric_limits<double>::max(); // minimum distance from the nearest item

	// find nearest item in the table
	size_t vectPos = (size_t) active.mode;
	for (size_t i=0;i<data[vectPos].size();i++)
	{
		const DataItem &di = data[vectPos][i];

		double dist = sqrt(pow(di.gap-active.gap,2) + pow(di.phase-active.phase,2));
		if (dist<minDist)
		{
			minDist=dist;
			minItem=i;
		}
	}

	// if item found, apply correction
	if (minItem!=-1)
	{
		// Apply the correction on the devices
		for (int i=0;i<4;i++)
			supplies->set_current(data[vectPos][minItem].coils[i], i);
	}
}

void CorrectionCoils::load_correction_data(const char *path)
{
	// Check if compensation is running , if so stop it
	if (is_compensating())
		set_compensate(false); // will wait for a process call to finish

	ifstream inFile(path);
	dataFileLoaded=false;

    if (!inFile.good())
    {
        Tango::Except::throw_exception("Invalid filename provided.","The file-name in the argument could not be opened.",__PRETTY_FUNCTION__);
    }

    int n = count(istreambuf_iterator<char>(inFile), istreambuf_iterator<char>(), '\n');

    if (n<=1)
    {
        Tango::Except::throw_exception("The file does not contain any data.","",__PRETTY_FUNCTION__);
    }

    for (size_t i=0;i<4;i++) {
		data[i].clear();
		data[i].reserve(n/2);
    }

    inFile.seekg(0,ios_base::beg);

    string line;
    // read the first header line
    std::getline(inFile,line);

    // Load the data
    DataItem item;
    while (true)
    {
	    inFile >> item;
	    if (!inFile.good()) break;

	    item.normalize();

	    // Save the item in the appropriate (PhaseMode wise) vector
	    data[(size_t)item.mode].push_back(item);
    }

    dataFileLoaded = true;
}

}
