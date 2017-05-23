/* === S Y N F I G ========================================================= */
/*!	\file timeloop.cpp
**	\brief Implementation of the "Time Loop" layer
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
**
** === N O T E S ===========================================================
**
** ========================================================================= */

/* === H E A D E R S ======================================================= */

#ifdef USING_PCH
#	include "pch.h"
#else
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "timewarp.h"
#include <synfig/valuenode.h>
#include <synfig/valuenodes/valuenode_const.h>
#include <synfig/valuenodes/valuenode_subtract.h>
#include <synfig/time.h>
#include <synfig/context.h>
#include <synfig/paramdesc.h>
#include <synfig/renddesc.h>
#include <synfig/value.h>
#include <synfig/canvas.h>

#endif

using namespace synfig;
using namespace std;
using namespace etl;

/* === M A C R O S ========================================================= */

/* === G L O B A L S ======================================================= */

SYNFIG_LAYER_INIT(Layer_TimeWarp);
SYNFIG_LAYER_SET_NAME(Layer_TimeWarp,"timewarp");
SYNFIG_LAYER_SET_LOCAL_NAME(Layer_TimeWarp,N_("Time Warp"));
SYNFIG_LAYER_SET_CATEGORY(Layer_TimeWarp,N_("Other"));
SYNFIG_LAYER_SET_VERSION(Layer_TimeWarp,"0.2");
SYNFIG_LAYER_SET_CVS_ID(Layer_TimeWarp,"$Id$");

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

Layer_TimeWarp::Layer_TimeWarp()
{
	old_version=false;
	param_ignore_negative_offset=ValueBase(true);
	param_symmetrical=ValueBase(false);
	param_time_shift=ValueBase(Time(0));
	param_anim_origin=ValueBase(Time(0));
	param_anim_duration=ValueBase(Time(1));

	SET_INTERPOLATION_DEFAULTS();
	SET_STATIC_DEFAULTS();
}

Layer_TimeWarp::~Layer_TimeWarp()
{
}

bool
Layer_TimeWarp::set_param(const String & param, const ValueBase &value)
{
	if(old_version)
	{
		if(param=="start_time" && value.same_type_as(start_time))
		{
			value.put(&start_time);
			return true;
		}
		if(param=="end_time" && value.same_type_as(end_time))
		{
			value.put(&end_time);
			return true;
		}
	}
	else
	{
		IMPORT_VALUE(param_anim_origin);
		IMPORT_VALUE(param_time_shift);
		IMPORT_VALUE(param_anim_duration);
		IMPORT_VALUE(param_ignore_negative_offset);
		IMPORT_VALUE(param_symmetrical);
	}

	return Layer::set_param(param,value);
}

ValueBase
Layer_TimeWarp::get_param(const String & param)const
{
	EXPORT_VALUE(param_time_shift);
	EXPORT_VALUE(param_anim_origin);
	EXPORT_VALUE(param_anim_duration);
	EXPORT_VALUE(param_ignore_negative_offset);
	EXPORT_VALUE(param_symmetrical);
	EXPORT_NAME();
	EXPORT_VERSION();

	return Layer::get_param(param);
}

Layer::Vocab
Layer_TimeWarp::get_param_vocab()const
{
	Layer::Vocab ret(Layer::get_param_vocab());

	ret.push_back(ParamDesc("time_shift")
		.set_local_name(_("Time Shift"))
		.set_description(_("Offset to apply to current frame"))
		.set_static(false)
	);

	ret.push_back(ParamDesc("anim_origin")
		.set_local_name(_("Animation Origin Time"))
		.set_description(_("The initial frame of the nested animation"))
		.set_static(true)
	);

	ret.push_back(ParamDesc("anim_duration")
		.set_local_name(_("Animation Duration"))
		.set_description(_("Lenght of the nested animation"))
		.set_static(true)
	);

	ret.push_back(ParamDesc("ignore_negative_offset")
		.set_local_name(_("Ignore Negative Offset"))
		.set_description(_("Do not change the time if the target is before Animation Origin"))
		.set_static(true)
	);

	ret.push_back(ParamDesc("symmetrical")
		.set_local_name(_("Symmetrical"))
		.set_description(_("When checked, loops are mirrored centered at Animation Origin"))
		.set_static(true)
	);

	return ret;
}

