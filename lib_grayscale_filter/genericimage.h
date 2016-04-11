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
#include "fimage.h"


namespace daw {
	namespace imaging {
		template<class T>
		struct GenericImage {
			using pos_t = unsigned int;
		private:
			pos_t m_width;
			pos_t m_height;
			size_t m_size;
			size_t m_id;
			boost::shared_array<T> m_image_data;
		public:
			GenericImage( pos_t const width, pos_t const height ): m_width( width ), m_height( height ), m_size( width*height ), m_id( Random<int>::getNext( ) ), m_image_data( new T[m_size] ) {
				nullcheck( m_image_data.get( ), "Error creating GenericImage" );
			}

			pos_t width( ) const {
				return m_width;
			}

			pos_t height( ) const {
				return m_height;
			}

			size_t size( ) const {
				return m_size;
			}

			size_t id( ) const {
				return m_id;
			}

			T const & operator( )( pos_t const row, pos_t const col ) const {
				return m_image_data[row*m_width + col];
			}

			T & operator( )( pos_t const row, pos_t const col ) {
				return m_image_data[row*m_width + col];
			}

			T const & operator[]( size_t const pos ) const {
				return m_image_data[pos];
			}

			T & operator[]( size_t const pos ) {
				return m_image_data[pos];
			}

#ifdef DAWFILTER_USEPYTHON
			static void register_python( std::string const & nameoftype ) {
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
		class GenericImage<rgb3> {
		public:
			using pos_t = unsigned int;
		private:
			pos_t m_width;
			pos_t m_height;
			size_t m_size;
			size_t m_id;
			boost::shared_array<rgb3> m_image_data;
		public:
			GenericImage( pos_t const width, pos_t const height );
			static void toFile( std::string const& image_filename, GenericImage<rgb3> const& image_input );
			static GenericImage<rgb3> fromFile( std::string const& image_filename );
			pos_t width( ) const;
			pos_t height( ) const;
			size_t size( ) const;
			size_t id( ) const;
			rgb3 const & operator( )( pos_t const y, pos_t const x ) const;
			rgb3 & operator( )( pos_t const y, pos_t const x );
			rgb3 const & operator[]( size_t const pos ) const;
			rgb3 & operator[]( size_t const pos );

#ifdef DAWFILTER_USEPYTHON
			static void register_python( std::string const& nameoftype );
#endif
		};
	}	// namespace imaging
}	// namespace daw
