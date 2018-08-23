/* === S Y N F I G ========================================================= */
/*!	\file perlin.cpp
**	\brief Implementation of the "Perlin Noise" layer
**
**	$Id$
**
**	\legal
**	Copyright (c) 2018 Sylvain Leroux
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

#include "perlin.h"

#include <synfig/string.h>
#include <synfig/time.h>
#include <synfig/context.h>
#include <synfig/paramdesc.h>
#include <synfig/renddesc.h>
#include <synfig/surface.h>
#include <synfig/value.h>
#include <synfig/valuenode.h>
#include <time.h>

#endif

/* === M A C R O S ========================================================= */

using namespace synfig;
using namespace std;
using namespace etl;

/* === G L O B A L S ======================================================= */

SYNFIG_LAYER_INIT(PerlinNoise);
SYNFIG_LAYER_SET_NAME(PerlinNoise,"perlin_noise");
SYNFIG_LAYER_SET_LOCAL_NAME(PerlinNoise,N_("Perlin Noise"));
SYNFIG_LAYER_SET_CATEGORY(PerlinNoise,N_("Geometry"));
SYNFIG_LAYER_SET_VERSION(PerlinNoise,"0.0");
SYNFIG_LAYER_SET_CVS_ID(PerlinNoise,"$Id$");

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

PerlinNoise::PerlinNoise():
	Layer_Composite(1.0,Color::BLEND_STRAIGHT),
	param_displacement(ValueBase(Vector(0.25,0.25))),
	param_size(ValueBase(Vector(1,1))),
	param_random(ValueBase(int(time(NULL)))),
	param_smooth(ValueBase(int(RandomNoise::SMOOTH_COSINE))),
	param_detail(ValueBase(int(4))),
	param_speed(ValueBase(Real(0))),
	param_turbulent(bool(false))
{
	SET_INTERPOLATION_DEFAULTS();
	SET_STATIC_DEFAULTS();
}

#if 0
inline Point
PerlinNoise::point_func(const Point &point)const
{
	Vector displacement=param_displacement.get(Vector());
	Vector size=param_size.get(Vector());
	RandomNoise random;
	random.set_seed(param_random.get(int()));
	int smooth_=param_smooth.get(int());
	int detail=param_detail.get(int());
	Real speed=param_speed.get(Real());
	bool turbulent=param_turbulent.get(bool());

	float x(point[0]/size[0]*(1<<detail));
	float y(point[1]/size[1]*(1<<detail));

	int i;
	Time time;
	time=speed*curr_time;
	int temp_smooth(smooth_);
	int smooth((!speed && temp_smooth == (int)(RandomNoise::SMOOTH_SPLINE)) ? (int)(RandomNoise::SMOOTH_FAST_SPLINE) : temp_smooth);

	Vector vect(0,0);
	for(i=0;i<detail;i++)
	{
		vect[0]=random(RandomNoise::SmoothType(smooth),0+(detail-i)*5,x,y,time)+vect[0]*0.5;
		vect[1]=random(RandomNoise::SmoothType(smooth),1+(detail-i)*5,x,y,time)+vect[1]*0.5;

		if(vect[0]<-1)vect[0]=-1;if(vect[0]>1)vect[0]=1;
		if(vect[1]<-1)vect[1]=-1;if(vect[1]>1)vect[1]=1;

		if(turbulent)
		{
			vect[0]=abs(vect[0]);
			vect[1]=abs(vect[1]);
		}

		x/=2.0f;
		y/=2.0f;
	}

	if(!turbulent)
	{
		vect[0]=vect[0]/2.0f+0.5f;
		vect[1]=vect[1]/2.0f+0.5f;
	}
	vect[0]=(vect[0]-0.5f)*displacement[0];
	vect[1]=(vect[1]-0.5f)*displacement[1];

	return point+vect;
}

inline CairoColor
PerlinNoise::cairocolor_func(const Point &point, float /*supersample*/,Context context)const
{
	CairoColor ret(0,0,0,0);
	ret=context.get_cairocolor(point_func(point));
	return ret;
}


inline float
PerlinNoise::calc_supersample(const synfig::Point &/*x*/, float /*pw*/,float /*ph*/)const
{
	return 0.0f;
}

