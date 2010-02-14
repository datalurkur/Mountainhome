/*
 *  ModelLoaderMD5.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 8/23/06.
 *  Copyright 2006 Brent Wilson. All rights reserved.
 *
 */

#include "ModelLoaderMD5.h"
#include <Base/FileSystem.h>
#include <Base/Exception.h>
#include <algorithm>

bool ModelLoaderMD5::CanLoad(const std::string &name) {
    std::string ext;
    FileSystem::ExtractExtension(name, ext);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == "md5";
}

Model *ModelLoaderMD5::Load(const std::string &name) {
    THROW(NotImplementedError, "MD5 model loading has not been implemented, yet!");
}
