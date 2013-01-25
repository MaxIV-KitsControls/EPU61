#ifndef POLLERTHREAD_H_
#define POLLERTHREAD_H_

#include <tango.h>

namespace Undulator_ns
{

class Undulator;
class Communicator;

/**
 * Concrete class, runs polling thread and executes Processor objects every N-th cycle.
 */
class PollerThread :
	public omni_thread, public Tango::LogAdapter
{
public:
	/**
	 * Implement this class to register a processor with the thread.
	 */
	class Processor
	{
	public:
		/*
		 * Executes a processor, supplying a motion controller communicator object
		 *
		 * @param comm Motion controller communicator object
		 */
		virtual void process(Communicator *comm) = 0;
	};

private:
	/**
	 * A processor item, stores number of cycle to use to trigger the UpdateProcessor specified.
	 */
	typedef std::pair<int,Processor*> ProcessorItem;

	/**
	 * A datatype, ProcessorItems in a vector
	 */
	typedef std::vector <ProcessorItem> ProcessorVector;

public:
	/**
	 * Constructor of updater class.
	 * @param dev Reference to Undulator device implementation class.
	 * @param sleepTime Time in milliseconds between updates.
	 */
	PollerThread(Undulator& dev, ulong sleepTime,const std::string &gapProxy, const std::string &phaseProxy);

	/**
	 * Destructor for the thread object
	 */
	virtual ~PollerThread();

	/**
	 * Starts the thread.
	 */
	void go();

	/**
	 * Stops the thread.
	 */
	void abort();

	/**
	 *	Registers a processor for every n-th cycle.
	 *
	 *	The caller is responsible for the ownershipo of the proc object.
	 *
	 *	Note: Should be called only before executing go() method. Not thread-safe.
	 *
	 *	@param n Processor will be executed every n-th cycle
	 *	@param proc An object implementing the UpdateProcessor abstract interface
	 */
	void register_processor(int n, Processor *proc) {
		processors.push_back(ProcessorItem(n,proc));
	}

private:
	/**
	 * Runs the loop until user stops it or gets destroyed.
	 *
	 * @param arg Argument passed to the thread
	 */
	virtual void* run_undetached(void *arg);

// Private variables.
private:
	volatile bool run; 	//!< Flag signaling whether the thread loop should run.
	ulong sleepTime; 	//!< Sleep time between iterations of the scanning in the thread main loop.
	Communicator *comm; //!< Communicator object for the Processors

	Undulator &dev;

	ProcessorVector processors; //!< Contains all registered processors
	unsigned int cycleCounter;	//!< Counts the cycles

	time_t previousTime; //!< Previous time
};

};

#endif /* POLLERTHREAD_H_ */
