#ifndef HAS_DAWFIMAGE_HEADER
#define HAS_DAWFIMAGE_HEADER

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

#endif	//HAS_DAWFIMAGE_HEADER
