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
    Light();
    virtual ~Light();
    void makePositionalLight(Real x, Real y, Real z);
    void makePositionalLight(const Vector3 &vec);

    void makeDirectionalLight(Real x, Real y, Real z);
    void makeDirectionalLight(const Vector3 &vec);

    void setAmbient(Real r, Real g, Real b, Real a = 1.0f);
    void setDiffuse(Real r, Real g, Real b, Real a = 1.0f);
	void setSpecular(Real r, Real g, Real b, Real a = 1.0f);

    void setupState(int lightIndex);

	void enable();
	void disable();

private:
    void setPosition(Real x, Real y, Real z, Real w);

private:
    bool _enabled;
    Vector4 _position;
	Vector4 _ambient;
	Vector4 _diffuse;
	Vector4 _specular;
};

#endif
