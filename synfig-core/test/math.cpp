/* === S Y N F I G ========================================================= */
/*!	\file bone.cpp
**	\brief Math Test File
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

/* === H E A D E R S ======================================================= */
#include "gtest/gtest.h"

#ifdef USING_PCH
#	include "pch.h"
#else
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <iostream>
#include <cmath>
#include <synfig/math.h>

#endif

/* === U S I N G =========================================================== */

using namespace synfig;
using namespace std;

/* === M A C R O S ========================================================= */

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === T E S T S =========================================================== */

const double epsilon = 1e-6;
TEST(ShapingFunction, Linear)
{
  EXPECT_NEAR(ShapingFunction<double>::linear(0.0), 0.0, epsilon);
  EXPECT_NEAR(ShapingFunction<double>::linear(0.5), 0.5, epsilon);
  EXPECT_NEAR(ShapingFunction<double>::linear(1.0), 1.0, epsilon);
}

TEST(ShapingFunction, Cubic)
{
  EXPECT_NEAR(ShapingFunction<double>::cubic(0.0), 0.0, epsilon);
  EXPECT_NEAR(ShapingFunction<double>::cubic(0.5), 0.5, epsilon);
  EXPECT_NEAR(ShapingFunction<double>::cubic(1.0), 1.0, epsilon);
}

TEST(ShapingFunction, Atan)
{
  EXPECT_NEAR(ShapingFunction<double>::atan(0.0), 0.0, epsilon);
  EXPECT_NEAR(ShapingFunction<double>::atan(0.5), 0.5, epsilon);
  EXPECT_NEAR(ShapingFunction<double>::atan(1.0), 1.0, epsilon);
}

TEST(ShapingFunction, Step)
{
  EXPECT_NEAR(ShapingFunction<double>::step(0.0), 0.0, epsilon);
  EXPECT_NEAR(ShapingFunction<double>::step(1.0), 1.0, epsilon);
}

TEST(ShapingFunction, Smoothstep)
{
  EXPECT_NEAR(ShapingFunction<double>::smoothstep(0.0), 0.0, epsilon);
  EXPECT_NEAR(ShapingFunction<double>::smoothstep(0.5), 0.5, epsilon);
  EXPECT_NEAR(ShapingFunction<double>::smoothstep(1.0), 1.0, epsilon);
}

struct D {
  static constexpr const double Min = 3.0;
  static constexpr const double Max = 5.0;
};

TEST(ShapingFunction, Abs)
{
  EXPECT_NEAR(ShapingFunction<double>::abs(0.0), 0.0, epsilon);
  EXPECT_NEAR(ShapingFunction<double>::abs(0.5), 1.0, epsilon);
  EXPECT_NEAR(ShapingFunction<double>::abs(1.0), 0.0, epsilon);


  EXPECT_NEAR((ShapingFunction<double, D>::abs(3.0)), D::Min, epsilon);
  EXPECT_NEAR((ShapingFunction<double, D>::abs(4.0)), D::Max, epsilon);
  EXPECT_NEAR((ShapingFunction<double, D>::abs(5.0)), D::Min, epsilon);
}

TEST(ShapingFunction, Pulse)
{
  EXPECT_NEAR(ShapingFunction<double>::pulse(0.0), 0.0, epsilon);
  EXPECT_NEAR(ShapingFunction<double>::pulse(0.5), 1.0, epsilon);
  EXPECT_NEAR(ShapingFunction<double>::pulse(1.0), 0.0, epsilon);

  EXPECT_NEAR((ShapingFunction<double, D>::pulse(3.0)), D::Min, epsilon);
  EXPECT_NEAR((ShapingFunction<double, D>::pulse(4.0)), D::Max, epsilon);
  EXPECT_NEAR((ShapingFunction<double, D>::pulse(5.0)), D::Min, epsilon);
}

TEST(ShapingFunction, Parabola)
{
  EXPECT_NEAR(ShapingFunction<double>::parabola(0.0), 0.0, epsilon);
  EXPECT_NEAR(ShapingFunction<double>::parabola(0.5), 0.25, epsilon);
  EXPECT_NEAR(ShapingFunction<double>::parabola(1.0), 1.0, epsilon);
}
