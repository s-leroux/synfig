/* === S Y N F I G ========================================================= */
/*!	\file valuenode_scale.cpp
**	\brief Implementation of the "Time Scale" valuenode conversion.
**
**	$Id$
**
**	\legal
**	Copyright (c) 2002-2005 Robert B. Quattlebaum Jr., Adrian Bentley
**	Copyright (c) 2007, 2008 Chris Moore
**  Copyright (c) 2011 Carlos López
**  Copyright (c) 2018 Sylvain Leroux
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

#include <synfig/general.h>
#include "valuenode_timescale.h"
#include "valuenode_const.h"
#include <stdexcept>
#include <synfig/color.h>
#include <synfig/vector.h>
#include <synfig/time.h>
#include <synfig/angle.h>
#include <ETL/misc>

#endif

/* === U S I N G =========================================================== */

using namespace std;
using namespace etl;
using namespace synfig;

/* === M A C R O S ========================================================= */

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

ValueNode_TimeScale::ValueNode_TimeScale(const ValueBase &value):
	LinkableValueNode(value.get_type())
{
	Vocab ret(get_children_vocab());
	set_children_vocab(ret);
	set_link("origin",ValueNode_Const::create(Time(0)));
	set_link("scale",ValueNode_Const::create(Time(1)));
	Type &type(value.get_type());

	if (type == type_angle)
		set_link("link",ValueNode_Const::create(value.get(Angle())));
	else
	if (type == type_color)
		set_link("link",ValueNode_Const::create(value.get(Color())));
	else
	if (type == type_integer)
		set_link("link",ValueNode_Const::create(value.get(int())));
	else
	if (type == type_real)
		set_link("link",ValueNode_Const::create(value.get(Real())));
	else
	if (type == type_time)
		set_link("link",ValueNode_Const::create(value.get(Time())));
	else
	if (type == type_vector)
		set_link("link",ValueNode_Const::create(value.get(Vector())));
	else
	{
		assert(0);
		throw runtime_error(get_local_name()+_(":Bad type ")+type.description.local_name);
	}

	assert(value_node);
	assert(value_node->get_type()==type);
	assert(get_type()==type);
}

LinkableValueNode*
ValueNode_TimeScale::create_new()const
{
	return new ValueNode_TimeScale(get_type());
}

ValueNode_TimeScale*
ValueNode_TimeScale::create(const ValueBase& value)
{
	return new ValueNode_TimeScale(value);
}

synfig::ValueNode_TimeScale::~ValueNode_TimeScale()
{
	unlink_all();
}

synfig::ValueBase
synfig::ValueNode_TimeScale::operator()(Time t)const
{
	if (getenv("SYNFIG_DEBUG_VALUENODE_OPERATORS"))
		printf("%s:%d operator()\n", __FILE__, __LINE__);

	if(!value_node || !origin || !scale)
		throw runtime_error(strprintf("ValueNode_TimeScale: %s",_("One or several of my parameters aren't set!")));
	
  Real scalar_value = (Real(t)-(*origin)(t).get(Real()))/(*scale)(t).get(Real());
  
  if(get_type()==type_angle)
		return (*value_node)(t).get(Angle())*scalar_value;
	else if(get_type()==type_color)
	{
		Color ret((*value_node)(t).get(Color()));
		ret.set_r(ret.get_r()*scalar_value);
		ret.set_g(ret.get_g()*scalar_value);
		ret.set_b(ret.get_b()*scalar_value);
		return ret;
	}
	else if(get_type()==type_integer)
		return round_to_int((*value_node)(t).get(int())*scalar_value);
	else if(get_type()==type_real)
		return (*value_node)(t).get(Real())*scalar_value;
	else if(get_type()==type_time)
		return (*value_node)(t).get(Time())*scalar_value;
	else if(get_type()==type_vector)
		return (*value_node)(t).get(Vector())*scalar_value;

	assert(0);
	return ValueBase();
}

synfig::ValueBase
synfig::ValueNode_TimeScale::get_inverse(Time t, const synfig::Vector &target_value) const
{
  Real scalar_value = (Real(t)-(*origin)(t).get(Real()))/(*scale)(t).get(Real());
	if(scalar_value==0)
			throw runtime_error(strprintf("ValueNode_TimeScale: %s",_("Attempting to get the inverse of a non invertible Valuenode")));
	else
		{
			if (get_type() == type_real)
				return target_value.mag() / scalar_value;
			if (get_type() == type_angle)
				return Angle::tan(target_value[1] / scalar_value ,target_value[0] / scalar_value);
			return target_value / scalar_value;
		}
	return ValueBase();
}

synfig::ValueBase
synfig::ValueNode_TimeScale::get_inverse(Time t, const synfig::Angle &target_value) const
{
  Real scalar_value = (Real(t)-(*origin)(t).get(Real()))/(*scale)(t).get(Real());
	if(scalar_value==0)
		throw runtime_error(strprintf("ValueNode_TimeScale: %s",_("Attempting to get the inverse of a non invertible Valuenode")));
	else
		return target_value / scalar_value;
	return ValueBase();
}

synfig::ValueBase
synfig::ValueNode_TimeScale::get_inverse(Time t, const synfig::Real &target_value) const
{
  Real scalar_value = (Real(t)-(*origin)(t).get(Real()))/(*scale)(t).get(Real());
	if(scalar_value==0)
		throw runtime_error(strprintf("ValueNode_TimeScale: %s",_("Attempting to get the inverse of a non invertible Valuenode")));
	else
		return target_value / scalar_value;
	return ValueBase();
}

bool
synfig::ValueNode_TimeScale::is_invertible(Time t) const
{
  Real scalar_value = (Real(t)-(*origin)(t).get(Real()))/(*scale)(t).get(Real());
	return (!(scalar_value==0));
}

bool
ValueNode_TimeScale::set_link_vfunc(int i,ValueNode::Handle value)
{
	assert(i>=0 && i<link_count());

	switch(i)
	{
	case 0: CHECK_TYPE_AND_SET_VALUE(value_node, get_type());
	case 1: CHECK_TYPE_AND_SET_VALUE(origin,     type_real);
	case 2: CHECK_TYPE_AND_SET_VALUE(scale,     type_real);
	}
	return false;
}

ValueNode::LooseHandle
ValueNode_TimeScale::get_link_vfunc(int i)const
{
	assert(i>=0 && i<link_count());

	switch(i)
	{
	case 0: return value_node;
	case 1: return origin;
	case 2: return scale;
	}
	return 0;
}

String
ValueNode_TimeScale::get_name()const
{
	return "timescale";
}

String
ValueNode_TimeScale::get_local_name()const
{
	return _("TimeScale");
}

bool
ValueNode_TimeScale::check_type(Type &type)
{
	return
		type==type_angle	||
		type==type_color	||
		type==type_integer	||
		type==type_real		||
		type==type_time		||
		type==type_vector;
}

LinkableValueNode::Vocab
ValueNode_TimeScale::get_children_vocab_vfunc()const
{
	if(children_vocab.size())
		return children_vocab;

	LinkableValueNode::Vocab ret;

	ret.push_back(ParamDesc(ValueBase(),"link")
		.set_local_name(_("Link"))
		.set_description(_("The value node used to scale"))
	);

	ret.push_back(ParamDesc(ValueBase(),"origin")
		.set_local_name(_("Origin"))
		.set_description(_("The time origin"))
	);

	ret.push_back(ParamDesc(ValueBase(),"scale")
		.set_local_name(_("Scale"))
		.set_description(_("The time scale"))
	);

	return ret;
}
