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

#include "filterrotate.h"
#include "genericimage.h"
#include "genericrgb.h"

#ifdef DAWFILTER_USEPYTHON
#include <boost/python.hpp>
#endif
#include <string>

namespace daw { namespace imaging {
	GenericImage<uchar3> FilterRotate::filter( const GenericImage<uchar3> &image_input, const unsigned int angle ) {
		typedef GenericImage<uchar3>::pos_t pos_t;

		if( angle > 3 ) {
			throw std::runtime_error( "Cannot specify an angle other than 0 to 3 inclusive" );
		}
		switch( angle ) {	// 0/default = no rotation, 1 = 90 degrees, 2 = 180 degrees, 3 = 270 degrees
		case 1: {
			GenericImage<uchar3> image_rotated( image_input.height( ), image_input.width( ) );
			pos_t const maxy = image_input.height( ) - 1;

			#pragma omp parallel for
			for( int y=0; y<(int)image_input.height( ); ++y ) {
				for( pos_t x=0; x<image_input.width( ); ++x ) {
					image_rotated( x, maxy - y ) = image_input( y, x );
				}
			}			
			return image_rotated;
				}
		case 2: {
			GenericImage<uchar3> image_rotated( image_input.width( ), image_input.height( ) );			
			pos_t const maxx = image_input.width( ) - 1;
			pos_t const maxy = image_input.height( ) - 1;
			#pragma omp parallel for
			for( int y=0; y<(int)image_input.height( ); ++y ) {
				for( pos_t x=0; x<image_input.width( ); ++x ) {
					image_rotated( maxy - (pos_t)y, maxx - x ) = image_input( y, x );
				}
			}	
			return image_rotated;
				}
		case 3: {
			GenericImage<uchar3> image_rotated( image_input.height( ), image_input.width( ) );			
			pos_t const maxx = image_input.width( ) - 1;
			#pragma omp parallel for
			for( int y=0; y<(int)image_input.height( ); ++y ) {
				for( pos_t x=0; x<image_input.width( ); ++x ) {
					image_rotated( maxx - x, y ) = image_input( y, x );
				}
			}	
			return image_rotated;
				}
		default: {	// This is here to catch.  You should not use a rotate of 0
			std::cerr << "Rotate Filter called without a rotation.  Returning copied original image" << std::endl;
			GenericImage<uchar3> image_rotated( image_input.height( ), image_input.width( ) );
			#pragma omp parallel for
			for( int y=0; y<(int)image_input.height( ); ++y ) {
				for( pos_t x=0; x<image_input.width( ); ++x ) {
					image_rotated( y, x ) = image_input( y, x );
				}
			}
			return image_rotated;
				 }
		}
	}

#ifdef DAWFILTER_USEPYTHON
	static void register_python( const std::string nameoftype = "filter_rotate" ) {
		boost::python::def( nameoftype.c_str( ), &FilterRotate::filter );
	}
#endif

}}
