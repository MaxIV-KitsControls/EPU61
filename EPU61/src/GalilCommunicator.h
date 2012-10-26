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
     * @param parameters Structure holding all needed parameters for gap movement.
     */
    void start_gap_movement(const gapParameters &parameters);

    /**
     * Starts phase movement with desired parameters.
     *
     * @param parameters Structure holding all needed parameters for phase movement.
     */
    void start_phase_movement(const phaseParameters &parameters);

    /**
     * Sets phase and gap axes soft limits to off value.
     */
    void disable_limits();

    /**
     * Sets soft limits on motion controller for phase axes.
     *
     * @param forwardLimits Reference to double vector holding forward limits in counts.
     * @param backwardLimits Reference to double vector holding backward limits in counts.
     */
    void set_phase_limits(const vector<double> &forwardLimits, const vector<double> &backwardLimits);

    /**
     * Sets soft limits on motion controller for gap axes.
     *
     * @param forwardLimits Reference to double vector holding forward limits in counts.
     * @param backwardLimits Reference to double vector holding backward limits in counts.
     */
    void set_gap_limits(const vector<double> &forwardLimits, const vector<double> &backwardLimits);

    /**
     * Gets current phase mode.
     */
    short get_phaseMode();

    /**
     * Gets current interlock status.
     *
     * @return Interlock status. (true = interlock in effect)
     */
    bool get_interlock_status();

    /**
     * Enables or disables stop all state.
     *
     * @param confirm Enable(true) or disable(false) stop all state.
     */
    void toggle_stop_all(bool confirm);

    /**
     * Stops phase axes.
     */
    void stop_phase();

    /**
     * Stops gap axes.
     */
    void stop_gap();

    /**
     * Reads auxiliary encoder values and adds them to array.
     *
     * @param auxPos Double vector by reference to be filled with auxiliary encoder values.
     */
    void get_gap_auxiliary_encoder_pos(vector<double> &encoderPos);

    /**
     * Reads current gap, phase offset and phase mode directly from DMC.
     * @param data Structure to fill with read values.
     */
    void get_coil_correction_data(coilData &data);

    /**
     * Destructor.
     */
    ~GalilCommunicator();

private:

    /**
     * Sends low level execute command through control box to DMC.
     *
     * @param command Command sent to DMC.
     * @param controlBox Reference to receiving controlbox.
     */
    void send_command(string command, Tango::DeviceProxy &controlBox);

    /**
     * Extracts Tango::DevData to string.
     *
     * @return DMC response in string format.
     */
    string devdata_to_string();

    /**
     * Creates string command for software limits to send to DMC using values given in 2 vectors.
     *
     * @param forwardLimits Forward limit value in counts for each axis.
     * @param backwardLimits Backward limit value in counts for each axis.
     * @return Command in Galil format.
     */
    string create_limits_command(const vector<double> &forwardLimits, const vector<double> &backwardLimits);


private:

    Tango::DeviceProxy *controlBoxGap; //!< Pointer to controlbox with gap DMC.
    Tango::DeviceProxy *controlBoxPhase; //!< Pointer to controlbox with phase DMC.
    Tango::DeviceData devdata; //!< Device data for sending commands and receiving responses.

};

}
#endif /* GALILCOMMUNICATOR_H_ */
