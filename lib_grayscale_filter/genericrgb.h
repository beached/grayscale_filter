// The MIT License (MIT)
//
// Copyright (c) 2016 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#pragma once

#include "helpers.h"

#ifdef DAWFILTER_USEPYTHON
#include <boost/python.hpp>
#endif
#include <string>
#include <cstdint>

namespace daw {
	namespace imaging {
		template<class T>
		class GenericRGB final {
		public:
			T blue;
			T green;
			T red;

			GenericRGB( ): blue( 0 ), green( 0 ), red( 0 ) { }
			GenericRGB( T const & GS ): blue( GS ), green( GS ), red( GS ) { }
			GenericRGB( T const & Red, T const & Green, T const & Blue ): blue( Blue ), green( Green ), red( Red ) { }

			void set_all( T const & Red, T const & Green, T const & Blue ) {
				blue = Blue;
				green = Green;
				red = Red;
			}

			void set_all( T const & grayscale ) {
				blue = grayscale;
				green = grayscale;
				red = grayscale;
			}


			GenericRGB( GenericRGB const & ) = default;
			GenericRGB( GenericRGB && ) = default;
			GenericRGB & operator=( GenericRGB const & ) = default;
			GenericRGB & operator=( GenericRGB && ) = default;
			~GenericRGB( ) = default;

			GenericRGB & operator=( T const & src ) {
				red = src;
				green = src;
				blue = src;
				return *this;
			}

			static float colform( GenericRGB<T> const & c, float Red, float Green, float Blue ) {
				return Red*static_cast<float>( c.red ) + Green*static_cast<float>( c.green ) + Blue*static_cast<float>( c.blue );
			}

			float colform( float Red, float Green, float Blue ) const {
				return colform( *this, std::move( Red ), std::move( Green ), std::move( Blue ) );
			}

			void clampvalue( T const & min, T const & max ) {
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

			void mul( T const & value ) {
				blue *= value;
				green *= value;
				red *= value;
			}

			void div( T const & value ) {
				blue /= value;
				green /= value;
				red /= value;
			}

			float too_float_gs( ) const {
				return helpers::too_gs_small( red, green, blue );
			}

#ifdef DAWFILTER_USEPYTHON
			static void register_python( std::string const & nameoftype ) {
				boost::python::class_<GenericRGB>( nameoftype.c_str( ), boost::python::init<>( ) )
					.def( bpython::init<T, T, T>( ) )
					.def_readwrite( "red", & GenericRGB::red )
					.def_readwrite( "green", & GenericRGB::green )
					.def_readwrite( "blue", & GenericRGB::blue );
			}
#endif
		};
		
		template<typename L, typename R>
		void min( GenericRGB<L> const & value, GenericRGB<R> & cur_min ) {
			if( value.red < cur_min.red ) {
				cur_min.red = value.red;
			}
			if( value.green < cur_min.green ) {
				cur_min.green = value.green;
			}
			if( value.blue < cur_min.blue ) {
				cur_min.blue = value.blue;
			}
		}

		template<typename L, typename R>
		void max( GenericRGB<L> const & value, GenericRGB<R> & cur_max ) {
			if( value.red > cur_max.red ) {
				cur_max.red = value.red;
			}
			if( value.green > cur_max.green ) {
				cur_max.green = value.green;
			}
			if( value.blue > cur_max.blue ) {
				cur_max.blue = value.blue;
			}
		}
	}	// namespace imaging
}	// namespace daw

using rgb3 = daw::imaging::GenericRGB<uint8_t>;

