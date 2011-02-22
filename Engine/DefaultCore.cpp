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

#include <Content/Content.h>

#include "AudioSystem.h"
#include "DefaultCore.h"
#include "EventPump.h"
#include "Window.h"

DefaultCore::DefaultCore(const std::string &caption) {
    // Build our personal directory location.
#if SYS_PLATFORM == PLATFORM_APPLE
    _personalDirectory = std::string(getenv("HOME")) + "/Library/Application Support/Mountainhome/";
    if (!FileSystem::CreateDirectory(_personalDirectory)) {
        THROW(InternalError, "Could not make directory: " << _personalDirectory);
    }
#else
#   error This is not implemented.
#endif

    // And make sure they're formatted properly.
    FileSystem::FormatPath(_personalDirectory);

    // Create the window and add it as the primary target.
    _mainWindow = new Window(caption);

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

    // Wait until the window has been created, which gives us our GL context, to
    // intitialize Content.
#   ifdef RELEASE_BUILD
        Content::Initialize();
#   else
        Content::Initialize("../../../Mountainhome/Resources/");
#   endif
}

DefaultCore::~DefaultCore() {}

const std::string& DefaultCore::getPersonalDir() {
    return _personalDirectory;
}

void DefaultCore::innerLoop(int elapsedMilliseconds) {
    update(elapsedMilliseconds);

    _renderContext->resetCounts();
    _renderContext->clear(Color4(0, 0, 0, 1));

    draw();

    _mainWindow->swapBuffers();
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
    _renderContext->clear(Color4(0.0, 0.0, 0.0, 1.0));
    _mainWindow->swapBuffers();
}

AudioSystem *DefaultCore::getAudioSystem() { return _audioSystem; }
OptionsModule *DefaultCore::getOptionsModule() { return _optionsModule; }
