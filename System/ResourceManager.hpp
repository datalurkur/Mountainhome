/*
 *  ResourceManager.cpp
 *  Base
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _RESOURCEMANAGER_HPP_
#define _RESOURCEMANAGER_HPP_
#include "ResourceManager.h"
#include "Logger.h"

template <typename Resource>
ResourceManager<Resource>::ResourceManager() {}

template <typename Resource>
ResourceManager<Resource>::~ResourceManager() {
    clearResources();
}

template <typename Resource>
void ResourceManager<Resource>::clearResources() {
    typename std::map<std::string, Resource*>::iterator itr;
    for (itr = _namedResources.begin(); itr != _namedResources.end(); itr++) {
        delete itr->second;
    }

    _namedResources.clear();

    typename std::list<Resource*>::iterator itr2;
    for (itr2 = _unnamedResources.begin(); itr2 != _unnamedResources.end(); itr2++) {
        delete (*itr2);
    }

    _unnamedResources.clear();
}

template <typename Resource>
void ResourceManager<Resource>::registerResource(const std::string &name,
                                                 Resource *resource) {
    if (!name.length()) {
        _unnamedResources.push_back(resource);
        return;
    }

    Resource *oldResource = getCachedResource(name);
    if (oldResource) {
        Warn("Resource named " << name << " already exisits. Deleting the old version.");
        delete oldResource;
    }

    _namedResources[name] = resource;
}

template <typename Resource>
Resource* ResourceManager<Resource>::getCachedResource(const std::string &name) {
    typename std::map<std::string, Resource*>::iterator itr = _namedResources.find(name);
    if (itr != _namedResources.end()) {
        return itr->second;
    }

    return NULL;
}

template <typename Resource>
void ResourceManager<Resource>::removeResource(const std::string &name) {
    typename std::map<std::string, Resource*>::iterator itr = _namedResources.find(name);
    if (itr != _namedResources.end()) {
        delete itr->second;
        _namedResources.erase(itr);
    }
}

template <typename Resource>
int ResourceManager<Resource>::resourcesLoaded() const {
    return _namedResources.size() + _unnamedResources.size();
}

#endif
