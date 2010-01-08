/*
 *  Material.h
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _MATERIAL_H_
#define _MATERIAL_H_
#include <Base/Math3D.h>

///\todo Look into making a parent class for everything that needs set/get ambient/diffuse/specular

class Material {
public:
    Material(const std::string &name)                      { /*!\todo implement me */ }
    virtual ~Material()                                    { /*!\todo implement me */ }

    void setAmbient(Real r, Real g, Real b, Real a = 1.0f) { /*!\todo implement me */ }
    void setDiffuse(Real r, Real g, Real b, Real a = 1.0f) { /*!\todo implement me */ }
};

#endif
