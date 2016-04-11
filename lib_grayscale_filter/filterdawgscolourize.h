//dawgscolourizefilter.h
#ifndef DAWGSCOLOURIZEFILTER_H
#define DAWGSCOLOURIZEFILTER_H

#include "genericimage.h"
#include "genericrgb.h"

#ifdef DAWFILTER_USEPYTHON
#include <boost/python.hpp>
#endif
#include <map>
#include <string>


namespace daw { namespace imaging {
	class FilterDAWGSColourize {
	public:
		static GenericImage<uchar3> filter( const GenericImage<uchar3> &input_image, const GenericImage<uchar3> &input_gsimage, const unsigned int repaint_formula = 0 );

		template<class T>
		static std::map<T, int> get_repaint_formulas( ) {
			std::map<T, int> ret;
			int count = 0;
			ret["Ratio"] = count++;
			ret["YUV"] = count++;
			ret["Multiply 1"] = count++;
			ret["Addition"] = count++;
			ret["Multiply 2"] = count++;
			ret["HSL"] = count++;

			return ret;
		}

#ifdef DAWFILTER_USEPYTHON
		static void register_python( ) {
			boost::python::def( "filter_dawgscolourize", &FilterDAWGSColourize::filter );
		}
#endif
	};
}}

#endif	//DAWGSCOLOURIZEFILTER_H