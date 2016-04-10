// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
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

#include <boost/math/special_functions/round.hpp>
#include <cassert>
#include <cmath>
#include <limits>
#include <map>
#include <omp.h>

namespace daw { namespace imaging {
	
	namespace {
		float colour_calc( float c, float t1, float t2 ) {
			if( c < 0.0f ) {
				c += 1.0f;
			} else if( c > 1.0f ) {
				c -= 1.0f;
			}
			if( 6.0 * c < 1.0f ) {
				return t1 + (t2-t1) * 6.0f * c;
			} else if( 2.0f * c < 1.0f ) {
				return t2;
			} else if( 3.0f * c < 2.0f ) {
				return t1 + (t2 - t1) * (2.0f/3.0f - c) * 6.0f;
			}
			return t1;		
		}

		inline const GenericRGB<int> repaint_methods( const uchar3 orig, const unsigned char grayscale, const int repaint_formula ) {
			switch( repaint_formula ) {
			case 0: {	// Ratio
				// Luma = Rx + Gy +Bz
				// We want Luma -> Luma2 
				// (Rx + Gy + Bz)/Luma = 1
				// Luma2*(Rx + Gy + Bz)/Luma = Luma2
				GenericRGB<float> forig( (float)orig.red, (float)orig.green, (float)orig.blue );

				const float curL = forig.too_float_gs( );

				if( abs( curL ) < 0.114f ) {
					//prevent div by 0 as 0.114 is the minimum value;
					return GenericRGB<int>( 0, 0, 0 );
				}
				const float rat = (float)grayscale/curL;
				forig.red = boost::math::round( forig.red*rat );
				forig.green = boost::math::round( forig.green*rat );
				forig.blue = boost::math::round( forig.blue*rat );
				return GenericRGB<int>( (int)forig.red, (int)forig.green, (int)forig.blue );
					}
					break;
			case 1: {	// YUV
				// Convert back to YUV with Y being the current grayscale value and then back to RGB
				//const int retval = 19595*pixel.red + 38469*pixel.green + 7471*pixel.blue;
				const float Y = (float)grayscale;
				const float U = orig.colform( -0.147f, -0.289f, 0.436f );
				const float V = orig.colform( 0.615f, -0.515f, -0.1f );

				const int R = (int)(Y + 1.14f*V);
				const int G = (int)(Y - 0.395f*U - 0.581f*V);
				const int B = (int)(Y + 2.032f*U);
				//int B = (int)(Y + 1.5f*U);
				return GenericRGB<int>( R, G, B );
					}
					break;
			case 2: // Mul 1
				return GenericRGB<int>( orig.red*grayscale, orig.green*grayscale, orig.blue*grayscale );
				break;
			case 3: // Add
				return GenericRGB<int>( orig.red+grayscale, orig.green+grayscale, orig.blue+grayscale );
				break;
			case 4: {	// Mul 2, Mul with individual scaling based on max( R, G, B )
				const int maxval = orig.max( );
				if( maxval > 0 ) {
					const float luma = (float)grayscale;
					const float red = ((float)orig.red*luma)/maxval;
					const float green = ((float)orig.green*luma)/maxval;
					const float blue = ((float)orig.blue*luma)/maxval;
					return GenericRGB<int>( (int)red, (int)green, (int)blue );
				} else {
					return GenericRGB<int>( 0, 0, 0 );
				}
					}
					break;
			case 5: {	// HSL
				float luma = (float)grayscale/255.0f;
				float hue = 0.0f;
				float saturation = 0.0f;
				const unsigned char orig_max = orig.max( );
				const unsigned char orig_min = orig.min( );

				if( 0 == grayscale ) {
					return GenericRGB<int>( 0, 0, 0 );
				} else if( orig_max == orig_min ) {
					return GenericRGB<int>( (int)grayscale );
				} else {
					{
						const float orig_maxf = (float)orig_max/255.0f;
						const float orig_minf = (float)orig_min/255.0f;
						const float orig_range = orig_maxf - orig_minf;
						saturation = orig_range;
						float L = (orig_maxf + orig_minf)/2.0f;
						GenericRGB<float> rgb( (float)orig.red/255.0f, (float)orig.green/255.0f, (float)orig.blue/255.0f );
						if( orig_range != 0 ) {
							if( L < 0.5f ) {
								saturation = (orig_range/(orig_maxf + orig_minf));
							} else {
								saturation = (orig_range/(2.0f-orig_maxf - orig_minf));
							}

							if( orig_max == orig.red ) {
								hue = (rgb.green - rgb.blue)/orig_range;
							} else if( orig_max == orig.green ) {							
								hue = 2.0f + (rgb.blue-rgb.red)/orig_range;
							} else if( orig_max == orig.blue ) {
								hue = 4.0f + (rgb.red - rgb.green)/orig_range;
							}
						}
					}				
					GenericRGB<float> rgb( 0.0f );
					float t1, t2;
					float th = hue/6.0f;
					if( luma < 0.5f ) {
						t2 = luma * (1.0f + saturation);
					} else {
						t2 = (luma + saturation) - (luma * saturation);
					}
					t1 = 2.0f*luma - t2;

					rgb.red = th + (1.0f/3.0f);
					rgb.green = th;
					rgb.blue = th - (1.0f/3.0f);

					rgb.red = colour_calc( rgb.red, t1, t2 );
					rgb.green = colour_calc( rgb.green, t1, t2 );
					rgb.blue = colour_calc( rgb.blue, t1, t2 );

					rgb.mul( 255.0f );

					return GenericRGB<int>( (int)rgb.red, (int)rgb.green, (int)rgb.blue );
				}
				
			}
			break;
			default:
				const std::string msg = "Invalid repaint formula"; 
				throw std::runtime_error( msg );
				break;
			}
		}
	}

