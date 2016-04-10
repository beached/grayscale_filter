cmake_minimum_required( VERSION 2.8.4 )

project( lib_grayscale_filter )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR} )

find_package( Boost 1.59.0 COMPONENTS python REQUIRED )
find_package( FreeImage REQUIRED )

IF( ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC )
	set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601 )
	set( Boost_Libraries )
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL Clang )
	find_package( Threads REQUIRED )
	set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -g -O3 -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion )
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL GNU )
	find_package( Threads REQUIRED )
	set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++14 -g -O3 -Wall -Wno-deprecated-declarations )
ENDIF( )

enable_testing( )

set( SOURCE_FILES
	filterdawgs.cpp
	filterdawgs.h
	filterdawgscolourize.cpp
	filterdawgscolourize.h
	filterrotate.cpp
	filterrotate.h
	fimage.h
	genericimage.h
	genericrgb.h
	helpers.h
	nullptr.h
	random.cpp
	random.h
 )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
include_directories( SYSTEM ${FreeImage_INCLUDE_DIRS} )

add_library( lib_grayscale_filter STATIC ${SOURCE_FILES} )
target_link_libraries( lib_grayscale_filter ${Boost_Libraries} )

