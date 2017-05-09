/* === S Y N F I G ========================================================= */
/*!	\file synfig/rendering/opengl/task/tasksurfaceresamplesw.cpp
**	\brief TaskSurfaceResampleSW
**
**	$Id$
**
**	\legal
**	......... ... 2015 Ivan Mahonin
**
**	This package is free software; you can redistribute it and/or
**	modify it under the terms of the GNU General Public License as
**	published by the Free Software Foundation; either version 2 of
**	the License, or (at your option) any later version.
**
**	This package is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
**	General Public License for more details.
**	\endlegal
*/
/* ========================================================================= */

/* === H E A D E R S ======================================================= */

#ifdef USING_PCH
#	include "pch.h"
#else
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#endif

#include <synfig/general.h>
#include <synfig/localization.h>
#include <synfig/debug/debugsurface.h>

#include "tasksurfaceresamplesw.h"

#include "../surfacesw.h"
#include "../function/packedsurface.h"

#endif

using namespace synfig;
using namespace rendering;

/* === M A C R O S ========================================================= */

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

namespace {
	class Helper
	{
	public:
		template<Color reader(const void*,int,int)>
		class Generic {
		public:
			typedef etl::sampler<ColorAccumulator, float, ColorAccumulator, reader> Sampler;
			typedef Color FilterFunc(const void*, int, int, const Vector&);
			typedef void GammaFunc(Color&, float);

			struct Args {
				const void *surface;
				const int w;
				const int h;
				const RectInt &bounds;
				float gamma_adjust;
				Vector pos, pos_dx, pos_dy;
				Vector aa0, aa0_dx, aa0_dy;
				Vector aa1, aa1_dx, aa1_dy;
				Args(const void *surface, int w, int h, const RectInt &bounds):
					surface(surface), w(w), h(h), bounds(bounds), gamma_adjust() { }
			};

			static inline Color nearest(const void *surface, int w, int h, const Vector &pos)
			{
				return reader(surface,
					std::max(std::min((int)floor(pos[0]), w-1), 0),
					std::max(std::min((int)floor(pos[1]), h-1), 0) );
			}

			static inline Color linear(const void *surface, int w, int h, const Vector &pos)
				{ return ColorPrep::uncook_static( Sampler::linear_sample(surface, w, h, pos[0] - 0.5, pos[1] - 0.5) ); }

			static inline Color cosine(const void *surface, int w, int h, const Vector &pos)
				{ return ColorPrep::uncook_static( Sampler::cosine_sample(surface, w, h, pos[0] - 0.5, pos[1] - 0.5) ); }

			static inline Color cubic(const void *surface, int w, int h, const Vector &pos)
				{ return ColorPrep::uncook_static( Sampler::cubic_sample(surface, w, h, pos[0] - 0.5, pos[1] - 0.5) ); }

			static inline void gamma(Color &color, float gamma_adjust)
			{
				if (gamma_adjust != 1.f)
				{
					color.set_r( powf((float)color.get_r(), gamma_adjust) );
					color.set_g( powf((float)color.get_g(), gamma_adjust) );
					color.set_b( powf((float)color.get_b(), gamma_adjust) );
				}
			}

			static inline void nogamma(Color&, float) { }

			template<typename pen, GammaFunc gamma_func, FilterFunc filter_func>
			static inline void fill(pen &p, Args &a)
			{
				int idx = a.bounds.maxx - a.bounds.minx;
				int idy = a.bounds.maxy - a.bounds.miny;
				for(int y = idy; y; --y)
				{
					for(int x = idx; x; --x)
					{
						Color c = filter_func(a.surface, a.w, a.h, a.pos);
						gamma_func(c, a.gamma_adjust);
						p.put_value(c);
						a.pos += a.pos_dx;
						p.inc_x();
					}
					p.dec_x(idx);
					p.inc_y();
					a.pos += a.pos_dy;
				}
			}

