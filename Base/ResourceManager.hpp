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

template <typename Resource, typename IdType>
ResourceManager<Resource, IdType>::ResourceManager() {}

template <typename Resource, typename IdType>
ResourceManager<Resource, IdType>::~ResourceManager() {
    unloadAllResources();
    clear_list(_factories);
}

template <typename Resource, typename IdType>
void ResourceManager<Resource, IdType>::unloadAllResources() {
    typename std::map<IdType, Resource*>::iterator itr;
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

template <typename Resource, typename IdType>
void ResourceManager<Resource, IdType>::registerResource(const IdType &name, Resource *resource) {
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

template <typename Resource, typename IdType>
Resource* ResourceManager<Resource, IdType>::getCachedResource(const IdType &name) {
    typename std::map<IdType, Resource*>::iterator itr = _namedResources.find(name);
    if (itr != _namedResources.end()) { return itr->second; }
    return NULL;
}

template <typename Resource, typename IdType>
void ResourceManager<Resource, IdType>::unloadResource(const IdType &name) {
    typename std::map<std::string, Resource*>::iterator itr = _namedResources.find(name);
    if (itr != _namedResources.end()) {
        delete itr->second;
        _namedResources.erase(itr);
    }
}

template <typename Resource, typename IdType>
int ResourceManager<Resource, IdType>::resourcesLoaded() const {
    return _namedResources.size() + _unnamedResources.size();
}

template <typename Resource, typename IdType>
Resource* ResourceManager<Resource, IdType>::getOrLoadResource(const IdType &name) {
    Resource *resource = ResourceManager<Resource, IdType>::getCachedResource(name);
    if (resource) { return resource; }
    return loadResource(name);    
}

template <typename Resource, typename IdType>
Resource* ResourceManager<Resource, IdType>::loadResource(const IdType &name) {
    FactoryIterator itr;
    for (itr = _factories.begin(); itr != _factories.end(); itr++) {
        if ((*itr)->canLoad(name)) { return (*itr)->load(name); }
    }

    THROW(InternalError, "This manager doesn't know how to load this resource.");
}

template <typename Resource, typename IdType>
void ResourceManager<Resource, IdType>::registerFactory(ResourceFactory<Resource, IdType> *factory) {
    _factories.push_back(factory);
}

#endif
