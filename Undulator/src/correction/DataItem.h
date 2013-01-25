#ifndef DATAITEM_H_
#define DATAITEM_H_

#include <iostream>
#include <math.h>

/**
 * Represents data-entry in the interpolation data array
 */
struct DataItem {
	const static double MIN_GAP = 14000;
	const static double MAX_GAP = 220000;
	const static double MAX_PHASE = 30500;

	double MIN_GAP_LOG, MAX_GAP_LOG, GAP_LOG_RANGE;

	/**
	 * Construct an object. Will initialize MIN_GAP_LOG, MAX_GAP_LOG and GAP_LOG_RANGE
	 */
	DataItem() : gap(0.0f), phase(0.0f), mode(A) {
		MIN_GAP_LOG = log(MIN_GAP);
		MAX_GAP_LOG = log(MAX_GAP);

		GAP_LOG_RANGE = MAX_GAP_LOG-MIN_GAP_LOG;

		coils[0] = coils[1] = coils[2] = coils[3] = 0.0f;
	}

	/**
	 * Enum representing the phase mode
	 */
	enum PhaseMode {
		A,//!< A
		B,//!< B
		C,//!< C
		D //!< D
	};


	double gap;			//< Entry representing gap in um
	double phase;		//< Entry representing phase offset in um

	PhaseMode mode;		//< Entry representing phase mode

	double coils[4];	//< Currents for the 4 coils in A

	/**
	 * Normalizes gap and phase members to be in the 0 - 1.0 range
	 */
	void normalize()
	{
		// Normalize data, to put in range 0-1.0
		// gap is normalized on logarithmic scale
		gap 	= (log(gap)-MIN_GAP_LOG) / GAP_LOG_RANGE; 	// normalize gap
		phase	= (phase+MAX_PHASE) / (2*MAX_PHASE);		// normalize phase
	}

	/**
	 * Overload for the ostream << operator
	 *
	 * @param os the output stream
	 * @param d the data item
	 * @return the output stream object reference
	 */
	friend std::ostream& operator << (std::ostream &os, const DataItem &d);

	/**
	 * Overload for the ostream >> operator
	 *
	 * @param is the input stream
	 * @param d the data item
	 * @return the inpust stream object reference
	 */
	friend std::istream& operator >> (std::istream &is, DataItem &d);
};


#endif /* DATAITEM_H_ */
