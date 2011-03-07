/*
 *  BasicMaterial.h
 *  Mountainhome
 *
 *  Created by loch on 3/2/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _BASICMATERIAL_H_
#define _BASICMATERIAL_H_
#include <Render/Material.h>

class ShaderManager;

class BasicMaterial : public Material {
public:
    static void Init(ShaderManager *sManager);

public:
    BasicMaterial();
    BasicMaterial(const Vector4 &color, Texture *tex = NULL);
    BasicMaterial(const Vector4 &ambient, const Vector4 &diffuse, Texture *tex = NULL);

    void setAmbient(const Vector4 &ambient);
    const Vector4 & getAmbient() const;

    void setDiffuse(const Vector4 &diffuse);
    const Vector4 & getDiffuse() const;

    void setTexture(Texture *tex);
    const Texture * getTexture() const;

    void setLightingEnabled(bool enable);
    bool getLightingEnabled() const;

private:
    void updateTechnique();

private:
    Vector4 _ambient;
    Vector4 _diffuse;
    Texture *_texture;
    bool _lighting;

};

#endif
