/*
 *  MHSelection.cpp
 *  Mountainhome
 *
 *  Created by Paul on 9/22/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHSelection.h"

MHSelection::MHSelection() {}

void MHSelection::initialize(std::list <SceneNode*> selected) {
    _selected.clear();

    std::list <SceneNode*>::iterator itr = selected.begin();
    for(; itr != selected.end(); itr++) {
        // Cull scene nodes that aren't entities
        Info("Inspecting SceneNode of type " << (*itr)->getType());
        // TODO - Make this NOT a string compare
        if((*itr)->getType() == "MHActor") {
            _selected.push_back((MHActor*)(*itr));
        }
    }
}

MHSelection::~MHSelection() {}

std::list <MHActor*> &MHSelection::getSelected() {
    return _selected;
}
