/*
 *  TestFile.h
 *  Base
 *
 *  Created by Brent Wilson on 10/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _TESTFILE_H_
#define _TESTFILE_H_
#include "Test.h"

class TestFile : public Test<TestFile> {
public:
    TestFile(): Test<TestFile>() {}
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
