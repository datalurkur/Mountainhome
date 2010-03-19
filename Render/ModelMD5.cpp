/*
 *  ModelMD5.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 8/23/06.
 *  Copyright 2006 Brent Wilson. All rights reserved.
 *
 */

#include "ModelMD5.h"
#include <Base/FileSystem.h>
#include <Base/Exception.h>
#include <algorithm>

void ModelMD5::Setup() {}
void ModelMD5::Teardown() {}

bool ModelMD5::CanLoad(const std::string &name) {
    std::string ext;
    FileSystem::ExtractExtension(name, ext);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == "md5";
}

Model *ModelMD5::Load(const std::string &name) {
    THROW(NotImplementedError, "MD5 model loading has not been implemented, yet!");
}
