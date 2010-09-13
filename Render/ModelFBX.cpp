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

ModelFBX::Factory::Factory(ResourceGroupManager *manager):
    ResourceFactory<Model>(manager), _count(0), _verts(0), _norms(0), _texCoords(0)
{
    _sdkManager = KFbxSdkManager::Create();
    _importer = KFbxImporter::Create(_sdkManager, "");
}

ModelFBX::Factory::~Factory() {}

ModelFBX::ModelFBX() {}

bool ModelFBX::Factory::canLoad(const std::string &name) {
    std::string ext;
    FileSystem::ExtractExtension(name, ext);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == "fbx";
}

Model *ModelFBX::Factory::load(const std::string &name) {
    bool status;

    // Get version information for version checking
    int sdkMajorVersion, sdkMinorVersion, sdkRevision,
        fileMajorVersion, fileMinorVersion, fileRevision;
    KFbxSdkManager::GetFileFormatVersion(sdkMajorVersion, sdkMinorVersion, sdkRevision);

    // Get the absolute path to the file for the FBX SDK
    std::string fullFilename = _resourceGroupManager->findResource(name);

    // Initialize the importer and check the file for integrity
    status = _importer->Initialize(fullFilename.c_str(), -1, _sdkManager->GetIOSettings());
    _importer->GetFileVersion(fileMajorVersion, fileMinorVersion, fileRevision);

    if(status == false) {
        Error("Failed to load " << name << ": " << _importer->GetLastErrorString());
        return NULL;
    }

    if(!_importer->IsFBX()) {
        Error("Failed to load " << name << ": not a valid FBX file.");
        return NULL;
    }

    // Set up properties for the things we want out of the scene
    KFbxIOSettings *ios = KFbxIOSettings::Create(_sdkManager, IOSROOT);
    _sdkManager->SetIOSettings(ios);
    (*(_sdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_MATERIAL,        true);
    (*(_sdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_TEXTURE,         true);
    (*(_sdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_LINK,            true);
    (*(_sdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_SHAPE,           true);
    (*(_sdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_GOBO,            true);
    (*(_sdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_ANIMATION,       true);
    (*(_sdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);

    // Import the scene
    KFbxScene *scene = KFbxScene::Create(_sdkManager, "");
    status = _importer->Import(scene);

    if(status == false) {
        Error("Failed to import FBX " << name);
        return NULL;
    }

    // Get the root node
    KFbxNode *rootNode = scene->GetRootNode();
    if(!rootNode) {
        Error("FBX file " << name << " contains no data!");
        return NULL;
    }
    parseSceneNode(rootNode);

    // Clean up a bit
    _importer->Destroy();

    // Return the packed data
    if(_count > 0) {
        Model *model = new Model(_verts, _norms, _texCoords, _count);
        return model;
    } else {
        Error("No mesh data found in FBX model " << name);
        return NULL;
    }
}

bool ModelFBX::Factory::parseSceneNode(KFbxNode *node) {
    // Query the node name...
    KString nodeName = node->GetName();
    //Info("Parsing scene node: " << nodeName);

    // ...and attribute type
    // If this node as an attribute, it might have meshes/materials/data we care about
    KFbxNodeAttribute *attr = node->GetNodeAttribute();
    if(attr != NULL) {
        // Get the type of attribute present, and branch accordingly
        KFbxNodeAttribute::EAttributeType type = attr->GetAttributeType();
        if(type == KFbxNodeAttribute::eMESH) {
            KFbxMesh *mesh = (KFbxMesh*)attr;
            // TODO - The code to parse meshes still needs to be written
        }
        else if(type == KFbxNodeAttribute::eSKELETON) {
            // For now, we're ignoring skeleton stuff
        }
        else if(type == KFbxNodeAttribute::eMARKER) {
            // Not sure what these are, or if they're significant
        }
        else {
            // KFbxNodeAttribute::eLIGHT
            //                    eCAMERA
            //                    eBOUNDARY
            //                    eTRIM_NURBS_SURFACE
        }
    }

    // Parse the child nodes
    bool status;
    for(int i = 0; i < node->GetChildCount(); i++) {
        status = parseSceneNode(node->GetChild(i));
        if(!status) { return false; }
    }

    // This is mostly a bool so that we can return false if something goes wrong
    return true;
}
