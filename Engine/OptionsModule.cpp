/*
 *  OptionsModule.cpp
 *  Engine
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include <Base/FileSystem.h>
#include "OptionsModule.h"

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark OptionsModule definitions
//////////////////////////////////////////////////////////////////////////////////////////

OptionsModule::OptionsModule(const std::string &homeDirectory, const std::string &name) {
    _filename = homeDirectory + name;
    FileSystem::FormatPath(_filename);
}

OptionsModule::~OptionsModule() { }

void OptionsModule::load() {
    if (FileSystem::Exists(_filename)) {
        read_ini(_filename, _tempOptions);
    }

    // Setup up default values.
    put_default("video.resolution", Resolution(1024, 768));
    put_default("video.aasamples",  4);
    put_default("video.fullscreen", 0);

    put_default("sound.volume", 1.0);

    apply();
}

void OptionsModule::apply() {
    boost::property_tree::ptree::iterator itr, subItr;

    // update everything in _realOptions with whatever is in _tempOptions.
    for (itr = _tempOptions.begin(); itr != _tempOptions.end(); itr++) {
        for (subItr = itr->second.begin(); subItr != itr->second.end(); subItr++) {
            _realOptions.put(itr->first + "." + subItr->first, subItr->second.data());
        }
    }

    // Save to disk.
    write_ini(_filename, _realOptions);

    // call notify section for each.
    for (itr = _tempOptions.begin(); itr != _tempOptions.end(); itr++) {
        notifySection(itr->first);
    }

    // And clear all the old options out.
    _tempOptions.clear();
}

void OptionsModule::registerListener(const std::string &section, Listener *listener) {
    _listeners[section].push_back(listener);
}

void OptionsModule::notifySection(const std::string &section) {
    ListenerMap::iterator list = _listeners.find(section);
    if (list != _listeners.end()) {
        ListenerList::iterator itr = list->second.begin();
        for (; itr != list->second.end(); itr++) {
            (*itr)->optionsUpdated(section, this);
        }
    }
}