void
PerlinNoise::set_time(synfig::IndependentContext context, synfig::Time t)const
{
	curr_time=t;
	context.set_time(t);
}

void
PerlinNoise::set_time(synfig::IndependentContext context, synfig::Time t, const synfig::Point &point)const
{
	curr_time=t;
	context.set_time(t,point);
}
#endif

/*
 * Based on https://thebookofshaders.com/13/
 */
Real hash(const Vector& p)
{
  double result = fract(1e4 * sin(17.0 * p[0] + p[1] * 0.1) * (0.1 + abs(sin(p[1] * 13.0 + p[0]))));
  //cerr << "hash: " << result << endl;

  return result;
}

/*
 * Based on https://www.shadertoy.com/view/4dS3Wd
 */
Real noise(const Point& x) {
  Point i = floor(x);
  Point f = fract(x);

	// Four corners in 2D of a tile
	Real a = ::hash(i);
  Real b = ::hash(i + Point(1.0, 0.0));
  Real c = ::hash(i + Point(0.0, 1.0));
  Real d = ::hash(i + Point(1.0, 1.0));

  // Simple 2D lerp using smoothstep envelope between the values.
	Real result = mix(
          mix(a, b, smoothstep(0.0, 1.0, f[0])),
				  mix(c, d, smoothstep(0.0, 1.0, f[0])),
				  smoothstep(0.0, 1.0, f[1])
        );

  //cerr << "noise: " << result << endl;

  return result;
}


inline Color
PerlinNoise::color_func(const Point &point, float /*supersample*/,Context context)const
{
	Color ret = Color::white()*noise(point);
  ret.set_alpha(1.0);
//	ret=context.get_color(point_func(point));
	return ret;
}

synfig::Layer::Handle
PerlinNoise::hit_check(synfig::Context context, const synfig::Point &point)const
{
	if(get_blend_method()==Color::BLEND_STRAIGHT && get_amount()>=0.5)
		return const_cast<PerlinNoise*>(this);
	if(get_amount()==0.0)
		return context.hit_check(point);
	if(color_func(point,0,context).get_a()>0.5)
		return const_cast<PerlinNoise*>(this);
	return synfig::Layer::Handle();
}

bool
PerlinNoise::set_param(const String & param, const ValueBase &value)
{
	IMPORT_VALUE(param_displacement);
	IMPORT_VALUE(param_size);
	IMPORT_VALUE(param_random);
	IMPORT_VALUE(param_detail);
	IMPORT_VALUE(param_smooth);
	IMPORT_VALUE(param_speed);
	IMPORT_VALUE(param_turbulent);
	if(param=="seed")
		return set_param("random", value);
	return Layer_Composite::set_param(param,value);
}

ValueBase
PerlinNoise::get_param(const String & param)const
{
	EXPORT_VALUE(param_displacement);
	EXPORT_VALUE(param_size);
	EXPORT_VALUE(param_random);
	EXPORT_VALUE(param_detail);
	EXPORT_VALUE(param_smooth);
	EXPORT_VALUE(param_speed);
	EXPORT_VALUE(param_turbulent);

	if(param=="seed")
		return get_param("random");

	EXPORT_NAME();
	EXPORT_VERSION();

	return Layer_Composite::get_param(param);
}

Layer::Vocab
PerlinNoise::get_param_vocab()const
{
	Layer::Vocab ret(Layer_Composite::get_param_vocab());

	ret.push_back(ParamDesc("displacement")
		.set_local_name(_("Displacement"))
		.set_description(_("How big the distortion displaces the context"))
	);

	ret.push_back(ParamDesc("size")
		.set_local_name(_("Size"))
		.set_description(_("The distance between distortions"))
	);
	ret.push_back(ParamDesc("seed")
		.set_local_name(_("RandomNoise Seed"))
		.set_description(_("Change to modify the random seed of the noise"))
	);
	ret.push_back(ParamDesc("smooth")
		.set_local_name(_("Interpolation"))
		.set_description(_("What type of interpolation to use"))
		.set_hint("enum")
		.add_enum_value(RandomNoise::SMOOTH_DEFAULT,	"nearest",	_("Nearest Neighbor"))
		.add_enum_value(RandomNoise::SMOOTH_LINEAR,	"linear",	_("Linear"))
		.add_enum_value(RandomNoise::SMOOTH_COSINE,	"cosine",	_("Cosine"))
		.add_enum_value(RandomNoise::SMOOTH_SPLINE,	"spline",	_("Spline"))
		.add_enum_value(RandomNoise::SMOOTH_CUBIC,	"cubic",	_("Cubic"))
	);
	ret.push_back(ParamDesc("detail")
		.set_local_name(_("Detail"))
		.set_description(_("Increase to obtain fine details of the noise"))
	);
	ret.push_back(ParamDesc("speed")
		.set_local_name(_("Animation Speed"))
		.set_description(_("In cycles per second"))
	);
	ret.push_back(ParamDesc("turbulent")
		.set_local_name(_("Turbulent"))
		.set_description(_("When checked produces turbulent noise"))
	);

	return ret;
}


