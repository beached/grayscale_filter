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


#include "genericimage.h"

namespace daw {
	namespace imaging {

		GenericImage<rgb3>::GenericImage( pos_t const width, pos_t const height ): m_width( width ), m_height( height ), m_size( width*height ), m_id( Random<int>::getNext( ) ), m_image_data( new GenericRGB<uint8_t>[m_size] ) {
			nullcheck( m_image_data.get( ), "Error creating GenericImage" );
		}

		void GenericImage<rgb3>::to_file( std::string const& image_filename, GenericImage<rgb3> const& image_input ) {
			try {
				typedef GenericImage<rgb3>::pos_t pos_t;
				FreeImage image_output( FreeImage_Allocate( static_cast<int32_t>(image_input.width( )), static_cast<int32_t>(image_input.height( )), 24 ) );
				{
					int const maxy = static_cast<int32_t>(image_input.height( )) - 1;
#pragma omp parallel for
					for( int32_t y = 0; y < static_cast<int32_t>(image_input.height( )); ++y ) {
						RGBQUAD rgb_out;
						for( pos_t x = 0; x < image_input.width( ); ++x ) {
							rgb3 const rgb_in = image_input( y, x );
							rgb_out.rgbBlue = rgb_in.blue;
							rgb_out.rgbGreen = rgb_in.green;
							rgb_out.rgbRed = rgb_in.red;
							if( !FreeImage_SetPixelColor( image_output.ptr( ), static_cast<uint32_t>( x ), static_cast<uint32_t>( maxy - y ), &rgb_out ) ) {
								std::string const msg = "Error setting pixel data";
								throw std::runtime_error( msg );
							}
						}
					}
				}
				FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename( image_filename.c_str( ) );
				if( !FreeImage_Save( fif, image_output.ptr( ), image_filename.c_str( ) ) ) {
					std::string const msg = "Error Saving image to file '" + image_filename + "'";
					throw std::runtime_error( msg );
				}
				image_output.close( );
			} catch( std::runtime_error const & ) {
				throw;
			} catch( ... ) {
				std::string const msg = "An unknown exception has been thrown while saving image to file '" + image_filename + "'";
				throw std::runtime_error( msg );
			}
		}

		void GenericImage<rgb3>::to_file( std::string const& image_filename ) const { 
			GenericImage<rgb3>::to_file( image_filename, *this );
		}

		GenericImage<rgb3> GenericImage<rgb3>::from_file( boost::string_ref image_filename ) {
			try {
				typedef GenericImage<rgb3>::pos_t pos_t;
				{
					boost::filesystem::path const pImageFile( image_filename.data( ) );
					if( !boost::filesystem::exists( pImageFile ) ) {
						std::string const msg = "The file '" + image_filename.to_string( ) + "' cannot be found";
						throw std::runtime_error( msg );
					} else if( !boost::filesystem::is_regular_file( pImageFile ) ) {
						std::string const msg = "The file '" + image_filename.to_string( ) + "' is not a regular file";
						throw std::runtime_error( msg );
					}
				}

				// Get Image Format and open the image
				FREE_IMAGE_FORMAT fif = FreeImage_GetFileType( image_filename.data( ) );
				if( fif == FIF_UNKNOWN ) {
					fif = FreeImage_GetFIFFromFilename( image_filename.data( ) );
					if( fif == FIF_UNKNOWN ) {
						std::string const msg = "The file '" + image_filename.to_string( ) + "' cannot be opened.  Cannot determine image type";
						throw std::runtime_error( msg );
					}
				}

				FreeImage image_input( FreeImage_Load( fif, image_filename.data( ) ), "Could not open input image '" + image_filename.to_string( ) + "'" );
				if( !( image_input.bpp( ) == 24 || image_input.bpp( ) == 32 ) || FreeImage_GetColorType( image_input.ptr( ) ) != FIC_RGB ) {
					FIBITMAP * bitmap_test = nullptr;
					bitmap_test = FreeImage_ConvertTo24Bits( image_input.ptr( ) );
					if( nullptr == bitmap_test ) {
						bitmap_test = FreeImage_ConvertTo32Bits( image_input.ptr( ) );
						if( nullptr == bitmap_test ) {
							std::string const msg = "'" + image_filename.to_string( ) + "' is a non RGB8 file.  Files must be RGB8";
							throw std::runtime_error( msg );
						} else {
							std::cerr << "Had to convert image to 32bit RGBA" << std::endl;
						}
					} else {
						std::cerr << "Had to convert image to 24bit RGB" << std::endl;
					}
					image_input.take( bitmap_test );
				}
				GenericImage<rgb3> image_output( image_input.width( ), image_input.height( ) );

				{
					int const maxy = static_cast<int32_t>( image_output.height( ) ) - 1;
#pragma omp parallel for
					for( int32_t y = 0; y < static_cast<int32_t>( image_output.height( ) ); ++y ) {
						RGBQUAD rgb_in;
						for( pos_t x = 0; x < image_output.width( ); ++x ) {
							if( FreeImage_GetPixelColor( image_input.ptr( ), static_cast<uint32_t>( x ), static_cast<uint32_t>( maxy - y ), &rgb_in ) ) {
								rgb3 const rgb_out( rgb_in.rgbRed, rgb_in.rgbGreen, rgb_in.rgbBlue );
								image_output( y, x ) = rgb_out;
							} else {
								std::string const msg = "Error retrieving pixel data from '" + image_filename.to_string( ) + "'";
								throw std::runtime_error( msg );
							}
						}
					}
				}
				image_input.close( );
				return image_output;
			} catch( std::runtime_error const & ) {
				throw;
			} catch( ... ) {
				auto const msg = "Unknown error while reading file'" + image_filename.to_string( ) + "'";
				throw std::runtime_error( msg );
			}
		}

