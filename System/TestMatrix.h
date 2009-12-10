/*
 *  TestMatrix.h
 *  System
 *
 *  Created by loch on 10/23/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _TESTMATRIX_H_
#define _TESTMATRIX_H_
#include "Test.h"

class TestMatrix : public Test<TestMatrix> {
public:
    TestMatrix(): Test<TestMatrix>() {}
    static void RunTests();

private:
    static void TestRotation();
    static void TestTranslation();
    static void TestScaling();
    
    static void TestCombination();
    static void TestComplex();
    static void TestFindMatrix();

    static void TestEulerConversions();
    static void TestAxisAngleConversions();
    static void TestQuaternionConversions();

};

#endif
