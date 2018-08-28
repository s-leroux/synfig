/* === S Y N F I G ========================================================= */
/*!	\file fractal.cpp
**	\brief Implementation of the "Fractal Noise" layer
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

#include "fractal.h"

#include <synfig/string.h>
#include <synfig/time.h>
#include <synfig/context.h>
#include <synfig/paramdesc.h>
#include <synfig/renddesc.h>
#include <synfig/surface.h>
#include <synfig/value.h>
#include <synfig/valuenode.h>
#include <synfig/math.h>
#include <time.h>
#include <random>
#include <memory>

#endif

/* === M A C R O S ========================================================= */

using namespace synfig;
using namespace std;
using namespace etl;

/* === G L O B A L S ======================================================= */

SYNFIG_LAYER_INIT(FractalNoise);
SYNFIG_LAYER_SET_NAME(FractalNoise,"fractal_noise");
SYNFIG_LAYER_SET_LOCAL_NAME(FractalNoise,N_("Fractal Noise"));
SYNFIG_LAYER_SET_CATEGORY(FractalNoise,N_("Geometry"));
SYNFIG_LAYER_SET_VERSION(FractalNoise,"0.0");
SYNFIG_LAYER_SET_CVS_ID(FractalNoise,"$Id$");

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

FractalNoise::FractalNoise():
	Layer_Composite(1.0,Color::BLEND_STRAIGHT),

	param_interpolation(ValueBase(int(INTERPOLATION_LINEAR))),
	param_iterations(ValueBase(int(2))),
	param_rotation(ValueBase(Angle::deg(15))),
	param_gain(ValueBase(Real(0.5))),
	param_lacunarity(ValueBase(Real(2.0))),
	param_shape(ValueBase(int(SHAPE_LINEAR))),
	param_time(ValueBase(Real(0))),
	param_size(ValueBase(int(10))),
  param_scale(ValueBase(Real(5.0))),
	param_seed(ValueBase(int(time(NULL))))
{
	SET_INTERPOLATION_DEFAULTS();
	SET_STATIC_DEFAULTS();
}

template<Real (*SHAPE)(const Real&)>
struct FractalGrid
{
  std::mt19937 _prng;
  Real _scale;
  const int  _width;
  const int  _size3D;
  const int  _size2D;
  Real   *_v; // 3D map in the [0; 1] range
  Real   *_t; // 2D map in the [0; 1] range

  FractalGrid(int seed, Real scale, int width) :
    _prng(seed),
    _scale(scale),
    _width(width),
    _size3D(width*width*width),
    _size2D(width*width),
    _v(new Real[_size3D+_size2D]),
    _t(_v+_size3D)
  {
    Real  RANDMAX(_prng.max());

  	for(int i=0;i<_width*_width;++i)
    {
      _t[i] = Real(_prng())/RANDMAX;
      for(int j = 0; j < _width; ++j)
      {
        _v[j*_size2D+i] = Real(_prng())/(RANDMAX/2.0)-1.0;
      }
    }
  }

  ~FractalGrid() { delete[] _v; }

