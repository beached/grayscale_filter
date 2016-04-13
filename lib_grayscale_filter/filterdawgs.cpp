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

#include "filterdawgs.h"
#include "genericimage.h"
#include "genericrgb.h"
#include "daw_parallel_algorithm.h"
#include "daw_array.h"
#include <boost/scoped_array.hpp>
#include <unordered_map>
#include <iterator>
#include <map>
//#include <omp.h>
#include <stdexcept>
#include <utility>
#include <vector>

namespace daw {
	namespace imaging {
		GenericImage<rgb3> FilterDAWGS::filter( GenericImage<rgb3> image_input ) {
			//no parallel to valuepos
			using valuepos_t = std::unordered_map<int32_t, int32_t>;
			valuepos_t valuepos;

			//no parallel to valuepos			
			for( auto & rgb : image_input ) {
				valuepos[FilterDAWGS::too_gs( rgb )] = 0;
			}

			if( valuepos.size( ) > 256 ) {
				auto const inc = static_cast<float>( valuepos.size( ) ) / 256.0f;
				{
					daw::array<int32_t> keys( valuepos.size( ) );
					daw::algorithm::parallel::transform( valuepos.begin( ), valuepos.end( ), keys.begin( ), []( auto val ) {
						return val.first;
					} );

					std::sort( keys.begin( ), keys.end( ) );

					daw::algorithm::parallel::for_each( 0, keys.size( ), [&]( auto n ) {
						auto const curval = static_cast<int32_t>(static_cast<float>(n) / inc);
						auto const curkey = static_cast<int32_t>(keys[n]);	// TODO clarify why sign changes
						valuepos[curkey] = curval;
					} );
				}

				GenericImage<rgb3> image_output( image_input.width( ), image_input.height( ) );

				// TODO: make parallel
				daw::algorithm::parallel::transform( image_input.begin( ), image_input.end( ), image_output.begin( ), [&vp=valuepos]( rgb3 const & rgb ) {
					return static_cast<uint8_t>(vp[FilterDAWGS::too_gs( rgb )]);
				} );
				return image_output;
			} else { // Already a grayscale image or has enough room for all possible values and no compression needed
				std::cerr << "Already a grayscale image or has enough room for all possible values and no compression needed:" << valuepos.size( ) << std::endl;
				GenericImage<rgb3> image_output( image_input.width( ), image_input.height( ) );

				assert( image_input.size( ) <= image_output.size( ) );
				// TODO: make parallel
				daw::algorithm::parallel::transform( image_input.begin( ), image_input.end( ), image_output.begin( ), []( rgb3 const & rgb ) {
					return static_cast<uint8_t>(rgb.too_float_gs( ));
				} );
				return image_output;
			}
		}

		int32_t FilterDAWGS::too_gs( rgb3 const & pixel ) {
			return 19595 * pixel.red + 38469 * pixel.green + 7471 * pixel.blue;	// 0.299r + 0.587g + 0.114b
		}

#ifdef DAWFILTER_USEPYTHON
		void FilterDAWGS::register_python( std::string const nameoftype ) {
			boost::python::def( nameoftype.c_str( ), &FilterDAWGS::filter );
		}
#endif
	}	// namespace imaging
}	// namespace daw

