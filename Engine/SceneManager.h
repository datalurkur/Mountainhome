/*
 *  Scene.h
 *  System
 *
 *  Created by loch on 12/29/09.
 *  Copyright 2009 Brent Wilson. All rights reserved.
 *
 */
#ifndef _SCENEMANAGER_H_
#define _SCENEMANAGER_H_
#include <Base/Math3D.h>
#include <Base/Vector.h>

#include "SceneNode.h"
#include "Camera.h"
#include "Entity.h"

class RenderContext;
class Light;
class Model;

class SceneManager {
public:
    SceneManager();
    virtual ~SceneManager();

    void clearScene();

    void render(RenderContext *context, Camera *source);

    SceneNode* getRootNode();

    template <typename T>
    bool has(const std::string &name);

    template <typename T>
    T* create(const std::string &name);

    template <typename T>
    T* get(const std::string &name);

    template <typename T>
    void destroy(const std::string &name);

    Light *createLight(const std::string &name);

    void setAmbientLight(const Vector4 &color);
    const Vector4& getAmbientLight() const;

    void addVisibleObjectsToList(Frustum *bounds, std::list<SceneNode*> &visible);

protected:
    typedef std::map<std::string, Light*>  LightMap;

    bool genericHas(const std::string &name, const std::string &type);
    SceneNode* genericGet(const std::string &name, const std::string &type);
    void genericDestroy(const std::string &name, const std::string &type);

protected:
    SceneNodeMap _nodeMap;
    SceneNode *_rootNode;
    LightMap _lightMap;

    Vector4 _ambientLight;
};

template <typename T>
bool SceneManager::has(const std::string &name) {
    return genericHas(name, T::TypeName);
}

template <typename T>
T* SceneManager::create(const std::string &name) {
    if (_nodeMap.find(name) != _nodeMap.end()) {
        THROW(DuplicateItemError, "Node named " << name <<
              " already exists in this scene!");
    }

    T* node = new T(name);
    _rootNode->attach(node);
    _nodeMap[name] = node;
    return node;
}

// Specialization of Camera, since it has a different c'tor. See the cpp for defintion.
template <> Camera* SceneManager::create<Camera>(const std::string &name);

template <typename T>
T* SceneManager::get(const std::string &name) {
    return static_cast<T*>(genericGet(name, T::TypeName));
}

template <typename T>
void SceneManager::destroy(const std::string &name) {
    genericDestroy(name, T::TypeName);
}

#endif
