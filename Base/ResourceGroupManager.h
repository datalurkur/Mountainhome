/*
 *  ResourceGroupManager.h
 *  Mountainhome
 *
 *  Created by loch on 3/19/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _RESOURCEGROUPMANAGER_H_
#define _RESOURCEGROUPMANAGER_H_
#include "Singleton.h"
#include "IOTarget.h"
#include "Base.h"

class ResourceGroupManager : public Singleton<ResourceGroupManager> {
public:
    /*! Adds a resource location to the list of possible resource locations. The set of
     *  resource locations is used when attempting to load resources from disk. */
    void addResourceLocation(const std::string &location, bool recurse = false);

    /*! Finds a resource in the resource location list based on the given IdType and
     *  creates an IOTarget representing the resource. */
    IOTarget* openResource(const std::string &name);

    /*! Finds a resource in the resource location list based on the given IdType and
     *  returns the path to it. */
    std::string findResource(const std::string &name);

protected:
    typedef std::pair<std::string, bool> ResourceLocation;
    typedef std::list<ResourceLocation> ResourceLocationList;

    std::string findResourceInLocation(const std::string &name, ResourceLocation loc);

    ResourceLocationList _resourceLocs;

protected:
    ResourceGroupManager();
    virtual ~ResourceGroupManager();

    template <class T> friend class Singleton;
};

#endif
