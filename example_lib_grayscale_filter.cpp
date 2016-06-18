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

#include <cstdlib>
#include <cassert>
#include <daw/lib_grayscale_filter/genericimage.h>
#include <daw/lib_grayscale_filter/filterdawgs.h>
#include <chrono>

int main( int argc, char ** argv ) {
	assert( argc > 2 );
	using namespace daw::imaging;

	auto start = std::chrono::system_clock::now( );
	FilterDAWGS::filter( from_file( argv[1] ) ).to_file( argv[2] );
	auto end = std::chrono::system_clock::now( );

	std::chrono::duration<double> elapsed_seconds = end - start;
	auto end_time = std::chrono::system_clock::to_time_t( end );

	std::cout << "finished computation at " << std::ctime( &end_time ) << "elapsed time: " << elapsed_seconds.count( ) << "s\n";


	return EXIT_SUCCESS;
}

