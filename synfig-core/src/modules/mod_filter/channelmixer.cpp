/* === S Y N F I G ========================================================= */
/*!	\file channelmixer.cpp
**	\brief Implementation of the "Channel Mixer" layer
**
**	$Id$
**
**	\legal
**	Copyright (c) 2002-2005 Robert B. Quattlebaum Jr., Adrian Bentley
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

#include "channelmixer.h"
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

SYNFIG_LAYER_INIT(ChannelMixer);
SYNFIG_LAYER_SET_NAME(ChannelMixer,"channelmixer");
SYNFIG_LAYER_SET_LOCAL_NAME(ChannelMixer,N_("Channel Mixer"));
SYNFIG_LAYER_SET_CATEGORY(ChannelMixer,N_("Filters"));
SYNFIG_LAYER_SET_VERSION(ChannelMixer,"0.1");
SYNFIG_LAYER_SET_CVS_ID(ChannelMixer,"$Id$");

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

/* === E N T R Y P O I N T ================================================= */
namespace synfig
{

class ChannelMixerHelper
{
  const int rm;
  const Real rr;
  const Real rg;
  const Real rb;
  const Real ra;
  const Real r1;

  const int gm;
  const Real gr;
  const Real gg;
  const Real gb;
  const Real ga;
  const Real g1;

  const int bm;
  const Real br;
  const Real bg;
  const Real bb;
  const Real ba;
  const Real b1;

  const int am;
  const Real ar;
  const Real ag;
  const Real ab;
  const Real aa;
  const Real a1;

public:
  ChannelMixerHelper(const ChannelMixer& layer)
    : rm(layer.param_rm.get(int())),
      rr(layer.param_rr.get(Real())),
      rg(layer.param_rg.get(Real())),
      rb(layer.param_rb.get(Real())),
      ra(layer.param_ra.get(Real())),
      r1(layer.param_r1.get(Real())),

      gm(layer.param_gm.get(int())),
      gr(layer.param_gr.get(Real())),
      gg(layer.param_gg.get(Real())),
      gb(layer.param_gb.get(Real())),
      ga(layer.param_ga.get(Real())),
      g1(layer.param_g1.get(Real())),

      bm(layer.param_bm.get(int())),
      br(layer.param_br.get(Real())),
      bg(layer.param_bg.get(Real())),
      bb(layer.param_bb.get(Real())),
      ba(layer.param_ba.get(Real())),
      b1(layer.param_b1.get(Real())),

      am(layer.param_am.get(int())),
      ar(layer.param_ar.get(Real())),
      ag(layer.param_ag.get(Real())),
      ab(layer.param_ab.get(Real())),
      aa(layer.param_aa.get(Real())),
      a1(layer.param_a1.get(Real()))
  {}

  inline Real combine(int mode, Real a1, Real v1,
                                Real a2, Real v2,
                                Real a3, Real v3,
                                Real a4, Real v4,
                                Real a5, Real v5) const
  {
    switch(mode)
    {
      case ChannelMixer::SUM:
        return a1*v1+a2*v2+a3*v3+a4*v4+a5*v5;
      case ChannelMixer::AVG:
        return (a1*a1*v1 + a2*a2*v2 + a3*a3*v3 + a4*a4*v4 + a5*a5*v5)/(a1+a2+a3+a4+a5);
      default:
        return 0.0;
    };
  }

  inline Color mix(const Color &in) const
  {
    Real r = in.get_r();
    Real g = in.get_g();
    Real b = in.get_b();
    Real a = in.get_a();

    return Color(
      combine(rm, rr, r, rg, g, rb, b, ra, a, r1, 1.0),
      combine(gm, gr, r, gg, g, gb, b, ga, a, g1, 1.0),
      combine(bm, br, r, bg, g, bb, b, ba, a, b1, 1.0),
      combine(am, ar, r, ag, g, ab, b, aa, a, a1, 1.0)
    );
  }
};

}; // namespace synfig

