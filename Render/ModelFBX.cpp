/*
 *  ModelFBX.cpp
 *  Engine
 *
 *  Created by Andrew Jean on on 9/12/10.
 *
 */

#include "ModelFBX.h"
#include <Base/FileSystem.h>
#include <Base/Exception.h>
#include <algorithm>

ModelFBX::Factory::Factory(): ResourceFactory<Model>(NULL) {}
ModelFBX::Factory::~Factory() {}

ModelFBX::ModelFBX() {
    _sdkManager = KFbxSdkManager::Create();
    _importer = KFbxImporter::Create(_sdkManager, "");
}

bool ModelFBX::Factory::canLoad(const std::string &name) {
    std::string ext;
    FileSystem::ExtractExtension(name, ext);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == "fbx";
}

Model *ModelFBX::Factory::load(const std::string &name) {
    THROW(NotImplementedError, "FBX model loading has not been implemented, yet!");
}