  inline Real noise(Real xf, Real yf, Real zf) const {
    xf /= _scale;
    yf /= _scale;

    Real xfloor = floor(xf);
    Real yfloor = floor(yf);
    Real xfract = xf - xfloor;
    Real yfract = yf - yfloor;

    int x0 = int(xfloor)%_width;
    int y0 = int(yfloor)%_width;
    if (x0<0) x0 = _width+x0;
    if (y0<0) y0 = _width+y0;

    int x1 = (x0+1)%_width;
    int y1 = (y0+1)%_width;

    int a = x0+y0*_width;
    int b = x1+y0*_width;
    int c = x0+y1*_width;
    int d = x1+y1*_width;

    // Adjust zf according to _t
    Real deltaz0 = _t[a] + xfract*(_t[b]-_t[a]);
    Real deltaz1 = _t[c] + xfract*(_t[d]-_t[c]);

    zf += deltaz0+yfract*(deltaz1-deltaz0);

    Real zfloor = floor(zf);
    Real zfract = zf - zfloor;

    int z0 = int(zfloor)%_width;
    if (z0<0) z0 = _width+z0;
    int z1 = (z0+1)%_width;

    int a_z0 = a+z0*_size2D;
    int b_z0 = b+z0*_size2D;
    int c_z0 = c+z0*_size2D;
    int d_z0 = d+z0*_size2D;

    int a_z1 = a+z1*_size2D;
    int b_z1 = b+z1*_size2D;
    int c_z1 = c+z1*_size2D;
    int d_z1 = d+z1*_size2D;

    // shape the {x,y,f}fract parts
    xfract = SHAPE(xfract);
    yfract = SHAPE(yfract);
    zfract = SHAPE(zfract);

    Real v_y0_z0 = _v[a_z0] + xfract*(_v[b_z0]-_v[a_z0]);
    Real v_y0_z1 = _v[a_z1] + xfract*(_v[b_z1]-_v[a_z1]);
    Real v_y0 = v_y0_z0 + zfract*(v_y0_z1-v_y0_z0);

    Real v_y1_z0 = _v[c_z0] + xfract*(_v[d_z0]-_v[c_z0]);
    Real v_y1_z1 = _v[c_z1] + xfract*(_v[d_z1]-_v[c_z1]);
    Real v_y1 = v_y1_z0 + zfract*(v_y1_z1-v_y1_z0);

    Real v = v_y0 + yfract*(v_y1-v_y0);

    // std::cout << v << '\n';
    return v;
  }

};

class NoiseGenerator
{
public:
  virtual Real noise(Real x, Real y, Real z) const = 0;
};

template<Real (*SHAPE)(const Real&)>
struct NoiseGeneratorAdaptor : public NoiseGenerator {
  FractalGrid<SHAPE> _grid;

  NoiseGeneratorAdaptor(int seed, Real scale, int width)
    : _grid(seed, scale, width) {}

  virtual Real noise(Real x, Real y, Real z) const
  {
    return _grid.noise(x,y,z);
  }
};

class ColorFunc
{
public:
  virtual Color get(int iterations, Real gain, Real lacunarity, const Angle& angle, const Point& point, Real time, Context /*context*/)const = 0;

  static std::unique_ptr<ColorFunc> make(int interpolation, int shape, int seed, Real scale, int width);
};

template<Real (*SHAPER)(const Real&) = ShapingFunction<Real>::linear>
struct ColorFuncAdaptor : public ColorFunc {
  unique_ptr<NoiseGenerator>  _generator;

  ColorFuncAdaptor(unique_ptr<NoiseGenerator>&& generator)
    : _generator(std::move(generator)) {}

  virtual Color get(int iterations, Real gain, Real lacunarity, const Angle& angle, const Point& point, Real time, Context /*context*/)const
  {
    Vector p(point[0],point[1]);
    Real v = 0.0;
    Real a = 0.5;
    Vector shift(100.0, 100.0);

    Real m = 0.0;

    for (int i = 0; i < iterations; ++i) {
      v += a*SHAPER(_generator->noise(p[0], p[1], time));
      m += a;

      a = a*gain;
      p = p.rotate(angle)*lacunarity + shift;
      time = time *lacunarity+100.0;
    }

    Color ret = Color::white()*(v/m);
    ret.set_alpha(1.0);
  //	ret=context.get_color(point_func(point));
    return ret;
  }
};

