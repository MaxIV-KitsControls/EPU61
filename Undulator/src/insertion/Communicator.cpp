#include "Communicator.h"
#include "GalilCommunicator.h"


namespace Undulator_ns {

Communicator* Communicator::create_communicator(const std::vector<std::string> &params, CommunicatorType type)
{
	// Currently only Galil supported
	return new GalilCommunicator(params[0], params[1]);
}


} /* namespace Undulator_ns */
