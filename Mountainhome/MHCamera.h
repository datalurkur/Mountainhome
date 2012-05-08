//
//  MHCamera.h
//  Mountainhome
//
//  Created by Brent Wilson on 5/7/12.
//  Copyright (c) 2012 Mountainhome Project. All rights reserved.
//

#ifndef Mountainhome_MHCamera_h
#define Mountainhome_MHCamera_h
#include <Engine/Camera.h>
#include <Base/Degree.h>

class MHWorld;

class MHCamera : public Camera {
public:
    MHCamera(std::string name, float maxViewDistance, MHWorld *world);

    void makeCenterOrtho(float x, float y, float width, float height);

    void makePerspective(float ratio, Degree fov);

    MHWorld *getWorld();

private:
    float _viewDistance;
    MHWorld *_world;

};

#endif
