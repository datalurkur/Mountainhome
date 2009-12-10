/*
 *  TestQuaternion.h
 *  System
 *
 *  Created by loch on 10/23/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _TESTQUATERNION_H_
#define _TESTQUATERNION_H_
#include "Test.h"

class TestQuaternion : public Test<TestQuaternion> {
public:
    TestQuaternion(): Test<TestQuaternion>() {}
    static void RunTests();

private:
    static void TestNormalize();
    static void TestInvert();
    static void TestApply();
    static void TestOperators();
    static void TestEulerConversion();
    static void TestMatrixConversion();
    static void TestAxisAngleConversion();

};

#endif
