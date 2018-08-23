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
#include <synfig/vector.h>

#endif

/* === U S I N G =========================================================== */

using namespace std;
using namespace etl;
using namespace synfig;

/* === M A C R O S ========================================================= */

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */
namespace synfig {
  std::ostream& operator<<(std::ostream& os, const Vector& v) {
    return os << "Vector(" << v[0] << ", " << v[1] << ")";
  }

}; //namespace synfig

/* === T E S T S =========================================================== */

TEST(VectorTest, Floor)
{
  EXPECT_EQ(floor(Vector(1.5, 2.5)), Vector(1,2));
  EXPECT_EQ(floor(Vector(1.8, 2.2)), Vector(1,2));
  EXPECT_EQ(floor(Vector(-1.8, -2.2)), Vector(-2,-3));
}

TEST(VectorTest, Fract)
{
  const double epsilon = 1e-8;

  EXPECT_NEAR(fract(Vector(1.5, 2.5))[0], 0.5, epsilon);
  EXPECT_NEAR(fract(Vector(1.5, 2.5))[1], 0.5, epsilon);
  EXPECT_NEAR(fract(Vector(1.8, 2.2))[0], 0.8, epsilon);
  EXPECT_NEAR(fract(Vector(1.8, 2.2))[1], 0.2, epsilon);
  EXPECT_NEAR(fract(Vector(-1.8, -2.2))[0], 0.2, epsilon);
  EXPECT_NEAR(fract(Vector(-1.8, -2.2))[1], 0.8, epsilon);
}
