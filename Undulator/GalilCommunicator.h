/*
 * GalilCommunicator.h
 *
 *  Created on: Sep 19, 2012
 *      Author: thumar
 */

#ifndef GALILCOMMUNICATOR_H_
#define GALILCOMMUNICATOR_H_

#include "Communicator.h"
#include "tango.h"

namespace Undulator_ns
{

class GalilCommunicator : public Communicator
{
public:

    /**
     * Constructor for Galil communication class.
     *
     * @param gapProxy Proxy to controlBox controlling gap movements.
     * @param phaseProxy Proxy to controlBox controlling phase movements.
     */
    GalilCommunicator(string gapProxy, string phaseProxy);

    /**
     * Starts gap movement with desired parameters.
     *
     * @param offsets Array of gap axes offsets.
     * @param center Desired offset.
     * @param gap Desired gap.
     * @param taper Desired taper.
     * @param gapSpeed Desired gap speed.
     * @param taperSpeed Desired taper speed.
     * @param gapAcc Desired gap acceleration.
     * @param taperAcc Desired taper acceleration.
     */
    void start_gap_movement(const vector<double> &offsets,double center,double gap,double taper,double gapSpeed,double taperSpeed,double gapAcc,double taperAcc);

    /**
     * Starts phase movement with desired parameters.
     *
     * @param offsets Array of phase axes offsets.
     * @param phaseMode Desired phase mode.
     * @param finalAxisPos Array of final motor positions without offsets applied.
     * @param phaseSpeed Desired phase speed.
     * @param phaseAcc Desired phase acceleration.
     */
    void start_phase_movement(const vector<double> &offsets,short phaseMode,const vector<double> &finalAxisPos,double phaseSpeed,double phaseAcc);

    /**
     * Sets phase and gap axes soft limits to off value.
     */
    void disable_limits();

    void set_phase_limits(const vector<double> &forwardLimits, const vector<double> &backwardLimits);

    void set_gap_limits(const vector<double> &forwardLimits, const vector<double> &backwardLimits);

    short get_phaseMode();

    bool get_interlock_status();

    void toggle_stop_all(bool confirm);
    /**
     * Stops the movement of phase axes.
     */
    void stop_phase();

    void stop_gap();
    /**
     * Fills the array with auxiliary encoder motor positions read from DMC.
     *
     * @param auxPos Array by reference for auxiliary encoder motor positions.
     */
    void get_gap_auxiliary_encoder_pos(vector<double> &encoderPos);

    /**
     * Destructor.
     */
    ~GalilCommunicator();

private:

    /**
     * Sends low level execute command through control box to DMC.
     *
     * @param command Command sent to DMC.
     * @param controlBox Reference to receiver controlbox.
     */
    void send_command(string command, Tango::DeviceProxy &controlBox);

    /**
     * Extracts Tango::DevData to string.
     *
     * @return DMC response in string format.
     */
    string devdata_to_string();

    string create_limits_command(const vector<double> &forwardLimits, const vector<double> &backwardLimits);
private:

    Tango::DeviceProxy *controlBoxGap; //!< Pointer to controlbox with gap DMC.
    Tango::DeviceProxy *controlBoxPhase; //!< Pointer to controlbox with phase DMC.
    Tango::DeviceData devdata; //!< Device data for sending commands and receiving responses.

};

}
#endif /* GALILCOMMUNICATOR_H_ */
