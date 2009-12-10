/*
 *  TestCamera.cpp
 *  System
 *
 *  Created by loch on 11/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "TestCamera.h"
#include "Camera.h"

void TestCamera::RunTests() {
    TestMove();
    TestYawPitchRoll();
}

void TestCamera::TestMove() {
    Camera test;
    test.strafeLeft(1);
    assert(test._position == Vector3(-1, 0, 0));
    assert(test._up == Vector3(0, 1, 0));
    assert(test._lookAt == Vector3(0, 0, -1));
    
    test.strafeRight(2);
    assert(test._position == Vector3(1, 0, 0));
    assert(test._up == Vector3(0, 1, 0));
    assert(test._lookAt == Vector3(0, 0, -1));

    test.strafeLeft(1);
    assert(test._position == Vector3(0, 0, 0));
    assert(test._up == Vector3(0, 1, 0));
    assert(test._lookAt == Vector3(0, 0, -1));

    test.moveForward(1);
    assert(test._position == Vector3(0, 0, -1));
    assert(test._up == Vector3(0, 1, 0));
    assert(test._lookAt == Vector3(0, 0, -1));

    test.moveBackward(2);
    assert(test._position == Vector3(0, 0, 1));
    assert(test._up == Vector3(0, 1, 0));
    assert(test._lookAt == Vector3(0, 0, -1));
}

void TestCamera::TestYawPitchRoll() {
    Camera test;
    test.adjustRoll(-90);
    assert(test._position == Vector3(0, 0, 0));
    assert(test._up == Vector3(1, 0, 0));
    assert(test._lookAt == Vector3(0, 0, -1));

    test.adjustRoll(180);
    assert(test._position == Vector3(0, 0, 0));
    assert(test._up == Vector3(-1, 0, 0));
    assert(test._lookAt == Vector3(0, 0, -1));

    test.adjustRoll(-90);
    assert(test._position == Vector3(0, 0, 0));
    assert(test._up == Vector3(0, 1, 0));
    assert(test._lookAt == Vector3(0, 0, -1));

    test.adjustPitch(-90);
    assert(test._position == Vector3(0, 0, 0));
    assert(test._up == Vector3(0, 0, -1));
    assert(test._lookAt == Vector3(0, -1, 0));

    test.adjustPitch(180);
    assert(test._position == Vector3(0, 0, 0));
    assert(test._up == Vector3(0, 0, 1));
    assert(test._lookAt == Vector3(0, 1, 0));

    test.adjustPitch(-90);
    assert(test._position == Vector3(0, 0, 0));
    assert(test._up == Vector3(0, 1, 0));
    assert(test._lookAt == Vector3(0, 0, -1));

    test.adjustYaw(-90);
    assert(test._position == Vector3(0, 0, 0));
    assert(test._up == Vector3(0, 1, 0));
    assert(test._lookAt == Vector3(1, 0, 0));

    test.adjustYaw(180);
    assert(test._position == Vector3(0, 0, 0));
    assert(test._up == Vector3(0, 1, 0));
    assert(test._lookAt == Vector3(-1, 0, 0));
}
