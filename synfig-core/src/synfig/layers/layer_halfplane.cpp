/* === S Y N F I G ========================================================= */
/*!	\file layer_solidcolor.cpp
**	\brief Implementation of the "Solid Color" layer
**
**	$Id$
**
**	\legal
**	Copyright (c) 2002-2005 Robert B. Quattlebaum Jr., Adrian Bentley
**	Copyright (c) 2007, 2008 Chris Moore
**	Copyright (c) 2011-2013 Carlos López
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

#include "layer_halfplane.h"
#include <synfig/range.h>
#include <synfig/string.h>
#include <synfig/time.h>
#include <synfig/context.h>
#include <synfig/paramdesc.h>
#include <synfig/renddesc.h>
#include <synfig/surface.h>
#include <synfig/value.h>
#include <synfig/valuenode.h>

#endif

/* === U S I N G =========================================================== */

using namespace etl;
using namespace std;
using namespace synfig;

/* === G L O B A L S ======================================================= */

SYNFIG_LAYER_INIT(Layer_HalfPlane);
SYNFIG_LAYER_SET_NAME(Layer_HalfPlane,"HalfPlane"); // todo: use solid_color
SYNFIG_LAYER_SET_LOCAL_NAME(Layer_HalfPlane,N_("Half Plane"));
SYNFIG_LAYER_SET_CATEGORY(Layer_HalfPlane,N_("Geometry"));
SYNFIG_LAYER_SET_VERSION(Layer_HalfPlane,"0.1");
SYNFIG_LAYER_SET_CVS_ID(Layer_HalfPlane,"$Id$");

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

/* === E N T R Y P O I N T ================================================= */

Layer_HalfPlane::Layer_HalfPlane():
	Layer_Composite(1.0,Color::BLEND_COMPOSITE),
	param_color(ValueBase(Color::black())),
  param_origin(ValueBase(Point(0,0))),
  param_direction(ValueBase(Vector(1,1)))
{
	SET_INTERPOLATION_DEFAULTS();
	SET_STATIC_DEFAULTS();
}

bool
Layer_HalfPlane::set_param(const String & param, const ValueBase &value)
{
  IMPORT_VALUE(param_origin);
  IMPORT_VALUE(param_direction);
  IMPORT_VALUE(param_color);

	return Layer_Composite::set_param(param,value);
}

ValueBase
Layer_HalfPlane::get_param(const String &param)const
{
  EXPORT_VALUE(param_origin);
  EXPORT_VALUE(param_direction);
	EXPORT_VALUE(param_color);

	EXPORT_NAME();
	EXPORT_VERSION();

	return Layer_Composite::get_param(param);
}

Layer::Vocab
Layer_HalfPlane::get_param_vocab()const
{
	Layer::Vocab ret(Layer_Composite::get_param_vocab());

	ret.push_back(ParamDesc("color")
		.set_local_name(_("Color"))
		.set_description(_("Fill color of the layer"))
	);

	ret.push_back(ParamDesc("origin")
		.set_local_name(_("Origin"))
		.set_description(_("A point at the border of the half plane"))
	);

	ret.push_back(ParamDesc("direction")
		.set_local_name(_("Direction"))
		.set_description(_("Direction vector"))
	);

	return ret;
}

Range
Layer_HalfPlane::get_border(float line) const
{    
	Point origin(param_origin.get(Point()));
	Vector direction(param_direction.get(Vector()));
  
  float inv_slope = direction.inv_slope();
  if (isinf(inv_slope))
  {
    if ( ((inv_slope > 0) && (line >= origin[1]))
         ||((inv_slope < 0) && (line <= origin[1])) )
    {
      return Range(-INFINITY, +INFINITY);
    }
    else
    {
      return Range();
    }
  }
    
  float a = -INFINITY;
  float b = +INFINITY;
  float &limit  = (direction[1] > 0) ? b : a;
  limit = origin[0]+(line-origin[1])*inv_slope;
  
  return Range(a,b);
}

