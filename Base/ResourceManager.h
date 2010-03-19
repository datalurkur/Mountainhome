/*
 *  ResourceManager.h
 *  Base
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_
#include "Exception.h"
#include "Logger.h"
#include <string>
#include <list>
#include <map>

/*! This class provides an abstract interface for classes which implement loading
 *  algorithms for individual resource types.
 * \brief An abstrct interface for resource factories. */
template <typename Resource, typename IdType = std::string>
class ResourceFactory {
public:
    /*! Takes a file name and does whatever magic is needed to determine if it can be
     *  loaded by this Factory. The simplest implementation involve simply checking the
     *  file extension. */
    virtual bool canLoad(const IdType &name) = 0;

    /*! Actually builds a new resource and returns it. */
    virtual Resource* load(const IdType &name) = 0;
};

/*! The ResourceManager gives some basic functionality to aid in resource management. It
 *  gives the user a basic method of caching, accessing, and clearing resources from the
 *  Base. Loading a resource and registering it with the manager is left to the
 *  subclasses.
 * \brief Provides the base functionality for Resource caching and access.
 * \todo Make some way to clear the resource cache
 * \todo Add resource groups (Temp, Level, Always) which can be pre-emptively loaded into
 *  the cache an cleared. */
template <typename Resource, typename IdType = std::string>
class ResourceManager {
public:
    ResourceManager();
    virtual ~ResourceManager();

    /*! Reports the number of resources (of type Resource) currently loaded in the system. */
    int resourcesLoaded() const;

    /*! Reports the number of resources (of type Resource) that can be found in the set of
     *  current resource paths.
     * \todo implement me! */
    int resourcesAvailable() const { THROW(NotImplementedError, "resourcesAvailable"); }

    /*! Reports how much space (in bytes) the loaded resources are taking up.
     * \todo implement me! */
    int bytesTakenOnDisk() const { THROW(NotImplementedError, "bytesTakenOnDisk"); }

    /*! Register a new resource with the system. A name is not necessarily needed, but if
     *  none is given then there will be no way to again retrieve the Resource from here.
     *  This is useful in situations when the user wants to load something manually and
     *  will be maintaining the reference to the resource themselves but would still like
     *  the convenience of automatic deletion.
     * \param name The name to store the resource under. This name allows retrieval from
     *        the cache. If it is not unique, the previous resource will be replaced.
     * \param resource The actual resource to add to the cache. */
    void registerResource(const IdType &name, Resource *resource);

    /*! Removes all named and unnamed resources from the manager. */
    void unloadAllResources();

    /*! Removes the named resource from the cache.
     * \param name The name of the resource to remove from the cache */
    void unloadResource(const IdType &name);

    /*! Loads a resource into memory. */
    Resource* loadResource(const IdType &name);

    /*! Retrieves a Resource from the cache.
     * \param name The name the resource was stored under
     * \return The cached Resource or null if nothing was found. */
    Resource* getCachedResource(const IdType &name);

    /*! Attempts to find and load the given resource. */
    Resource* getOrLoadResource(const IdType &name);

    /*! Registers a new ResourceFactory. */
    void registerFactory(ResourceFactory<Resource, IdType> *factory);

protected:
    typedef std::list<ResourceFactory<Resource, IdType>*> FactoryList;
    typedef typename FactoryList::iterator FactoryIterator;
    friend class TestResourceManager;

protected:
    std::map<IdType, Resource*> _namedResources;
    std::list<Resource*> _unnamedResources;
    FactoryList _factories;

};

#include "ResourceManager.hpp"

#endif
