/*
 *  TestRay.h
 *  Base
 *
 *  Created by Brent Wilson on 10/23/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
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
