// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
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

namespace daw { namespace imaging {
	GenericImage<uchar3> FilterDAWGS::filter( GenericImage<uchar3> const & image_input ) {
		//no parallel to valuepos
		typedef boost::unordered_map<int, int> valuepos_t;
		valuepos_t valuepos;

		//no parallel to valuepos
		for( size_t n=0; n<image_input.size( ); ++n ) {
			valuepos[FilterDAWGS::too_gs( image_input[n] )] = 0;
		}

		if( valuepos.size( ) > 256 ) {
			float const inc = (float)valuepos.size( )/256.0f;
			{
				size_t const keys_size = valuepos.size( );
				boost::scoped_array<unsigned int> keys( new unsigned int[keys_size] );
				{
					size_t n=0;
					for( valuepos_t::iterator it=valuepos.begin( ); it != valuepos.end( ); ++it ) {
						keys[n++] = it->first;
					}
				}

				std::sort( keys.get( ), keys.get( ) + keys_size );

				#pragma omp parallel for
				for( int n=0; n<(int)keys_size; ++n ) {
					int const curval = (int)((float)n/inc);
					int const curkey = keys[n];
					#ifdef _DEBUG
					if( curval > 255 ) {
						std::string const msg = "Position in grayscale is too large and cannot fit into 8 bits";
						throw std::runtime_error( msg );
					}
					#endif				
					valuepos[curkey] = curval;
				}
			}

			GenericImage<uchar3> image_output( image_input.width( ), image_input.height( ) );
						
			#pragma omp parallel for
			for( int n=0; n<(int)image_input.size( ); ++n ) {
				unsigned char const curval = (unsigned char)valuepos[FilterDAWGS::too_gs( image_input[n] )];
				image_output[n] = curval;
			}
			return image_output;
		} else { // Already a grayscale image or has enough room for all possible values and no compression needed
			std::cerr << "Already a grayscale image or has enough room for all possible values and no compression needed:" << valuepos.size( ) << std::endl;
			GenericImage<uchar3> image_output( image_input.width( ), image_input.height( ) );

			#pragma omp parallel for
			for( int n=0; n<(int)image_input.size( ); ++n ) {
				unsigned char const curval = (unsigned char)image_input[n].too_float_gs( );
				image_output[n] = curval;
			}
			return image_output;
		}
	}
}}
