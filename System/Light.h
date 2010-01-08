/*
 *  Light.h
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _LIGHT_H_
#define _LIGHT_H_
#include <Base/Vector.h>

///\todo Look into making a parent class for everything that needs set/get position/rotation/transformation
///\todo Look into making a parent class for everything that needs set/get ambient/diffuse/specular

class Light {
public:
    Light()                                                { /*!\todo implement me */ }
    virtual ~Light()                                       { /*!\todo implement me */ }
    void setPosition(Real x, Real y, Real z)               { /*!\todo implement me */ }
    void setPosition(const Vector3 &vec)                   { /*!\todo implement me */ }
    void setAmbient(Real r, Real g, Real b, Real a = 1.0f) { /*!\todo implement me */ }
    void setDiffuse(Real r, Real g, Real b, Real a = 1.0f) { /*!\todo implement me */ }
    const Vector3& getPosition()                           { /*!\todo implement me */ return _position; }

private:
    Vector3 _position;
};

#endif
