#include "GalilCommunicator.h"


namespace Undulator_ns
{

GalilCommunicator::GalilCommunicator(string gapProxy,
        string phaseProxy)
{
    controlBoxGap=controlBoxPhase=0;

    // Create device proxys.
    controlBoxGap = new Tango::DeviceProxy(gapProxy);
    controlBoxPhase = new Tango::DeviceProxy(phaseProxy);
}

void GalilCommunicator::disable_limits()
{
    // Command to send to disable soft limits on DMC.
    const string limitsCommand = "FL 2147483647,2147483647,2147483647,2147483647; BL -2147483648,-2147483648,-2147483648,-2147483648";

    send_command(limitsCommand, *controlBoxGap);

    bool success = true;
    // Check gap limits disabled response.
    if(devdata_to_string().compare("::") != 0)
        success = false;

    send_command(limitsCommand, *controlBoxPhase);

    // Check phase limits disabled response.
    if(devdata_to_string().compare("::") != 0)
        success = false;

    // If error occurred throw exception.
    if(!success)
        Tango::Except::throw_exception("Disable limits command couldn't be sent to DMC.","Disable limits command couldn't be sent to DMC.","Disable limits - true",Tango::ERR);
}

void GalilCommunicator::set_phase_limits(const vector<double> &forwardLimits,
        const vector<double> &backwardLimits)
{
    if(forwardLimits.size() != 4 || backwardLimits.size() != 4)
        Tango::Except::throw_exception("Passed vector is wrong size.",
                "Vectors must contain 4 elements each.","Set phase limits.",Tango::ERR);

    // Send to phase DMC.
    send_command(create_limits_command(forwardLimits, backwardLimits), *controlBoxPhase);

    // Check for errors.
    if(devdata_to_string().compare("::") != 0)
        Tango::Except::throw_exception("Error in setting soft limits on DMC.","Error in setting soft limits on DMC.","Set soft limits phase.", Tango::ERR);
}

void GalilCommunicator::set_gap_limits(const vector<double> &forwardLimits,
        const vector<double> &backwardLimits)
{
    if(forwardLimits.size() != 4 || backwardLimits.size() != 4)
        Tango::Except::throw_exception("Passed vector is wrong size.",
                "Vectors must contain 4 elements each.","Set gap limits.",Tango::ERR);

    // Send to gap DMC.
    send_command(create_limits_command(forwardLimits, backwardLimits), *controlBoxGap);

    // Check for errors.
    if(devdata_to_string().compare("::") != 0)
        Tango::Except::throw_exception("Error in setting soft limits on DMC.","Error in setting soft limits on DMC.","Set soft limits gap.",Tango::ERR);

}

short GalilCommunicator::get_phaseMode()
{
    // Get the current phase mode from DMC.
    send_command("PhMd=?", *controlBoxPhase);
    stringstream convert(devdata_to_string());

    // Extract value
    short value;
    convert >> value;

    return value;
}

bool GalilCommunicator::get_interlock_status()
{
    int interlock = 0;
    send_command("IlockAct=?", *controlBoxGap);
    stringstream response(devdata_to_string());

    // If valid response.
    if (std::string::npos != response.str().find(":"))
        response >> interlock;
    else
        Tango::Except::throw_exception("Interlock status couldn't be read from gap DMC.","Interlock status couldn't be read from gap DMC.","Get intelock status from gap DMC",Tango::ERR);

    // If gap DMC is not in interlock.
    if(!interlock)
    {
        send_command("IlockAct=?", *controlBoxPhase);
        stringstream response(devdata_to_string());

        // If valid response.
        if (std::string::npos != response.str().find(":"))
            response >> interlock;
        else
            Tango::Except::throw_exception("Interlock status couldn't be read from phase DMC.","Interlock status couldn't be read from phase DMC.","Get intelock status from phase DMC",Tango::ERR);
    }

    return static_cast<bool>(interlock);

}

void GalilCommunicator::toggle_stop_all(bool confirm)
{
    string command;

    // Set command based on input.
    if(confirm)
        command = "StopRq=1";
    else
        command = "StopRq=0";

    bool error = false;

    // Sends command to stop gap motors.
    send_command(command, *controlBoxGap);

    // Check for error.
    if(devdata_to_string().compare(":") != 0)
        error = true;

    // Sends command to stop phase motors.
    send_command(command, *controlBoxPhase);

    // Check for error.
    if(devdata_to_string().compare(":") != 0)
        error = true;

    // If no error occurred set stop all status to desired.
    if(error)
        Tango::Except::throw_exception("Stop command couldn't be sent to DMC.","Stop command couldn't be sent to DMC.","Stop all command.",Tango::ERR);
}

void GalilCommunicator::stop_phase()
{
    // Sends command to stop gap motors.
    send_command("HX 7; ST S", *controlBoxPhase);

    // If command failed throw exception.
    if(devdata_to_string().compare("::") != 0)
        Tango::Except::throw_exception("Stop phase command couldn't be sent to DMC.","Stop phase command couldn't be sent to DMC.","Stop phase.",Tango::ERR);

}

void GalilCommunicator::stop_gap()
{
    // Sends command to stop gap motors.
    send_command("HX 7; ST S", *controlBoxGap);

    // If command failed throw exception.
    if(devdata_to_string().compare("::") != 0)
        Tango::Except::throw_exception("Stop gap command couldn't be sent to DMC.","Stop gap command couldn't be sent to DMC.","Stop gap.",Tango::ERR);

}

void GalilCommunicator::get_gap_auxiliary_encoder_pos(vector<double> &encoderPos)
{
    // Read aux encoder positions.
    send_command("TDA;TDB;TDC;TDD", *controlBoxGap);
    stringstream response(devdata_to_string());
    string delimiter;
    double temp[4];

    // Clear any previous values.
    encoderPos.clear();

    // Extract response.
    response >> temp[0] >> delimiter >> temp[1] >> delimiter >> temp[2] >> delimiter >> temp[3];

    // Write to vector.
    for(int i = 0; i < 4; i++)
        encoderPos.push_back(temp[i]);
}

string GalilCommunicator::create_limits_command(const vector<double> &forwardLimits, const vector<double> &backwardLimits)
{
    stringstream cmdFL,cmdBL;

    // Create both commands.
    cmdFL << "FL ";
    cmdBL << "BL ";
    for(int i = 0; i < 4; i++) // No need for size check as it's checked before this function is called.
    {
        cmdFL << fixed << forwardLimits[i];
        cmdBL << fixed << backwardLimits[i];
        if (i != 3)
        {
            cmdFL << ",";
            cmdBL << ",";
        }
    }
    // Return both commands as string. Add ; after first command.
    return (cmdFL.str() + ";" + cmdBL.str());
}

string GalilCommunicator::devdata_to_string()
{
    string temp;
    devdata >> temp;
    return temp;
}

void GalilCommunicator::send_command(string command, Tango::DeviceProxy &controlBox )
{
    // Send command through controlbox to DMC.
    devdata << command;
    devdata = controlBox.command_inout("ExecLowLevelCmd", devdata);
}

void GalilCommunicator::get_coil_correction_data(coilData &data)
{
    // Send command to retrieve current gap.
    send_command("coilG=?", *controlBoxGap);

    // Extract response.
    stringstream response(devdata_to_string());
    response >> data.gap;

    // Send command to retrieve current phase offset.
    send_command("coilPO=?", *controlBoxPhase);

    // Extract response.
    response.str(devdata_to_string());
    response >> data.phaseOffset;

    // Send command to retrieve current phase mode.
    send_command("coilPM=?", *controlBoxPhase);

    // Extract response.
    response.str(devdata_to_string());
    response >> data.phaseMode;
}

void GalilCommunicator::start_gap_movement(const gapParameters &parameters)
{
    // Create command to send. Send center,gap and taper parameters.

    stringstream cmd;

    // Offsets update part.
    if(parameters.offsets.size() != 4)
        Tango::Except::throw_exception("Offsets vector is wrong size.",
                    "Size of offsets vector is not four.","Start gap movement.",Tango::ERR);

    for(int i = 0; i < 4; i++)
    {
        cmd << "Offsets[" << i << "]=" << fixed << parameters.offsets[i] << ";";
    }

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

    // Send command.
    send_command(cmd.str(), *controlBoxGap);

    // If we don't get positive confirmation of all commands throw exception.
    if (devdata_to_string().compare("::::::::::::") != 0)
    {
        Tango::Except::throw_exception("Error starting gap movement.",
            "Start gap movement command failed.","Start gap movement.",Tango::ERR);
    }
}

void GalilCommunicator::start_phase_movement(const phaseParameters &parameters)
{
    // Create command to send.
    stringstream cmd;

    // Offsets and final positions vector check..
    if(parameters.offsets.size() != 4 || parameters.finalAxisPos.size() != 4)
        Tango::Except::throw_exception("Passed vector is wrong size.",
            "Vectors must contain 4 elements each.","Start phase movement.",Tango::ERR);

    // Offsets part of command.
    for(int i = 0; i < 4; i++)
    {
        cmd << "Offsets[" << i << "]=" << fixed << parameters.offsets[i] << ";";
    }

    // Final position part of command.
    for(int i = 0; i < 4; i++)
    {
        cmd << "PhDest[" << i << "]=" << fixed << parameters.finalAxisPos[i] << ";";
    }

    // Rest of command.
    cmd << "PhMdRq="
        << parameters.phaseMode
        << ";PhSpd="
        << fixed << parameters.phaseSpeed
        << ";PhAcc="
        << fixed << parameters.phaseAcceleration
        << ";XQ#PHASE,7";

    // Send command.
    send_command(cmd.str(), *controlBoxPhase);

    // If we get positive confirmation of all commands.
    if (devdata_to_string().compare("::::::::::::") != 0)
        Tango::Except::throw_exception("Error starting phase movement.",
            "Start phase movement command failed.","Start phase movement.",Tango::ERR);
}

GalilCommunicator::~GalilCommunicator()
{
    if (controlBoxGap) delete controlBoxGap;
    if (controlBoxPhase) delete controlBoxPhase;
}

}
