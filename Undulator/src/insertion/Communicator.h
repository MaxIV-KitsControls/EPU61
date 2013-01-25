#ifndef COMMUNICATOR_H_
#define COMMUNICATOR_H_

#include <vector>
#include <string>
#include "EPUData.h"

namespace Undulator_ns
{

enum CommunicatorType {
	GALIL,
	UNKNOWN_TYPE = 65535
};

/**
 * A class representing an abstract communicator with a motion controller for an EPU.
 *
 * Sub-classes should implement concrete behavior and expose it through this interface.
 */
class Communicator
{

public:

	const static CommunicatorType DEFAULT_COMMUNICATOR = GALIL;

	/*
	 * Factory method to get a communicator
	 *
	 * @param params A vector of string parameters to pass to the constructor of the communicator object.
	 * @param type Optional parameter, communicator type.
	 */
	static Communicator* create_communicator(const std::vector<std::string> &params, CommunicatorType type=DEFAULT_COMMUNICATOR);

    /**
     * Empty destructor.
     */
    virtual ~Communicator() {};

    /**
     * Starts gap movement with desired parameters.
     *
     * @param parameters Structure holding all needed parameters for gap movement.
     */
    virtual void start_gap_movement(const GapParameters &parameters) = 0;

    /**
     * Starts phase movement with desired parameters.
     *
     * @param parameters Structure holding all needed parameters for phase movement.
     */
    virtual void start_phase_movement(const PhaseParameters &parameters) = 0;

    /**
     * Stops gap axes.
     */
    virtual void stop_gap() = 0;

    /**
     * Stops phase axes.
     */
    virtual void stop_phase() = 0;

    /**
     * Enables or disables stop all state.
     *
     * @param confirm Enable(true) or disable(false) stop all state.
     */
    virtual void toggle_stop_all(bool confirm) = 0;

    /**
     * Reads current insertion device position from the motion controller.
     *
     * @param data Structure to fill with read values.
     */
    virtual void get_id_position(IDPosition &data) = 0;


    /**
     * Reads  current insertion device status.
     *
     * @param data Structure to fill with read values.
     */
    virtual void get_id_status(IDStatus &data) = 0;

    /**
     * Reads auxiliary encoder values and adds them to array.
     *
     * @param auxPos Double vector by reference to be filled with auxiliary encoder values.
     */
    virtual void get_gap_linear_encoder_pos(std::vector<double> &encoderPos) = 0;

    /**
     * Writes new updated gap offsets to the motion controller
     *
     * @param offsets An array of offset values to be transfered for each of the gap encoders.
     * @param ratio Offset is scaled by this value
     */
    virtual void update_gap_encoder_offsets(const std::vector<double> &offsets, double ratio) = 0;
    
     /**
     * Writes new updated phase offsets to the motion controller
     *
     * @param offsets An array of offset values to be transfered for each of the phase encoders.
     * @param ratio Offset is scaled by this value
     */
    virtual void update_phase_encoder_offsets(const std::vector<double> &offsets, double ratio) = 0;
    
    /**
     * Sends drive-reset command to the appropriate drive numbered 1-8
     * 
     * @param driveNo Drive to reset 1..8
     */
    virtual void reset_drive(unsigned int driveNo) = 0;
};


}

#endif /* COMMUNICATOR_H_ */
