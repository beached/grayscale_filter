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

#include "nullptr.h"

#include <FreeImage.h>
#include <string>

namespace daw { namespace imaging {
	class FreeImage {
	private:
		FIBITMAP* m_bitmap;			

	public:
		FreeImage( FIBITMAP* bitmap ): m_bitmap( bitmap ) {
			nullcheck( m_bitmap, "Error while loading FreeImage bitmap" );
		}

		FreeImage( FIBITMAP* bitmap, const std::string& errmsg ): m_bitmap( bitmap ) {
			nullcheck( m_bitmap, errmsg );
		}
		
		FreeImage( const FreeImage& other ): m_bitmap( FreeImage_Clone( other.m_bitmap ) ) {
			nullcheck( m_bitmap, "Error while loading FreeImage bitmap" );
		}

		inline FreeImage& take( FreeImage& other ) {
			if( this != &other ) {
				nullcheck( other.m_bitmap, "Error, attempt to take ownership from a null FreImage" );
				m_bitmap = other.m_bitmap;
				other.m_bitmap = nullptr;
			}
			return *this;
		}

		inline FreeImage& take( FIBITMAP* bitmap ) {
			if( m_bitmap != bitmap ) {
				nullcheck( bitmap, "Error, attempt to take ownership from a null FreImage" );
				m_bitmap = bitmap;				
			}
			return *this;
		}

		inline void close( ) {
			if( nullptr != m_bitmap ) {
				FreeImage_Unload( m_bitmap );
				m_bitmap = nullptr;
			}
		}

		inline FIBITMAP* const ptr( ) {
			return m_bitmap;
		}

		inline const unsigned int height( ) const {
			return FreeImage_GetHeight( m_bitmap );
		}

		inline const unsigned int width( ) const {
			return FreeImage_GetWidth( m_bitmap );
		}

		inline const unsigned int bpp( ) const {
			return FreeImage_GetBPP( m_bitmap );
		}		

		~FreeImage( ) {
			close( );
		}
	};
}}

