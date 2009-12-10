/*
 *  TestQuaternion.cpp
 *  System
 *
 *  Created by loch on 10/23/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "TestQuaternion.h"
#include "Quaternion.h"
#include "Matrix.h"

void TestQuaternion::RunTests() {
    TestNormalize();
    TestApply();
    TestInvert();
    TestOperators();
    TestNormalize();
    TestEulerConversion();
    TestMatrixConversion();
    TestAxisAngleConversion();
}

void TestQuaternion::TestEulerConversion() {
    Vector3 rot(Math::PI, 1, -.3 * Math::PI);
    Vector3 rhs(1, 1, 1);

    { // Check fromEuler
        Quaternion q(rot);
        Matrix m(q);
        TASSERT_EQ(rot, m.toEuler());
    }

    { // Check toEuler
        Matrix m(rot);
        Quaternion q(m);

        TASSERT_EQ(rot, q.toEuler());    
    }

    { // Check both
        Quaternion q(rot);
        Matrix m(rot);

        TASSERT_EQ(rot, q.toEuler());

        Vector3 result = m * rhs;
        TASSERT_EQ(result, q * rhs);
    }
}

void TestQuaternion::TestMatrixConversion() {
    Matrix m(Math::PI * .432, Vector3(1, 1, 1).getNormalized());
    Quaternion q(m);
    Matrix m2(q);

    TASSERT_EQ(m, m2);
}

void TestQuaternion::TestAxisAngleConversion() {
    Real    derivAngle;
    Vector3 derivAxis;

    Real    origAngle = Math::PI / 1.5;
    Vector3 origAxis(5, 10, 8);
    origAxis.normalize();

    Quaternion q(origAngle, origAxis);
    Matrix m(origAngle, origAxis);
    Vector3 rhs(9, 4.9, 15);

    q.toAngleAxis(derivAngle, derivAxis);

    TASSERT_EQ(m * rhs, q * rhs);
    TASSERT_EQ(origAngle, derivAngle);
    TASSERT_EQ(origAxis,  derivAxis);
}

void TestQuaternion::TestNormalize() {
    Quaternion q(Math::PI * .873, Vector3(3, 4, 5));
    Vector3 rhs(5, 6, 7);

    TASSERT_EQ(q * rhs, q.getNormalized() * rhs);
    TASSERT_EQ(1, q.getNormalized().length());
}

void TestQuaternion::TestInvert() {
    Matrix m(Math::PI * .873, Vector3(3, 4, 5));
    Quaternion q(Math::PI * .873, Vector3(3, 4, 5));

    Vector3 rhs(5, 6, 7);
    Vector3 mid;

    TASSERT_EQ(m * rhs, q * rhs);

    mid = q * rhs;
    TASSERT_EQ(rhs, q.getInverse() * mid);
}

void TestQuaternion::TestApply() {
    Vector3 t1, t2;

    t1 = Vector3(5,0,0);
    Quaternion(Math::PI, 0, 0).apply(t1);
    TASSERT_EQ(t1, Vector3(5,0,0));

    t1 = Vector3(0,1,0);
    Quaternion(Math::PI, 0, 0).apply(t1);
    TASSERT_EQ(t1, Vector3(0,-1,0));

    t1 = Vector3(5,5,0);
    Quaternion(Math::PI / 2.0, 0, 0).apply(t1);
    TASSERT_EQ(t1, Vector3(5,0,5));

    t1 = Vector3(5,5,0);
    Quaternion(Math::PI / 2.0, Vector3(1, 0, 0)).apply(t1);
    TASSERT_EQ(t1, Vector3(5,0,5));    
}

void TestQuaternion::TestOperators() {
    { // * Vector
        Vector3 one(1, 13, 3);
        Vector3 two(one);

        Quaternion q(7, 19, 17);
        q.apply(one);

        TASSERT_EQ(one, q * two);
    }

    { // * Real
        Quaternion q(2, 4, 2, 1);
        TASSERT_EQ(Quaternion(6, 12, 6, 3), q * 3);
    }

    { // *
        Vector3 one(1, 13, 3);
        Vector3 two(one);

        Quaternion q1(7, 19, 17);
        Quaternion q2(2, 4, 2);

        q2.apply(one);
        q1.apply(one);

        TASSERT_EQ(one, q1 * q2 * two);
    }

    { // +
        Quaternion q1(2, 4, 2, 1);
        Quaternion q2(-1, 3, 20, 0);
        TASSERT_EQ(Quaternion(1, 7, 22, 1), q1 + q2);
    }

    { // -
        Quaternion q1(2, 4, 2, 1);
        Quaternion q2(-1, 3, 20, 0);
        TASSERT_EQ(Quaternion(3, 1, -18, 1), q1 - q2);    
    }

    { // *= Real
        Quaternion q(2, 4, 2, 1);
        q *= 3;

        TASSERT_EQ(Quaternion(6, 12, 6, 3), q);
    }

    { // *=
        Vector3 one(1, 13, 3);
        Vector3 two(one);

        Quaternion q1(7, 19, 17);
        Quaternion q2(2, 4, 2);

        q2.apply(one);
        q1.apply(one);

        q1 *= q2;

        TASSERT_EQ(one, q1 * two);
    }

    { // +=
        Quaternion q1(2, 4, 2, 1);
        Quaternion q2(-1, 3, 20, 0);
        q1 += q2;

        TASSERT_EQ(Quaternion(1, 7, 22, 1), q1);
    }

    { // -=
        Quaternion q1(2, 4, 2, 1);
        Quaternion q2(-1, 3, 20, 0);
        q1 -= q2;

        TASSERT_EQ(Quaternion(3, 1, -18, 1), q1);
    }
}
