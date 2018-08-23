/* === S Y N F I G ========================================================= */
/*!	\file real.h
**	\brief Provides the synfig::Real typedef
**
**	$Id$
**
**	\legal
**	Copyright (c) 2002-2005 Robert B. Quattlebaum Jr., Adrian Bentley
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

#ifndef __SYNFIG_REAL_H
#define __SYNFIG_REAL_H

/* === H E A D E R S ======================================================= */
#include <cmath>

/* === T Y P E D E F S ===================================================== */

namespace synfig {

/*!	\typedef Real
**	\todo writeme
*/
typedef double Real;


}; // END of namespace synfig

namespace std {

inline synfig::Real fract(synfig::Real x)
{
  return x-floor(x);
}

inline synfig::Real clamp(synfig::Real x, synfig::Real min, synfig::Real max)
{
  return (x < min) ? min : (x > max) ? max : x;
}

inline synfig::Real smoothstep(synfig::Real edge0, synfig::Real edge1, synfig::Real x)
{
  synfig::Real t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
  return t * t * (3.0 - 2.0 * t);
}


inline synfig::Real mix(synfig::Real x,
                          synfig::Real y,
                          synfig::Real a)
    { return x*(1-a)+y*a; }


}; // END of namespace std


/* === E N D =============================================================== */

#endif
