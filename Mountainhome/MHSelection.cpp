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
        _selected.push_back((Entity*)(*itr));
    }
}

MHSelection::~MHSelection() {}

std::list <Entity*> &MHSelection::getSelected() {
    return _selected;
}
