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

#include "nullptr.h"
#include "random.h"

#include <cstdlib>
#include <ctime>
#include <climits>

namespace random_help {
	unsigned int time_seed( ) {
		time_t now = time( nullptr );
		unsigned char* p = (unsigned char *)&now;
		nullcheck( p, "Point to a struct should never return null" );
		size_t sizeof_now = sizeof( now );
		unsigned int seed = 0;
		for( size_t i = 0; i<sizeof_now; ++i ) {
			seed = seed * (UCHAR_MAX + 2U) + p[i];
		}
		return seed;
	}

	double uniform_deviate( int seed ) {
		return seed*(1.0/(RAND_MAX+1.0));
	}
}	// namespace random_help

