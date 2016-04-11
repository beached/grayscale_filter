#ifndef HAS_DAWHELPERS_HEADER
#define HAS_DAWHELPERS_HEADER

#include "genericimage.h"
#include "genericrgb.h"

namespace daw { namespace imaging {	
	class Helpers {
	public:
		template<class T>
		static const T max3( const T &value1, const T &value2, const T &value3 ) {
			T maxval = value1;
			if( value2 > maxval ) {
				maxval = value2;
			}
			if( value3 > maxval ) {
				maxval = value3;
			}
			return maxval;
		}

		template<class T>
		static const T min3( const T &value1, const T &value2, const T &value3 ) {
			T maxval = value1;
			if( value2 < maxval ) {
				maxval = value2;
			}
			if( value3 < maxval ) {
				maxval = value3;
			}
			return maxval;
		}

		template<class T>
		static void clampvalue( T &value, const T min, const T max ) {
			if( value < min ) {
				value = min;
			} else if( value > max ) {
				value = max;
			}
		}

		template<class T>
		static float too_gs_small( const T &red, const T &green, const T &blue ) {
			return 0.299f*(float)red + 0.587f*(float)green + 0.114f*(float)blue;
		}
	};
}}

#endif	//HAS_DAWHELPERS_HEADER
