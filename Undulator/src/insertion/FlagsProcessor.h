/*
 * FlagsProcessor.h
 *
 *  Created on: Jan 9, 2013
 *      Author: green
 */

#ifndef FLAGSPROCESSOR_H_
#define FLAGSPROCESSOR_H_

#include "PollerThread.h"

namespace Undulator_ns {

class Undulator;

/**
 * PollerThread::Processor object that handles polling lower priority information from the motion controller
 */
class FlagsProcessor : public PollerThread::Processor {
public:
	/**
	 * Constructs a FlagsProcessor object
	 *
	 * @param und Reference to undulator device implementation object
	 */
	FlagsProcessor(Undulator &und);

	/**
	 * Destructs the object
	 */
	virtual ~FlagsProcessor() {}

	virtual void process(Communicator *comm);
private:
	Undulator &und;		//!< reference to device implementation object
};

} /* namespace Undulator_ns */
#endif /* FLAGSPROCESSOR_H_ */
