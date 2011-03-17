/*
 *  ResourceGroupManager.cpp
 *  Mountainhome
 *
 *  Created by loch on 3/19/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "ResourceGroupManager.h"
#include "FileSystem.h"

ResourceGroupManager::ResourceGroupManager() {}
ResourceGroupManager::~ResourceGroupManager() {}

void ResourceGroupManager::addResourceLocation(const std::string &loc, bool recurse) {
    Info("Adding resource location: " << loc << " [recursive: " << recurse << "]");
    _resourceLocs.push_back(ResourceLocation(loc, recurse));
}

IOTarget* ResourceGroupManager::openResource(const std::string &name) {
    std::string loc = findResource(name);
    return FileSystem::GetFile(loc, IOTarget::Read);
}

std::string ResourceGroupManager::findResource(const std::string &name) {
    ResourceLocationList::iterator itr;
    for (itr = _resourceLocs.begin(); itr != _resourceLocs.end(); itr++) {
        std::string loc = findResourceInLocation(name, *itr);
        if (loc.size() > 0) { return loc; }
    }

    // Nothing matched. Bail!
    THROW(ItemNotFoundError, "Could not find resource named '" << name << "'.");
}

std::string ResourceGroupManager::findResourceInLocation(const std::string &name, ResourceLocation loc) {
    std::list<std::string> *listing = FileSystem::GetListing(loc.first, loc.second, false);
    std::list<std::string>::iterator itr;
    std::string result = "", file;

    if (listing) {
        for (itr = listing->begin(); itr != listing->end(); itr++) {
            if (FileSystem::ExtractFilename(*itr, file) == name) {
                result = *itr;
                FileSystem::FormatPath(result);
                break;
            }
        }
        delete listing;
    }

    // Nothing matched. Bail!
    return result;
}
