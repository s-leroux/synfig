/* === S Y N F I G ========================================================= */
/*!	\file valuenodedynamiclistinsert.cpp
**	\brief Template File
**
**	$Id$
**
**	\legal
**	Copyright (c) 2002-2005 Robert B. Quattlebaum Jr., Adrian Bentley
**  Copyright (c) 2008 Chris Moore
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

#include "valuenodedynamiclistinsertangular.h"
#include <synfigapp/canvasinterface.h>
#include <synfig/valuenodes/valuenode_radialcomposite.h>

#include <synfigapp/general.h>

#endif

using namespace std;
using namespace etl;
using namespace synfig;
using namespace synfigapp;
using namespace Action;

/* === M A C R O S ========================================================= */

ACTION_INIT(Action::ValueNodeDynamicListInsertAngular);
ACTION_SET_NAME(Action::ValueNodeDynamicListInsertAngular,"ValueNodeDynamicListInsertAngular");
ACTION_SET_LOCAL_NAME(Action::ValueNodeDynamicListInsertAngular,N_("Insert Angular Point"));
ACTION_SET_TASK(Action::ValueNodeDynamicListInsertAngular,"insert");
ACTION_SET_CATEGORY(Action::ValueNodeDynamicListInsertAngular,Action::CATEGORY_VALUEDESC|Action::CATEGORY_VALUENODE);
ACTION_SET_PRIORITY(Action::ValueNodeDynamicListInsertAngular,-22);
ACTION_SET_VERSION(Action::ValueNodeDynamicListInsertAngular,"0.0");
ACTION_SET_CVS_ID(Action::ValueNodeDynamicListInsertAngular,"$Id$");

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

Action::ValueNodeDynamicListInsertAngular::ValueNodeDynamicListInsertAngular()
{
	index=0;
	time=0;
	origin=0.5f;
	set_dirty(true);
}

Action::ParamVocab
Action::ValueNodeDynamicListInsertAngular::get_param_vocab()
{
	ParamVocab ret(Action::CanvasSpecific::get_param_vocab());

	ret.push_back(ParamDesc("value_desc",Param::TYPE_VALUEDESC)
		.set_local_name(_("ValueDesc"))
	);
	ret.push_back(ParamDesc("time",Param::TYPE_TIME)
		.set_local_name(_("Time"))
		.set_optional()
	);
	ret.push_back(ParamDesc("origin",Param::TYPE_REAL)
		.set_local_name(_("Origin"))
		.set_optional()
	);

	return ret;
}

bool
Action::ValueNodeDynamicListInsertAngular::is_candidate(const ParamList &x)
{
	if (!candidate_check(get_param_vocab(),x))
		return false;
    
	ValueDesc value_desc(x.find("value_desc")->second.get_value_desc());

	return ( (value_desc.parent_is_value_node() &&
			// We need a Dynamic List parent.
			ValueNode_DynamicList::Handle::cast_dynamic(value_desc.get_parent_value_node()))
			||
			// Or a Dynamic List value node
			(value_desc.is_value_node() &&
			ValueNode_DynamicList::Handle::cast_dynamic(value_desc.get_value_node())) );
}

bool
Action::ValueNodeDynamicListInsertAngular::set_param(const synfig::String& name, const Action::Param &param)
{
	if(name=="value_desc" && param.get_type()==Param::TYPE_VALUEDESC)
	{
		ValueDesc value_desc(param.get_value_desc());

		if(!value_desc.parent_is_value_node())
		{
			if(value_desc.is_value_node())
			{
				value_node=ValueNode_DynamicList::Handle::cast_dynamic(value_desc.get_value_node());
				index=0;
			}
			else
				return false;
		}
		else
		{
			value_node=ValueNode_DynamicList::Handle::cast_dynamic(value_desc.get_parent_value_node());
			index=value_desc.get_index();
		}
		if(!value_node) // Not a Dynamic list
			return false;

		list_entry=value_node->create_list_entry(index,time,origin);

    // Post-processing:
    // Assume if the item is a composite and it has both t1 and t2, those
    // two later value nodes are tangents.
    LinkableValueNode *linkable=dynamic_cast<LinkableValueNode*>(&*list_entry.value_node);
    if (linkable) try
    {
      int t1_index = linkable->get_link_index_from_name("t1");
      int t2_index = linkable->get_link_index_from_name("t2");
      
  		linkable->set_link(t1_index,ValueNode_RadialComposite::create(Vector(0,0)));
  		linkable->set_link(t2_index,ValueNode_RadialComposite::create(Vector(0,0)));
      
      // Optionally:
      try { linkable->set_link("split_angle", ValueNode_Const::create(true)); } catch(Exception::BadLinkName& e) {}
      try { linkable->set_link("split_radius", ValueNode_Const::create(true)); } catch(Exception::BadLinkName& e) {}
    }
    catch(Exception::BadLinkName& e)
    {
      // ignore: this node do not have tangents  
    }

		assert(list_entry.value_node.rcount()==1);
		return true;

	}
	if(name=="time" && param.get_type()==Param::TYPE_TIME)
	{
		time=param.get_time();

		return true;
	}
	if(name=="origin" && param.get_type()==Param::TYPE_REAL)
	{
		origin=param.get_real();

		return true;
	}

	return Action::CanvasSpecific::set_param(name,param);
}

bool
Action::ValueNodeDynamicListInsertAngular::is_ready()const
{
	if(!value_node)
		return false;
	return Action::CanvasSpecific::is_ready();
}

void
Action::ValueNodeDynamicListInsertAngular::perform()
{
	if(index>value_node->link_count())
		index=value_node->link_count();

	value_node->add(list_entry,index);
	assert(list_entry.value_node.rcount()>=2);

	// Signal that a layer has been inserted
	value_node->changed();
/*_if(get_canvas_interface())
	{
		get_canvas_interface()->signal_value_node_changed()(value_node);
	}
	else synfig::warning("CanvasInterface not set on action");*/
}

void
Action::ValueNodeDynamicListInsertAngular::undo()
{
	assert(list_entry.value_node.rcount()>=2);
	value_node->erase((value_node->list.begin()+index)->value_node);
	assert(list_entry.value_node.rcount()>=1);

	// Signal that a layer has been inserted
	value_node->changed();
/*_if(get_canvas_interface())
	{
		get_canvas_interface()->signal_value_node_changed()(value_node);
	}
	else synfig::warning("CanvasInterface not set on action");*/
}