std::unique_ptr<ColorFunc> ColorFunc::make(int interpolation, int shape, int seed, Real scale, int width)
{
  NoiseGenerator *ng;

  switch (interpolation) {
    case FractalNoise::INTERPOLATION_STEP:
      ng = new NoiseGeneratorAdaptor<InterpolationFunction<Real>::step>(seed, scale, width);
      break;
    case FractalNoise::INTERPOLATION_SMOOTHSTEP:
      ng = new NoiseGeneratorAdaptor<InterpolationFunction<Real>::smoothstep>(seed, scale, width);
      break;
    case FractalNoise::INTERPOLATION_ATAN:
      ng = new NoiseGeneratorAdaptor<InterpolationFunction<Real>::atan>(seed, scale, width);
      break;
    case FractalNoise::INTERPOLATION_LINEAR:
    default:
      ng = new NoiseGeneratorAdaptor<InterpolationFunction<Real>::linear>(seed, scale, width);
  }

  ColorFunc *cf;
  switch (shape) {
    case FractalNoise::SHAPE_ABS:
      cf = new ColorFuncAdaptor<ShapingFunction<Real>::abs>(unique_ptr<NoiseGenerator>(ng));
      break;
    case FractalNoise::SHAPE_RIDGE:
      cf = new ColorFuncAdaptor<ShapingFunction<Real>::ridge>(unique_ptr<NoiseGenerator>(ng));
      break;
    case FractalNoise::SHAPE_PULSE:
      cf = new ColorFuncAdaptor<ShapingFunction<Real>::pulse>(unique_ptr<NoiseGenerator>(ng));
      break;
    default:
    case FractalNoise::SHAPE_LINEAR:
      cf = new ColorFuncAdaptor<ShapingFunction<Real>::linear>(unique_ptr<NoiseGenerator>(ng));
      break;
  }

  return std::unique_ptr<ColorFunc>(cf);
}


synfig::Layer::Handle
FractalNoise::hit_check(synfig::Context context, const synfig::Point &point)const
{
	if(get_blend_method()==Color::BLEND_STRAIGHT && get_amount()>=0.5)
		return const_cast<FractalNoise*>(this);
	if(get_amount()==0.0)
		return context.hit_check(point);

 	int interpolation=param_interpolation.get(int());

 	Real time=param_time.get(Real());
 	int size=param_size.get(int());
 	Real scale=param_scale.get(Real());
 	int seed=param_seed.get(int());

 	int iterations=param_iterations.get(int());
 	int shape=param_shape.get(int());
 	Angle angle=param_rotation.get(Angle());
 	Real gain=param_gain.get(Real());
 	Real lacunarity=param_lacunarity.get(Real());

  auto color = ColorFunc::make(interpolation, shape, seed, scale, size);
	if(color->get(iterations, gain, lacunarity, angle, point, time, context).get_a()>0.5)
		return const_cast<FractalNoise*>(this);
	return synfig::Layer::Handle();
}

bool
FractalNoise::set_param(const String & param, const ValueBase &value)
{
	IMPORT_VALUE(param_interpolation);
	IMPORT_VALUE(param_iterations);
	IMPORT_VALUE(param_rotation);
	IMPORT_VALUE(param_gain);
	IMPORT_VALUE(param_lacunarity);
	IMPORT_VALUE(param_shape);
	IMPORT_VALUE(param_time);

	IMPORT_VALUE(param_size);
	IMPORT_VALUE(param_scale);
	IMPORT_VALUE(param_seed);
	if(param=="seed")
		return set_param("random", value);
	return Layer_Composite::set_param(param,value);
}

ValueBase
FractalNoise::get_param(const String & param)const
{
	EXPORT_VALUE(param_interpolation);
	EXPORT_VALUE(param_iterations);
	EXPORT_VALUE(param_rotation);
	EXPORT_VALUE(param_gain);
	EXPORT_VALUE(param_lacunarity);
	EXPORT_VALUE(param_shape);
	EXPORT_VALUE(param_time);

	EXPORT_VALUE(param_size);
	EXPORT_VALUE(param_scale);
	EXPORT_VALUE(param_seed);

	if(param=="seed")
		return get_param("random");

	EXPORT_NAME();
	EXPORT_VERSION();

	return Layer_Composite::get_param(param);
}

