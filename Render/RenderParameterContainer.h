/*
 *  RenderParameterContainer.h
 *  Mountainhome
 *
 *  Created by loch on 12/29/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _RENDERPARAMETERCONTAINER_H_
#define _RENDERPARAMETERCONTAINER_H_
#include "ShaderParameter.h"
#include "Render.h"

class Shader;

/*! There are multiple levels at which certain rendering parameters can be set, ranging
 *  from very broad levels (Material) to very specific levels (Renderable). This class
 *  provides the ability to set these parameters (both ShaderParameters and certain,
 *  generic RenderContext settings) and to do so in a stack based manner, pushing on new
 *  parameters and popping off old ones, restoring the previously set values. Not all
 *  values need be set, either, allowing previous set values to remain if the top of stack
 *  does not explicitely set them. This class also contains ShaderParameters, which are
 *  handled slightly differently. ShaderParameters are NOT unset when popping. */
class RenderParameterContainer {
public:
    RenderParameterContainer();
    virtual ~RenderParameterContainer();

    void clearShaderParameter(const std::string &name);

    void setShaderParameter(const std::string &name, ShaderParameter *data);

    template <typename T>
    void setShaderParameter(const std::string &name, T *data, int count = 1, bool cleanUp = false) {
        if (data == NULL) {
            clearShaderParameter(name);
        } else {
            ShaderParameterMap::iterator itr = _params.find(name);
            if (itr == _params.end()) {
                itr = _params.insert(itr, std::pair<std::string, ShaderParameter*>(name, new ShaderParameter()));
            }

            itr->second->setData(data, count, cleanUp);
        }
    }

    template <typename T>
    T* getShaderParameter(const std::string &name) {
        return _params[name]->getData<T>();
    }

    void setPolygonMode(PolygonMode mode);
    void setTransparency(bool enable);
    void setDepthTest(bool enable);
    void setWireframe(bool enable);

    void unsetPolygonMode();
    void unsetTransparency();
    void unsetDepthTest();
    void unsetWireframe();

    PolygonMode getPolygonMode();
    bool getTransparency();
    bool getDepthTest();
    bool getWireframe();

    /*! Sets the stored RenderParameters.
     * \note Be sure to pair push and pop calls WITHOUT overlapping them, otherwise
     *  unintented side effects may occur. */
    void pushParameters(Shader *shaderTarget);

    /*! Resets the RenderParameters to what they previously were. Note that
     *  ShaderParameters are NOT affected by this.
     * \note Be sure to pair push and pop calls WITHOUT overlapping them, otherwise
     *  unintented side effects may occur. */
    void popParameters();

private:
    void setOldValues();

private:
    template <typename T>
    struct RenderParameter {
        RenderParameter(T defaultVal): current(defaultVal), old(defaultVal), set(false) {}

        T current; /*!< The value to set on push. */
        T old;     /*!< The value to set on pop, to restore old state. */
        bool set;  /*!< Whether or not the value has been explicitely set. */
    };

private:
    ShaderParameterMap _params;

    RenderParameter<PolygonMode> _polygonMode;
    RenderParameter<bool> _transparency;
    RenderParameter<bool> _depthTest;
    RenderParameter<bool> _wireframe;

};

#endif
