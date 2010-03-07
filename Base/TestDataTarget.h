/*
 *  TestDataTarget.h
 *  Base
 *
 *  Created by Brent Wilson on 11/30/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _TESTDATATARGET_H_
#define _TESTDATATARGET_H_
#include "Test.h"

class TestDataTarget : public Test<TestDataTarget> {
public:
    TestDataTarget(): Test<TestDataTarget>() {}
    static void RunTests();

private:
    static void TestConstructor();
    static void TestBasics();
    static void TestStreamPosition();
    static void TestPeekGetc();
    static void TestRead();
    static void TestWrite();

};


#endif
