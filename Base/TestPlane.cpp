/*
 *  TestPlane.cpp
 *  Base
 *
 *  Created by Brent Wilson on 10/23/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
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
    TASSERT_EQ(a.getNormal(), Vector3(0, 1, 0));
    TASSERT_EQ(a.getDistance(), 0);

    Vector3 n(-1, 5, 1);
    Plane b(n, 5);
    TASSERT_EQ(b.getNormal(), n);
    TASSERT_EQ(b.getDistance(), 5);
}

void TestPlane::TestDistance() {
    Plane a;
    TASSERT_EQ(a.distanceFrom(Vector3(0, 1, 0)), 1);
    TASSERT_EQ(a.distanceFrom(Vector3(1, 1, 0)), 1);
    TASSERT_EQ(a.distanceFrom(Vector3(0, 1, -129)), 1);
    TASSERT_EQ(a.distanceFrom(Vector3(0, 0, 0)), 0);

    Plane b(Vector3(1, 0, 0), 5);
    TASSERT_EQ(b.distanceFrom(Vector3(0, 0, 0)), -5);
    TASSERT_EQ(b.distanceFrom(Vector3(5, 15, 0)), 0);
    TASSERT_EQ(b.distanceFrom(Vector3(-5, 0, -49)), -10);

    Plane c(Vector3(-1, 0, 0), 5);
    TASSERT_EQ(c.distanceFrom(Vector3(0, 0, 0)), -5);
    TASSERT_EQ(c.distanceFrom(Vector3(-5, 15, 0)), 0);
    TASSERT_EQ(c.distanceFrom(Vector3(5, 0, -49)), -10);
}

void TestPlane::TestNormalize() {
    Plane a(Vector3(3, 4, 0), 10);
    a.normalize();
    TASSERT_EQ(a.getNormal().lengthSquared(), 1);
    TASSERT_EQ(a.getDistance(), 2);
}