		GenericImage<rgb3>::pos_t GenericImage<rgb3>::width( ) const {
			return m_width;
		}

		GenericImage<rgb3>::pos_t GenericImage<rgb3>::height( ) const {
			return m_height;
		}

		size_t GenericImage<rgb3>::size( ) const {
			return m_size;
		}

		size_t GenericImage<rgb3>::id( ) const {
			return m_id;
		}

		GenericImage<rgb3>::const_reference GenericImage<rgb3>::operator( )( pos_t const y, pos_t const x ) const {
			return m_image_data[y*m_width + x];
		}

		GenericImage<rgb3>::reference GenericImage<rgb3>::operator( )( pos_t const y, pos_t const x ) {
			return m_image_data[y*m_width + x];
		}

		GenericImage<rgb3>::const_reference GenericImage<rgb3>::operator[]( size_t const pos ) const {
			return m_image_data[pos];
		}

		GenericImage<rgb3>::reference GenericImage<rgb3>::operator[]( size_t const pos ) {
			return m_image_data[pos];
		}

		GenericImage<rgb3>::iterator GenericImage<rgb3>::begin( ) {
			return m_image_data.get( );
		}

		GenericImage<rgb3>::const_iterator GenericImage<rgb3>::begin( ) const {
			return m_image_data.get( );
		}

		GenericImage<rgb3>::iterator GenericImage<rgb3>::end( ) {
			return m_image_data.get( ) + m_size;
		}

		GenericImage<rgb3>::const_iterator GenericImage<rgb3>::end( ) const {
			return m_image_data.get( ) + m_size;
		}


		GenericImage<rgb3> from_file( boost::string_ref image_filename ) {
			return GenericImage<rgb3>::from_file( image_filename );
		}
#ifdef DAWFILTER_USEPYTHON
		void GenericImage<rgb3>::register_python( std::string const& nameoftype ) {
			boost::python::class_<GenericImage<rgb3> >( nameoftype.c_str( ), boost::python::init<const GenericImage<rgb3>::pos_t, const GenericImage<rgb3>::pos_t>( ) )
				.def( "from_file", &GenericImage<rgb3>::from_file ).static method( "from_file" )
				.def( "to_file", &GenericImage<rgb3>::to_file ).static method( "to_file" )
				.add_property( "size", &GenericImage<rgb3>::size )
				.add_property( "width", &GenericImage<rgb3>::width )
				.add_property( "height", &GenericImage<rgb3>::height )
				.add_property( "id", &GenericImage<rgb3>::id );
		}
#endif
	}	// namespace imaging
}	// namespace daw
