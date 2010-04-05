/*
 *  DefaultCore.cpp
 *  Base
 *
 *  Created by Brent Wilson on 11/13/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include <Render/RenderContext.h>

#include <Base/ResourceGroupManager.h>
#include <Render/MaterialManager.h>
#include <Render/TextureManager.h>
#include <Render/ShaderManager.h>
#include <Render/ModelManager.h>
#include <Render/FontManager.h>

#include "DefaultCore.h"
#include "EventPump.h"
#include "Window.h"

DefaultCore::DefaultCore(const std::string &caption) {
    _resourceGroupManager = new ResourceGroupManager();
    _textureManager = new TextureManager(_resourceGroupManager);
    _shaderManager = new ShaderManager(_resourceGroupManager);
    _modelManager = new ModelManager(_resourceGroupManager);
    _materialManager = new MaterialManager(_resourceGroupManager, _shaderManager, _textureManager);
    _fontManager = new FontManager(_resourceGroupManager, _shaderManager);

    // \fixme Load from some sort of persistent settings.
    _mainWindow = new Window(1024, 768, false, caption);
    _eventPump = new EventPump(_mainWindow);
    _eventPump->addWindowListener(this);
    _eventPump->addMouseButtonListener(this);
    _eventPump->addMouseMotionListener(this);
    _eventPump->addKeyListener(this);

    _renderContext = new RenderContext();

    _renderContext->clearBuffers(Color4(0.0, 0.0, 0.0, 1.0));
    _mainWindow->swapBuffers();

    _targets.push_back(_mainWindow);
}

DefaultCore::~DefaultCore() {}

void DefaultCore::display(int elapsed) {
    _renderContext->resetMetrics();
    std::list<RenderTarget*>::iterator itr;
    for (itr = _targets.begin(); itr != _targets.end(); itr++) {
        (*itr)->render(_renderContext);
    }

    _mainWindow->swapBuffers();
    //Info("Render context metrics:");
    LogStream::IncrementIndent();
    //Info("Rendered prims:  " << _renderContext->getPrimitiveCount());
    //Info("Rendered verts:  " << _renderContext->getVertexCount());
    //Info("Rendered models: " << _renderContext->getModelCount());
    LogStream::DecrementIndent();
}

void DefaultCore::innerLoop(int elapsed) {
    update(elapsed);
    display(elapsed);
}

void DefaultCore::teardown() {
    // Ensure everything is torndown before we start deleting stuff!
    clearStates();
}
