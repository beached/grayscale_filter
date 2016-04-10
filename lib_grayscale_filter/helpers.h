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