ChannelMixer::ChannelMixer():
	Layer_Composite(1.0,Color::BLEND_COMPOSITE),

	param_rm(ValueBase(int(SUM))),
	param_rr(ValueBase(Real(1.0))),
	param_rg(ValueBase(Real(0.0))),
	param_rb(ValueBase(Real(0.0))),
	param_ra(ValueBase(Real(0.0))),
	param_r1(ValueBase(Real(0.0))),

	param_gm(ValueBase(int(SUM))),
	param_gr(ValueBase(Real(0.0))),
	param_gg(ValueBase(Real(1.0))),
	param_gb(ValueBase(Real(0.0))),
	param_ga(ValueBase(Real(0.0))),
	param_g1(ValueBase(Real(0.0))),

	param_bm(ValueBase(int(SUM))),
	param_br(ValueBase(Real(0.0))),
	param_bg(ValueBase(Real(0.0))),
	param_bb(ValueBase(Real(1.0))),
	param_ba(ValueBase(Real(0.0))),
	param_b1(ValueBase(Real(0.0))),

	param_am(ValueBase(int(SUM))),
	param_ar(ValueBase(Real(0.0))),
	param_ag(ValueBase(Real(0.0))),
	param_ab(ValueBase(Real(0.0))),
	param_aa(ValueBase(Real(1.0))),
	param_a1(ValueBase(Real(0.0)))
{
	SET_INTERPOLATION_DEFAULTS();
	SET_STATIC_DEFAULTS();
}

bool
ChannelMixer::set_param(const String & param, const ValueBase &value)
{
	IMPORT_VALUE(param_rm);
	IMPORT_VALUE(param_rr);
	IMPORT_VALUE(param_rg);
	IMPORT_VALUE(param_rb);
	IMPORT_VALUE(param_ra);
	IMPORT_VALUE(param_r1);

	IMPORT_VALUE(param_gm);
	IMPORT_VALUE(param_gg);
  IMPORT_VALUE(param_gr);
	IMPORT_VALUE(param_gb);
	IMPORT_VALUE(param_ga);
	IMPORT_VALUE(param_g1);

	IMPORT_VALUE(param_bm);
	IMPORT_VALUE(param_br);
	IMPORT_VALUE(param_bg);
	IMPORT_VALUE(param_bb);
	IMPORT_VALUE(param_ba);
	IMPORT_VALUE(param_b1);

	IMPORT_VALUE(param_am);
	IMPORT_VALUE(param_ar);
	IMPORT_VALUE(param_ag);
	IMPORT_VALUE(param_ab);
	IMPORT_VALUE(param_aa);
	IMPORT_VALUE(param_a1);

	return Layer_Composite::set_param(param,value);
}

ValueBase
ChannelMixer::get_param(const String &param)const
{
	EXPORT_VALUE(param_rm);
	EXPORT_VALUE(param_rr);
	EXPORT_VALUE(param_rg);
	EXPORT_VALUE(param_rb);
	EXPORT_VALUE(param_ra);
	EXPORT_VALUE(param_r1);

	EXPORT_VALUE(param_gm);
	EXPORT_VALUE(param_gr);
	EXPORT_VALUE(param_gg);
	EXPORT_VALUE(param_gb);
	EXPORT_VALUE(param_ga);
	EXPORT_VALUE(param_g1);

	EXPORT_VALUE(param_bm);
	EXPORT_VALUE(param_br);
	EXPORT_VALUE(param_bg);
	EXPORT_VALUE(param_bb);
	EXPORT_VALUE(param_ba);
	EXPORT_VALUE(param_b1);

	EXPORT_VALUE(param_am);
	EXPORT_VALUE(param_ar);
	EXPORT_VALUE(param_ag);
	EXPORT_VALUE(param_ab);
	EXPORT_VALUE(param_aa);
	EXPORT_VALUE(param_a1);

	EXPORT_NAME();
	EXPORT_VERSION();

	return Layer_Composite::get_param(param);
}

