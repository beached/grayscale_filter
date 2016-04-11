#ifndef HAS_DAWFILTERROTATE_HEADER
#define HAS_DAWFILTERROTATE_HEADER

#include "genericimage.h"
#include "genericrgb.h"

#ifdef DAWFILTER_USEPYTHON
#include <boost/python.hpp>
#endif
#include <string>

namespace daw { namespace imaging {
	class FilterRotate {	
	public:
		static GenericImage<uchar3> filter( const GenericImage<uchar3> &image_input, const unsigned int angle );

#ifdef DAWFILTER_USEPYTHON
		static void register_python( const std::string nameoftype = "filter_rotate" ) {
			boost::python::def( nameoftype.c_str( ), &FilterRotate::filter );
		}
#endif
	};
}}

#endif	//HAS_DAWFILTERROTATE_HEADER
