#include "GalilCommunicator.h"

#include <iomanip>


namespace Undulator_ns
{

GalilCommunicator::GalilCommunicator(const string &gapProxy, const string &phaseProxy)
{
    cout << "Initializing GalilCommunicator" << endl;
    
    controlBoxGap=controlBoxPhase=0;

    // Create device proxys.
    controlBoxGap = new Tango::DeviceProxy(gapProxy.c_str());
    controlBoxPhase = new Tango::DeviceProxy(phaseProxy.c_str());
    
    cout << fixed << "GalilCommunicator Initialized." << endl;
}

GalilCommunicator::~GalilCommunicator()
{
    if (controlBoxGap) delete controlBoxGap;
    if (controlBoxPhase) delete controlBoxPhase;
}
void GalilCommunicator::start_gap_movement(const GapParameters &parameters)
{
    // Create command to send. Send center,gap and taper parameters.
    stringstream cmd;

    cmd << setprecision(4);

    // Rest of command.
    cmd << "GapOfst="
        << fixed << parameters.offset
        << ";Gap="
        << fixed << parameters.gap
        << ";Taper="
        << fixed << parameters.taper
        << ";TprSpd="
        << fixed << parameters.taperSpeed
        << ";GapSpd="
        << fixed << parameters.gapSpeed
        << ";GapAcc="
        << fixed << parameters.gapAcceleration
        << ";TprAcc="
        << fixed << parameters.taperAcceleration
        << ";XQ#GAP,7"; // Execute movement on 7th thread.


    send_command_string(cmd.str().c_str(), *controlBoxGap);

    // If we don't get positive confirmation of all commands throw exception.
    if (stringResponse.compare("::::::::") != 0)
    {
        Tango::Except::throw_exception("Error starting gap movement.",
            "Start gap movement command failed.","Start gap movement.",Tango::ERR);
    }
}

void GalilCommunicator::start_phase_movement(const PhaseParameters &parameters)
{
    // Create command to send.
    stringstream cmd;

    cmd << setprecision(4);

    // Final position part of command.
    for(int i = 0; i < 4; i++)
    {
        cmd << "PhDest[" << i << "]=" << fixed << parameters.finalAxisPos[i] << ";";
    }

    // Rest of command.
    cmd << "PhMdRq="
        << parameters.phaseMode
        << ";PhSpd="
        << parameters.phaseSpeed
        << ";PhAcc="
        << parameters.phaseAcceleration
        << ";XQ#PHASE,7";


    send_command_string(cmd.str().c_str(), *controlBoxPhase);

    // If we get positive confirmation of all commands.
    if (stringResponse.compare("::::::::") != 0)
        Tango::Except::throw_exception("Error starting phase movement.",
            "Start phase movement command failed.","Start phase movement.",Tango::ERR);
}

void GalilCommunicator::stop_gap()
{
    // Sends command to stop gap motors.
	// Stops the movement thread, stops movement in the appropriate coordinate system and clears
	// the moving flag
    send_command_string("StopGap=1\r", *controlBoxGap);

    // If command failed throw exception.
    if(stringResponse.compare(":") != 0)
        Tango::Except::throw_exception("Stop gap command couldn't be sent to DMC.","Stop gap command couldn't be sent to DMC.","Stop gap.",Tango::ERR);
}

void GalilCommunicator::stop_phase()
{
    // Sends command to stop phase motors.
	// Stops the movement thread, stops movement in the appropriate coordinate system and clears
	// the moving flag [ inverse logic ]
	send_command_string("StopPh=1\r", *controlBoxPhase);

    // If command failed throw exception.
    if(stringResponse.compare(":") != 0)
        Tango::Except::throw_exception("Stop phase command couldn't be sent to DMC.","Stop phase command couldn't be sent to DMC.","Stop phase.",Tango::ERR);
}

void GalilCommunicator::toggle_stop_all(bool confirm)
{
    string command;

    const char *cmd = confirm ? "StopRq=1" : "StopRq=0";

    bool error = false;

    // Sends command to stop gap motors.
    send_command_string(cmd, *controlBoxGap);

    // Check for error.
    if(stringResponse.compare(":") != 0)
        error = true;

    // Sends command to stop phase motors.
    send_command_string(cmd, *controlBoxPhase);

    // Check for error.
    if(stringResponse.compare(":") != 0)
        error = true;

    // Handle error
    if(error)
        Tango::Except::throw_exception("Stop command couldn't be sent to DMC.","Stop command couldn't be sent to DMC.","Stop all command.",Tango::ERR);
}

void GalilCommunicator::get_gap_linear_encoder_pos(vector<double> &encoderPos)
{
    // Read aux encoder positions.
	send_command_stream("TDA;TDB;TDC;TDD", *controlBoxGap);

    string delimiter;
    double temp[4];

    // Clear any previous values.
    encoderPos.clear();

    // Extract response.
    streamResponse >> temp[0] >> delimiter >> temp[1] >> delimiter >> temp[2] >> delimiter >> temp[3];

    // Write to vector.
    for(int i = 0; i < 4; i++)
        encoderPos.push_back(temp[i]);
}

void GalilCommunicator::update_encoder_offsets(const std::vector<double> &offsets, double ratio, Tango::DeviceProxy &devProxy)
{
    stringstream ss;

	ss << fixed << setprecision(4);

	ss << 	"Offsets[0]="  << ratio*offsets[0] <<
			";Offsets[1]=" << ratio*offsets[1] <<
			";Offsets[2]=" << ratio*offsets[2] <<
			";Offsets[3]=" << ratio*offsets[3] <<
            ";UMOfsts[0]=" <<   offsets[0] <<
			";UMOfsts[1]=" <<   offsets[1] <<
			";UMOfsts[2]=" <<   offsets[2] <<
			";UMOfsts[3]=" <<   offsets[3] <<
            ";OffInit=1\r";  // Signal that the offsets have been initialized           

	send_command_string(ss.str().c_str(), devProxy);

	if(stringResponse.compare(":::::::::") != 0)
	        Tango::Except::throw_exception("Error executing DMC command","Failed while transferring new offsets to the DMC.","GalilCommunicator::update_encoder_offsets",Tango::ERR);
}

void GalilCommunicator::update_gap_encoder_offsets(const vector<double> &offsets, double ratio)
{
	update_encoder_offsets(offsets,ratio,*controlBoxGap);
}

void GalilCommunicator::update_phase_encoder_offsets(const vector<double> &offsets, double ratio)
{
	update_encoder_offsets(offsets,ratio,*controlBoxPhase);
}

void GalilCommunicator::reset_drive(unsigned int driveNo) 
{
    const unsigned long SLEEPTIME_USEC = 200000;
    if (driveNo<1 || driveNo>8)
        Tango::Except::throw_exception("Drive reset","Invalid drive number specified. Please use 1 to 8.","GalilCommunicator::reset_drive",Tango::ERR);

    Tango::DeviceProxy *devProxy = 0;
    
    if (driveNo<=4) {
        devProxy = controlBoxGap;
    } else {
        driveNo-=4;
        devProxy = controlBoxPhase;
    }
    
    stringstream ss;
    
    ss << "SB " << driveNo << '\r';
    send_command_string(ss.str().c_str(), *devProxy);
    
    if (stringResponse.compare(":") != 0)
        Tango::Except::throw_exception("Drive reset","Failed while setting drive-reset bit on the DMC","GalilCommunicator::reset_drive",Tango::ERR);
    
    usleep(SLEEPTIME_USEC);
    
    ss.str("");
    ss << "CB "<< driveNo << '\r';
    send_command_string(ss.str().c_str(), *devProxy);
    
    if (stringResponse.compare(":") != 0)
        Tango::Except::throw_exception("Drive reset","Failed while clearing drive-reset bit on the DMC","GalilCommunicator::reset_drive",Tango::ERR);
}

void GalilCommunicator::send_command_string(const char *command, Tango::DeviceProxy &controlBox)
{
	stringResponse.clear();

	// Do not send anything if empty
	if (!command) return;

    // Send command through controlbox to DMC.
    devdata << command;

    devdata = controlBox.command_inout("ExecLowLevelCmd", devdata);

	devdata >> stringResponse;
}

void GalilCommunicator::send_command_stream(const char *command, Tango::DeviceProxy &controlBox)
{
	send_command_string(command,controlBox);
	streamResponse.str(stringResponse);
}

void GalilCommunicator::get_id_position(IDPosition &data)
{
	// Implementation note !
	// Due to fast querying sometimes the responses get mixed up if all variables are put in one line.
	// That is why several commands are sent.
    data.gap 		= get_variable("coilG", *controlBoxGap);
    data.center 	= get_variable("coilC", *controlBoxGap);
    data.taper		= get_variable("coilT", *controlBoxGap);

    data.phaseMode		= get_variable("coilPM", *controlBoxPhase);
    data.phaseOffset 	= get_variable("coilPO", *controlBoxPhase);
}

void GalilCommunicator::get_id_status(IDStatus &data)
{
	data.interlock 		= get_interlock_status();
	data.stopAll 		= get_stopall_status();
	data.gapMoving  	= !(bool)get_variable("MG @OUT[5]", *controlBoxGap, false); // inverse logic used on input
	data.phaseMoving 	= !(bool)get_variable("MG @OUT[5]", *controlBoxPhase, false); // inverse logic used on input

	unsigned int gapFlags 	= get_variable("AxesFlgs", *controlBoxGap);
	unsigned int phaseFlags = get_variable("AxesFlgs", *controlBoxPhase);

	// Combine the flags in one bit-field
	data.axesFlags = (gapFlags & 0x000000000F) | ((phaseFlags & 0x0000000F)<<4) |
					((gapFlags & 0x00000000F0)<<4) | ((phaseFlags & 0x00000000F0)<<8) |
					((gapFlags & 0x0000000F00)<<8) | ((phaseFlags & 0x0000000F00)<<12);
                    
    // This will transfer PhCmd from the Gap to the Phase galil:
    int PhCmd = get_variable("PhCmd",*controlBoxGap);
    // Clear the flag after it has been read
    send_command_string("PhCmd=0\r",*controlBoxGap);
    if (stringResponse.compare(":") != 0)
        Tango::Except::throw_exception("Clearing flag on Gap MC","Failed while clearing PhCmd flag  on the Gap DMC","GalilCommunicator::get_id_status",Tango::ERR);
        
    // Copy the flag to the Phase DMC
    stringstream ss;
    ss << "PhCmd=" << PhCmd << '\r';
    send_command_string(ss.str().c_str(),*controlBoxPhase);
    
    if (stringResponse.compare(":") != 0)
        Tango::Except::throw_exception("Sending to Phase DMC","Failed while pushing PhCmd flag  on the Phase DMC","GalilCommunicator::get_id_status",Tango::ERR);
}

double GalilCommunicator::get_variable(const char *variableName,Tango::DeviceProxy &devProxy, bool addQuestionMark)
{
    //cout << "Reading variable: " << variableName << "  ";
    
    stringstream ss;
    ss << variableName << (addQuestionMark ? "=?" : "") << '\r';
    send_command_stream(ss.str().c_str(), devProxy);
    
    double rv = 0.0f;
    // If valid response.
    if (boost::algorithm::ends_with(stringResponse,"\r\n:"))
    {
        // Cut trailing:
        char whitespace;
        while (streamResponse.peek()==' ') 
            streamResponse.get(whitespace);

        streamResponse >> rv;
    }
    else
    {
        ss.clear();
        ss << "Failed to read variable " << variableName <<
                " from the device " << devProxy.name();

        Tango::Except::throw_exception("Failed while reading DMC variable",
                ss.str().c_str(), __PRETTY_FUNCTION__ , Tango::ERR);
    }

    return rv;
}

bool GalilCommunicator::get_interlock_status()
{
    int interlock = get_variable("IlockAct", *controlBoxGap);
    // If gap DMC is not in interlock check phase as well.
    if(!interlock)
    	interlock = get_variable("IlockAct", *controlBoxPhase);

    return interlock;
}

bool GalilCommunicator::get_stopall_status()
{
    int stopAll	= get_variable("StopRq", *controlBoxGap);

    if (!stopAll)
    	stopAll = get_variable("StopRq", *controlBoxPhase);

	return stopAll;
}

}
