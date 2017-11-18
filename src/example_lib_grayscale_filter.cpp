// The MIT License (MIT)
//
// Copyright (c) 2016-2017 Darrell Wright
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

#include <cassert>
#include <chrono>
#include <cstdlib>

#include <daw/daw_benchmark.h>
#include <daw/daw_exception.h>
#include <daw/grayscale_filter/filterdawgs2.h>
#include <daw/grayscale_filter/genericimage.h>

int main( int argc, char **argv ) {
	daw::exception::daw_throw_on_false( argc >= 3, "Must supply a source and destination file" );
	using namespace daw::imaging;

	auto const elapsed = daw::benchmark( [argv]( ) { FilterDAWGS2::filter( from_file( argv[1] ) ).to_file( argv[2] ); } );

	std::cout << "elapsed time: " << daw::utility::format_seconds( elapsed, 2 ) << '\n';

	return EXIT_SUCCESS;
}

