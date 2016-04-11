#ifndef HAS_DAWGENERICRGB_HEADER
#define HAS_DAWGENERICRGB_HEADER

#include "helpers.h"

#ifdef DAWFILTER_USEPYTHON
#include <boost/python.hpp>
#endif
#include <string>

namespace daw { namespace imaging {
	template<class T>
	class GenericRGB {
	public:		
		T blue;
		T green;
		T red;

		GenericRGB( ): blue( 0 ), green( 0 ), red( 0 ) { }
		GenericRGB( const T& GS ): blue( GS ), green( GS ), red( GS ) { }
		GenericRGB( const T& Red, const T& Green, const T& Blue ):  blue( Blue ), green( Green ), red( Red ) { }

		void set_all( const T& Red, const T& Green, const T& Blue ) {
			blue = Blue;
			green = Green;
			red = Red;
		}

		void set_all( const T& grayscale ) {
			blue = grayscale;
			green = grayscale;
			red = grayscale;
		}


		GenericRGB &operator=( const GenericRGB &src ) {
			red = src.red;
			green = src.green;
			blue = src.blue;
			return *this;
		}

		GenericRGB &operator=( const T &src ) {
			red = src;
			green = src;
			blue = src;
			return *this;
		}

		static const float colform( const GenericRGB<T>& c, const float Red, const float Green, const float Blue ) {
			return Red*(float)c.red + Green*(float)c.green + Blue*(float)c.blue;
		}

		const float colform( const float Red, const float Green, const float Blue ) const {
			return colform( *this, Red, Green, Blue );
		}

		void clampvalue( const T& min, const T& max ) {
			if( red < min ) {
				red = min;
			} else if( red > max ) {
				red = max;
			}
			if( green < min ) {
				green = min;
			} else if( green > max ) {
				green = max;
			}
			if( blue < min ) {
				blue = min;
			} else if( blue > max ) {
				blue = max;
			}
		}

		static void min( const GenericRGB &value, GenericRGB &cur_min ) {
			if ( value.red < cur_min.red ) {
				cur_min.red = value.red;
			}
			if ( value.green < cur_min.green ) {
				cur_min.green = value.green;
			}
			if ( value.blue < cur_min.blue ) {
				cur_min.blue = value.blue;
			}
		}

		static void max( const GenericRGB &value, GenericRGB &cur_max ) {
			if ( value.red > cur_max.red ) {
				cur_max.red = value.red;
			}
			if ( value.green > cur_max.green ) {
				cur_max.green = value.green;
			}
			if ( value.blue > cur_max.blue ) {
				cur_max.blue = value.blue;
			}
		}

		T min( ) const {
			T ret = red;
			if( green < ret ) {
				ret = green;
			}
			if( blue < ret ) {
				ret = blue;
			}
			return ret;
		}

		T max( ) const {
			T ret = red;
			if( green > ret ) {
				ret = green;
			}
			if( blue > ret ) {
				ret = blue;
			}
			return ret;
		}

		void mul( const T& value ) {
			blue *= value;
			green *= value;
			red *= value;			
		}

		void div( const T& value ) {
			blue /= value;
			green /= value;
			red /= value;			
		}

		float too_float_gs( ) const {
			return Helpers::too_gs_small( red, green, blue );
		}

#ifdef DAWFILTER_USEPYTHON
		static void register_python( const std::string& nameoftype ) {
			boost::python::class_<GenericRGB>( nameoftype.c_str( ), boost::python::init<>( ) )
				.def( bpython::init<T, T, T>( ) )
				.def_readwrite( "red", &GenericRGB::red )
				.def_readwrite( "green", &GenericRGB::green )
				.def_readwrite( "blue", &GenericRGB::blue );
		}
#endif
	};
}}

typedef daw::imaging::GenericRGB<unsigned char> uchar3;

#else
namespace daw { namespace imaging {
	template<class T> class GenericRGB;
	typedef daw::imaging::GenericRGB<unsigned char> uchar3;
}}
#endif	//HAS_DAWGENERICRGB_HEADER
