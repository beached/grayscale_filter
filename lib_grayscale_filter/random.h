#ifndef HAS_RANDOM_HEADER
#define HAS_RANDOM_HEADER

#include <cstdlib>

namespace random_help {
	const double uniform_deviate( int seed );
	const unsigned int time_seed( );
}

template<class T, T min=(T)0, T max=(T)100000000>
class Random {
private:
	static const int getNextValue( const T low, const T high ) {
		return (T)((double)low + random_help::uniform_deviate( rand( ) )*(double)(high - low));
	}
public:
	static const T getNext( ) {
		static bool has_seeded = false;
		if( !has_seeded ) {
			has_seeded = true;
			srand( random_help::time_seed( ) );
		}
		return getNextValue( min, max );
	}
};

#endif	//HAS_RANDOM_HEADER
