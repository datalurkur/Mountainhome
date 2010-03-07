/*
 *  TestSocketTCP.h
 *  Base
 *
 *  Created by Brent Wilson on 10/23/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _TESTSOCKETTCP_H_
#define _TESTSOCKETTCP_H_
#include "Test.h"

class TestSocketTCP : public Test<TestSocketTCP> {
public:
    TestSocketTCP(): Test<TestSocketTCP>() {}
    static void RunTests();

private:
    static void TestBasics();
    static void TestPeekGetc();
    static void TestRead();

};

#endif
