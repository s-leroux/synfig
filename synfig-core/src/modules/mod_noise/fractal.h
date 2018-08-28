/* === S Y N F I G ========================================================= */
/*!	\file fractal.h
**	\brief Header file for implementation of the "Fractal Noise" layer
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
#include <synfig/real.h>
#include "random_noise.h"

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */
template<synfig::Real (*SHAPE)(const synfig::Real&)>
struct FractalGrid;

class FractalNoise : public synfig::Layer_Composite
{
	SYNFIG_LAYER_MODULE_EXT

private:
  friend struct FractalNoiseParams;

	//!Parameter: (int)
	synfig::ValueBase param_interpolation;
	//!Parameter: (int)
	synfig::ValueBase param_iterations;
	//!Parameter: (Angle)
	synfig::ValueBase param_rotation;
	//!Parameter: (synfig::Real)
	synfig::ValueBase param_gain;
	//!Parameter: (synfig::Real)
	synfig::ValueBase param_lacunarity;
	//!Parameter: (int)
	synfig::ValueBase param_shape;
	//!Parameter: (synfig::Real)
	synfig::ValueBase param_nsscale;
	//!Parameter: (synfig::Real)
	synfig::ValueBase param_nsoffset;
	//!Parameter: (synfig::Real)
	synfig::ValueBase param_time;
	//!Parameter: (int)
	synfig::ValueBase param_size;
	//!Parameter: (synfig::Real)
	synfig::ValueBase param_scale;
	//!Parameter: (int)
	synfig::ValueBase param_seed;

	//void sync();
	mutable synfig::Time curr_time;

public:
	FractalNoise();

	enum InterpolationType
	{
		INTERPOLATION_LINEAR		        = 0,
		INTERPOLATION_STEP		          = 10,
		INTERPOLATION_SMOOTHSTEP		    = 11,
		INTERPOLATION_ATAN      		    = 20,
	};

  enum
  {
    SHAPE_LINEAR                    = 0,
    SHAPE_ABS                       = 1,
    SHAPE_RIDGE                     = 2,
    SHAPE_PULSE                     = 3,
  };


	virtual bool set_param(const synfig::String &param, const synfig::ValueBase &value);
	virtual synfig::ValueBase get_param(const synfig::String &param)const;
	virtual Vocab get_param_vocab()const;

	virtual synfig::Color get_color(synfig::Context context, const synfig::Point &pos)const;

	virtual bool accelerated_render(synfig::Context context,synfig::Surface *surface,int quality, const synfig::RendDesc &renddesc, synfig::ProgressCallback *cb)const;
	synfig::Layer::Handle hit_check(synfig::Context context, const synfig::Point &point)const;

	virtual bool reads_context()const { return true; }
}; // EOF of class FractalNoise

/* === E N D =============================================================== */

#endif
