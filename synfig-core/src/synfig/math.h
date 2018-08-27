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
struct Domain
{
  static constexpr const T Min = 0;
  static constexpr const T Max = 1;
};

template <typename T, typename D = Domain<T>>
struct Test {
  static bool higher_half(const T& v) { return (v > (D::Min + D::Max)/2); }
};

/**
 * ShapingFunction
 */
template <typename T, typename D = Domain<T>>
struct ShapingFunction {
  static T linear(const T& v) { return v; }

  static T cubic(const T& v) {
    const T x = 2*(v - D::Min)/(D::Max-D::Min)-1;

    return D::Min + (D::Max - D::Min) * (x * x * x + 1) / 2;
  }

  static T atan(const T& v) {
    const T x = 2*(v - D::Min)/(D::Max-D::Min)-1;

    return D::Min + (D::Max - D::Min) * (::atan(4 * x)/::atan(4) + 1) / 2;
  }

  template<bool (*Test)(const T& v) = Test<T,D>::higher_half>
  static T step(const T& v) { return Test(v) ? D::Max : D::Min; }

  static T smoothstep(const T& v) {
    const T x = (v - D::Min)/(D::Max-D::Min);

    return D::Min + (D::Max - D::Min) * x * x * (3.0 - 2.0 * x);
  }

  static T parabola(const T& v) {
    const T x = (v - D::Min)/(D::Max-D::Min);

    return D::Min + (D::Max - D::Min) * x * x;
  }

  static T abs(const T& v) {
    return D::Min + 2*fabs(v - (D::Max+D::Min)/2);
  }

};


}; // namespace synfig

/* === E N D =============================================================== */

#endif
