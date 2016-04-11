#include "nullptr.h"
#include "random.h"

#include <cstdlib>
#include <ctime>
#include <climits>

namespace random_help {
	const unsigned int time_seed( ) {
		time_t now = time( nullptr );
		unsigned char* p = (unsigned char *)&now;
		nullcheck( p, "Point to a struct should never return null" );
		size_t sizeof_now = sizeof( now );
		unsigned int seed = 0;
		for( size_t i = 0; i<sizeof_now; ++i ) {
			seed = seed * (UCHAR_MAX + 2U) + p[i];
		}
		return seed;
	}

	const double uniform_deviate( int seed ) {
		return seed*(1.0/(RAND_MAX+1.0));
	}
}
