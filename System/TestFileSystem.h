/*
 *  TestFileSystem.h
 *  Base
 *
 *  Created by Brent Wilson on 10/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _TESTFILESYSTEM_H_
#define _TESTFILESYSTEM_H_
#include "Test.h"

class TestFileSystem : public Test<TestFileSystem> {
public:
    TestFileSystem(): Test<TestFileSystem>() {}
    static void RunTests();

private:
    static void TestIsDirectory();
    static void TestExtractDir();
    static void TestExtractExtension();
    static void TestExtractFilename();
    static void TestHasLeadingDecorations();
    static void TestAdjustCurrentDir();
    static void TestFormatPath();
    static void TestExists();
    static void TestLength();
    static void TestTouchAndDelete();
    static void TestListing();

};

#endif