	GenericImage<uchar3> FilterDAWGSColourize::filter( const GenericImage<uchar3> &input_image, const GenericImage<uchar3> &input_gsimage, const unsigned int repaint_formula ) {
		// Valid data checks - Start
		if( input_image.width( ) != input_gsimage.width( ) ) {
			const std::string msg = "FilterDAWGSColourize::runfilter with input_image->width != _input_gsimage->width";
			throw std::runtime_error( msg );
		}
		if( input_image.height( ) != input_gsimage.height( ) ) {
			const std::string msg = "FilterDAWGSColourize::runfilter with input_image->height != _input_gsimage->height";
			throw std::runtime_error( msg );
		}
		// Valid data checks - End

		GenericImage<GenericRGB<int> > tmpimgdata( input_image.width( ), input_image.height( ) );

		#pragma omp parallel for
		for( int n=0; n<(int)input_image.size( ); ++n ) {
			tmpimgdata[n] = repaint_methods( input_image[n], input_gsimage[n].blue, repaint_formula );
		}

		GenericRGB<int> pd_min( std::numeric_limits<unsigned int>::max( ), std::numeric_limits<unsigned int>::max( ), std::numeric_limits<unsigned int>::max( ) );
		GenericRGB<int> pd_max( std::numeric_limits<unsigned int>::min( ), std::numeric_limits<unsigned int>::min( ), std::numeric_limits<unsigned int>::min( ) );

		// Do not parallelize without accounting for shared data
		for( unsigned int n=0; n<tmpimgdata.size( ); ++n ) {
			const GenericRGB<int> curval = tmpimgdata[n];
			GenericRGB<int>::min( curval, pd_min );
			GenericRGB<int>::max( curval, pd_max );
		}

		const float mul_fact = 255.0f/(float)(pd_max.max( ) - pd_min.min( ));		

		GenericImage<uchar3> output_image( input_image.width( ), input_image.height( ) );

		#pragma omp parallel for
		for( int n=0; n<(int)input_image.size( ); ++n ) {
			GenericRGB<int> cur_value;
			cur_value.red = (int)((float)(tmpimgdata[n].red-pd_min.red)*mul_fact);
			cur_value.green = (int)((float)(tmpimgdata[n].green-pd_min.green)*mul_fact);
			cur_value.blue = (int)((float)(tmpimgdata[n].blue-pd_min.blue)*mul_fact);
			cur_value.clampvalue( 0, 255 );
			output_image[n] = uchar3( (unsigned char)cur_value.red, (unsigned char)cur_value.green, (unsigned char)cur_value.blue );
		}
		return output_image;
	}
}}

