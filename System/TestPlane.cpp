/*
 *  TestPlane.cpp
 *  System
 *
 *  Created by loch on 10/23/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "TestPlane.h"
#include "Plane.h"

void TestPlane::RunTests() {
    SmokeTest();
    TestDistance();
    TestNormalize();
}

void TestPlane::SmokeTest() {
    Plane a;
    TASSERT(a.normal() == Vector3(0, 1, 0));
    TASSERT(a.distance() == 0);

    Vector3 n(-1, 5, 1);
    Plane b(n, 5);
    TASSERT(b.normal() == n);
    TASSERT(b.distance() == 5);
}

void TestPlane::TestDistance() {
    Plane a;
    TASSERT(a.distanceFrom(Vector3(0, 1, 0)) == 1);
    TASSERT(a.distanceFrom(Vector3(1, 1, 0)) == 1);
    TASSERT(a.distanceFrom(Vector3(0, 1, -129)) == 1);
    TASSERT(a.distanceFrom(Vector3(0, 0, 0)) == 0);

    Plane b(Vector3(1, 0, 0), 5);
    TASSERT(b.distanceFrom(Vector3(0, 0, 0)) == 5);
    TASSERT(b.distanceFrom(Vector3(5, 15, 0)) == 10);
    TASSERT(b.distanceFrom(Vector3(-5, 0, -49)) == 0);

    Plane c(Vector3(-1, 0, 0), 5);
    TASSERT(c.distanceFrom(Vector3(0, 0, 0)) == 5);
    TASSERT(c.distanceFrom(Vector3(-5, 15, 0)) == 10);
    TASSERT(c.distanceFrom(Vector3(5, 0, -49)) == 0);
}

void TestPlane::TestNormalize() {
    Plane a(Vector3(3, 4, 0), 10);
    a.normalize();
    TASSERT(Math::eq(a.normal().lengthSquared(), 1));
    TASSERT(a.distance() == 2);
}
