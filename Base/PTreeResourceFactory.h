/*
 *  PTreeResourceFactory.h
 *  Mountainhome
 *
 *  Created by loch on 3/21/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _PTREERESOURCEFACTORY_H_
#define _PTREERESOURCEFACTORY_H_
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "ResourceGroupManager.h"
#include "ResourceFactory.h"

/*! This class provides a specialization of the ResourceFactory that makes use of the
 *  boost ptree library. It automatically loads up the property tree before calling the
 *  canLoad and load. It also does some basic sanity checking of the property list,
 *  throwing an exception if certain required keys are missing.
 * \brief A ResourceFactory that supports boost property trees. */
template <typename Resource>
class PTreeResourceFactory : public ResourceFactory<Resource> {
public:
    /*! \seealso ResourceFactory::ResourceFactory() */
    PTreeResourceFactory(bool autoRegister = true): ResourceFactory<Resource>(autoRegister) {}

    /*! \seealso ResourceFactory::loadIfPossible */
    virtual Resource* loadIfPossible(const std::string &basename) {
        // Load the property tree from disk.
        std::string filename = ResourceGroupManager::Get()->findResource(basename);
        read_json(filename, _ptree);

        // Loop over the required keys list and make sure all are present.
        std::list<std::string>::iterator itr = _requiredKeys.begin();
        for (; itr != _requiredKeys.end(); itr++) {
            if (_ptree.find(*itr) == _ptree.not_found()) {
                THROW(ItemNotFoundError, "Could not find necessary '" << *itr << "'.");
            }
        }

        // Setup is done, call the actual canLoad and load methods.
        if (canLoad(basename)) { return load(basename); }
        return NULL;
    }

    /*! \seealso ResourceFactory::canLoad */
    virtual bool canLoad(const std::string &name) = 0;

    /*! \seealso ResourceFactory::load */
    virtual Resource* load(const std::string &name) = 0;

protected:
    std::list<std::string> _requiredKeys;
    boost::property_tree::ptree _ptree;

    /*! Adds a new key to the list of required keys. If a property tree is loaded up that
     *  lacks any of the required keys, an error will be thrown in loadIfPossible. */
    void addRequiredKey(const std::string &key) {
        _requiredKeys.push_back(key);
    }

};

#endif
