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

class AudioSystem;
class RenderTarget;

class DefaultCore : public AbstractCore, public OptionsModule::Listener {
public:
    //\todo Load the particulars from persistent data storage.
    DefaultCore(const std::string &projectName, const std::string &resourceDir = "");
    virtual ~DefaultCore();

    virtual void innerLoop(int elapsedMilliseconds);
    virtual void teardown();

    const std::string& getPersonalDir();
    const std::string& getResourceDir();

    void optionsUpdated(const std::string &section, OptionsModule *module);

    AudioSystem * getAudioSystem();
    OptionsModule * getOptionsModule();

protected:
    std::string _personalDirectory;
    OptionsModule *_optionsModule;
    AudioSystem *_audioSystem;

};

// Create a 'state' class that acts much like the simple core. The default core will act
// much like the simple core except it will have support for states and setting the
// current state. Rather than accessing the core directly through subclassing, the current
// state will have a pointer to the core and use accessors.

#endif
