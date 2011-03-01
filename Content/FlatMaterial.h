/*
 *  FlatMaterial.h
 *  Mountainhome
 *
 *  Created by loch on 1/20/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _FLATMATERIAL_H_
#define _FLATMATERIAL_H_
#include <Render/Material.h>

class ShaderManager;

class FlatMaterial : public Material {
public:
    static void Init(ShaderManager *sManager);

public:
    FlatMaterial();
    FlatMaterial(const Vector4 &color, Texture *tex = NULL);

    void setColor(const Vector4 &color);
    const Vector4 & getColor();

    void setTexture(Texture *tex);
    const Texture * getTexture();

private:
    void update();

private:
    Texture *_texture;
    Vector4 _color;

};

#endif
