/*
 *  RenderParameterContainer.cpp
 *  Mountainhome
 *
 *  Created by loch on 12/29/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "RenderParameterContainer.h"
#include "GL_Helper.h"
#include "Shader.h"

#if DEBUG
#include <stack>

// Not properly matching push/pop calls or attempting to interleave the push/pop calls can
// result in unintended side effects (for example, the next renderable may render with
// parameters specific to the previous). This is a simple safety net to prevent this from
// happening. The easy safety check happens on pop. To prevent the case where pop is just
// never called, a fence is put in place (an artifically enforced maximum stack size).
// Currently we only have the RenderContext, Material, and Renderable that should be
// calling push/pop, so the fence size is 3, for now.
static std::stack<RenderParameterContainer*> SafetyCheck_PushPopStack;
static int SafetyCheck_StackFence = 3;
#endif

RenderParameterContainer::RenderParameterContainer():
    _cullMode(BACK),
    _transparency(false),
    _depthTest(true),
    _wireframe(false)
{}

RenderParameterContainer::~RenderParameterContainer() {
    clear_map(_params);
}

void RenderParameterContainer::clearShaderParameter(const std::string &name) {
    ShaderParameterMap::iterator itr = _params.find(name);
    if (itr != _params.end()) {
        delete itr->second;
        _params.erase(itr);
    }
}

void RenderParameterContainer::setShaderParameter(const std::string &name, ShaderParameter *data) {
    clearShaderParameter(name);
    _params[name] = data;
}

void RenderParameterContainer::setOldValues() {
    if (_cullMode.set) {
        _cullMode.old = GetCullMode();
    }
    
    if (_wireframe.set) {
        _wireframe.old = GetWireframe();
    }

    if (_transparency.set) {
        glGetBooleanv(GL_BLEND, (GLboolean*)&_transparency.old);
    }

    if (_depthTest.set) {
        glGetBooleanv(GL_DEPTH_TEST, (GLboolean*)&_depthTest.old);
    }
}

void RenderParameterContainer::pushParameters(Shader *shaderTarget) {
#if DEBUG
    SafetyCheck_PushPopStack.push(this);
    if (SafetyCheck_PushPopStack.size() > SafetyCheck_StackFence) {
        THROW(InternalError, "RenderParameterContainer StackFence has been passed.");
    }
#endif //DEBUG

    if (shaderTarget) {
        shaderTarget->setParameters(_params);
    }

    // Update the old values before we set the new ones.
    setOldValues();

    if (_cullMode.set) {
        SetCullMode(_cullMode.set ? _cullMode.current : _cullMode.old);
    }

    if (_wireframe.set) {
        SetWireframe(_wireframe.set ? _wireframe.current : _wireframe.old);
    }

    if (_transparency.set) {
        if (_transparency.current) {
            glEnable(GL_BLEND);
        } else {
            glDisable(GL_BLEND);
        }
    }

    if (_depthTest.set) {
        if (_depthTest.current) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
    }
}

void RenderParameterContainer::popParameters() {
#if DEBUG
    if (SafetyCheck_PushPopStack.top() != this) {
        THROW(InternalError, "Calling RenderParameterContainer::popParameters on a "
            "RenderParameterContainer that is not on top of the stack!!");
    }

    SafetyCheck_PushPopStack.pop();
#endif //DEBUG

    if (_cullMode.set) {
        SetCullMode(_cullMode.set ? _cullMode.old : GetCullMode());
    }
    
    if (_wireframe.set) {
        SetWireframe(_wireframe.set ? _wireframe.old : GetWireframe());
    }

    if (_transparency.set) {
        if (_transparency.old) {
            glEnable(GL_BLEND);
        } else {
            glDisable(GL_BLEND);
        }
    }

    if (_depthTest.set) {
        if (_depthTest.old) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
    }
}

void RenderParameterContainer::setCullMode(CullMode mode) {
    _cullMode.current = mode;
    _cullMode.set = true;
}

void RenderParameterContainer::setTransparency(bool enable) {
    _transparency.current = enable;
    _transparency.set = true;
}

void RenderParameterContainer::setDepthTest(bool enable) {
    _depthTest.current = enable;
    _depthTest.set = true;
}

void RenderParameterContainer::setWireframe(bool enable) {
    _wireframe.current = enable;
    _wireframe.set = true;
}

void RenderParameterContainer::unsetCullMode()     { _cullMode.set  = false; }
void RenderParameterContainer::unsetTransparency() { _transparency.set = false; }
void RenderParameterContainer::unsetDepthTest()    { _depthTest.set    = false; }
void RenderParameterContainer::unsetWireframe()    { _wireframe.set    = false; }

CullMode RenderParameterContainer::getCullMode() { return _cullMode.current;  }
bool RenderParameterContainer::getTransparency() { return _transparency.current; }
bool RenderParameterContainer::getDepthTest()    { return _depthTest.current;    }
bool RenderParameterContainer::getWireframe()    { return _wireframe.current;    }