Layer::Vocab
FractalNoise::get_param_vocab()const
{
	Layer::Vocab ret(Layer_Composite::get_param_vocab());

	ret.push_back(ParamDesc("interpolation")
		.set_local_name(_("Interpolation"))
		.set_description(_("What type of interpolation to use"))
		.set_hint("enum")
		.add_enum_value(INTERPOLATION_LINEAR,	"linear",	_("Linear"))
		.add_enum_value(INTERPOLATION_STEP,	"step",	_("Step"))
		.add_enum_value(INTERPOLATION_SMOOTHSTEP,	"smoothstep",	_("Smooth Step"))
		.add_enum_value(INTERPOLATION_ATAN,	"atan",	_("Arctangent"))
	);

	ret.push_back(ParamDesc("iterations")
		.set_local_name(_("Iterations"))
		.set_description(_("Number of iterations (octave) applied"))
	);

	ret.push_back(ParamDesc("rotation")
		.set_local_name(_("Rotation"))
		.set_description(_("Rotation applied at each iteration"))
	);

	ret.push_back(ParamDesc("gain")
		.set_local_name(_("Gain"))
		.set_description(_("Gain for each octave (usually < 1.0)"))
	);

	ret.push_back(ParamDesc("lacunarity")
		.set_local_name(_("Lacunarity"))
		.set_description(_("Frequence ratio between two consecutive octaves (usually 2.0)"))
	);

	ret.push_back(ParamDesc("shape")
		.set_local_name(_("Shape"))
		.set_description(_("Noise shaping function"))
		.set_hint("enum")
		.add_enum_value(SHAPE_LINEAR,	"linear",	_("Linear"))
		.add_enum_value(SHAPE_ABS,	"abs",	_("Abs"))
		.add_enum_value(SHAPE_RIDGE,	"ridge",	_("Ridge"))
		.add_enum_value(SHAPE_PULSE,	"pulse",	_("Pulse"))
	);

	ret.push_back(ParamDesc("time")
		.set_local_name(_("Time (z-axis position)"))
		.set_description(_("In arbitrary units"))
	);

	ret.push_back(ParamDesc("size")
		.set_local_name(_("Size"))
		.set_description(_("Grid size (O^3 memory size: use a small value!)"))
	);

	ret.push_back(ParamDesc("scale")
		.set_local_name(_("Scale"))
		.set_description(_("The grid scale"))
	);

	ret.push_back(ParamDesc("seed")
		.set_local_name(_("Random Noise Seed"))
		.set_description(_("Change to modify the random seed of the noise"))
	);

	return ret;
}

bool
FractalNoise::accelerated_render(Context context,Surface *surface,int quality, const RendDesc &renddesc, ProgressCallback *cb)const
{
	RENDER_TRANSFORMED_IF_NEED(__FILE__, __LINE__)

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

 	int interpolation=param_interpolation.get(int());

 	Real time=param_time.get(Real());
 	int size=param_size.get(int());
 	Real scale=param_scale.get(Real());

 	int seed=param_seed.get(int());

 	int iterations=param_iterations.get(int());
 	int shape=param_shape.get(int());

 	Angle angle=param_rotation.get(Angle());
 	Real gain=param_gain.get(Real());
 	Real lacunarity=param_lacunarity.get(Real());

  auto color = ColorFunc::make(interpolation, shape, seed, scale, size);

	for(y=0,pos[1]=tl[1];y<h;y++,pen.inc_y(),pen.dec_x(x),pos[1]+=ph)
		for(x=0,pos[0]=tl[0];x<w;x++,pen.inc_x(),pos[0]+=pw)
			pen.put_value(color->get(iterations, gain, lacunarity, angle, pos, time, context));

	// Mark our progress as finished
	if(cb && !cb->amount_complete(10000,10000))
		return false;

	return true;
}

Color
FractalNoise::get_color(Context context, const Point &point)const
{
 	int interpolation=param_interpolation.get(int());

 	Real time=param_time.get(Real());
 	int size=param_size.get(int());
 	Real scale=param_scale.get(Real());
 	int seed=param_seed.get(int());

 	int iterations=param_iterations.get(int());
 	int shape=param_shape.get(int());

 	Angle angle=param_rotation.get(Angle());
 	Real gain=param_gain.get(Real());
 	Real lacunarity=param_lacunarity.get(Real());

  auto color = ColorFunc::make(interpolation, shape, seed, scale, size)->get(iterations, gain, lacunarity, angle, point, time, context);

	if(get_amount()==1.0 && get_blend_method()==Color::BLEND_STRAIGHT)
		return color;
	else
		return Color::blend(color,context.get_color(point),get_amount(),get_blend_method());
}


/*
bool
FractalNoise::accelerated_render(Context context,Surface *surface,int quality, const RendDesc &renddesc, ProgressCallback *cb)const
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
