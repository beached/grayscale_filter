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


#include "fimage.h"

namespace daw {
	namespace imaging {
		FreeImage::FreeImage( FIBITMAP * bitmap ): m_bitmap( bitmap ) {
			nullcheck( m_bitmap, "Error while loading FreeImage bitmap" );
		}

		FreeImage::FreeImage( FIBITMAP * bitmap, std::string const & errmsg ): m_bitmap( bitmap ) {
			nullcheck( m_bitmap, errmsg );
		}

		FreeImage::FreeImage( FreeImage const & other ): m_bitmap( FreeImage_Clone( other.m_bitmap ) ) {
			nullcheck( m_bitmap, "Error while loading FreeImage bitmap" );
		}

		FreeImage & FreeImage::take( FreeImage & other ) {
			if( this != &other ) {
				nullcheck( other.m_bitmap, "Error, attempt to take ownership from a null FreImage" );
				m_bitmap = other.m_bitmap;
				other.m_bitmap = nullptr;
			}
			return *this;
		}

		FreeImage & FreeImage::take( FIBITMAP * bitmap ) {
			if( m_bitmap != bitmap ) {
				nullcheck( bitmap, "Error, attempt to take ownership from a null FreImage" );
				m_bitmap = bitmap;
			}
			return *this;
		}

		void FreeImage::close( ) {
			if( nullptr != m_bitmap ) {
				FreeImage_Unload( m_bitmap );
				m_bitmap = nullptr;
			}
		}

		FIBITMAP * FreeImage::ptr( ) {
			return m_bitmap;
		}

		FIBITMAP const* FreeImage::ptr( ) const {
			return m_bitmap;
		}

		unsigned FreeImage::height( ) const {
			return FreeImage_GetHeight( m_bitmap );
		}

		unsigned FreeImage::width( ) const {
			return FreeImage_GetWidth( m_bitmap );
		}

		unsigned FreeImage::bpp( ) const {
			return FreeImage_GetBPP( m_bitmap );
		}

		FreeImage::~FreeImage( ) {
			close( );
		}
	}	// namespace imaging
}	// namespace daw
