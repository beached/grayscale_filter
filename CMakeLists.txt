cmake_minimum_required(VERSION 2.8.4)
project(grayscale_filter)


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
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -g -O3 -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion -Wno-global-constructors")
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
	find_package(Threads REQUIRED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -O3 -Wall -Wno-deprecated-declarations")
ENDIF( )

enable_testing( )

add_subdirectory( ./lib_grayscale_filter )

include_directories( ./lib_grayscale_filter )
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )
include_directories( SYSTEM ${FREEIMAGE_INCLUDE_DIRS} )
link_directories( ${FREEIMAGE_LIBRARIES} )

add_executable( example_lib_grayscale_filter example_lib_grayscale_filter.cpp )
target_link_libraries( example_lib_grayscale_filter lib_grayscale_filter ${CMAKE_DL_LIBS} ${Boost_Libs} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS} )
