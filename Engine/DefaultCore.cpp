/*
 *  DefaultCore.cpp
 *  Base
 *
 *  Created by Brent Wilson on 11/13/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include <Base/ResourceGroupManager.h>
#include <Base/FileSystem.h>

#include <Render/RenderContext.h>
#include <Render/MaterialManager.h>
#include <Render/TextureManager.h>
#include <Render/ShaderManager.h>
#include <Render/ModelManager.h>
#include <Render/FontManager.h>

#include "AudioSystem.h"
#include "DefaultCore.h"
#include "EventPump.h"
#include "Window.h"

DefaultCore::DefaultCore(const std::string &caption) {
    // Build our base and personal directory locations.
#if SYS_PLATFORM == PLATFORM_APPLE
#   ifdef RELEASE_BUILD
        _resourceDirectory = macBundlePath() + "/Contents/Resources/";
#   else
        _resourceDirectory = "../../../Mountainhome/Resources/";
#   endif
    _personalDirectory = std::string(getenv("HOME")) + "/Library/Application Support/Mountainhome/";
    if (!FileSystem::CreateDirectory(_personalDirectory)) {
        THROW(InternalError, "Could not make directory: " << _personalDirectory);
    }
#else
#   error This is not implemented.
#endif

    // And make sure they're formatted properly.
    FileSystem::FormatPath(_resourceDirectory);
    FileSystem::FormatPath(_personalDirectory);

    // Setup our resource managers.
    _resourceGroupManager = new ResourceGroupManager();
    _textureManager = new TextureManager(_resourceGroupManager);
    _shaderManager = new ShaderManager(_resourceGroupManager);
    _modelManager = new ModelManager(_resourceGroupManager, _textureManager);
    _materialManager = new MaterialManager(_resourceGroupManager, _shaderManager, _textureManager);
    _fontManager = new FontManager(_resourceGroupManager, _shaderManager);

    // Create the window and add it as the primary target.
    _mainWindow = new Window(caption);
    _targets.push_back(_mainWindow);

    // Load our options from disk.
    _optionsModule = new OptionsModule(_personalDirectory);
    _optionsModule->registerListener("video", this);
    _optionsModule->load();

    // Setup the event pump.
    _eventPump = new EventPump(_mainWindow);
    _eventPump->addWindowListener(this);
    _eventPump->addMouseButtonListener(this);
    _eventPump->addMouseMotionListener(this);
    _eventPump->addKeyListener(this);

    // And create our audio system (do this AFTER window creation, because of SDL).
    _audioSystem = new AudioSystem();
}

DefaultCore::~DefaultCore() {}

const std::string& DefaultCore::getPersonalDir() {
    return _personalDirectory;
}

const std::string& DefaultCore::getResourceDir() {
    return _resourceDirectory;
}

void DefaultCore::display(int elapsed) {
    _renderContext->resetMetrics();
    std::list<RenderTarget*>::iterator itr;
    for (itr = _targets.begin(); itr != _targets.end(); itr++) {
        (*itr)->render(_renderContext);
    }
    _mainWindow->swapBuffers();
//    Info("Render context metrics:");
//    LogStream::IncrementIndent();
//    Info("Rendered prims:  " << _renderContext->getPrimitiveCount());
//    Info("Rendered verts:  " << _renderContext->getVertexCount());
//    Info("Rendered models: " << _renderContext->getModelCount());
//    LogStream::DecrementIndent();
}

void DefaultCore::innerLoop(int elapsed) {
    update(elapsed);
    display(elapsed);
}

void DefaultCore::teardown() {
    // Ensure everything is torndown before we start deleting stuff!
    clearStates();
}

void DefaultCore::optionsUpdated(const std::string &section, OptionsModule *module) {
    Resolution res  = module->get<Resolution>("video.resolution");
    bool fullscreen = module->get<bool>("video.fullscreen"); 
    int  aasamples  = module->get<int> ("video.aasamples");
    bool vsync      = module->get<bool>("video.vsync"); 

    _mainWindow->rebuild(res.width, res.height, aasamples, fullscreen, vsync);

    if (_renderContext) {
        delete _renderContext;
    }

    _renderContext = new RenderContext();
    _renderContext->clearBuffers(Color4(0.0, 0.0, 0.0, 1.0));
    _mainWindow->swapBuffers();
}

MaterialManager *DefaultCore::getMaterialManager() { return _materialManager; }
ModelManager *DefaultCore::getModelManager() { return _modelManager; }
FontManager *DefaultCore::getFontManager() { return _fontManager; }

OptionsModule *DefaultCore::getOptionsModule() { return _optionsModule; }
