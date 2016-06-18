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

#include "nullptr.h"

#include <FreeImage.h>

#include <boost/utility/string_ref.hpp>

namespace daw {
	namespace imaging {
		class FreeImage final {
			FIBITMAP * m_bitmap;

		public:
			using pos_t = decltype(FreeImage_GetWidth( m_bitmap ));
			using bpp_t = decltype(FreeImage_GetBPP( m_bitmap ));
			FreeImage( ) = delete;
			FreeImage( FIBITMAP * bitmap );
			FreeImage( FIBITMAP * bitmap, boost::string_ref errmsg );			
			~FreeImage( );
			FreeImage( FreeImage const & other );
			FreeImage & operator=( FreeImage const & rhs );
			FreeImage( FreeImage && ) = default;
			FreeImage & operator=( FreeImage && ) = default;

			FreeImage & take( FreeImage & other );
			FreeImage & take( FIBITMAP * bitmap );
			void close( );
			
			FIBITMAP * ptr( );
			FIBITMAP const * ptr( ) const;
			
			pos_t height( ) const;
			pos_t width( ) const;
			bpp_t bpp( ) const;
		};
	}
}