			template<typename pen, GammaFunc gamma_func, FilterFunc filter_func>
			static inline void fill_aa(pen &p, Args &a)
			{
				int idx = a.bounds.maxx - a.bounds.minx;
				int idy = a.bounds.maxy - a.bounds.miny;
				for(int y = idy; y; --y)
				{
					for(int x = idx; x; --x)
					{
						if ( a.aa0[0] > 0.0 && a.aa0[1] > 0.0
						  && a.aa1[0] > 0.0 && a.aa1[1] > 0.0 )
						{
							Color c = filter_func(a.surface, a.w, a.h, a.pos);
							c.set_a( c.get_a()
								   * std::min(a.aa0[0], 1.0)
								   * std::min(a.aa0[1], 1.0)
								   * std::min(a.aa1[0], 1.0)
								   * std::min(a.aa1[1], 1.0) );
							gamma_func(c, a.gamma_adjust);
							p.put_value(c);
						}

						a.pos += a.pos_dx;
						a.aa0 += a.aa0_dx;
						a.aa1 += a.aa1_dx;
						p.inc_x();
					}
					p.dec_x(idx);
					p.inc_y();
					a.pos += a.pos_dy;
					a.aa0 += a.aa0_dy;
					a.aa1 += a.aa1_dy;
				}
			}

			template<typename pen, GammaFunc gamma_func, FilterFunc filter_func>
			static inline void fill(
				bool antialiasing,
				pen &p, Args &a )
			{
				if (antialiasing)
					fill_aa<pen, gamma_func, filter_func>(p, a);
				else
					fill<pen, gamma_func, filter_func>(p, a);
			}

			template<typename pen, GammaFunc gamma_func>
			static inline void fill(
				Color::Interpolation interpolation,
				bool antialiasing,
				pen &p, Args &a )
			{
				switch(interpolation)
				{
				case Color::INTERPOLATION_LINEAR:
					fill<pen, gamma_func, Generic::linear>(antialiasing, p, a); break;
				case Color::INTERPOLATION_COSINE:
					fill<pen, gamma_func, Generic::cosine>(antialiasing, p, a); break;
				case Color::INTERPOLATION_CUBIC:
					fill<pen, gamma_func, Generic::cubic>(antialiasing, p, a); break;
				default:
					fill<pen, gamma_func, Generic::nearest>(antialiasing, p, a); break;
				}
			}

			template<typename pen>
			static inline void fill(
				Color::value_type gamma_adjust,
				Color::Interpolation interpolation,
				bool antialiasing,
				pen &p, Args &a )
			{
				bool no_transform =
					 approximate_equal(fabs(a.pos_dx[0]), 0.0)
				  && approximate_equal(fabs(a.pos_dx[1]), 1.0)
				  && approximate_equal(fabs(a.pos_dy[0]), Real(a.bounds.minx - a.bounds.maxx))
				  && approximate_equal(fabs(a.pos_dy[1]), 1.0)
				  && approximate_equal(a.pos[0] - 0.5, round(a.pos[0] - 0.5))
				  && approximate_equal(a.pos[1] - 0.5, round(a.pos[1] - 0.5));

				bool no_gamma =
					approximate_equal(gamma_adjust, 1.f);

				if (no_transform)
				{
					interpolation = Color::INTERPOLATION_NEAREST;
					antialiasing = false;
				}

				if (no_gamma)
				{
					fill<pen, Generic::nogamma>(interpolation, antialiasing, p, a);
				}
				else
				{
					a.gamma_adjust = gamma_adjust;
					fill<pen, Generic::gamma>(interpolation, antialiasing, p, a);
				}
			}

