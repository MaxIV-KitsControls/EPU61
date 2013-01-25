/*
 * Communicator.h
 *
 *  Created on: Sep 19, 2012
 *      Author: thumar
 */

#ifndef COMMUNICATOR_H_
#define COMMUNICATOR_H_
#include <vector>
#include <EPU61Stuctures.h>

namespace Undulator_ns
{

class Communicator
{

public:

    /**
     * Starts gap movement with desired parameters.
     *
     * @param parameters Structure holding all needed parameters for gap movement.
     */
    virtual void start_gap_movement(const gapParameters &parameters) = 0;

    /**
     * Starts phase movement with desired parameters.
     *
     * @param parameters Structure holding all needed parameters for phase movement.
     */
    virtual void start_phase_movement(const phaseParameters &parameters) = 0;

    /**
     * Sets phase and gap axes soft limits to off value.
     */
    virtual void disable_limits() = 0;

    /**
     * Sets soft limits on motion controller for phase axes.
     *
     * @param forwardLimits Reference to double vector holding forward limits in counts.
     * @param backwardLimits Reference to double vector holding backward limits in counts.
     */
    virtual void set_phase_limits(const std::vector<double> &forwardLimits, const std::vector<double> &backwardLimits) = 0;

    /**
     * Sets soft limits on motion controller for gap axes.
     *
     * @param forwardLimits Reference to double vector holding forward limits in counts.
     * @param backwardLimits Reference to double vector holding backward limits in counts.
     */
    virtual void set_gap_limits(const std::vector<double> &forwardLimits, const std::vector<double> &backwardLimits) = 0;

    /**
     * Gets current phase mode.
     */
    virtual short get_phaseMode() = 0;

    /**
     * Gets current interlock status.
     *
     * @return Interlock status. (true = interlock in effect)
     */
    virtual bool get_interlock_status() = 0;

    /**
     * Enables or disables stop all state.
     *
     * @param confirm Enable(true) or disable(false) stop all state.
     */
    virtual void toggle_stop_all(bool confirm) = 0;

    /**
     * Stops phase axes.
     */
    virtual void stop_phase() = 0;

    /**
     * Stops gap axes.
     */
    virtual void stop_gap() = 0;

    /**
     * Reads auxiliary encoder values and adds them to array.
     *
     * @param auxPos Double vector by reference to be filled with auxiliary encoder values.
     */
    virtual void get_gap_auxiliary_encoder_pos(std::vector<double> &encoderPos) = 0;

    /**
     * Reads current gap, phase offset and phase mode directly from DMC.
     * @param data Structure to fill with read values.
     */
    virtual void get_coil_correction_data(coilData &data) = 0;

    /**
     * Empty destructor.
     */
    virtual ~Communicator() {};


};


}

#endif /* COMMUNICATOR_H_ */
