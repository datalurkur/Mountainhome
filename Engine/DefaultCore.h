/*
 *  DefaultCore.h
 *  Base
 *
 *  Created by Brent Wilson on 11/13/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _DEFAULTCORE_H_
#define _DEFAULTCORE_H_
#include "SimpleCore.h"
#include "OptionsModule.h"

class ResourceGroupManager;
class MaterialManager;
class TextureManager;
class ShaderManager;
class ModelManager;
class FontManager;

class RenderTarget;
class DefaultCore : public AbstractCore {
public:
    //\todo Load the particulars from persistent data storage.
    DefaultCore(const std::string &caption);
    virtual ~DefaultCore();

    virtual void display(int elapsed);
    virtual void innerLoop(int elapsed);
    virtual void teardown();

    MaterialManager *getMaterialManager();
    ModelManager *getModelManager();
    FontManager *getFontManager();
    OptionsModule *getOptionsModule();

protected:
    std::list<RenderTarget*> _targets;
    ResourceGroupManager *_resourceGroupManager;
    MaterialManager *_materialManager;
    TextureManager *_textureManager;
    ShaderManager *_shaderManager;
    ModelManager *_modelManager;
    FontManager *_fontManager;   

    OptionsModule *_optionsModule;
};

// Create a 'state' class that acts much like the simple core. The default core will act
// much like the simple core except it will have support for states and setting the
// current state. Rather than accessing the core directly through subclassing, the current
// state will have a pointer to the core and use accessors.

#endif
