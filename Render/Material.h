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

#include "RenderParameterContainer.h"

///\todo Look into making a parent class for everything that needs set/get ambient/diffuse/specular
class Shader;
class ShaderManager;
class TextureManager;

/*! Typically, there are many rendered objects that will have certain ShaderParameters in
 *  common, and there will often be many sets of these ShaderParameter groups for any
 *  single Shader. This class allows for a set of ShaderParameters to be packaged with the
 *  Shader they're associated with.
 *
 *  For example, you may have a shader, which does a simple per-pixel lighting algorithm
 *  and a Brick Wall Material, which specifies a texture and certain lighting parameters.
 *
 *  Materials are typically loaded from .material files. These files have several keywords:
 *
 *    type - Specifies the type of the material. May be one of: flat, lambert, generic.
 *           Defaults to generic if nothing is specified.
 *    shader - The name of the shader to load up. This is only valid for the generic type.
 *    transparent - Whether or not transparency should be enabled.
 *    depthtest - Whether or not the depth test should be applied.
 *    wireframe - Whether or not the object should be drawn as a wireframe.
 *
 *  For the generic material type, any non-keyword key is assumed to contain a generic
 *  ShaderParameter. Since there is no default type for a ShaderParameter, the type must
 *  be specified in the value, as so:
 *
 *  <type> <value>
 *
 *  where type may be either 'int', 'float', or 'texture', at the moment. Array types are
 *  NOT currently supported. int and float types may be multi value vectors, up to a size
 *  of 4. Here are some concrete examples:
 *
 *  float 1.5 2
 *  int 10
 *  float 10
 *  float 10.5
 *  int 1 2 3 4
 *  texture black.png
 *
 * \note See Renderable for a more complete explanation of how rendering works. */
class Material : public RenderParameterContainer {
public:
    Material();

    virtual ~Material();

    void enable();

    void disable();

    void setShader(Shader *shader);

    Shader * getShader();

private:
    Shader *_shader;

};

#endif
