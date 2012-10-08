#ifndef POWERSUPPLY_H_
#define POWERSUPPLY_H_

/**
 * This PowerSupply class is to provide an interface to actual power supply device.
 *
 * ToDo: When the Power supply specification is defined this class implementation needs to be expanded with the actual functionality.
 */
class PowerSupply
{
public:
	/**
	 * Constructs a PowerSupply object
	 */
	PowerSupply() : current(0) {}

	/**
	 * Turns on the power supply.
	 */
	void on() { }

	/**
	 * Turns off the power supply.
	 */
	void off() { }

	/**
	 * Sets new supply current.
	 *
	 * @param val desired current in ampers
	 */
	void set_current(double val) { current = val; }

	/**
	 * Reads the current from the power supply.
	 *
	 * @return the current on the output
	 */
	double get_current() { return current; }
private:

	double current;	//!< desired and actual current
};



#endif /* POWERSUPPLY_H_ */
