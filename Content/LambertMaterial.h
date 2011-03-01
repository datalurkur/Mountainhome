/*
 *  LambertMaterial.h
 *  Mountainhome
 *
 *  Created by loch on 1/20/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _LAMBERTMATERIAL_H_
#define _LAMBERTMATERIAL_H_
#include <Render/Material.h>

class ShaderManager;

class LambertMaterial : public Material {
public:
    static void Init(ShaderManager *sManager);

public:
    LambertMaterial();
    LambertMaterial(const Vector4 &ambient, const Vector4 &diffuse);

    void setAmbient(const Vector4 &ambient);
    const Vector4 & getAmbient();

    void setDiffuse(const Vector4 &diffuse);
    const Vector4 & getDiffuse();

private:
    void init();

private:
    Vector4 _ambient;
    Vector4 _diffuse;

};

#endif
