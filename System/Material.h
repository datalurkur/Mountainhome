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
#include <Base/Vector.h>
#include <Render/Texture.h>

///\todo Look into making a parent class for everything that needs set/get ambient/diffuse/specular

class Material {
public:
    Material(const std::string &name);
    virtual ~Material()                                    { /*!\todo implement me */ }

	void setColor(Real r, Real g, Real b, Real a);
    void setAmbient(Real r, Real g, Real b, Real a = 1.0f);
    void setDiffuse(Real r, Real g, Real b, Real a = 1.0f);
	void setTexture(Texture *t);
	
	Texture *getTexture() const;
	Vector4 *getColor() const;
	Vector4 *getAmbient() const;
	Vector4 *getDiffuse() const;
	
private:
	Vector4 _color;
	Vector4 _ambient;
	Vector4 _diffuse;
	Texture *_texture;
};

#endif
