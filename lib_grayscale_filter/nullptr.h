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

#include <iostream>
#include <stdexcept>
#include <string>

#ifdef _MSC_VER
#if _MSC_VER >= 1600
#define DONTUSEDAWNULLPTR
#endif
#endif

#ifndef DONTUSEDAWNULLPTR
namespace std {
	class nullptr_t {
	private:
		void* p;
		void operator&( ) {}
	public:
		nullptr_t( ): p( 0 ) { }
		template<class T> operator T*( ) const {
			return 0;
		}

		template<class T, class U> operator T U::*( ) const {
			return 0;
		}

		operator bool( ) const {
			return false;
		}		
	};

	template<class T> bool operator==(T* lhs, const nullptr_t rhs) {
		rhs;
		return lhs == 0;
	}

	template<class T> bool operator==(const nullptr_t lhs, T* rhs) {
		lhs;
		return rhs == 0;
	}

	template<class T, class U> bool operator==(T U::* lhs, const nullptr_t rhs) {
		rhs;
		return lhs == 0;
	}

	template<class T, class U> bool operator==(const nullptr_t lhs, T U::* rhs) {
		lhs;
		return rhs == 0;
	}

	template<class T> bool operator!=(T* lhs, const nullptr_t rhs) {
		rhs;
		return lhs != 0;
	}

	template<class T> bool operator!=(const nullptr_t lhs, T* rhs) {
		lhs;
		return rhs != 0;
	}

	template<class T, class U> bool operator!=(T U::* lhs, const nullptr_t rhs) {
		rhs;
		return lhs != 0;
	}

	template<class T, class U> bool operator!=(const nullptr_t lhs, T U::* rhs) {
		lhs;
		return rhs != 0;
	}
}

const std::nullptr_t nullptr = std::nullptr_t( );

#endif	//DONTUSEDAWNULLPTR

template<class T>
inline void nullcheck( const T* ptr, const std::string &msg ) {
	if( nullptr == ptr ) {
		std::cerr << msg << std::endl;
		throw std::runtime_error( msg );
	}
}

template<class T>
inline void nullcheck( T* ptr, const std::string &msg ) {
	if( nullptr == ptr ) {
		std::cerr << msg << std::endl;
		throw std::runtime_error( msg );
	}
}

