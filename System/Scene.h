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
#include <Base/Vector.h>

class RenderContext;
class Light;
class Camera;
class Entity;
class Node;
class Model;

class Scene {
public:
    Scene();
    virtual ~Scene();

    void clearScene();

    void render(RenderContext *context, Camera *source);

    Node*   getRootNode();

    Entity* createEntity(Model *model, const std::string &name);
    Camera* createCamera(const std::string &name);
    Light*  createLight(const std::string &name);

    Entity* getEntity(const std::string &name);
    Camera* getCamera(const std::string &name);
    Light*  getLight(const std::string &name);

    void setAmbientLight(const Vector4 &color);
    const Vector4& getAmbientLight() const;

private:
    typedef std::map<std::string, Entity*> EntityMap;
    typedef std::map<std::string, Camera*> CameraMap;
    typedef std::map<std::string, Light*>  LightMap;

    Node      *_rootNode;
    EntityMap _entityMap;
    CameraMap _cameraMap;
    LightMap  _lightMap;

    Vector4 _ambientLight;
};

#endif
