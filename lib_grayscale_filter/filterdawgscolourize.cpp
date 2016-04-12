// The MIT License (MIT)
//
// Copyright (c) 2016 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "filterdawgscolourize.h"
#include "genericimage.h"
#include "genericrgb.h"
#include "helpers.h"
#include "daw_algorithm.h"

#include <boost/math/special_functions/round.hpp>
#include <cassert>
#include <cmath>
#include <limits>
#include <map>
//#include <omp.h>

namespace daw {
	namespace imaging {
		namespace {
			float colour_calc( float c, float t1, float t2 ) {
				if( c < 0.0f ) {
					c += 1.0f;
				} else if( c > 1.0f ) {
					c -= 1.0f;
				}
				if( 6.0 * c < 1.0f ) {
					return t1 + ( t2 - t1 ) * 6.0f * c;
				} else if( 2.0f * c < 1.0f ) {
					return t2;
				} else if( 3.0f * c < 2.0f ) {
					return t1 + ( t2 - t1 ) * ( 2.0f / 3.0f - c ) * 6.0f;
				}
				return t1;
			}

			GenericRGB<int32_t> repaint_methods( rgb3 const orig, uint8_t const grayscale, FilterDAWGSColourize::repaint_formulas const repaint_formula ) {
				switch( repaint_formula ) {
				case FilterDAWGSColourize::repaint_formulas::Ratio:
				{	// Ratio
					// Luma = Rx + Gy +Bz
					// We want Luma -> Luma2 
					// ( Rx + Gy + Bz )/Luma = 1
					// Luma2*( Rx + Gy + Bz )/Luma = Luma2
					GenericRGB<float> forig( static_cast<float>( orig.red ), static_cast<float>( orig.green ), static_cast<float>( orig.blue ) );

					auto const curL = forig.too_float_gs( );

					if( abs( curL ) < 0.114f ) {
						//prevent div by 0 as 0.114 is the minimum value;
						return GenericRGB<int32_t>( 0, 0, 0 );
					}
					auto const rat = static_cast<float>( grayscale ) / curL;
					forig.red = boost::math::round( forig.red*rat );
					forig.green = boost::math::round( forig.green*rat );
					forig.blue = boost::math::round( forig.blue*rat );
					return GenericRGB<int32_t>( static_cast<int32_t>( forig.red ), static_cast<int32_t>( forig.green ), static_cast<int32_t>( forig.blue ) );
				}
				break;
				case FilterDAWGSColourize::repaint_formulas::YUV:
				{	// YUV
					// Convert back to YUV with Y being the current grayscale value and then back to RGB
					//int32_t const retval = 19595*pixel.red + 38469*pixel.green + 7471*pixel.blue;
					auto const Y = static_cast<float>( grayscale );
					auto const U = orig.colform( -0.147f, -0.289f, 0.436f );
					auto const V = orig.colform( 0.615f, -0.515f, -0.1f );

					auto const R = static_cast<int32_t>( Y + 1.14f*V );
					auto const G = static_cast<int32_t>( Y - 0.395f*U - 0.581f*V );
					auto const B = static_cast<int32_t>( Y + 2.032f*U );
					//int B = (int32_t)( Y + 1.5f*U );
					return GenericRGB<int32_t>( R, G, B );
				}
				break;
				case FilterDAWGSColourize::repaint_formulas::Multiply_1:
					return GenericRGB<int32_t>( orig.red*grayscale, orig.green*grayscale, orig.blue*grayscale );
					break;
				case FilterDAWGSColourize::repaint_formulas::Addition:
					return GenericRGB<int32_t>( orig.red + grayscale, orig.green + grayscale, orig.blue + grayscale );
					break;
				case FilterDAWGSColourize::repaint_formulas::Multiply_2:
				{	// Mul 2, Mul with individual scaling based on max( R, G, B )
					auto const maxval = static_cast<float>(orig.max( ));
					if( maxval > 0 ) {
						auto const luma = static_cast<float>( grayscale );
						auto red = static_cast<int32_t>( ( static_cast<float>( orig.red )*luma ) / maxval );
						auto green = static_cast<int32_t>( ( static_cast<float>( orig.green )*luma ) / maxval );
						auto blue = static_cast<int32_t>( ( static_cast<float>( orig.blue )*luma ) / maxval );
						return GenericRGB<int32_t>( std::move( red ), std::move( green ), std::move( blue ) );
					} else {
						return GenericRGB<int32_t>( 0, 0, 0 );
					}
				}
				break;
				case FilterDAWGSColourize::repaint_formulas::HSL:
				{	// HSL
					auto luma = static_cast<float>( grayscale ) / 255.0f;
					auto hue = 0.0f;
					auto saturation = 0.0f;
					auto const orig_max = orig.max( );
					auto const orig_min = orig.min( );

					if( 0 == grayscale ) {
						return GenericRGB<int32_t>( 0, 0, 0 );
					} else if( orig_max == orig_min ) {
						return GenericRGB<int32_t>( static_cast<int32_t>(grayscale) );
					} else {
						{
							auto const orig_maxf = static_cast<float>( orig_max ) / 255.0f;
							auto const orig_minf = static_cast<float>( orig_min ) / 255.0f;
							auto const orig_range = orig_maxf - orig_minf;
							saturation = orig_range;
							auto L = ( orig_maxf + orig_minf ) / 2.0f;
							GenericRGB<float> rgb( static_cast<float>( orig.red ) / 255.0f, static_cast<float>( orig.green ) / 255.0f, static_cast<float>( orig.blue ) / 255.0f );
							if( orig_range != 0 ) {
								if( L < 0.5f ) {
									saturation = ( orig_range / ( orig_maxf + orig_minf ) );
								} else {
									saturation = ( orig_range / ( 2.0f - orig_maxf - orig_minf ) );
								}

								if( orig_max == orig.red ) {
									hue = ( rgb.green - rgb.blue ) / orig_range;
								} else if( orig_max == orig.green ) {
									hue = 2.0f + ( rgb.blue - rgb.red ) / orig_range;
								} else if( orig_max == orig.blue ) {
									hue = 4.0f + ( rgb.red - rgb.green ) / orig_range;
								}
							}
						}
						GenericRGB<float> rgb( 0.0f );
						float t1, t2;
						float th = hue / 6.0f;
						if( luma < 0.5f ) {
							t2 = luma * ( 1.0f + saturation );
						} else {
							t2 = ( luma + saturation ) - ( luma * saturation );
						}
						t1 = 2.0f*luma - t2;

						rgb.red = th + ( 1.0f / 3.0f );
						rgb.green = th;
						rgb.blue = th - ( 1.0f / 3.0f );

						rgb.red = colour_calc( rgb.red, t1, t2 );
						rgb.green = colour_calc( rgb.green, t1, t2 );
						rgb.blue = colour_calc( rgb.blue, t1, t2 );

						rgb.mul( 255.0f );

						return GenericRGB<int32_t>( static_cast<int32_t>( rgb.red ), static_cast<int32_t>( rgb.green ), static_cast<int32_t>( rgb.blue ) );
					}

				}
				break;
				default:
					throw std::runtime_error( "Invalid repaint formula" );
				}
			}

