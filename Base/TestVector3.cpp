/*
 *  TestVector3.cpp
 *  Base
 *
 *  Created by Brent Wilson on 10/23/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "TestVector3.h"
#include "Vector.h"

void TestVector3::RunTests() {
    TestOperators();
    TestDot();
    TestCross();
    TestAngle();
    TestReflect();
    TestInterpolate();
    TestDirection();
    TestLength();
    TestNormalize();
    TestDistance();
    TestMidpoint();
}

void TestVector3::TestMidpoint() {
    { // Points are equal.
        Vector3 one(0, 0, 0);
        Vector3 two(0, 0, 0);
        TASSERT_EQ(Vector3(0, 0, 0), one.getMidpoint(two));
    }

    { // Simple example on the y axis.
        Vector3 one(0, 0, 0);
        Vector3 two(0, 2, 0);
        TASSERT_EQ(Vector3(0, 1, 0), one.getMidpoint(two));
    }

    { // Points are same side of zero.
        Vector3 one(50, 100, 0);
        Vector3 two(0, 50, 100);
        TASSERT_EQ(Vector3(25, 75, 50), one.getMidpoint(two));
    }

    { // Points are opposite sides of zero.
        Vector3 one(50, 100, 0);
        Vector3 two(0, -50, -100);
        TASSERT_EQ(Vector3(25, 25, -50), one.getMidpoint(two));
    }
}

void TestVector3::TestDirection() {
    Vector3 a(0,1,0);
    Vector3 b(0,2,0);
    TASSERT(a.hasSameDirectionAs(b));
}

void TestVector3::TestDot() {
    Vector3 a(0, 1, 0);
    Vector3 b(0, 0, 1);
    Real d = a.dotProduct(b);
    ASSERT_EQ(d, 0);
    ASSERT_EQ(d, a.length() * b.length() * Math::Cos(Math::PI / 2.0f));
    
    a = Vector3(0.0f, 1.0f, 0.0f);
    b = Vector3(0.0f, 1.0f, 1.0f);
    d = a.dotProduct(b);
    ASSERT_EQ(d, 1);
    ASSERT_EQ(d, a.length() * b.length() * Math::Cos(Math::PI / 4.0f));

    a = Vector3(-1.5f, 1.0f, 0.5f);
    b = Vector3(2.0f, -1.0f, 1.0f);
    d = a.dotProduct(b);
    ASSERT_EQ(d, -3.5);
}

void TestVector3::TestCross() {
    Vector3 a(1,0,0);
    Vector3 b(0,1,0);
    a.crossProduct(b);
    TASSERT_EQ(a.crossProduct(b), Vector3(0,0,1));
    TASSERT_EQ(b.crossProduct(a), Vector3(0,0,-1));
}

void TestVector3::TestAngle() {
    Vector3 a(0,1,0);
    Vector3 b(0,0,1);
    TASSERT_EQ(a.radiansBetween(b), Math::PI / 2.0f);
    TASSERT_EQ(a.degreesBetween(b), 90);
    
    Vector3 c(0,1,0);
    Vector3 d(0,1,1);
    TASSERT_EQ(c.radiansBetween(d), Math::PI / 4.0f);
    TASSERT_EQ(c.degreesBetween(d), 45);

    Vector3 e(0,0,0);
    Vector3 f(0,0,0);
    TASSERT_EQ(e.radiansBetween(f), 0);
    TASSERT_EQ(e.degreesBetween(f), 0);

    Vector3 g(0,1,0);
    Vector3 h(0,1,0);
    TASSERT_EQ(g.radiansBetween(h), 0);
    TASSERT_EQ(g.degreesBetween(h), 0);
}

void TestVector3::TestReflect() {
    { // Y normal
        Vector3 one(1, 1, 0);
        Vector3 normal(0, 1, 0);
        TASSERT_EQ(Vector3(1, -1, 0), one.getReflected(normal));
    }

    { // X normal
        Vector3 one(1, 1, 0);
        Vector3 normal(1, 0, 0);
        TASSERT_EQ(Vector3(-1, 1, 0), one.getReflected(normal));    
    }

    { // Z normal
        Vector3 one(1, 0, 1);
        Vector3 normal(0, 0, 1);
        TASSERT_EQ(Vector3(1, 0, -1), one.getReflected(normal));    
    }

    { // Non-basic normals
        Vector3 one(1, 0, 0);
        Vector3 normal(1, 1, 0);
        normal.normalize(); // This is a precondition to the function.
        TASSERT_EQ(one.getReflected(normal), Vector3(0, -1, 0));
    }
}

void TestVector3::TestInterpolate() {
    Vector3 a(10,10,10);
    Vector3 b(20,0,10);
    Vector3 c(234,35,29043);
    
    c = a.getLerp(b, .25);
    TASSERT_EQ(c, Vector3(12.5,7.5,10));
    
    c = a.getLerp(b, .75);
    TASSERT_EQ(c, Vector3(17.5,2.5,10));
    
    c = a.getLerp(b, .5);
    TASSERT_EQ(c, Vector3(15,5,10));
    
    c = a.getLerp(b, 0);
    TASSERT_EQ(c, Vector3(10,10,10));
    
    c = a.getLerp(b, 1);
    TASSERT_EQ(c, Vector3(20,0,10));
}

void TestVector3::TestOperators() {
    Vector3 test1, test2, test3, test4;
    
    {    //Test ==
        test1 = Vector3(1.0f, 0.0f, -1.0f);
        test2 = Vector3(1.0f, 0.0f, -1.0f);
        test3 = Vector3(1.00001f, -0.0f, -1.00001f);
        TASSERT(test1 == test2);
        TASSERT(test1 == test3);
        TASSERT(test2 == test3);
    }

    {    //Test !=
        test1 = Vector3(1.0f, 0.0f, -1.0f);
        test3 = Vector3(1.0f, 1.0f, -1.0f);
        TASSERT(test1 != test3);
        TASSERT(test1 != Vector3());
        TASSERT(test3 != Vector3());
    }

    {    //Test =
        test1 = Vector3(1.0f, 0.0f, -1.0f);
        test2 = Vector3(1.0f, 1.0f, -1.0f);
        test2 = test1;
        TASSERT_EQ(test1, test2);
    }
    
    {    //Test []
        test1 = Vector3(1.0f, 2.0f, 1.0f);
        TASSERT_EQ(test1[0], 1.0f);
        TASSERT_EQ(test1[1], 2.0f);
        TASSERT_EQ(test1[2], 1.0f);
        
        test1[0] = -2.0f;
        test1[1] = -1.0f;
        test1[2] = -2.0f;
        TASSERT_EQ(test1, Vector3(-2.0f, -1.0f, -2.0f));
        TASSERT_EQ(test1[0], -2.0f);
        TASSERT_EQ(test1[1], -1.0f);
        TASSERT_EQ(test1[2], -2.0f);
        
    }
    
    {    //Test /
        test1 = Vector3(1.0f, 0.0f, -200.0f);
        test2 = test1 / 2.0f;
        TASSERT_EQ(test2, Vector3(.5f, 0.0f, -100.0f));
    }
    
    {    //Test *
        test1 = Vector3(1.0f, 0.0f, -.5f);
        test2 = test1 * 2.0f;
        TASSERT_EQ(test2, Vector3(2.0f, 0.0f, -1.0f));
    }
    
    {    //Test -
        test1 = Vector3(1.0f, 0.0f, .5f);
        test2 = Vector3(0.0f, .1f, -10.0f);
        test3 = test1 - test2;
        test4 = test2 - test1;
        TASSERT_EQ(test3, Vector3(1.0f, -.1f, 10.5f));
        TASSERT_EQ(test4, Vector3(-1.0f, .1f, -10.5f));
    }
    
    {    //Test +
        test1 = Vector3(1.0f, 0.0f, -.5f);
        test2 = Vector3(0.0f, .1f, -10.0f);
        test3 = test1 + test2;
        test4 = test2 + test1;
        TASSERT_EQ(test3, Vector3(1.0f, .1f, -10.5f));
        TASSERT_EQ(test4, Vector3(1.0f, .1f, -10.5f));
    }
    
    {    //Test - Real
        test1 = Vector3(1.0f, 0.0f, .5f);
        test2 = test1 - .5f;
        TASSERT_EQ(test2, Vector3(.5f, -.5f, 0.0f));
    }
    
    {    //Test + Real
        test1 = Vector3(1.0f, 0.0f, .5f);
        test2 = test1 + .5f;
        TASSERT_EQ(test2, Vector3(1.5f, .5f, 1.0f));
    }
    
    {    //Test /=
        test1 = Vector3(1.0f, 0.0f, -200.0f);
        test1 /= 2;
        TASSERT_EQ(test1, Vector3(.5f, 0.0f, -100.0f));
    }
    
    {    //Test *=
        test1 = Vector3(1.0f, 0.0f, -.5f);
        test1 *= 2;
        TASSERT_EQ(test1, Vector3(2.0f, 0.0f, -1.0f));
    }
    
    {    //Test -=
        test1 = Vector3(1.0f, 0.0f, .5f);
        test2 = Vector3(0.0f, .1f, -10.0f);
        test3 = test2;
        test4 = test1;
        test1 -= test2;
        test3 -= test4;
        TASSERT_EQ(test1, Vector3(1.0f, -.1f, 10.5f));
        TASSERT_EQ(test3, Vector3(-1.0f, .1f, -10.5f));
    }
    
    {    //Test +=
        test1 = Vector3(1.0f, 0.0f, -.5f);
        test2 = Vector3(0.0f, .1f, -10.0f);
        test3 = test2;
        test4 = test1;
        test1 += test2;
        test3 += test4;
        TASSERT_EQ(test1, Vector3(1.0f, .1f, -10.5f));
        TASSERT_EQ(test3, Vector3(1.0f, .1f, -10.5f));
    }
    
    {    //Test -= Real
        test1 = Vector3(1.0f, 0.0f, .5f);
        test1 -= .5f;
        TASSERT_EQ(test1, Vector3(.5f, -.5f, 0.0f));
    }
    
    {    //Test +=
        test1 = Vector3(1.0f, 0.0f, .5f);
        test1 += .5f;
        TASSERT_EQ(test1, Vector3(1.5, .5, 1));
    }
}

void TestVector3::TestLength() {
    { //
        Vector3 x(1, 0, 0);
        Vector3 y(0, 1, 0);
        Vector3 z(0, 0, 1);

        TASSERT_EQ(1, x.length());
        TASSERT_EQ(1, x.lengthSquared());

        TASSERT_EQ(1, y.length());
        TASSERT_EQ(1, y.lengthSquared());

        TASSERT_EQ(1, z.length());
        TASSERT_EQ(1, z.lengthSquared());
    }

    { // Large vector
        Vector3 one(10, 15, 20);
        TASSERT_EQ(725, one.lengthSquared());
        TASSERT_EQ(26.925824, one.length());
        
    }

    { // Small Vector
        Vector3 one(.1, .15, .2);
        TASSERT_EQ(.0725, one.lengthSquared());
        TASSERT_EQ(.26925824, one.length());
    }
}

void TestVector3::TestNormalize() {
    { // Test a prenormalized x vector.
        Vector3 x(1, 0, 0);
        x.normalize();
        TASSERT_EQ(Vector3(1, 0, 0), x);
    }

    { // Test a prenormalized y vector.
        Vector3 y(0, 1, 0);
        y.normalize();
        TASSERT_EQ(Vector3(0, 1, 0), y);
    }

    { // Test a prenormalized z vector.
        Vector3 z(0, 0, 1);
        z.normalize();
        TASSERT_EQ(Vector3(0, 0, 1), z);
    }

    { // Test some random large unnormalized values.
        Vector3 one(10, 15, 20);
        one.normalize();
        TASSERT_EQ(1, one.length());
        TASSERT_EQ(one, Vector3(.371391, .557086, .742781));

        Vector3 two(-12.34, 1.123, 8.4);
        two.normalize();
        TASSERT_EQ(1, two.length());
        TASSERT_EQ(two, Vector3(-.824323, .075017, .561127));

        Vector3 three(-10, 15, -20);
        three.normalize();
        TASSERT_EQ(1, three.length());
        TASSERT_EQ(three, Vector3(-.371391, .557086, -.742781));
    }

    { // Test some random small unnormalized values.
        Vector3 one(.1, .15, .2);
        one.normalize();
        TASSERT_EQ(1, one.length());
        TASSERT_EQ(one, Vector3(.371391, .557086, .742781));

        Vector3 two(-.1234, .01123, .084);
        two.normalize();
        TASSERT_EQ(1, two.length());
        TASSERT_EQ(two, Vector3(-.824323, .075017, .561127));

        Vector3 three(-.1, .15, -.2);
        three.normalize();
        TASSERT_EQ(1, three.length());
        TASSERT_EQ(three, Vector3(-.371391, .557086, -.742781));
    }

    { // Test some random normalized values.
        Vector3 one(.371391, .557086, .742781);
        one.normalize();
        TASSERT_EQ(1, one.length());
        TASSERT_EQ(one, Vector3(.371391, .557086, .742781));

        Vector3 two(-.824323, .075017, .561127);
        two.normalize();
        TASSERT_EQ(1, two.length());
        TASSERT_EQ(two, Vector3(-.824323, .075017, .561127));

        Vector3 three(-.371391, .557086, -.742781);
        three.normalize();
        TASSERT_EQ(1, three.length());
        TASSERT_EQ(three, Vector3(-.371391, .557086, -.742781));
    }
}

void TestVector3::TestDistance() {
    { // Basic test
        Vector3 one(1, 0, 0);
        Vector3 two(-1, 0, 0);
        TASSERT_EQ(4, one.distanceToSquared(two));
        TASSERT_EQ(2, one.distanceTo(two));
    }

    { // Triangle
        Vector3 one(3, 0, 0);
        Vector3 two(0, 4, 0);
        TASSERT_EQ(25, one.distanceToSquared(two));
        TASSERT_EQ(5,  one.distanceTo(two));
    }    
}

