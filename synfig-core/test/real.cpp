/* === S Y N F I G ========================================================= */
/*!	\file bone.cpp
**	\brief Bone Test File
**
**	$Id$
**
**	\legal
**	Copyright (c) 2008 Chris Moore
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
#include <synfig/real.h>

#endif

/* === U S I N G =========================================================== */

using namespace std;
using namespace synfig;

/* === M A C R O S ========================================================= */

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === T E S T S =========================================================== */

TEST(RealTest, Fract)
{
  EXPECT_DOUBLE_EQ(fract(0.0), 0.0);
  EXPECT_DOUBLE_EQ(fract(0.2), 0.2);
  EXPECT_DOUBLE_EQ(fract(0.5), 0.5);
  EXPECT_DOUBLE_EQ(fract(0.8), 0.8);
  EXPECT_DOUBLE_EQ(fract(-0.2), 0.8);
  EXPECT_DOUBLE_EQ(fract(-0.5), 0.5);
  EXPECT_DOUBLE_EQ(fract(-0.8), 0.2);
}

TEST(RealTest, Clamp)
{
  Real a = -1.0;
  Real b = 1.0;
  Real c = 2.0;

  EXPECT_EQ(clamp(c, a,b), b);
  EXPECT_EQ(clamp(b, a,c), b);
  EXPECT_EQ(clamp(a, b,c), b);
  EXPECT_EQ(clamp(a, a,a), a);
}

TEST(RealTest, Smoothstep)
{
  EXPECT_DOUBLE_EQ(smoothstep(0.0, 1.0, 0.0), 0.0);
  EXPECT_DOUBLE_EQ(smoothstep(0.0, 1.0, 1.0), 1.0);
  EXPECT_DOUBLE_EQ(smoothstep(0.0, 1.0, 0.5), 0.5);
}

TEST(RealTest, Mix)
{
  EXPECT_DOUBLE_EQ(mix(0.0, 1.0, 0.0), 0.0);
  EXPECT_DOUBLE_EQ(mix(0.0, 1.0, 1.0), 1.0);
  EXPECT_DOUBLE_EQ(mix(0.0, 1.0, 0.5), 0.5);
}
