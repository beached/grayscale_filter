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
#include <boost/utility/string_ref.hpp>


namespace daw {
	namespace imaging {
		template<class T>
		struct GenericImage {
			using pos_t = uint32_t;
		private:
			pos_t m_width;
			pos_t m_height;
			size_t m_size;
			size_t m_id;
			boost::shared_array<T> m_image_data;
		public:
			GenericImage( pos_t width, pos_t height ): m_width( width ), m_height( height ), m_size( width*height ), m_id( Random<size_t>::getNext( ) ), m_image_data( new T[m_size] ) {
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
					.def( "from_file", &GenericImage::from_file ).static method( "from_file" )
					.def( "to_file", &GenericImage::to_file ).static method( "to_file" )
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
			using pos_t = uint32_t;
			using values_type = boost::shared_array<rgb3>;
			using iterator = rgb3 *;
			using const_iterator = rgb3 const *;
			using value_type = rgb3;
			using reference = rgb3 &;
			using const_reference = rgb3 const &;
		private:
			pos_t m_width;
			pos_t m_height;
			size_t m_size;
			size_t m_id;
			values_type m_image_data;
		public:
			GenericImage( pos_t const width, pos_t const height );
			static void to_file( std::string const& image_filename, GenericImage<rgb3> const& image_input );
			void to_file( std::string const & image_filename ) const;
			static GenericImage<rgb3> from_file( boost::string_ref image_filename );
			pos_t width( ) const;
			pos_t height( ) const;
			size_t size( ) const;
			size_t id( ) const;
			const_reference operator( )( pos_t const y, pos_t const x ) const;
			reference operator( )( pos_t const y, pos_t const x );
			const_reference operator[]( size_t const pos ) const;
			reference operator[]( size_t const pos );
			iterator begin( );
			const_iterator begin( ) const;
			iterator end( );
			const_iterator end( ) const;
#ifdef DAWFILTER_USEPYTHON
			static void register_python( std::string const& nameoftype );
#endif
		};
		GenericImage<rgb3> from_file( boost::string_ref image_filename );
	}	// namespace imaging
}	// namespace daw
