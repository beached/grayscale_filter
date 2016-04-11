cmake_minimum_required(VERSION 2.8.4)
project(grayscale_filter)


set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.59.0 REQUIRED COMPONENTS python)
set(Boost_Libs ${BOOST_Libraries} -lboost_python )

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

enable_testing( )

add_subdirectory( ./lib_grayscale_filter )

include_directories( ./lib_grayscale_filter )
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )

add_executable(test_gs_filter test_gs_filter.cpp )
target_link_libraries(test_gs_filter ${CMAKE_DL_LIBS} ${Boost_Libs} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS})