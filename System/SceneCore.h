/*
 *  SceneCore.h
 *  System
 *
 *  Created by loch on 11/13/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SCENECORE_H_
#define _SCENECORE_H_
#include "AbstractCore.h"

class SceneCore : public AbstractCore {

};

// Like the state core, this will have a current state and allow for setting the current
// one. States access the core through a pointer to it. The main difference is that rather
// than having 'display' and 'update' functions, the individual states will keep track
// of a set of scenes, overlays, guis, etc... The SceneCore will display them as needed.

#endif
