/* === S Y N F I G ========================================================= */
/*!	\file channelmixer.h
**	\brief Header file for implementation of the "Channel Mixer" layer
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

/* === S T A R T =========================================================== */

#ifndef __SYNFIG_LAYER_CHANNELMIXER_H
#define __SYNFIG_LAYER_CHANNELMIXER_H

/* === H E A D E R S ======================================================= */

#include <synfig/layer.h>
#include <synfig/layers/layer_composite.h>
#include <synfig/angle.h>
#include <synfig/gamma.h>
#include <synfig/rect.h>

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */

namespace synfig {

class ChannelMixer : public synfig::Layer_Composite
{
	SYNFIG_LAYER_MODULE_EXT

private:
  friend class ChannelMixerHelper;

	//! Parameter: (int)
	ValueBase param_rm;
	//! Parameter: (Real)
	ValueBase param_rr;
	//! Parameter: (Real)
	ValueBase param_rg;
	//! Parameter: (Real)
	ValueBase param_rb;
	//! Parameter: (Real)
	ValueBase param_ra;
	//! Parameter: (Real)
	ValueBase param_r1;

	//! Parameter: (int)
	ValueBase param_gm;
	//! Parameter: (Real)
	ValueBase param_gr;
	//! Parameter: (Real)
	ValueBase param_gg;
	//! Parameter: (Real)
	ValueBase param_gb;
	//! Parameter: (Real)
	ValueBase param_ga;
	//! Parameter: (Real)
	ValueBase param_g1;

	//! Parameter: (int)
	ValueBase param_bm;
	//! Parameter: (Real)
	ValueBase param_br;
	//! Parameter: (Real)
	ValueBase param_bg;
	//! Parameter: (Real)
	ValueBase param_bb;
	//! Parameter: (Real)
	ValueBase param_ba;
	//! Parameter: (Real)
	ValueBase param_b1;

	//! Parameter: (int)
	ValueBase param_am;
	//! Parameter: (Real)
	ValueBase param_ar;
	//! Parameter: (Real)
	ValueBase param_ag;
	//! Parameter: (Real)
	ValueBase param_ab;
	//! Parameter: (Real)
	ValueBase param_aa;
	//! Parameter: (Real)
	ValueBase param_a1;

  enum Mode
  {
    SUM = 0,
    AVG = 1,
  };

public:

	ChannelMixer();

	virtual bool set_param(const String & param, const synfig::ValueBase &value);

	virtual ValueBase get_param(const String & param)const;

	virtual Color get_color(Context context, const Point &pos)const;

	virtual Rect get_full_bounding_rect(Context context)const;

	virtual bool accelerated_render(Context context,Surface *surface,int quality, const RendDesc &renddesc, ProgressCallback *cb)const;

	virtual Vocab get_param_vocab()const;
}; // END of class ChannelMixer

}; // END of namespace synfig

/* === E N D =============================================================== */

#endif