			static void resample(
				synfig::Surface &dest,
				const RectInt &dest_bounds,
				const void *src,
				int src_w,
				int src_h,
				const RectInt &src_bounds,
				const Matrix &transformation,
				ColorReal gamma,
				Color::Interpolation interpolation,
				bool antialiasing,
				bool blend,
				ColorReal blend_amount,
				Color::BlendMethod blend_method )
			{
				// bounds

				Vector corners[] = {
					transformation.get_transformed(Vector( Real(src_bounds.minx), Real(src_bounds.miny) )),
					transformation.get_transformed(Vector( Real(src_bounds.maxx), Real(src_bounds.miny) )),
					transformation.get_transformed(Vector( Real(src_bounds.minx), Real(src_bounds.maxy) )),
					transformation.get_transformed(Vector( Real(src_bounds.maxx), Real(src_bounds.maxy) )) };

				Rect boundsf(   corners[0] );
				boundsf.expand( corners[1] );
				boundsf.expand( corners[2] );
				boundsf.expand( corners[3] );

				RectInt bounds( (int)approximate_floor(boundsf.minx) - 1,
								(int)approximate_floor(boundsf.miny) - 1,
								(int)approximate_ceil (boundsf.maxx) + 1,
								(int)approximate_ceil (boundsf.maxy) + 1 );

				etl::set_intersect(bounds, bounds, dest_bounds);
				etl::set_intersect(bounds, bounds, RectInt(0, 0, dest.get_w(), dest.get_h()));

				// texture matrices

				if (bounds.valid())
				{
					Matrix back_transformation = transformation;
					back_transformation.invert();

					Args args(src, src_w, src_h, bounds);

					Vector start((Real)bounds.minx + 0.5, (Real)bounds.miny + 0.5);
					Vector dx(1.0, 0.0);
					Vector dy((Real)(bounds.minx - bounds.maxx), 1.0);

					args.pos    = back_transformation.get_transformed( start );
					args.pos_dx = back_transformation.get_transformed( dx, false );
					args.pos_dy = back_transformation.get_transformed( dy, false );

					bool aa = antialiasing;

					if (aa)
					{
						Vector sub_corners[] = {
							back_transformation.get_transformed(Vector( Real(bounds.minx), Real(bounds.miny) )),
							back_transformation.get_transformed(Vector( Real(bounds.maxx), Real(bounds.miny) )),
							back_transformation.get_transformed(Vector( Real(bounds.minx), Real(bounds.maxy) )),
							back_transformation.get_transformed(Vector( Real(bounds.maxx), Real(bounds.maxy) )) };

						Rect sub_boundsf(   sub_corners[0] );
						sub_boundsf.expand( sub_corners[1] );
						sub_boundsf.expand( sub_corners[2] );
						sub_boundsf.expand( sub_corners[3] );

						RectInt sub_bounds(
							(int)approximate_floor(sub_boundsf.minx) - 1,
							(int)approximate_floor(sub_boundsf.miny) - 1,
							(int)approximate_ceil (sub_boundsf.maxx) + 1,
							(int)approximate_ceil (sub_boundsf.maxy) + 1 );

						if (src_bounds.contains(sub_bounds))
							aa = false;
					}

					if (aa)
					{
						Real sx = (corners[1] - corners[0]).mag()/Real(src_bounds.maxx - src_bounds.minx);
						Real sy = (corners[2] - corners[0]).mag()/Real(src_bounds.maxy - src_bounds.miny);

						Matrix aa0_matrix = back_transformation
										  * Matrix().set_scale(sx, sy)
										  * Matrix().set_translate(0.5, 0.5);
						Matrix aa1_matrix = back_transformation
										  * Matrix().set_translate(
												-Real(src_bounds.maxx - src_bounds.minx),
												-Real(src_bounds.maxy - src_bounds.miny) )
										  * Matrix().set_scale(-sx, -sy)
										  * Matrix().set_translate(0.5, 0.5);

						args.aa0    = aa0_matrix.get_transformed( start );
						args.aa0_dx = aa0_matrix.get_transformed( dx, false );
						args.aa0_dy = aa0_matrix.get_transformed( dy, false );

						args.aa1    = aa1_matrix.get_transformed( start );
						args.aa1_dx = aa1_matrix.get_transformed( dx, false );
						args.aa1_dy = aa1_matrix.get_transformed( dy, false );
					}

					if (blend)
					{
						synfig::Surface::alpha_pen p(dest.get_pen(bounds.minx, bounds.miny));
						p.set_blend_method(blend_method);
						p.set_alpha(blend_amount);
						fill(gamma, interpolation, aa, p, args);
					}
					else
					{
						synfig::Surface::pen p(dest.get_pen(bounds.minx, bounds.miny));
						fill(gamma, interpolation, aa, p, args);
					}
				}
			}
		};
	};
}

