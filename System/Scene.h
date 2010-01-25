/*
 *  Scene.h
 *  System
 *
 *  Created by loch on 12/29/09.
 *  Copyright 2009 Brent Wilson. All rights reserved.
 *
 */
#ifndef _SCENE_H_
#define _SCENE_H_
#include <Base/Math3D.h>

class Entity;
class Model;

class Scene {
public:
    Scene();
    virtual ~Scene();

    void clearScene();

    Entity* createEntity(Model *model, const std::string &name);
    Entity* getEntity(const std::string &name);

private:
    typedef std::map<std::string, Entity*> EntityMap;
    EntityMap _entityMap;
};

#endif