bool
PerlinNoise::accelerated_render(Context context,Surface *surface,int quality, const RendDesc &renddesc, ProgressCallback *cb)const
{
	SuperCallback supercb(cb,0,9500,10000);

	if(!context.accelerated_render(surface,quality,renddesc,&supercb))
		return false;

	Surface::alpha_pen pen(surface->begin());

	pen.set_alpha(get_amount());
	pen.set_blend_method(get_blend_method());

  int x,y;
	const Real pw(renddesc.get_pw()),ph(renddesc.get_ph());
	Point pos;
	Point tl(renddesc.get_tl());
	const int w(surface->get_w());
	const int h(surface->get_h());

	for(y=0,pos[1]=tl[1];y<h;y++,pen.inc_y(),pen.dec_x(x),pos[1]+=ph)
		for(x=0,pos[0]=tl[0];x<w;x++,pen.inc_x(),pos[0]+=pw)
			pen.put_value(color_func(pos,0.0,context));

	// Mark our progress as finished
	if(cb && !cb->amount_complete(10000,10000))
		return false;

	return true;
}

Color
PerlinNoise::get_color(Context context, const Point &point)const
{
	const Color color = color_func(point,0,context);

	if(get_amount()==1.0 && get_blend_method()==Color::BLEND_STRAIGHT)
		return color;
	else
		return Color::blend(color,context.get_color(point),get_amount(),get_blend_method());
}


/*
bool
PerlinNoise::accelerated_render(Context context,Surface *surface,int quality, const RendDesc &renddesc, ProgressCallback *cb)const
{
	RENDER_TRANSFORMED_IF_NEED(__FILE__, __LINE__)

	SuperCallback supercb(cb,0,9500,10000);

	if(get_amount()==1.0 && get_blend_method()==Color::BLEND_STRAIGHT)
	{
		surface->set_wh(renddesc.get_w(),renddesc.get_h());
	}
	else
	{
		if(!context.accelerated_render(surface,quality,renddesc,&supercb))
			return false;
		if(get_amount()==0)
			return true;
	}


	int x,y;

	Surface::pen pen(surface->begin());
	const Real pw(renddesc.get_pw()),ph(renddesc.get_ph());
	Point pos;
	Point tl(renddesc.get_tl());
	const int w(surface->get_w());
	const int h(surface->get_h());

	if(get_amount()==1.0 && get_blend_method()==Color::BLEND_STRAIGHT)
	{
		for(y=0,pos[1]=tl[1];y<h;y++,pen.inc_y(),pen.dec_x(x),pos[1]+=ph)
			for(x=0,pos[0]=tl[0];x<w;x++,pen.inc_x(),pos[0]+=pw)
				pen.put_value(color_func(pos,calc_supersample(pos,pw,ph),context));
	}
	else
	{
		for(y=0,pos[1]=tl[1];y<h;y++,pen.inc_y(),pen.dec_x(x),pos[1]+=ph)
			for(x=0,pos[0]=tl[0];x<w;x++,pen.inc_x(),pos[0]+=pw)
				pen.put_value(Color::blend(color_func(pos,calc_supersample(pos,pw,ph),context),pen.get_value(),get_amount(),get_blend_method()));
	}

	// Mark our progress as finished
	if(cb && !cb->amount_complete(10000,10000))
		return false;

	return true;
}
*/
