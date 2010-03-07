/*
 *  TestVector3.h
 *  Base
 *
 *  Created by Brent Wilson on 10/23/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _TESTVECTOR3_H_
#define _TESTVECTOR3_H_
#include "Test.h"

class TestVector3 : public Test<TestVector3> {
public:
    TestVector3(): Test<TestVector3>() {}
    static void RunTests();

private:
    static void TestDot();
    static void TestCross();
    static void TestAngle();
    static void TestReflect();
    static void TestDirection();
    static void TestOperators();
    static void TestInterpolate();
    static void TestLength();
    static void TestNormalize();
    static void TestDistance();
    static void TestMidpoint();

};

#endif
