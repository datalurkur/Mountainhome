/*
 *  TestPlane.h
 *  System
 *
 *  Created by loch on 10/23/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _TESTPLANE_H_
#define _TESTPLANE_H_
#include "Test.h"

class TestPlane : public Test<TestPlane> {
public:
    TestPlane(): Test<TestPlane>() {}
    static void RunTests();

private:
    static void SmokeTest();
    static void TestDistance();
    static void TestNormalize();

};

#endif