void
TaskSurfaceResampleSW::resample(
	synfig::Surface &dest,
	const RectInt &dest_bounds,
	const synfig::Surface &src,
	const RectInt &src_bounds,
	const Matrix &transformation,
	ColorReal gamma,
	Color::Interpolation interpolation,
	bool antialiasing,
	bool blend,
	ColorReal blend_amount,
	Color::BlendMethod blend_method )
{
	Helper::Generic<synfig::Surface::reader_cook>::resample(
		dest,
		dest_bounds,
		&src,
		src.get_w(),
		src.get_h(),
		src_bounds,
		transformation,
		gamma,
		interpolation,
		antialiasing,
		blend,
		blend_amount,
		blend_method );
}

void
TaskSurfaceResampleSW::resample(
	synfig::Surface &dest,
	const RectInt &dest_bounds,
	const software::PackedSurface &src,
	const RectInt &src_bounds,
	const Matrix &transformation,
	ColorReal gamma,
	Color::Interpolation interpolation,
	bool antialiasing,
	bool blend,
	ColorReal blend_amount,
	Color::BlendMethod blend_method )
{
	software::PackedSurface::Reader src_reader(src);
	Helper::Generic<software::PackedSurface::Reader::reader_cook>::resample(
		dest,
		dest_bounds,
		&src_reader,
		src.get_width(),
		src.get_height(),
		src_bounds,
		transformation,
		gamma,
		interpolation,
		antialiasing,
		blend,
		blend_amount,
		blend_method );
}

bool
TaskSurfaceResampleSW::run(RunParams & /* params */) const
{
	synfig::Surface &target =
		SurfaceSW::Handle::cast_dynamic(target_surface)->get_surface();

	if (valid_target() && sub_task()->valid_target())
	{
		// transformation matrix

		Matrix src_pixels_to_units;
		src_pixels_to_units.m00 = sub_task()->get_units_per_pixel()[0];
		src_pixels_to_units.m11 = sub_task()->get_units_per_pixel()[1];
		src_pixels_to_units.m20 = -sub_task()->get_target_offset()[0]*src_pixels_to_units.m00 + sub_task()->get_source_rect_lt()[0];
		src_pixels_to_units.m21 = -sub_task()->get_target_offset()[1]*src_pixels_to_units.m11 + sub_task()->get_source_rect_lt()[1];

		Matrix dest_units_to_pixels;
		dest_units_to_pixels.m00 = get_pixels_per_unit()[0];
		dest_units_to_pixels.m11 = get_pixels_per_unit()[1];
		dest_units_to_pixels.m20 = -get_source_rect_lt()[0]*dest_units_to_pixels.m00 + get_target_offset()[0];
		dest_units_to_pixels.m21 = -get_source_rect_lt()[1]*dest_units_to_pixels.m11 + get_target_offset()[1];

		Matrix matrix = src_pixels_to_units * transformation * dest_units_to_pixels;

		// resample
		if (SurfaceSW::Handle a_sw = SurfaceSW::Handle::cast_dynamic(sub_task()->target_surface))
		{
			resample(
				target,
				get_target_rect(),
				a_sw->get_surface(),
				sub_task()->get_target_rect(),
				matrix,
				gamma,
				interpolation,
				antialiasing,
				blend,
				amount,
				blend_method );
		}
		else
		if (SurfaceSWPacked::Handle a_swpacked = SurfaceSWPacked::Handle::cast_dynamic(sub_task()->target_surface))
		{
			resample(
				target,
				get_target_rect(),
				a_swpacked->get_surface(),
				sub_task()->get_target_rect(),
				matrix,
				gamma,
				interpolation,
				antialiasing,
				blend,
				amount,
				blend_method );
		}

		//debug::DebugSurface::save_to_file(a, "TaskSurfaceResampleSW__run__a");
		//debug::DebugSurface::save_to_file(target, "TaskSurfaceResampleSW__run__target");
	}

	return true;
}

/* === E N T R Y P O I N T ================================================= */
