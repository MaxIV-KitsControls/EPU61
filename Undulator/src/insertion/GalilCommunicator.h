#ifndef GALILCOMMUNICATOR_H_
#define GALILCOMMUNICATOR_H_

#include "Communicator.h"
#include <tango.h>
#include <boost/algorithm/string/predicate.hpp>

namespace Undulator_ns
{

/**
 * Implements communication with two Galil controllers
 */
class GalilCommunicator : public Communicator
{
public:
    /**
     * Constructor for Galil communication class.
     *
     * @param gapProxy Proxy to controlBox controlling gap movements.
     * @param phaseProxy Proxy to controlBox controlling phase movements.
     */
    GalilCommunicator(const string &gapProxy, const string &phaseProxy);

    /**
     * Destructor.
     */
    virtual ~GalilCommunicator();

    // Implement the necessary Communicator methods:

    virtual void start_gap_movement(const GapParameters &parameters);
    virtual void start_phase_movement(const PhaseParameters &parameters);

    virtual void stop_gap();
    virtual void stop_phase();

    virtual void toggle_stop_all(bool confirm);

    virtual void get_id_position(IDPosition &data);
    virtual void get_id_status(IDStatus &data);

    virtual void get_gap_linear_encoder_pos(vector<double> &encoderPos);
    
    virtual void update_gap_encoder_offsets(const std::vector<double> &offsets, double ratio);
    
    virtual void update_phase_encoder_offsets(const std::vector<double> &offsets, double ratio);
    
    virtual void reset_drive(unsigned int driveNo);
private:
    /**
     *  Updates the encoder offsets on the specifiet device.
     * 
     *  @param offsets An array of offset values to be transfered for each of the gap encoders.
     *  @param ratio Offset is scaled by this value
     *  @param devProxy the device to use
     * 
     */
    void update_encoder_offsets(const std::vector<double> &offsets, double ratio, Tango::DeviceProxy &devProxy);

    /**
     * Reads variable value from the DMC. Throws exception if error occurs.
     *
     *	@param variableName the name of the DMC variable to be read.
     *	@param devProxy Tango DeviceProxy object to use for sending the request.
     *  @param addQuestionMark Whether to add '=?' to the variable query request
     *
     *	@return value that has been read
     */
    double get_variable(const char *variableName,Tango::DeviceProxy &devProxy, bool addQuestionMark=true);
    
    /**
	 * Gets current interlock status.
	 *
	 * @return Interlock status. (true = interlock in effect)
	 */
    bool get_interlock_status();

    /**
     * Retrieves StopAll status from the devices.
     * Also propagates StopAll among controllers.
     *
     * If StopAll is not consistent, it triggers StopAll on the device which lacks the status.
     *
     * @return true if StopAll is active
     */
    bool get_stopall_status();

    /**
     * Sends low level execute command through control box to DMC.
     *
     * @param command Command sent to DMC.
     * @param controlBox Reference to receiving controlbox.
     *
     *	After calling the command the response is in stringResponse member.
     *	Method HAS SIDE EFFECTS, modifies members devdata and stringResponse.
     */
    void send_command_string(const char *command, Tango::DeviceProxy &controlBox);


    /**
	 * Sends low level execute command through control box to DMC.
	 *
	 * @param command Command sent to DMC.
	 * @param controlBox Reference to receiving controlbox.
	 *
	 *	After calling the command the response is in streamResponse member.
	 *	Method HAS SIDE EFFECTS, modifies members devdata, stringResponse and streamResponse.
	 */
    void send_command_stream(const char *command, Tango::DeviceProxy &controlBox);
private:

    Tango::DeviceProxy *controlBoxGap; //!< Pointer to controlbox with gap DMC.
    Tango::DeviceProxy *controlBoxPhase; //!< Pointer to controlbox with phase DMC.

    // Shared inter-method calls
    Tango::DeviceData 	devdata;		//!< send_command_xxx methods use this variable to populate and receive command data

    std::string 		stringResponse;	//!< send_command_string uses this variable to store the command response
    std::stringstream 	streamResponse; //!< send_command_string uses this variable to store the command response
};

}
#endif /* GALILCOMMUNICATOR_H_ */
