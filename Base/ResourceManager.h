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

template <typename Resource, typename IdType>
class ConcreteResourceReference {
public:
    ConcreteResourceReference(
        void (*setup)(),
        bool (*cl)(const IdType &args),
        Resource* (*l)(const IdType &args),
        void (*td)()):
        canLoad(cl), load(l), teardown(td) {
        setup();
    }

    virtual ~ConcreteResourceReference() {
        teardown();
    }

    bool (*canLoad)(const IdType &args);
    Resource* (*load)(const IdType &args);

protected:
    void (*teardown)();

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
    void registerResource(const std::string &name, Resource *resource);

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

    /*! Registers a new Resource subclass. Registered subclasses are expected to implement
     *  The Setup, Teardown, CanLoad, and Load functions. Setup and Teardown do any one
     *  time setup or teardown required by a particular Resource subclass. An example of
     *  this would be a Font class implemented using the SDL_ttf library, which requires a
     *  one time TTF_Init()/TTF_Quit() calls for setup and teardown respectively. CanLoad
     *  and Load both take an IdType argument which uniquely identifies a Resource.
     *  CanLoad is expected to examine the IdType and determine if the Subclass is capable
     *  of loading it, and Load is responsible for loading and returning the Resource. */
    template <typename Subclass>
    void registerConcreteResourceType() {
        _concreteSubclasses.push_back(new ConcreteResourceReference<Resource, IdType>(Subclass::Setup, Subclass::CanLoad, Subclass::Load, Subclass::Teardown));
    }

protected:
    typedef std::list<ConcreteResourceReference<Resource, IdType>*> ConcreteResourceList;
    typedef typename ConcreteResourceList::iterator ConcreteResourceIterator;
    friend class TestResourceManager;

protected:
    ConcreteResourceList _concreteSubclasses;
    std::map<IdType, Resource*> _namedResources;
    std::list<Resource*> _unnamedResources;

};

#include "ResourceManager.hpp"

#endif
