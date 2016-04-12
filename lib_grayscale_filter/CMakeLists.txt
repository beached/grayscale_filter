cmake_minimum_required( VERSION 2.8.4 )

project( lib_grayscale_filter )

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.59.0 REQUIRED COMPONENTS system filesystem regex unit_test_framework program_options iostreams python )
set(Boost_Libs ${BOOST_Libraries} -lboost_system -lboost_regex -lboost_filesystem -lboost_program_options -lboost_iostreams -lboost_python )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR} )
find_package( FreeImage REQUIRED )


IF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601 /MP" )
	set( Boost_Libs )
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
	find_package(Threads REQUIRED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -g -O3 -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion")
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
	find_package(Threads REQUIRED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -O3 -Wall -Wno-deprecated-declarations")
ENDIF( )

if(THREADS_HAVE_PTHREAD_ARG)
	set(COMPILER_SPECIFIC_LIBS, "-lpthread" )
endif( )

enable_testing( )

set( SOURCE_FILES
filterdawgscolourize.cpp
filterdawgscolourize.h
filterdawgs.cpp
filterdawgs.h
filterrotate.cpp
filterrotate.h
fimage.cpp
fimage.h
genericimage.cpp
genericimage.h
genericrgb.h
helpers.h
nullptr.h
random.cpp
random.h
)

include(ExternalProject)
ExternalProject_Add( 
	header_libraries
	GIT_REPOSITORY "https://github.com/beached/header_libraries.git"
	SOURCE_DIR "${CMAKE_BINARY_DIR}/3rdparty/header_libraries"
	GIT_TAG "master"
	PATCH_COMMAND ""
	INSTALL_COMMAND ""
	TEST_COMMAND ""
)

include_directories( SYSTEM "${CMAKE_BINARY_DIR}/3rdparty/header_libraries" )
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
include_directories( SYSTEM ${FREEIMAGE_INCLUDE_DIRS} )
link_directories( ${FREEIMAGE_LIBRARIES} )

add_library( lib_grayscale_filter STATIC ${SOURCE_FILES} )
target_link_libraries( lib_grayscale_filter ${Boost_Libraries} freeimage )
add_dependencies( lib_grayscale_filter header_libraries )


