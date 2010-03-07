/*
 *  SingleFileManager.h
 *  Base
 *
 *  Created by Brent Wilson on 12/4/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _SINGLEFILEMANAGER_H_
#define _SINGLEFILEMANAGER_H_
#include "ResourceManager.h"

#include <string>
#include <list>
#include <map>

template <typename Resource>
class SingleFileManager : public ResourceManager<Resource> {
protected:
    typedef bool (*CheckPtr)(const std::string&);
    typedef Resource* (*LoadPtr)(const std::string&);
    typedef std::list< std::pair<CheckPtr, LoadPtr> > LoaderList;
    typedef typename LoaderList::iterator LoaderIterator;

    LoaderList _loaders;

public:
    Resource* loadResource(const std::string &name) {
        Resource *resource = ResourceManager<Resource>::getCachedResource(name);
        if (resource) { return resource; }
        return loadFromFile(name);    
    }

    Resource* loadFromFile(const std::string &name) {
        if (!name.length()) {
            Warn("Empty name given to loadFromFile");
            return NULL;
        }

        LoaderIterator itr;
        for (itr = _loaders.begin(); itr != _loaders.end(); itr++) {
            if (itr->first(name)) { return itr->second(name); }
        }

        Error("loadFromFile doesn't know how to load: " << name);
        return NULL;
    }

    template <typename T>
    void registerLoader() {
        _loaders.push_back(std::pair<CheckPtr, LoadPtr>(T::CanLoad, T::Load));
    }
};

#endif