bool
Layer_HalfPlane::is_inside(const synfig::Point &point) const
{    
  return get_border(point[1]).contains(point[0]);
}

synfig::Layer::Handle
Layer_HalfPlane::hit_check(synfig::Context context, const synfig::Point &point)const
{
  if (is_inside(point))
  {
  	Color color=param_color.get(Color());
  	if(get_blend_method()==Color::BLEND_STRAIGHT && get_amount()>=0.5)
  		return const_cast<Layer_HalfPlane*>(this);
  	else
  	if(get_blend_method()==Color::BLEND_COMPOSITE && get_amount()*color.get_a()>=0.5)
  		return const_cast<Layer_HalfPlane*>(this);
  }
	Layer::Handle layer(context.hit_check(point));

	return layer?layer:const_cast<Layer_HalfPlane*>(this);
}

Color
Layer_HalfPlane::get_color(Context context, const Point &pos)const
{
  if (!is_inside(pos))
    return context.get_color(pos);

	Color color=param_color.get(Color());
	if(get_amount()==1.0 && get_blend_method()==Color::BLEND_STRAIGHT)
		return color;
	else
		return Color::blend(color,context.get_color(pos),get_amount(),get_blend_method());
}

bool
Layer_HalfPlane::accelerated_render(Context context,Surface *surface,int quality, const RendDesc &renddesc, ProgressCallback *cb)const
{
	// Point origin(param_origin.get(Point()));
	Color color=param_color.get(Color());
	const Point	tl(renddesc.get_tl());
	const Point br(renddesc.get_br());

  const Range h_range(tl[0], br[0]);
  const Range v_range(tl[1], br[1]);
	const int	w(renddesc.get_w());
	const int	h(renddesc.get_h());
  
  
	SuperCallback supercb(cb,0,9500,10000);
	if(!context.accelerated_render(surface,quality,renddesc,&supercb))
    return false;

  for(int i = 0; i < h; ++i)
  {
    float line = v_range.unmap(i, 0, h);
    Range visible = h_range.clamp(get_border(line));
    //fprintf(stderr,"h_range: %f %f visible: %f %f\n", h_range.from(), h_range.to(), visible.from(), visible.to());

    if (visible)
    {
      Range line = h_range.map(visible, 0, w);
      //fprintf(stderr,"line: %f %f\n", line.from(), line.to());
      Surface::pen pen = surface->get_pen(line.from(),i);
      pen.set_value(color);
			Surface::alpha_pen apen(pen,get_amount(),get_blend_method());

      apen.put_hline(line.width()); 
    }
  }

	return true;
}

//////

bool
Layer_HalfPlane::accelerated_cairorender(Context context, cairo_t *cr, int quality, const RendDesc &renddesc, ProgressCallback *cb)const
{
	Color color=param_color.get(Color());
	float r(color.get_r()),
	      g(color.get_g()),
		  b(color.get_b()),
		  a(color.get_a());
	
	if((get_amount()==1.f && get_blend_method()==Color::BLEND_STRAIGHT)
	   ||
	   (get_amount()==1.f && color.get_a()==1.f && get_blend_method()==Color::BLEND_COMPOSITE)
	   )
	{
		// Mark our progress as starting
		if(cb && !cb->amount_complete(0,1000))
			return false;
		cairo_save(cr);
		cairo_set_source_rgba(cr, r, g, b, a);
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_paint(cr);
		cairo_restore(cr);
		// Mark our progress as finished
		if(cb && !cb->amount_complete(1000,1000))
			return false;
		return true;
	}
	
	SuperCallback supercb(cb,0,9500,10000);
	
	if(!context.accelerated_cairorender(cr,quality,renddesc,&supercb))
		return false;
	
	cairo_save(cr);
	cairo_reset_clip(cr);
	cairo_set_source_rgba(cr, r, g, b, a);
	cairo_paint_with_alpha_operator(cr, get_amount(), get_blend_method());
	cairo_restore(cr);

	// Mark our progress as finished
	if(cb && !cb->amount_complete(10000,10000))
		return false;
	
	return true;
}

//////
