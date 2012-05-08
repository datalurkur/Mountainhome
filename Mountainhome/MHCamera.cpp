//
//  MHCamera.cpp
//  Mountainhome
//
//  Created by Brent Wilson on 5/7/12.
//  Copyright (c) 2012 Mountainhome Project. All rights reserved.
//

#include <iostream>
#include <Base/Matrix.h>
#include "MHCamera.h"
#include "MHWorld.h"

MHCamera::MHCamera(
    std::string name,
    float maxViewDistance,
    MHWorld *world
):
    Camera(name),
    _viewDistance(maxViewDistance),
    _world(world)
{}


void MHCamera::makeCenterOrtho(float x, float y, float width, float height) {
    setProjectionMatrix(Matrix::CenterOrtho(width, height, Vector2(x, y), 1.0, _viewDistance));
}

void MHCamera::makePerspective(float ratio, Degree fov) {
    setProjectionMatrix(Matrix::Perspective(ratio, Radian(fov), 1.0f, _viewDistance));
}

MHWorld *MHCamera::getWorld() {
    return _world;
}
