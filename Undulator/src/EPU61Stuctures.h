/*
 * EPU61Stuctures.h
 *
 *  Created on: Oct 18, 2012
 *      Author: thumar
 */

#ifndef EPU61STUCTURES_H_
#define EPU61STUCTURES_H_

/**
 * Structure for correction coils data containing gap, phase offset and phase mode.
 */
struct coilData
{
    double gap; //!< Current gap.
    double phaseOffset; //!< Current phase offset.
    short phaseMode; //!< Current phase mode.
};

struct gapParameters
{
    std::vector<double> offsets; //!< Axes offsets.
    double gap; //!< Desired gap.
    double offset; //!< Desired offset.
    double taper; //!< Desired taper.
    double gapSpeed; //!< Desired gap speed.
    double taperSpeed; //!< Desired taper speed.
    double gapAcceleration; //!< Desired gap acceleration and deceleration.
    double taperAcceleration; //!< Desired taper acceleration and deceleration.
};

struct phaseParameters
{
    std::vector<double> offsets; //!< Axes offsets.
    std::vector<double> finalAxisPos; //!< Final axes positions based on desired phase mode without offsets applied.
    short phaseMode; //!< Desired phase mode.
    double phaseSpeed; //!< Desired phase speed.
    double phaseAcceleration; //!< Desired phase acceleration and deceleration.
};



#endif /* EPU61STUCTURES_H_ */
