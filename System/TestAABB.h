/*
 *  TestAABB.h
 *  Base
 *
 *  Created by Brent Wilson on 10/23/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _TESTAABB_H_
#define _TESTAABB_H_
#include "Test.h"

class TestAABB : public Test<TestAABB> {
public:
    TestAABB(): Test<TestAABB>() {}
    static void RunTests();

private:
    static void SmokeTest();
    static void TestRadAndDia();
    static void TestMin();
    static void TestMax();
    static void TestTouching();
    static void TestContains();
    static void TestOverlaps();
    static void TestDistance();
    static void TestNearbyWalls();
    static void TestSweep();
    static void TestSweepLine();
    static void TestSweepComplicated();

};

#endif
