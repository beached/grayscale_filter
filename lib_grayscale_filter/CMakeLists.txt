cmake_minimum_required( VERSION 2.8.12 )

project( lib_grayscale_filter )

include( ExternalProject )

set( Boost_USE_STATIC_LIBS OFF ) 
set( Boost_USE_MULTITHREADED ON )  
set( Boost_USE_STATIC_RUNTIME OFF ) 
find_package( Boost 1.59.0 REQUIRED COMPONENTS system filesystem regex unit_test_framework program_options iostreams python )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR} )
find_package( FreeImage REQUIRED )

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	add_compile_options( -D_WIN32_WINNT=0x0601 )
else( )
	find_package( Threads REQUIRED )
	if( THREADS_HAVE_PTHREAD_ARG )
		set( COMPILER_SPECifIC_LIBS, "-lpthread" )
	endif( )

	if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
		add_compile_options( -std=c++14 -stdlib=libc++ -Weverything -Wno-c++98-compat -Wfatal-
	elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
		add_compile_options( -std=c++14 -Wall -Wno-deprecated-declarations )
	endif( )
endif( )

enable_testing( )


externalproject_add( 
	header_libraries_prj
	GIT_REPOSITORY "https://github.com/beached/header_libraries.git"
	GIT_TAG "master"
	SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/header_libraries"
	INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
	PATCH_COMMAND ""
	TEST_COMMAND ""
)

include_directories( SYSTEM "${CMAKE_BINARY_DIR}/install/include" )
link_directories( "${CMAKE_BINARY_DIR}/install/lib" )
include_directories( "./include" )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
include_directories( SYSTEM ${FREEIMAGE_INCLUDE_DIRS} )
link_directories( ${FREEIMAGE_LIBRARIES} )

set( HEADER_FOLDER "include" )

SET( HEADER_FILES
	${HEADER_FOLDER}/filterdawgscolourize.h
	${HEADER_FOLDER}/filterdawgs.h
	${HEADER_FOLDER}/filterrotate.h
	${HEADER_FOLDER}/fimage.h
	${HEADER_FOLDER}/genericimage.h
	${HEADER_FOLDER}/genericrgb.h
	${HEADER_FOLDER}/helpers.h
	${HEADER_FOLDER}/nullptr.h
	${HEADER_FOLDER}/random.h
)

set( SOURCE_FILES
	filterdawgscolourize.cpp
	filterdawgs.cpp
	filterrotate.cpp
	fimage.cpp
	genericimage.cpp
	random.cpp
)

add_library( lib_grayscale_filter STATIC ${HEADER_FILES} ${SOURCE_FILES} )
add_dependencies( lib_grayscale_filter header_libraries_prj )
target_link_libraries( lib_grayscale_filter ${Boost_LIBRARIES} freeimage )

install( TARGETS nodepp DESTINATION lib )
install( DIRECTORY ${HEADER_FOLDER}/ DESTINATION include/daw/lib_grayscale_filter )

