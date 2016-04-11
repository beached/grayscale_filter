//dawgsfilter.cpp
#include "filterdawgs.h"
#include "genericimage.h"
#include "genericrgb.h"

#include <boost/scoped_array.hpp>
#include <boost/unordered/unordered_map.hpp>
#include <iterator>
#include <map>
#include <omp.h>
#include <stdexcept>
#include <utility>
#include <vector>

namespace daw { namespace imaging {
	GenericImage<uchar3> FilterDAWGS::filter( const GenericImage<uchar3> &image_input ) {
		//no parallel to valuepos
		typedef boost::unordered_map<int, int> valuepos_t;
		valuepos_t valuepos;

		//no parallel to valuepos
		for( size_t n=0; n<image_input.size( ); ++n ) {
			valuepos[FilterDAWGS::too_gs( image_input[n] )] = 0;
		}

		if( valuepos.size( ) > 256 ) {
			const float inc = (float)valuepos.size( )/256.0f;
			{
				const size_t keys_size = valuepos.size( );
				boost::scoped_array<unsigned int> keys( new unsigned int[keys_size] );
				{
					size_t n=0;
					for( valuepos_t::iterator it=valuepos.begin( ); it != valuepos.end( ); ++it ) {
						keys[n++] = it->first;
					}
				}

				std::sort( keys.get( ), keys.get( ) + keys_size );

				#pragma omp parallel for
				for( int n=0; n<(int)keys_size; ++n ) {
					const int curval = (int)((float)n/inc);
					const int curkey = keys[n];
					#ifdef _DEBUG
					if( curval > 255 ) {
						const std::string msg = "Position in grayscale is too large and cannot fit into 8 bits";
						throw std::runtime_error( msg );
					}
					#endif				
					valuepos[curkey] = curval;
				}
			}

			GenericImage<uchar3> image_output( image_input.width( ), image_input.height( ) );
						
			#pragma omp parallel for
			for( int n=0; n<(int)image_input.size( ); ++n ) {
				const unsigned char curval = (unsigned char)valuepos[FilterDAWGS::too_gs( image_input[n] )];
				image_output[n] = curval;
			}
			return image_output;
		} else { // Already a grayscale image or has enough room for all possible values and no compression needed
			std::cerr << "Already a grayscale image or has enough room for all possible values and no compression needed:" << valuepos.size( ) << std::endl;
			GenericImage<uchar3> image_output( image_input.width( ), image_input.height( ) );

			#pragma omp parallel for
			for( int n=0; n<(int)image_input.size( ); ++n ) {
				const unsigned char curval = (unsigned char)image_input[n].too_float_gs( );
				image_output[n] = curval;
			}
			return image_output;
		}
	}
}}
