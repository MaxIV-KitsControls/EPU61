#ifndef UTILEPU_H_
#define UTILEPU_H_

#include <time.h>

namespace Undulator_ns {

/**
 * Retrieves number of milliseconds passed from some epoch
 */
inline u_int64_t get_miliseconds()
{
	timeval tv;
	gettimeofday(&tv, NULL);

	return tv.tv_sec*1000 + tv.tv_usec;
}

} /* namespace Undulator_ns */

#endif /* UTILEPU_H_ */
