	cmake_minimum_required( VERSION 2.8.12 )

project( example_grayscale_filter_prj )

include( ExternalProject )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR} )
find_package( FreeImage REQUIRED )

set( Boost_USE_STATIC_LIBS OFF ) 
set( Boost_USE_MULTITHREADED ON )  
set( Boost_USE_STATIC_RUNTIME OFF ) 

find_package( Boost 1.59.0 REQUIRED COMPONENTS system filesystem regex unit_test_framework program_options iostreams )



if( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	add_compile_options( -D_WIN32_WINNT=0x0601 )
else( )
	find_package( Threads REQUIRED )
	if( THREADS_HAVE_PTHREAD_ARG )
		set( COMPILER_SPECifIC_LIBS, "-lpthread" )
	endif( )

	if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
		add_compile_options(-std=c++14 -stdlib=libc++ -Weverything -Wno-c++98-compat -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion)		
	elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
		add_compile_options( -std=c++14 -Wall -Wno-deprecated-declarations )
	endif( )

endif( )

externalproject_add(
	lib_grayscale_filter_prj
	GIT_REPOSITORY "https://github.com/beached/lib_grayscale_filter.git"
	GIT_TAG "master"
	SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/lib_grayscale_filter"
	INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
	PATCH_COMMAND ""
	TEST_COMMAND ""
)

include_directories( SYSTEM "${CMAKE_BINARY_DIR}/install/include" )
link_directories( "${CMAKE_BINARY_DIR}/install/lib" )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )
include_directories( SYSTEM ${FREEIMAGE_INCLUDE_DIRS} )
link_directories( ${FREEIMAGE_LIBRARIES} )


add_executable( example_lib_grayscale_filter example_lib_grayscale_filter.cpp )
add_dependencies( example_lib_grayscale_filter lib_grayscale_filter_prj )
target_link_libraries( example_lib_grayscale_filter grayscale_filter ${CMAKE_DL_LIBS} ${Boost_LIBRARIES} ${FREEIMAGE_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS} )

