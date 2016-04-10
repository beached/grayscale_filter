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

#pragma once

#include "genericimage.h"
#include "genericrgb.h"

#ifdef DAWFILTER_USEPYTHON
#include <boost/python.hpp>
#endif
#include <string>

namespace daw { namespace imaging {
	class FilterDAWGS {
	public:
		static GenericImage<uchar3> filter( GenericImage<uchar3> const & input_image );
	
		static std::string const description( ) {
			return "Convert an RGB image to an optimized grayscale image";
		}

		inline static int too_gs( uchar3 const & pixel ) {
			return 19595*pixel.red + 38469*pixel.green + 7471*pixel.blue;	// 0.299r + 0.587g + 0.114b
		}
		
#ifdef DAWFILTER_USEPYTHON
		static void register_python( std::string const nameoftype = "filter_dawgs" ) {
			boost::python::def( nameoftype.c_str( ), &FilterDAWGS::filter );
		}
#endif
	};
}}

