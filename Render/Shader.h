/*
 *  Shader.h
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _SHADER_H
#define _SHADER_H
#include "ShaderParameter.h"
#include "VertexArray.h"
#include <string>

/*! This is a system memory representation of a Shader on the local graphics hardware. It
 *  provides an interface for binding uniform variables and attributes to the shader, as
 *  well as for enabling it for rendering. Once a uniform (see ShaderParameter) or an
 *  attribute (see VertexBuffer and VertexArray) have been set, it stays set. However, a
 *  single Shader will likely be shared by MANY visible objects in a scene, each with
 *  their own set of ShaderParameters. To aid with this, Shaders may be associated with
 *  Materials, which allow sets of parameters to be shared by many things. Materials may
 *  then be associated with Renderables, which represent single objects in the scene,
 *  which have their own set of ShaderParameters that may be adjusted.
 * \note See Renderable for a more complete explanation of how rendering works.
 * \seealso Material
 * \seealso ShaderParameter
 * \seealso VertexBuffer
 * \seealso VertexArray
 * \seealso Renderable */
class Shader {
public:
    Shader();
    virtual ~Shader();

    /*! Enables the Shader for rendering. */
    virtual void enable() = 0;

    /*! Disables the Shader for rendering. */
    virtual void disable() = 0;

    /*! Binds the named attribute to the given attribute channel index. These channel
     *  indexes match up to enabled AttributeBuffers. VertexArrays bind AttributeBuffers
     *  one after the next when enabled.
     *  
     *  \todo I definitely want a better way to do this sort of thing. Ideally, the
     *  matching between actual buffers, channel indexes, and attributes would be done
     *  automatically. I'm going to leave it manually controlled for now. I can revisit
     *  this once I have a better idea of how all of this will fit together. */
    virtual void bindAttributeToChannel(const std::string &name, int channel) = 0;

    /*! Binds the given names to channels based on their index.
     * \seealso VertexArray::getAttributeBufferNames */
    void bindAttributesToChannel(const VertexArrayLayout &names);

    /*! Sets a single parameter for the named uniform in the shader.
     * \seealso ShaderParameter */
    virtual void setParameter(const std::string &name, ShaderParameter *param) = 0;

    /*! Sets each parameter in the map. The key is the name of the Shader uniform to set.
     *  The value is the ShaderParameter that contains the value to set the uniform to.
     * \seealso ShaderParameter */
    void setParameters(const ShaderParameterMap &params);

};

#endif
