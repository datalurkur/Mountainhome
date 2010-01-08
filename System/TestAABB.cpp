/*
 *  TestAABB.cpp
 *  Base
 *
 *  Created by Brent Wilson on 10/23/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "TestAABB.h"
#include "AABB.h"

void TestAABB::RunTests() {
    SmokeTest();
    TestRadAndDia();
    TestOffset();
    TestMin();
    TestMax();
    TestContains();
    TestOverlaps();
    TestDistance();
    TestNearbyWalls();
    TestSweep();
    TestSweepLine();
    TestSweepComplicated();
}

void TestAABB::SmokeTest() {
    AABB2 test;
    TASSERT(test.center() == (Vector2(0, 0)));
    TASSERT(test.radius() == (Vector2(0, 0)));

    AABB2 test2(Vector2(10, -10), Vector2(0, 50));
    TASSERT(test2.center() == (Vector2(10, -10)));
    TASSERT(test2.radius() == (Vector2(0, 50)));

    AABB2 test3(Vector2(10, 10), Vector2(-10, 10));
    TASSERT(test3.center() == (Vector2(10, 10)));
    TASSERT(test3.radius() == (Vector2(10, 10)));
}

void TestAABB::TestRadAndDia() {
    AABB2 test;
    TASSERT(test.radius() == (Vector2(0, 0)));
    TASSERT(test.radius(0) == 0);
    TASSERT(test.radius(1) == 0);
    TASSERT(test.diameter(0) == 0);
    TASSERT(test.diameter(1) == 0);

    AABB2 test2(Vector2(10, -10), Vector2(-5, 50));
    TASSERT(test2.radius() == (Vector2(5, 50)));
    TASSERT(test2.radius(0) == 5);
    TASSERT(test2.radius(1) == 50);
    TASSERT(test2.diameter() == (Vector2(10, 100)));
    TASSERT(test2.diameter(0) == 10);
    TASSERT(test2.diameter(1) == 100);
    
}

void TestAABB::TestOffset() {
    AABB2 test;
    test.offsetCenter(Vector2(5, 5));
    TASSERT(test.center() == (Vector2(5,5)));
    test.offsetCenter(Vector2(-5, -5));
    TASSERT(test.center() == (Vector2(0,0)));
}

void TestAABB::TestMin() {
    AABB2 test(Vector2(10, -10), Vector2(5, 50));
    TASSERT(test.min() == (Vector2(5, -60)));
    TASSERT(test.min(0) == 5);
    TASSERT(test.min(1) == -60);
}

void TestAABB::TestMax() {
    AABB2 test(Vector2(10, -10), Vector2(5, 50));
    TASSERT(test.max() == (Vector2(15, 40)));
    TASSERT(test.max(0) == 15);
    TASSERT(test.max(1) == 40);
}

void TestAABB::TestTouching() {
    AABB2 test1(Vector2(0, 0), Vector2(5, 5));
    AABB2 test2(Vector2(10, 10), Vector2(5, 5));
    AABB2 test3(Vector2(1, 1), Vector2(3, 3));
    AABB2 test4(Vector2(0, 0), Vector2(10, 1));
    TASSERT( test1.overlaps(test2));
    TASSERT( test1.overlaps(test3));
    TASSERT( test1.overlaps(test4));
    TASSERT(!test2.overlaps(test3));
    TASSERT(!test2.overlaps(test4));
    TASSERT( test3.overlaps(test4));
}

void TestAABB::TestContains() {
    AABB2 test(Vector2(10, -10), Vector2(5, 50));
    TASSERT(!test.contains(Vector2(5, -60)));
    TASSERT(!test.contains(Vector2(5, 40)));
    TASSERT(!test.contains(Vector2(15, 40)));
    TASSERT(!test.contains(Vector2(15, -60)));
    TASSERT(test.contains(Vector2(10, -10)));
    TASSERT(test.contains(Vector2(8, -20)));
}

void TestAABB::TestOverlaps() {
    AABB2 test1(Vector2(0, 0), Vector2(5, 5));
    AABB2 test2(Vector2(10, 10), Vector2(5, 5));
    AABB2 test3(Vector2(1, 1), Vector2(3, 3));
    AABB2 test4(Vector2(0, 0), Vector2(10, 1));
    TASSERT(!test1.overlaps(test2));
    TASSERT( test1.overlaps(test3));
    TASSERT( test1.overlaps(test4));
    TASSERT(!test2.overlaps(test3));
    TASSERT(!test2.overlaps(test4));
    TASSERT( test3.overlaps(test4));
}

void TestAABB::TestDistance() {
    AABB2 test1(Vector2(0, 0), Vector2(5, 5));
    AABB2 test2(Vector2(10, 10), Vector2(5, 5));
    AABB2 test3(Vector2(10, 10), Vector2(2, 1));
    AABB2 test4(Vector2(10, 0), Vector2(2, 2));

    TASSERT(test1.distanceFrom(test2) == 0);
    TASSERT(test2.distanceFrom(test3) == 0);
    TASSERT(test1.distanceFrom(test3) == 5);
    TASSERT(test1.distanceFrom(test4) == 3);
}

void TestAABB::TestNearbyWalls() {
    AABB2 sRight(Vector2(2, 0), Vector2(0, 1));
    AABB2 lRight(Vector2(2, 0), Vector2(0, 3));
    AABB2 vRight(Vector2(3, 0), Vector2(1, 0));
    AABB2 nRight1(Vector2(2, 3), Vector2(0, 1));
    AABB2 nRight2(Vector2(2, 4), Vector2(0, 1));

    AABB2 sLeft(Vector2(-2, 0), Vector2(0, 1));
    AABB2 lLeft(Vector2(-2, 0), Vector2(0, 3));
    AABB2 vLeft(Vector2(-3, 0), Vector2(1, 0));
    AABB2 nLeft1(Vector2(-2, 3), Vector2(0, 1));
    AABB2 nLeft2(Vector2(-2, 4), Vector2(0, 1));

    AABB2 sTop(Vector2(0, 2), Vector2(1, 0));
    AABB2 lTop(Vector2(0, 2), Vector2(3, 0));
    AABB2 vTop(Vector2(0, 3), Vector2(0, 1));
    AABB2 nTop1(Vector2(3, 2), Vector2(1, 0));
    AABB2 nTop2(Vector2(4, 2), Vector2(1, 0));

    AABB2 sBottom(Vector2(0, -2), Vector2(1, 0));
    AABB2 lBottom(Vector2(0, -2), Vector2(3, 0));
    AABB2 vBottom(Vector2(0, -3), Vector2(0, 1));
    AABB2 nBottom1(Vector2(3, -2), Vector2(1, 0));
    AABB2 nBottom2(Vector2(4, -2), Vector2(1, 0));

    AABB2 center(Vector2(0,0), Vector2(2,2));
    TASSERT(center.nearbyWalls(sRight) == RIGHT_WALL);
    TASSERT(center.nearbyWalls(lRight) == RIGHT_WALL);
    TASSERT(center.nearbyWalls(vRight) == RIGHT_WALL);
    TASSERT(!center.nearbyWalls(nRight1));
    TASSERT(!center.nearbyWalls(nRight2));

    TASSERT(center.nearbyWalls(sLeft) == LEFT_WALL);
    TASSERT(center.nearbyWalls(lLeft) == LEFT_WALL);
    TASSERT(center.nearbyWalls(vLeft) == LEFT_WALL);
    TASSERT(!center.nearbyWalls(nLeft1));
    TASSERT(!center.nearbyWalls(nLeft2));

    TASSERT(center.nearbyWalls(sTop) == CEILING);
    TASSERT(center.nearbyWalls(lTop) == CEILING);
    TASSERT(center.nearbyWalls(vTop) == CEILING);
    TASSERT(!center.nearbyWalls(nTop1));
    TASSERT(!center.nearbyWalls(nTop2));

    TASSERT(center.nearbyWalls(sBottom) == FLOOR);
    TASSERT(center.nearbyWalls(lBottom) == FLOOR);
    TASSERT(center.nearbyWalls(vBottom) == FLOOR);
    TASSERT(!center.nearbyWalls(nBottom1));
    TASSERT(!center.nearbyWalls(nBottom2));

    AABB2 eleven(Vector2(2656.037109f, -2219.243652f), Vector2(13.962769f,64.353516f));
    AABB2 twelve(Vector2(2740.0f,-2250.0f), Vector2(70.0f,0.0f));
    TASSERT(eleven.nearbyWalls(twelve) == RIGHT_WALL);

    AABB2 one(Vector2(1659.371094f, -75.646576f), Vector2(13.962830f,64.353409f));
    AABB2 two(Vector2(1680,-139.999985f), Vector2(10.0f,0.0f));
    TASSERT(one.nearbyWalls(two) == FLOOR);

    AABB2 three(Vector2(2129.325684f, -75.646576f), Vector2(13.962769f,64.353409f));
    AABB2 four(Vector2(2140,-149.999985f), Vector2(0.0f,10.0f));
    TASSERT(three.nearbyWalls(four) == FLOOR);
}

void TestAABB::TestSweep() {
    AABB2 A0(Vector2(0, 5), Vector2(1, 2));
    AABB2 A1(Vector2(0, -5), Vector2(1, 2));
    AABB2 B0(Vector2(5, 0), Vector2(2, 1));
    AABB2 B1(Vector2(-5, 0), Vector2(2, 1));
    AABB2 C0(Vector2(0, 0), Vector2(3, 3));
    AABB2 C1(Vector2(5, 5), Vector2(3, 3));

    Real r0, r1;
    TASSERT(!AABB2::Sweep(A0, A0, B0, B0, r0, r1));
    TASSERT(!AABB2::Sweep(A0, A1, B0, B0, r0, r1));
    TASSERT(!AABB2::Sweep(A0, A0, B0, B1, r0, r1));

    TASSERT(AABB2::Sweep(A0, A1, B0, B1, r0, r1));
    TASSERT(r0 == .2f); TASSERT(r1 == .8f);

    TASSERT(AABB2::Sweep(A1, A0, B1, B0, r0, r1));
    TASSERT(r0 == .2f); TASSERT(r1 == .8f);

    TASSERT(AABB2::Sweep(A0, A1, C0, C1, r0, r1));
    TASSERT(r0 == 0); TASSERT(r1 == (2.0f / 3.0f));

    TASSERT(AABB2::Sweep(B0, B1, C0, C1, r0, r1));
    TASSERT(r0 == 0); TASSERT(r1 == (2.0f / 3.0f));

    AABB2 wall(Vector2(10, 0), Vector2(0, 40));
    AABB2 box(Vector2(0, 0), Vector2(2, 2));
    TASSERT(!AABB2::Sweep(box, Vector2(-10, 0), wall, Vector2(0, 0), r0, r1));
    TASSERT(!AABB2::Sweep(wall, Vector2(0, 0), box, Vector2(-10, 0), r0, r1));

    TASSERT(AABB2::Sweep(box, Vector2(10, 0), wall, Vector2(0, 0), r0, r1));
    TASSERT(r0 == .8f); TASSERT(r1 == 1);

    TASSERT(AABB2::Sweep(wall, Vector2(0, 0), box, Vector2(10, 0), r0, r1));
    TASSERT(r0 == .8f); TASSERT(r1 == 1);
}

void TestAABB::TestSweepLine() {
    AABB2 A0(Vector2(0, 0), Vector2(1, 1));
    AABB2 A1(Vector2(10, 3), Vector2(1, 1));
    AABB2 line(Vector2(5, 0), Vector2(0, 5));

    Real r0, r1;
    TASSERT(AABB2::Sweep(A0, A1, line, line, r0, r1));
    TASSERT(r0 == .4f); TASSERT(r1 == .6f);
}

void TestAABB::TestSweepComplicated() {
    AABB2 one(Vector2(-18.704742f, -145.646576f), Vector2(13.962831f, 64.353409f));
    AABB2 two(Vector2(150.000000f, -89.999985f), Vector2(0, 39.999996f));

    Real r0, r1;
    TASSERT(!AABB2::Sweep(one, Vector2(-0.3, 0), two, Vector2(0,0), r0, r1));
    
    AABB2 three(Vector2(0,2), Vector2(2,2));
    AABB2 four(Vector2(0,0), Vector2(100,0));
    TASSERT(!AABB2::Sweep(three, Vector2(-10, 0), four, Vector2(0,0), r0, r1));

    AABB2 five(Vector2(163.962830,14.353426), Vector2(13.962830,64.353416));
    AABB2 six(Vector2(150,-89.999985), Vector2(0,39.999996));
    TASSERT(!AABB2::Sweep(five, Vector2(-.3, 0), six, Vector2(0,0), r0, r1));

    AABB2 seven(Vector2(163.962814,14.353418), Vector2(13.962814,64.353416));
    AABB2 eight(Vector2(150,-89.999985), Vector2(0,39.999996));
    TASSERT(!AABB2::Sweep(seven, Vector2(-.3, 0), eight, Vector2(0,0), r0, r1));
    
    AABB2 nine(Vector2(-237.437195f, -985.646606f), Vector2(13.962799f,64.353394f));
    AABB2 ten(Vector2(-240.0f,-1105.0f), Vector2(0.0f,55.000004f));
    TASSERT(!AABB2::Sweep(nine, Vector2(0, 10.476000f), ten, Vector2(0,0), r0, r1));
    
    AABB2 eleven(Vector2(0, 0), Vector2(4,10));
    AABB2 twelve(Vector2(10,0), Vector2(0,70));
    AABB2 thirteen(Vector2(10,0), Vector2(2,0));
    AABB2 fourteen(Vector2(14,0), Vector2(2.00001,0));
    TASSERT(AABB2::Sweep(eleven, Vector2(10, 0), twelve, Vector2(0,0), r0, r1));
    TASSERT(AABB2::Sweep(eleven, Vector2(10, 0), thirteen, Vector2(0,0), r0, r1));
    TASSERT(!AABB2::Sweep(thirteen, Vector2(0, 0), fourteen, Vector2(0,0), r0, r1));
}