			template<typename T>
			std::pair<GenericRGB<T>, GenericRGB<T>> minmax_element( GenericImage<GenericRGB<T>> const & img ) {
				std::pair<GenericRGB<T>, GenericRGB<T>> result { 
					{ std::numeric_limits<T>::max( ), std::numeric_limits<T>::max( ), std::numeric_limits<T>::max( ) },
					{ std::numeric_limits<T>::min( ), std::numeric_limits<T>::min( ), std::numeric_limits<T>::min( ) }
				};
				
				// Do not parallelize without accounting for shared data
				for( auto const & rgb : img ) {
					min( rgb, result.first );
					max( rgb, result.second );
				}
				return result;
			}

		}	// namespace anonymous

		GenericImage<rgb3> FilterDAWGSColourize::filter( GenericImage<rgb3> const & input_image, GenericImage<rgb3> const & input_gsimage, FilterDAWGSColourize::repaint_formulas repaint_formula ) {
			// Valid data checks - Start
			if( input_image.width( ) != input_gsimage.width( ) ) {
				auto const msg = "FilterDAWGSColourize::runfilter with input_image->width != _input_gsimage->width";
				throw std::runtime_error( msg );
			}
			if( input_image.height( ) != input_gsimage.height( ) ) {
				auto const msg = "FilterDAWGSColourize::runfilter with input_image->height != _input_gsimage->height";
				throw std::runtime_error( msg );
			}
			// Valid data checks - End

			GenericImage<GenericRGB<int32_t>> tmpimgdata( input_image.width( ), input_image.height( ) );

//#pragma omp parallel for
			daw::algorithm::transform_many( input_image.begin( ), input_image.end( ), input_gsimage.begin( ), tmpimgdata.begin( ), [repaint_formula]( auto const & v1, auto const & v2 ) {
				return repaint_methods( v1, v2.blue, repaint_formula );
			} );

			GenericRGB<int32_t> pd_min;
			GenericRGB<int32_t> pd_max;
			std::tie( pd_min, pd_max ) = minmax_element( tmpimgdata );

			auto const mul_fact = 255.0f / static_cast<float>( pd_max.max( ) - pd_min.min( ) );

			GenericImage<rgb3> output_image( input_image.width( ), input_image.height( ) );
//#pragma omp parallel for
			std::transform( tmpimgdata.begin( ), tmpimgdata.end( ), output_image.begin( ), [&]( auto const & rgb ) {
				GenericRGB<uint32_t> cur_value;
				
				cur_value.red = static_cast<int32_t>(static_cast<float>(rgb.red - pd_min.red)*mul_fact);
				cur_value.green = static_cast<int32_t>(static_cast<float>(rgb.green - pd_min.green)*mul_fact);
				cur_value.blue = static_cast<int32_t>(static_cast<float>(rgb.blue - pd_min.blue)*mul_fact);
				cur_value.clampvalue( 0, 255 );
				return rgb3( static_cast<uint8_t>(cur_value.red), static_cast<uint8_t>(cur_value.green), static_cast<uint8_t>(cur_value.blue) );
			} );

			return output_image;
		}

		std::unordered_map<std::string, FilterDAWGSColourize::repaint_formulas> FilterDAWGSColourize::get_repaint_formulas( ) {
			static std::unordered_map<std::string, repaint_formulas> ret = { { "Ratio", repaint_formulas::Ratio }, { "YUV", repaint_formulas::YUV }, { "Multiply 1", repaint_formulas::Multiply_1 }, { "Addition", repaint_formulas::Addition }, { "Multiply 2", repaint_formulas::Multiply_2 }, { "HSL", repaint_formulas::HSL } };
			return ret;
		}

#ifdef DAWFILTER_USEPYTHON
		void FilterDAWGSColourize::register_python( ) {
			boost::python::def( "filter_dawgscolourize", &FilterDAWGSColourize::filter );
		}
#endif
	}	// namespace imaging
}	// namespace daw
