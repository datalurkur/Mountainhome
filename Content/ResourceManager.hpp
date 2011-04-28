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
#include "Base.h"

template <typename Resource>
ResourceManager<Resource>::ResourceManager() {}

template <typename Resource>
ResourceManager<Resource>::~ResourceManager() {
    unloadAllResources();
    clear_list(_factories);
}

template <typename Resource>
void ResourceManager<Resource>::unloadAllResources() {
    typename std::map<std::string, Resource*>::iterator itr;
    for (itr = _namedResources.begin(); itr != _namedResources.end(); itr++) {
        delete itr->second;
    }

    _namedResources.clear();

    typename std::list<Resource*>::iterator itr2;
    for (itr2 = _unnamedResources.begin(); itr2 != _unnamedResources.end(); itr2++) {
        delete *itr2;
    }

    _unnamedResources.clear();
}

template <typename Resource>
void ResourceManager<Resource>::registerResource(const std::string &name, Resource *resource) {
    if (!name.length()) {
        _unnamedResources.push_back(resource);
        return;
    }

    typename std::map<std::string, Resource*>::iterator itr = _namedResources.find(name);
    if (itr != _namedResources.end()) {
        THROW(DuplicateItemError, "Attempting to register a resource that already exists: " << name);
//        Warn("Resource named " << name << " already exisits. Deleting the old version.");
//        Resource *oldResource = itr->second;
//        delete oldResource;
    }

    _namedResources[name] = resource;
}

template <typename Resource>
Resource* ResourceManager<Resource>::getCachedResource(const std::string &name) {
    typename std::map<std::string, Resource*>::iterator itr = _namedResources.find(name);

    if (itr == _namedResources.end()) {
        THROW(InternalError, "No cached resource named " << name << " exists.");
    }

    return itr->second;
}

template <typename Resource>
void ResourceManager<Resource>::unloadResource(const std::string &name) {
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

template <typename Resource>
Resource* ResourceManager<Resource>::getOrLoadResource(const std::string &name) {
    typename std::map<std::string, Resource*>::iterator itr = _namedResources.find(name);
    if (itr != _namedResources.end()) { return itr->second; }
    return loadResource(name);    
}

template <typename Resource>
Resource* ResourceManager<Resource>::loadResource(const std::string &name) {
    Resource *current;
    FactoryIterator itr;

    // Loop over the factories, trying each one.
    for (itr = _factories.begin(); itr != _factories.end(); itr++) {

        // If we get a valid Resource, return the result.
        current = (*itr)->loadIfPossible(name);

        if (current) {
            // Register the resource if we need to.
            if ((*itr)->autoRegister()) {
                registerResource(name, current);
            }

            return current;
        }
    }

    // If we never got anything valid back, something must have gone wrong.
    THROW(InternalError, "This manager doesn't know how to load this resource: " << name);
}

template <typename Resource>
void ResourceManager<Resource>::registerFactory(ResourceFactory<Resource> *factory) {
    _factories.push_back(factory);
}

template <typename Resource>
std::string ResourceManager<Resource>::getNameOf(const Resource *resource) const {
    typename std::map<std::string, Resource*>::const_iterator itr;
    for (itr = _namedResources.begin(); itr != _namedResources.end(); itr++) {
        if (itr->second == resource) {
            return itr->first;
        }
    }

    return "";
}

#endif
