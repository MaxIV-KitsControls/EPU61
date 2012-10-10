/*
 * Communicator.h
 *
 *  Created on: Sep 19, 2012
 *      Author: thumar
 */

#ifndef COMMUNICATOR_H_
#define COMMUNICATOR_H_
#include <vector>

namespace Undulator_ns
{

class Communicator
{

public:
    /**
     * Starts gap movement with desired parameters.
     *
     * @param offsets Reference to double vector holding gap axes offsets in counts.
     * @param center Desired offset in counts.
     * @param gap Desired gap in counts.
     * @param taper Desired taper in counts.
     * @param gapSpeed Desired gap speed in counts/s.
     * @param taperSpeed Desired taper speed in counts/s.
     * @param gapAcc Desired gap acceleration in counts/s^2.
     * @param taperAcc Desired taper acceleration in counts/s^2.
     */
    virtual void start_gap_movement(const std::vector<double> &offsets,double center,double gap,double taper,double gapSpeed,double taperSpeed,double gapAcc,double taperAcc) = 0;

    /**
     * Starts phase movement with desired parameters.
     *
     * @param offsets Reference to double vector holding phase axes offsets in counts.
     * @param phaseMode Desired phase mode in counts.
     * @param finalAxisPos Reference to double vector holding final motor positions in counts without offsets applied.
     * @param phaseSpeed Desired phase speed in counts/s.
     * @param phaseAcc Desired phase acceleration in counts/s^2.
     */
    virtual void start_phase_movement(const std::vector<double> &offsets,short phaseMode,const std::vector<double> &finalAxisPos,double phaseSpeed,double phaseAcc) = 0;

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
     * @param auxPos Double array by reference to be filled with auxiliary encoder values.
     */
    virtual void get_gap_auxiliary_encoder_pos(std::vector<double> &encoderPos) = 0;

    /**
     * Empty destructor.
     */
    virtual ~Communicator() {};


};


}

#endif /* COMMUNICATOR_H_ */
