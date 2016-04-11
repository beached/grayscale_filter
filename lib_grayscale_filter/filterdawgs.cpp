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

#include <boost/scoped_array.hpp>
#include <boost/unordered/unordered_map.hpp>
#include <iterator>
#include <map>
#include <omp.h>
#include <stdexcept>
#include <utility>
#include <vector>

namespace daw {
	namespace imaging {
		GenericImage<rgb3> FilterDAWGS::filter( GenericImage<rgb3> const & image_input ) {
			//no parallel to valuepos
			typedef boost::unordered_map<int32_t,int32_t> valuepos_t;
			valuepos_t valuepos;

			//no parallel to valuepos
			for( size_t n = 0; n < image_input.size( ); ++n ) {
				valuepos[FilterDAWGS::too_gs( image_input[n] )] = 0;
			}

			if( valuepos.size( ) > 256 ) {
				auto const inc = static_cast<float>( valuepos.size( ) ) / 256.0f;
				{
					auto const keys_size = valuepos.size( );
					boost::scoped_array<uint32_t> keys( new uint32_t[keys_size] );
					{
						size_t n = 0;
						for( auto it = valuepos.begin( ); it != valuepos.end( ); ++it ) {
							keys[n++] = it->first;
						}
					}

					std::sort( keys.get( ), keys.get( ) + keys_size );

#pragma omp parallel for
					for( int32_t n = 0; n < static_cast<int>( keys_size ); ++n ) {
						auto const curval = static_cast<int>( static_cast<float>( n ) / inc );
						int const curkey = keys[n];	// TODO clarify why sign changes
#ifdef _DEBUG
						if( curval > 255 ) {
							std::string const msg = "Position in grayscale is too large and cannot fitint32_to 8 bits";
							throw std::runtime_error( msg );
						}
#endif				
						valuepos[curkey] = curval;
					}
				}

				GenericImage<rgb3> image_output( image_input.width( ), image_input.height( ) );

#pragma omp parallel for
				for( int32_t n = 0; n < static_cast<int>( image_input.size( ) ); ++n ) {
					auto const curval = static_cast<uint8_t>( valuepos[FilterDAWGS::too_gs( image_input[n] )] );
					image_output[n] = curval;
				}
				return image_output;
			} else { // Already a grayscale image or has enough room for all possible values and no compression needed
				std::cerr << "Already a grayscale image or has enough room for all possible values and no compression needed:" << valuepos.size( ) << std::endl;
				GenericImage<rgb3> image_output( image_input.width( ), image_input.height( ) );

#pragma omp parallel for
				for( int32_t n = 0; n < static_cast<int>( image_input.size( ) ); ++n ) {
					auto const curval = static_cast<uint8_t>( image_input[n].too_float_gs( ) );
					image_output[n] = curval;
				}
				return image_output;
			}
		}

		int FilterDAWGS::too_gs( rgb3 const & pixel ) {
			return 19595 * pixel.red + 38469 * pixel.green + 7471 * pixel.blue;	// 0.299r + 0.587g + 0.114b
		}

#ifdef DAWFILTER_USEPYTHON
		void FilterDAWGS::register_python( std::string const nameoftype ) {
			boost::python::def( nameoftype.c_str( ), &FilterDAWGS::filter );
		}
#endif
	}	// namespace imaging
}	// namespace daw

