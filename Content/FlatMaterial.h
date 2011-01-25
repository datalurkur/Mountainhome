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

class FlatMaterial : public Material {
public:
    FlatMaterial();
    FlatMaterial(const Vector4 &color);
    void setColor(const Vector4 &color);
    const Vector4 & getColor();

private:
    void init();

private:
    Vector4 _color;

};

#endif
