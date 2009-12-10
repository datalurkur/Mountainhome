/*
 *  TestRay.h
 *  System
 *
 *  Created by loch on 10/23/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _TESTRAY_H_
#define _TESTRAY_H_
#include "Test.h"

class TestRay : public Test<TestRay> {
public:
    TestRay(): Test<TestRay>() {}
    static void RunTests();

private:

};

#endif
