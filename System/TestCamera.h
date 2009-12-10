/*
 *  TestCamera.h
 *  System
 *
 *  Created by loch on 11/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _TESTCAMERA_H_
#define _TESTCAMERA_H_
#include "Test.h"

class TempListener;
class TestCamera : public Test<TestCamera> {
public:
    TestCamera(): Test<TestCamera>() {}
    static void RunTests();

private:
    static void TestMove();
    static void TestYawPitchRoll();
};

#endif