bool
Layer_TimeWarp::set_version(const String &ver)
{
	if (ver=="0.1")
		old_version = true;

	return true;
}

void
Layer_TimeWarp::reset_version()
{
	Time link_time, local_time, duration;
	bool only_for_positive_duration, symmetrical;
	// if we're not converting from an old version of the layer, there's nothing to do
	if (!old_version)
		return;

	old_version = false;

	// these are the conversions to go from 0.1 to 0.2:
	//
	//	 local_time = start_time
	//	 duration = end_time - start_time
	//	 if (time < start_time)
	//	   link_time = -duration : if we want to reproduce the old behaviour - do we?
	//	 else
	//		 link_time = 0

	// convert the static parameters
	local_time = start_time;
	duration = end_time - start_time;
	only_for_positive_duration = true;
	symmetrical = false;

	param_anim_origin.set(local_time);
	param_anim_duration.set(duration);
	param_ignore_negative_offset.set(only_for_positive_duration);
	param_symmetrical.set(symmetrical);
	
	//! \todo layer version 0.1 acted differently before start_time was reached - possibly due to a bug
	link_time = 0;
	param_time_shift.set(link_time);

	// convert the dynamic parameters
	const DynamicParamList &dpl = dynamic_param_list();

	// if neither start_time nor end_time are dynamic, there's nothing more to do
	if (dpl.count("start_time") == 0 && dpl.count("end_time") == 0)
		return;

	etl::rhandle<ValueNode> start_time_value_node, end_time_value_node;
	LinkableValueNode* duration_value_node;

	if (dpl.count("start_time"))
	{
		start_time_value_node = dpl.find("start_time")->second;
		disconnect_dynamic_param("start_time");
	}
	else
		start_time_value_node = ValueNode_Const::create(start_time);

	if (dpl.count("end_time"))
	{
		end_time_value_node = dpl.find("end_time")->second;
		disconnect_dynamic_param("end_time");
	}
	else
		end_time_value_node = ValueNode_Const::create(end_time);

	duration_value_node = ValueNode_Subtract::create(Time(0));
	duration_value_node->set_link("lhs", end_time_value_node);
	duration_value_node->set_link("rhs", start_time_value_node);

	connect_dynamic_param("local_time", start_time_value_node);
	connect_dynamic_param("duration",   duration_value_node);
}

void
Layer_TimeWarp::set_time(IndependentContext context, Time t)const
{
	Time time_shift=param_time_shift.get(Time());
	Time anim_origin=param_anim_origin.get(Time());
	Time anim_duration=param_anim_duration.get(Time());
	bool ignore_negative_offset=param_ignore_negative_offset.get(bool());
	bool symmetrical=param_symmetrical.get(bool());
	
	float fps=get_canvas()->rend_desc().get_frame_rate();
  
  // perform all calculation in *frames* to minimize rounding errors
  float offset = fmod(round((t+time_shift)*fps), round(anim_duration*fps))/fps;
  
  if (offset >= 0)
  {
    // "Normal" case
    t = anim_origin+offset;  
  }
  else if (ignore_negative_offset)
  {
    t = anim_origin;
  }
  else if (symmetrical)
  {
    t = anim_origin-offset;
  }
  else
  {
    t = anim_origin+anim_duration-offset;
  }

	context.set_time(t);
}

Color
Layer_TimeWarp::get_color(Context context, const Point &pos)const
{
	return context.get_color(pos);
}

bool
Layer_TimeWarp::accelerated_render(Context context,Surface *surface,int quality, const RendDesc &renddesc, ProgressCallback *cb)const
{
	return context.accelerated_render(surface,quality,renddesc,cb);
}


bool
Layer_TimeWarp::accelerated_cairorender(Context context,cairo_t *cr, int quality, const RendDesc &renddesc, ProgressCallback *cb)const
{
	return context.accelerated_cairorender(cr,quality,renddesc,cb);
}
