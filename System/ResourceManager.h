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
#include <string>
#include <list>
#include <map>

/*! The ResourceManager gives some basic functionality to aid in resource management. It
 *  gives the user a basic method of caching, accessing, and clearing resources from the
 *  Base. Loading a resource and registering it with the manager is left to the
 *  subclasses.
 * \brief Provides the base functionality for Resource caching and access.
 * \todo Make some way to clear the resource cache
 * \todo Add resource levels (Temp, Level, Always).
 * \author Brent Wilson
 * \date 4/22/07 */
template <typename Resource>
class ResourceManager {
public:
    ResourceManager();
    virtual ~ResourceManager();

    /*! Reports the number of resources (of type Resource) currently loaded in the system.
     * \return The number of resources. */
    int resourcesLoaded() const;

    /*! Register a new resource with the system. A name is not necessarily needed, but if
     *  none is given then there will be no way to again retrieve the Resource from here.
     *  This is useful in situations when the user wants to load something manually and
     *  will be maintaining the reference to the resource themselves but would still like
     *  the convenience of automatic deletion.
     * \param name The name to store the resource under. This name allows retrieval from
     *        the cache. If it is not unique, the previous resource will be replaced.
     * \param resource The actual resource to add to the cache. */
    void registerResource(const std::string &name, Resource *resource);

    /*! Retrieves a Resource from the cache.
     * \param name The name the resource was stored under
     * \return The cached Resource or null if nothing was found. */
    Resource* getCachedResource(const std::string &name);

    /*! Removes the named resource from the cache.
     * \param name The name of the resource to remove from the cache */
    void removeResource(const std::string &name);

protected:
    friend class TestResourceManager;
    std::map<std::string, Resource*> _namedResources;
    std::list<Resource*> _unnamedResources;

    /*! Removes all named and unnamed resources from the manager. */
    void clearResources();

};

#include "ResourceManager.hpp"

#endif
