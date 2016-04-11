//dawgsfilter.h
#ifndef DAWGSFILTER_H
#define DAWGSFILTER_H

#include "genericimage.h"
#include "genericrgb.h"

#ifdef DAWFILTER_USEPYTHON
#include <boost/python.hpp>
#endif
#include <string>

namespace daw { namespace imaging {
	class FilterDAWGS {
	public:
		static GenericImage<uchar3> filter( const GenericImage<uchar3> &input_image );
	
		static const std::string description( ) {
			return "Convert an RGB image to an optimized grayscale image";
		}

		inline static int too_gs( const uchar3 &pixel ) {
			return 19595*pixel.red + 38469*pixel.green + 7471*pixel.blue;	// 0.299r + 0.587g + 0.114b
		}
		
#ifdef DAWFILTER_USEPYTHON
		static void register_python( const std::string nameoftype = "filter_dawgs" ) {
			boost::python::def( nameoftype.c_str( ), &FilterDAWGS::filter );
		}
#endif
	};


}}

#endif	//DAWGSFILTER_H
