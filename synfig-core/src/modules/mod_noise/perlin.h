/* === S Y N F I G ========================================================= */
/*!	\file perlin.h
**	\brief Header file for implementation of the "Perlin Noise" layer
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

/* === S T A R T =========================================================== */

#ifndef __SYNFIG_NOISE_PERLIN_H
#define __SYNFIG_NOISE_PERLIN_H

/* === H E A D E R S ======================================================= */

#include <synfig/vector.h>
#include <synfig/valuenode.h>
#include <synfig/layers/layer_composite.h>
#include <synfig/gradient.h>
#include <synfig/time.h>
#include "random_noise.h"

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */
struct PerlinGrid;

class PerlinNoise : public synfig::Layer_Composite
{
	SYNFIG_LAYER_MODULE_EXT

private:
	synfig::ValueBase param_iterations;
	//!Parameter: (synfig::Real)
	synfig::ValueBase param_time;
	//!Parameter: (bool)

	//!Parameter: (synfig::Vector)
	synfig::ValueBase param_displacement;
	//!Parameter: (synfig::Vector)
	synfig::ValueBase param_size;
	//!Parameter: (RandomNoise)
	synfig::ValueBase param_random;
	//!Parameter: (RandomNoise::SmoothType)
	synfig::ValueBase param_smooth;
	//!Parameter: (int)
	synfig::ValueBase param_turbulent;

	//void sync();
	mutable synfig::Time curr_time;

	synfig::Color color_func(const PerlinGrid& grid, const synfig::Point &x, synfig::Real time, synfig::Context context)const;
//	synfig::Point point_func(const synfig::Point &point)const;

//	float calc_supersample(const synfig::Point &x, float pw,float ph)const;

public:
	PerlinNoise();

	virtual bool set_param(const synfig::String &param, const synfig::ValueBase &value);
	virtual synfig::ValueBase get_param(const synfig::String &param)const;
	virtual Vocab get_param_vocab()const;

	virtual synfig::Color get_color(synfig::Context context, const synfig::Point &pos)const;

	virtual bool accelerated_render(synfig::Context context,synfig::Surface *surface,int quality, const synfig::RendDesc &renddesc, synfig::ProgressCallback *cb)const;
	synfig::Layer::Handle hit_check(synfig::Context context, const synfig::Point &point)const;

	virtual bool reads_context()const { return true; }
}; // EOF of class PerlinNoise

/* === E N D =============================================================== */

#endif
