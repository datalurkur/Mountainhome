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

    void update();

    /*! Removes and deletes all nodes in the scene. */
    void deleteAllNodes();

    /*! Checks to see if a node of the given name/type is in the scene. */
    template <typename T>
    bool hasNode(const std::string &name);

    /*! Adds a node of the given name/type to the scene. */
    template <typename T>
    void addNode(T* object);

    /*! Gets the node of the given name/type from the scene, returning NULL if none exists. */
    template <typename T>
    T* getNode(const std::string &name);

    /*! Removes the node of the given name/type from the scene, returning a reference to
     *  it. If the named node does not exist, an error is raised. */
    template <typename T>
    T* removeNode(const std::string &name);

    /*! Removes the node of the given name/type from the scene, and deletes the memory. If
     *  the named node does not exist, an error is raised. */
    template <typename T>
    void deleteNode(const std::string &name);

    /*! Creates a new light in the scene and returns a pointer for configuration. */
    Light *createLight(const std::string &name);

    /*! Sets the global ambient lighting term for the scene. */
    void setAmbientLight(const Vector4 &color);

    /*! Gets the global ambient lighting term for the scene. */
    const Vector4& getAmbientLight() const;

    /*! Finds all visible objects in the scene, based on the given Frustum, and adds them
     *  to the given list. */
    void addVisibleObjectsToList(const Frustum &bounds, SceneNodeList &visible);

    /*! Renders the scene to the given RenderContext, based on the named Camera. */
    void render(const std::string &camera, RenderContext *context);

    /*! Renders the scene to the given RenderContext, based on the given Camera. */
    void render(Camera *camera, RenderContext *context);

protected:
    typedef std::map<std::string, Light*>  LightMap;

    bool genericHasNode(const std::string &name, const std::string &type);
    SceneNode* genericGetNode(const std::string &name, const std::string &type);
    SceneNode* genericRemoveNode(const std::string &name, const std::string &type);

protected:
    SceneNodeMap _nodeMap;
    SceneNode *_rootNode;
    LightMap _lightMap;

    Vector4 _ambientLight;
};

template <typename T>
bool SceneManager::hasNode(const std::string &name) {
    return genericHasNode(name, T::TypeName);
}

template <typename T>
void SceneManager::addNode(T *object) {
    if (_nodeMap.find(object->getName()) != _nodeMap.end()) {
        THROW(DuplicateItemError, "Node named " << object->getName() << " already exists in this scene!");
    }

    _rootNode->attach(object);
    _nodeMap[object->getName()] = object;
}

template <typename T>
T* SceneManager::getNode(const std::string &name) {
    return static_cast<T*>(genericGetNode(name, T::TypeName));
}

template <typename T>
T* SceneManager::removeNode(const std::string &name) {
    return static_cast<T*>(genericRemoveNode(name, T::TypeName));
}

template <typename T>
void SceneManager::deleteNode(const std::string &name) {
    T* node = removeNode<T>(name);
    if (node) { delete node; }
}

#endif
