#ifndef HAS_DAWNULLPTR_HEADER
#define HAS_DAWNULLPTR_HEADER

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


#endif	//HAS_DAWNULLPTR_HEADER