Layer::Vocab
ChannelMixer::get_param_vocab()const
{
	Layer::Vocab ret(Layer_Composite::get_param_vocab());

	ret.push_back(ParamDesc("rm")
		.set_local_name(_("Red method"))
		.set_description(_("How individual components are mixed in the output Red channel"))
		.set_hint("enum")
		.add_enum_value(SUM,	"sum",	_("Sum"))
		.add_enum_value(AVG,	"avg",	_("Average"))
	);

	ret.push_back(ParamDesc("rr")
		.set_local_name(_("Red <- Red"))
		.set_description(_("Amount of input Red channel in output Red channel"))
	);

	ret.push_back(ParamDesc("rg")
		.set_local_name(_("Red <- Green"))
		.set_description(_("Amount of input Green channel in output Red channel"))
	);

	ret.push_back(ParamDesc("rb")
		.set_local_name(_("Red <- Blue"))
		.set_description(_("Amount of input Blue channel in output Red channel"))
  );

	ret.push_back(ParamDesc("ra")
		.set_local_name(_("Red <- Alpha"))
		.set_description(_("Amount of input Alpha channel in output Red channel"))
	);

	ret.push_back(ParamDesc("r1")
		.set_local_name(_("Red <- 1.0"))
		.set_description(_("Extra amount of Red in the output"))
	);

	ret.push_back(ParamDesc("gm")
		.set_local_name(_("Green method"))
		.set_description(_("How individual components are mixed in the output Green channel"))
		.set_hint("enum")
		.add_enum_value(SUM,	"sum",	_("Sum"))
		.add_enum_value(AVG,	"avg",	_("Average"))
	);

	ret.push_back(ParamDesc("gr")
		.set_local_name(_("Green <- Red"))
		.set_description(_("Amount of input Red channel in output Green channel"))
	);

	ret.push_back(ParamDesc("gg")
		.set_local_name(_("Green <- Green"))
		.set_description(_("Amount of input Green channel in output Green channel"))
	);

	ret.push_back(ParamDesc("gb")
		.set_local_name(_("Green <- Blue"))
		.set_description(_("Amount of input Blue channel in output Green channel"))

  );
	ret.push_back(ParamDesc("ga")
		.set_local_name(_("Green <- Alpha"))
		.set_description(_("Amount of input Alpha channel in output Green channel"))
	);

	ret.push_back(ParamDesc("g1")
		.set_local_name(_("Green <- 1.0"))
		.set_description(_("Extra amount of Green in the output"))
	);

	ret.push_back(ParamDesc("bm")
		.set_local_name(_("Blue method"))
		.set_description(_("How individual components are mixed in the output Blue channel"))
		.set_hint("enum")
		.add_enum_value(SUM,	"sum",	_("Sum"))
		.add_enum_value(AVG,	"avg",	_("Average"))
	);

	ret.push_back(ParamDesc("br")
		.set_local_name(_("Blue <- Red"))
		.set_description(_("Amount of input Red channel in output Blue channel"))
	);

	ret.push_back(ParamDesc("bg")
		.set_local_name(_("Blue <- Green"))
		.set_description(_("Amount of input Green channel in output Blue channel"))
	);

	ret.push_back(ParamDesc("bb")
		.set_local_name(_("Blue <- Blue"))
		.set_description(_("Amount of input Blue channel in output Blue channel"))

  );
	ret.push_back(ParamDesc("ba")
		.set_local_name(_("Blue <- Alpha"))
		.set_description(_("Amount of input Alpha channel in output Blue channel"))
	);

	ret.push_back(ParamDesc("b1")
		.set_local_name(_("Blue <- 1.0"))
		.set_description(_("Extra amount of Blue in the output"))
	);

	ret.push_back(ParamDesc("am")
		.set_local_name(_("Alpha method"))
		.set_description(_("How individual components are mixed in the output Alpha channel"))
		.set_hint("enum")
		.add_enum_value(SUM,	"sum",	_("Sum"))
		.add_enum_value(AVG,	"avg",	_("Average"))
	);

	ret.push_back(ParamDesc("ar")
		.set_local_name(_("Alpha <- Red"))
		.set_description(_("Amount of input Red channel in output Alpha channel"))
	);

	ret.push_back(ParamDesc("ag")
		.set_local_name(_("Alpha <- Green"))
		.set_description(_("Amount of input Green channel in output Alpha channel"))
	);

	ret.push_back(ParamDesc("ab")
		.set_local_name(_("Alpha <- Blue"))
		.set_description(_("Amount of input Blue channel in output Alpha channel"))

  );
	ret.push_back(ParamDesc("aa")
		.set_local_name(_("Alpha <- Alpha"))
		.set_description(_("Amount of input Alpha channel in output Alpha channel"))
	);

	ret.push_back(ParamDesc("a1")
		.set_local_name(_("Alpha <- 1.0"))
		.set_description(_("Extra amount of Alpha in the output"))
	);

	return ret;
}

Color
ChannelMixer::get_color(Context context, const Point &pos)const
{
  ChannelMixerHelper  mixer(*this);
	return mixer.mix(context.get_color(pos));
}

bool
ChannelMixer::accelerated_render(Context context,Surface *surface,int quality, const RendDesc &renddesc, ProgressCallback *cb)const
{
	RENDER_TRANSFORMED_IF_NEED(__FILE__, __LINE__)

	SuperCallback supercb(cb,0,9500,10000);

	if(!context.accelerated_render(surface,quality,renddesc,&supercb))
		return false;

	int x,y;

	Surface::pen pen(surface->begin());
  ChannelMixerHelper  mixer(*this);


	for(y=0;y<renddesc.get_h();y++,pen.inc_y(),pen.dec_x(x))
		for(x=0;x<renddesc.get_w();x++,pen.inc_x())
			pen.put_value(mixer.mix(pen.get_value()));

	// Mark our progress as finished
	if(cb && !cb->amount_complete(10000,10000))
		return false;

	return true;
}

Rect
ChannelMixer::get_full_bounding_rect(Context context)const
{
	return context.get_full_bounding_rect();
}
