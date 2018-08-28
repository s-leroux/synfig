/* === S Y N F I G ========================================================= */
/*!	\file math.h
**	\brief Some useful math functions
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

#ifndef __SYNFIG_MATH_H
#define __SYNFIG_MATH_H

/* === H E A D E R S ======================================================= */

/* === T Y P E D E F S ===================================================== */

namespace synfig {

template<typename T>
T clamp(const T& x, const T& minVal, const T& maxVal)
{
  return min(max(x, minVal), maxVal);
}

template<typename T>
T min(const T& x, T& maxVal)
{
  return ( x > maxVal) ? maxVal : x;
}

template<typename T>
T max(const T& x, const T& minVal)
{
  return ( x < minVal) ? minVal : x;
}

template<typename T>
struct Domain
{
  static constexpr const T Min = 0;
  static constexpr const T Max = 1;
};

/**
 * Interpolation functions
 */
template <typename T, typename D = Domain<T>>
struct InterpolationFunction {
  static T linear(const T& v) { return v; }

  static T atan(const T& v) {
    const T x = 2*(v - D::Min)/(D::Max-D::Min)-1;

    return D::Min + (D::Max - D::Min) * (::atan(4 * x)/::atan(4) + 1) / 2;
  }

  static T step(const T& v) { return (v > (D::Max + D::Min)/2) ? D::Max : D::Min; }

  static T smoothstep(const T& v) {
    const T x = (v - D::Min)/(D::Max-D::Min);

    return D::Min + (D::Max - D::Min) * x * x * (3.0 - 2.0 * x);
  }
};

/**
 * Shaping Function
 *
 * Defined for [-1:1] -> [0:1]
 */
template <typename T>
struct ShapingFunction {
  static T linear(const T& v) { return (1.0+v)/2; }

  static T step(const T& v) { return (v > 0) ? 1.0 : 0.0; }

  static T smoothstep(const T& v) {
    const T x = (1 + v)/2;
    return x * x * (3.0 - 2.0 * x);
  }

  static T parabola(const T& v) {
    return 1.0 - v * v;
  }

  static T abs(const T& v) {
    return 1.0 - fabs(v);
  }

  static T ridge(const T& v) {
    const T x = 1.0 - 2*fabs(v);

    return x * x;
  }

  static T pulse(const T& v) {
    const T x = 1.0 - 8*fabs(v);

    return (x <= -1.0) ? 0.0 : smoothstep(x);
  }
};


}; // namespace synfig

/* === E N D =============================================================== */

#endif
