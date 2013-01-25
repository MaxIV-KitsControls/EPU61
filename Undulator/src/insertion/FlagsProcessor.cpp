#include "FlagsProcessor.h"

#include <Undulator.h>
#include <insertion/Communicator.h>

namespace Undulator_ns {

FlagsProcessor::FlagsProcessor(Undulator &und) : und(und) {
}

void FlagsProcessor::process(Communicator *comm)
{
	IDStatus status;
	comm->get_id_status(status);

	und.update_status(status);
}


} /* namespace Undulator_ns */
