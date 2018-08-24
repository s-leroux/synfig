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

using namespace etl;
using namespace synfig;
using namespace std;

/* === M A C R O S ========================================================= */

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */
namespace synfig {
  std::ostream& operator<<(std::ostream& os, const Vector& v) {
    return os << "Vector(" << v[0] << ", " << v[1] << ")";
  }

  std::ostream& operator<<(std::ostream& os, const Vector2D& v) {
    return os << "Vector(" << v[0] << ", " << v[1] << ")";
  }

  std::ostream& operator<<(std::ostream& os, const Vector3D& v) {
    return os << "Vector(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
  }

}; //namespace synfig

/* === T E S T S =========================================================== */

TEST(VectorTest, Floor)
{
  EXPECT_EQ(std::floor(Vector(1.5, 2.5)), Vector(1,2));
  EXPECT_EQ(std::floor(Vector(1.8, 2.2)), Vector(1,2));
  EXPECT_EQ(std::floor(Vector(-1.8, -2.2)), Vector(-2,-3));
}

const double epsilon = 1e-6;
TEST(VectorTest, Fract)
{

  EXPECT_NEAR(std::fract(Vector(1.5, 2.5))[0], 0.5, epsilon);
  EXPECT_NEAR(std::fract(Vector(1.5, 2.5))[1], 0.5, epsilon);
  EXPECT_NEAR(std::fract(Vector(1.8, 2.2))[0], 0.8, epsilon);
  EXPECT_NEAR(std::fract(Vector(1.8, 2.2))[1], 0.2, epsilon);
  EXPECT_NEAR(std::fract(Vector(-1.8, -2.2))[0], 0.2, epsilon);
  EXPECT_NEAR(std::fract(Vector(-1.8, -2.2))[1], 0.8, epsilon);
}

TEST(Vector3DTest, Dimensions)
{
  Vector3D  v;

  EXPECT_EQ(Vector3D::dimensions, 3U);
  EXPECT_EQ(v.dimensions, 3U);
  EXPECT_EQ(v[0], 0.0);
  EXPECT_EQ(v[1], 0.0);
  EXPECT_EQ(v[2], 0.0);
}

TEST(Vector3DTest, Ctor)
{
  Vector3D  v = {2,3,4};
  EXPECT_EQ(v[0], 2.0);
  EXPECT_EQ(v[1], 3.0);
  EXPECT_EQ(v[2], 4.0);

  Vector3D  v2(1.1);
  EXPECT_EQ(v2[0], 1.1);
  EXPECT_EQ(v2[1], 1.1);
  EXPECT_EQ(v2[2], 1.1);
}

TEST(Vector3DTest, Equality)
{
  Vector3D  v1 = {0,1,2};
  Vector3D  v2 = {0,1,2};
  Vector3D  v3 = {1,3,2};

  EXPECT_EQ(v1, v2);
  EXPECT_NE(v1, v3);
  EXPECT_NE(v2, v3);
}

TEST(Vector3DTest, Arithmetics)
{
  Vector3D  v1 = {0,1,2};
  Vector3D  v2 = {3,4,5};
  Vector3D v3 = v1;

  EXPECT_EQ(v3+=v2, Vector3D(3,5,7));

  EXPECT_EQ(v1+v2, Vector3D(3,5,7));
  EXPECT_EQ(v1+10.1, Vector3D(10.1,11.1,12.1));
  EXPECT_EQ(v1-v2, Vector3D(-3,-3,-3));
  EXPECT_EQ(v1-10.1, Vector3D(-10.1,-9.1,-8.1));
  EXPECT_EQ(v1*v2, Vector3D(0,4,10));
  EXPECT_EQ(v1/v2, Vector3D(0.0/3,1.0/4,2.0/5));
}

TEST(Vector3DTest, Functions)
{
  Vector3D  v1 = {0.2, 1.5, 2.8};
  Vector3D  v2 = {3,4,5};

  EXPECT_EQ(floor(v1), Vector3D(0,1,2));
  EXPECT_NEAR(sum(v1), 4.5, epsilon);
  EXPECT_NEAR(dot(v1,v2), 20.6, epsilon);
  EXPECT_NEAR(dot(v2,v1), 20.6, epsilon);
}

TEST(Vector3DTest, BinaryFunctions)
{
  Vector3D  v1 = {1,   2,  3};
  Vector3D  v2 = {+3, -4, +5};

  EXPECT_EQ(min(v1,v2), Vector3D(1,-4,3));
  EXPECT_EQ(min(v1,2.0), Vector3D(1,2,2));
}

TEST(Vector3DTest, Rotate)
{
  Vector2D  v1 = {2, 1};
  Vector2D  v2 = rotate(v1, Angle::deg(90));

  EXPECT_NEAR(v2[0], -1, epsilon);
  EXPECT_NEAR(v2[1], 2, epsilon);
}
