#ifndef HAS_DAWGENERICIMAGE_HEADER
#define HAS_DAWGENERICIMAGE_HEADER

#include "fimage.h"
#include "genericrgb.h"
#include "nullptr.h"
#include "random.h"


#ifdef DAWFILTER_USEPYTHON
#include <boost/python.hpp>
#endif

#include <boost/shared_array.hpp>
#include <boost/filesystem.hpp>

#include <stdexcept>
#include <string>


namespace daw { namespace imaging {
	template<class T>
	class GenericImage {
	public:
		typedef unsigned int pos_t;
	private:				
		pos_t m_width;
		pos_t m_height;
		size_t m_size;
		size_t m_id;
		boost::shared_array<T> m_image_data;
	public:
		GenericImage( const pos_t width, const pos_t height ): m_width( width ), m_height( height ), m_size( width*height ), m_id( Random<int>::getNext( ) ),  m_image_data( new T[m_size] )  {
			nullcheck( m_image_data.get( ), "Error creating GenericImage" );
		}

		const pos_t width( ) const {
			return m_width;
		}

		const pos_t height( ) const {
			return m_height;
		}

		const size_t size( ) const {
			return m_size;
		}

		const size_t id( ) const {
			return m_id;
		}

		const T operator( )( const pos_t row, const pos_t col ) const {
			return m_image_data[row*m_width+col];
		}

		T& operator( )( const pos_t row, const pos_t col ) {
			return m_image_data[row*m_width+col];
		}

		const T operator[]( const size_t pos ) const {
			return m_image_data[pos];
		}

		T& operator[]( const size_t pos ) {
			return m_image_data[pos];
		}

#ifdef DAWFILTER_USEPYTHON
		static void register_python( const std::string& nameoftype ) {
			boost::python::class_<GenericImage>( nameoftype.c_str( ), boost::python::init<const GenericImage::pos_t, const GenericImage::pos_t>( ) )		
				.def( "fromFile", &GenericImage::fromFile ).staticmethod( "fromFile" )
				.def( "toFile", &GenericImage::toFile ).staticmethod( "toFile" )
				.add_property( "size", &GenericImage::size )
				.add_property( "width", &GenericImage::width )
				.add_property( "height", &GenericImage::height )
				.add_property( "id", &GenericImage::id );
		}
#endif
	};

	template<>
	class GenericImage<uchar3> {
	public:
		typedef unsigned int pos_t;
	private:				
		pos_t m_width;
		pos_t m_height;
		size_t m_size;
		size_t m_id;
		boost::shared_array<uchar3> m_image_data;
	public:
		GenericImage( const pos_t width, const pos_t height ): m_width( width ), m_height( height ), m_size( width*height ), m_id( Random<int>::getNext( ) ),  m_image_data( new GenericRGB<unsigned char>[m_size] )  {
			nullcheck( m_image_data.get( ), "Error creating GenericImage" );
		}

		static void toFile( const std::string &image_filename, const GenericImage<uchar3> &image_input ) {
			try {
				typedef GenericImage<uchar3>::pos_t pos_t;
				FreeImage image_output( FreeImage_Allocate( (int)image_input.width( ), (int)image_input.height( ), 24 ) );
				{
					const int maxy = (int)image_input.height( ) - 1;
					#pragma omp parallel for
					for( int y=0; y<(int)image_input.height( ); ++y ) {
						RGBQUAD rgb_out;
						for( pos_t x=0; x<image_input.width( ); ++x ) {
							const uchar3 rgb_in = image_input( y, x );
							rgb_out.rgbBlue = rgb_in.blue;
							rgb_out.rgbGreen = rgb_in.green;
							rgb_out.rgbRed = rgb_in.red;
							if( !FreeImage_SetPixelColor( image_output.ptr( ), (unsigned int)x, (unsigned int)(maxy-y), &rgb_out ) ) {
								const std::string msg = "Error setting pixel data";
								throw std::runtime_error( msg );
							}
						}
					}
				}
				FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename( image_filename.c_str( ) );
				if( !FreeImage_Save( fif, image_output.ptr( ), image_filename.c_str( ) ) ) {
					const std::string msg = "Error Saving image to file '" + image_filename + "'";
					throw std::runtime_error( msg );
				}
				image_output.close( );
			} catch( const std::runtime_error& ) {
				throw;
			} catch(...) {
				const std::string msg = "An unknown exception has been thrown while saving image to file '" + image_filename + "'";
				throw std::runtime_error( msg );
			}
		}

