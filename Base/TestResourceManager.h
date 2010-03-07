/*
 *  TestResourceManager.h
 *  Base
 *
 *  Created by Brent Wilson on 10/23/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _TESTRESOURCEMANAGER_H_
#define _TESTRESOURCEMANAGER_H_
#include "Test.h"

class TestResourceManager : public Test<TestResourceManager> {
public:
    TestResourceManager(): Test<TestResourceManager>() {}
    static void RunTests();

private:
    typedef int Resource;
    static void TestResourceAddition();
    static void TestResourceRetrieval();

};

#endif
