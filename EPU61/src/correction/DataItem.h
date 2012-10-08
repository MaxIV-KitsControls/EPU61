#ifndef DATAITEM_H_
#define DATAITEM_H_

#include <iostream>

/**
 * Represents data-entry in the interpolation data array
 */
struct DataItem {
	const static double MIN_GAP = 14000;
	const static double MAX_GAP = 220000;
	const static double MAX_PHASE = 30500;

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
		gap 	= (gap-MIN_GAP) / (MAX_GAP-MIN_GAP); 	// normalize gap
		phase	= (phase+MAX_PHASE) / (2*MAX_PHASE);	// normalize phase
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