		static GenericImage<uchar3> fromFile( const std::string& image_filename ) {
			try {
				typedef GenericImage<uchar3>::pos_t pos_t;
				{
					const boost::filesystem::path pImageFile( image_filename.c_str( ) );
					if( !boost::filesystem::exists( pImageFile ) ) {
						const std::string msg = "The file '" + image_filename + "' cannot be found";
						throw std::runtime_error( msg );
					} else if( !boost::filesystem::is_regular_file( pImageFile ) ) {
						const std::string msg = "The file '" + image_filename + "' is not a regular file";
						throw std::runtime_error( msg );
					}
				}

				// Get Image Format and open the image
				FREE_IMAGE_FORMAT fif = FreeImage_GetFileType( image_filename.c_str( ) );
				if( fif == FIF_UNKNOWN ) {
					fif = FreeImage_GetFIFFromFilename( image_filename.c_str( ) );
					if( fif == FIF_UNKNOWN ) {
						const std::string msg = "The file '" + image_filename + "' cannot be opened.  Cannot determine image type";
						throw std::runtime_error( msg );
					}
				}

				FreeImage image_input( FreeImage_Load( fif, image_filename.c_str( ) ), "Could not open input image '" + image_filename + "'" );
				if( !(image_input.bpp( ) == 24 || image_input.bpp( ) == 32 ) || FreeImage_GetColorType( image_input.ptr( ) ) != FIC_RGB ) {
					FIBITMAP* bitmap_test = nullptr;
					bitmap_test = FreeImage_ConvertTo24Bits( image_input.ptr( ) );
					if( nullptr == bitmap_test ) {
						bitmap_test = FreeImage_ConvertTo32Bits( image_input.ptr( ) );
						if( nullptr == bitmap_test ) {
							const std::string msg = "'" + image_filename + "' is a non RGB8 file.  Files must be RGB8";
							throw std::runtime_error( msg );
						} else {
							std::cerr << "Had to convert image to 32bit RGBA" << std::endl;
						}
					} else {
						std::cerr << "Had to convert image to 24bit RGB" << std::endl;
					}
					image_input.take( bitmap_test );
				}
				GenericImage<uchar3> image_output( image_input.width( ), image_input.height( ) );

				{
					const int maxy = (int)image_output.height( ) - 1;
					#pragma omp parallel for
					for( int y=0; y<(int)image_output.height( ); ++y ) {
						RGBQUAD rgb_in;
						for( pos_t x=0; x<image_output.width( ); ++x ) {					
							if( FreeImage_GetPixelColor( image_input.ptr( ), (unsigned int)x, (unsigned int)(maxy-y), &rgb_in ) ) {
								const uchar3 rgb_out( rgb_in.rgbRed, rgb_in.rgbGreen, rgb_in.rgbBlue );
								image_output( y, x ) = rgb_out;
							} else {
								const std::string msg = "Error retrieving pixel data from '" + image_filename + "'";
								throw std::runtime_error( msg );
							}
						}
					}
				}
				image_input.close( );
				return image_output;
			} catch( const std::runtime_error& ) {
				throw;
			} catch(...) {
				const std::string msg = "Unknown error while reading file'" + image_filename + "'";
				throw std::runtime_error( msg );
			}
		}

		const pos_t width( ) const {
			return m_width;
		}

		const pos_t height( ) const {
			return m_height;
		}

		const size_t size( ) const {
			return m_size;
		}

		const size_t id( ) const {
			return m_id;
		}

		const uchar3 operator( )( const pos_t y, const pos_t x ) const {
			return m_image_data[y*m_width+x];
		}

		uchar3& operator( )( const pos_t y, const pos_t x ) {
			return m_image_data[y*m_width+x];
		}

		const uchar3 operator[]( const size_t pos ) const {
			return m_image_data[pos];
		}

		uchar3& operator[]( const size_t pos ) {
			return m_image_data[pos];
		}

#ifdef DAWFILTER_USEPYTHON
		static void register_python( const std::string& nameoftype ) {
			boost::python::class_<GenericImage<uchar3> >( nameoftype.c_str( ), boost::python::init<const GenericImage<uchar3>::pos_t, const GenericImage<uchar3>::pos_t>( ) )		
				.def( "fromFile", &GenericImage<uchar3>::fromFile ).staticmethod( "fromFile" )
				.def( "toFile", &GenericImage<uchar3>::toFile ).staticmethod( "toFile" )
				.add_property( "size", &GenericImage<uchar3>::size )
				.add_property( "width", &GenericImage<uchar3>::width )
				.add_property( "height", &GenericImage<uchar3>::height )
				.add_property( "id", &GenericImage<uchar3>::id );
		}
#endif
	};

}}
#else
namespace daw { namespace imaging {
	template<class T> class GenericImage;
}}

#endif	//HAS_DAWGENERICIMAGE_HEADER
