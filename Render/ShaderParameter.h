/*
 *  ShaderParameter.h
 *  Mountainhome
 *
 *  Created by loch on 12/20/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _SHADERPARAMETER_H_
#define _SHADERPARAMETER_H_
#include <Base/Matrix.h>
#include <Base/Vector.h>
#include "Texture.h"

class ShaderParameter;

typedef std::map<std::string, ShaderParameter*> ShaderParameterMap;

enum ShaderParameterType {
    FloatParam,
    IntParam,
    Matrix4Param,
    TextureParam
};

/*! A ShaderParameter represents an abstract, uniform parameter to a Shader. The Shader
 *  class can be given a prameter and the value of the parameter will be associated with
 *  a specific variable in the Shader. Regardless of underlying type, ShaderParameters
 *  always store pointers to their values, allowing their underlying values to update
 *  automatically, without the need to refresh them when changes are made.
 * \note See Renderable for a more complete explanation of how rendering works.
 * \note The data pointer given is NOT cleaned up after.
 * \seealso Renderable */
class ShaderParameter {
public:
    ShaderParameter();
    ~ShaderParameter();

    /*!\param free If true, the data will be freed when appropriate. Note that because
     *  the data is internally stored as a void*, it must be free'd, not deleted. As such
     *  it must also be allocated with a malloc call. */
    template <typename T>
    void setData(T *data, int count = 1, bool free = false);

    template <typename T>
    T * getData() {
        return static_cast<T*>(_data);
    }

    void setData(void *data, ShaderParameterType type, int size, int count, bool free);

    ShaderParameterType getType();
    int getCount();
    int getSize();

private:
    ShaderParameter(const ShaderParameter &other);

    ShaderParameter & operator=(const ShaderParameter &other);

private:
    void *_data;
    ShaderParameterType _type;
    int _size;
    int _count;
    bool _free;

};

template <typename T>
void ShaderParameter::setData(T *data, int count, bool free) {
    THROW(NotImplementedError, "This ShaderParameter case has not been implemented.");
}

#endif
