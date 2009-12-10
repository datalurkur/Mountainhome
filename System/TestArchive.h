/*
 *  TestArchive.h
 *  System
 *
 *  Created by loch on 11/30/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _TESTARCHIVE_H_
#define _TESTARCHIVE_H_
#include "Test.h"

class TestArchive : public Test<TestArchive> {
public:
    TestArchive(): Test<TestArchive>() {}
    static void RunTests();

private:
    static void TestFromDisk();
    static void TestFromData();

};

#endif
