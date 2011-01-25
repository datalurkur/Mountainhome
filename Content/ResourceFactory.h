/*
 *  ResourceFactory.h
 *  Mountainhome
 *
 *  Created by loch on 3/21/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _RESOURCEFACTORY_H_
#define _RESOURCEFACTORY_H_

/*! This class provides an abstract interface for classes which implement loading
 *  algorithms for individual resource types.
 * \brief An abstrct interface for resource factories. */
template <typename Resource>
class ResourceFactory {
public:
    /*! Creates a new resource factory.
     * \param autoRegister Flag indicating whether or not the Resource created by this
     *  factory should be automatically registered. If this is set to false, the Factory
     *  is expected to register the new resource itself. */
    ResourceFactory(ResourceGroupManager *manager, bool autoRegister = true):
        _resourceGroupManager(manager), _autoRegister(autoRegister) {}

    virtual ~ResourceFactory() {}

    /*! Returns whether or not the resources created by this factory need to be
     *  automatically registered with the manager. */
    bool autoRegister() { return _autoRegister; }

    /*! Basic, overloadable method that checks to see if this factory can load the given
     *  resource and, if possible, loads and returns the result. */
    virtual Resource* loadIfPossible(const std::string &basename) {
        if (canLoad(basename)) { return load(basename); }
        return NULL;
    }

    /*! Takes a file name and does whatever magic is needed to determine if it can be
     *  loaded by this Factory. The simplest implementation involve simply checking the
     *  file extension. */
    virtual bool canLoad(const std::string &name) = 0;

    /*! Actually builds a new resource and returns it. */
    virtual Resource* load(const std::string &name) = 0;

protected:
    ResourceGroupManager *_resourceGroupManager;
    bool _autoRegister;

};

#endif
