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

// ======================================
// ModelFBX::Factory Function Definitions
// ======================================

ModelFBX::Factory::Factory(ResourceGroupManager *manager):
    ResourceFactory<Model>(manager)
{
    _sdkManager = KFbxSdkManager::Create();
    _importer = KFbxImporter::Create(_sdkManager, "");
}

ModelFBX::Factory::~Factory() {}

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

    // Instantiate an empty ModelFBX and begin populating it with data
    ModelFBX *model = new ModelFBX();
    parseSceneNode(rootNode, model);
    model->internVectors();

    // Clean up a bit
    _importer->Destroy();

    // Return the packed data
    return model;
}

bool ModelFBX::Factory::parseSceneNode(KFbxNode *node, ModelFBX *model) {
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
            unsigned int i, j;
            KFbxMesh *mesh = (KFbxMesh*)attr;

            // Convert non-triangular meshes
            if(!mesh->IsTriangleMesh()) {
                KFbxGeometryConverter converter(_sdkManager);
                mesh = converter.TriangulateMesh(mesh);
            }

            // Get the vertices from the mesh
            unsigned int vertCount = mesh->GetControlPointsCount();
            KFbxVector4 *vertex_data = mesh->GetControlPoints();

            // Prepare the vertices
            std::vector <Vector3> verts;
            for(i = 0; i < vertCount; i++) {
                verts.push_back(Vector3(vertex_data[i].GetAt(0), vertex_data[i].GetAt(1), vertex_data[i].GetAt(2)));
            }

            // Count the indices
            unsigned int indexCount=0;
            for(i = 0; i < mesh->GetPolygonCount(); i++) {
                indexCount += mesh->GetPolygonSize(i);
            }

            // Get the indices for each face
            std::vector <unsigned int> indices;
            for(i = 0; i < mesh->GetPolygonCount(); i++) {
                for(j = 0; j < mesh->GetPolygonSize(i); j++) {
                    indices.push_back(mesh->GetPolygonVertex(i,j));
                }
            }

            std::vector <Vector3> normals;
            std::vector <Vector2> texCoords;

            // Add the prim data to the model
            model->addMeshPart(&verts, &normals, &texCoords, &indices);
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
        status = parseSceneNode(node->GetChild(i), model);
        if(!status) { return false; }
    }

    // This is mostly a bool so that we can return false if something goes wrong
    return true;
}

// =============================
// ModelFBX Function Definitions
// =============================

ModelFBX::ModelFBX(): Model() {}

void ModelFBX::addMeshPart(std::vector<Vector3> *verts, std::vector<Vector3> *norms, std::vector<Vector2> *texCoords, std::vector<unsigned int> *indices) {
    // Add the new verts/norms/texCoords to the buffers
    // TODO - Eventually, we'll want to check for redundant verts in the array and cull them out as necessary
    _mutableVerts.insert(_mutableVerts.end(), verts->begin(), verts->end());

    if(norms != NULL) {
        _mutableNorms.insert(_mutableNorms.end(), norms->begin(), norms->end());
    }

    if(texCoords != NULL) {
        _mutableTexCoords.insert(_mutableTexCoords.end(), texCoords->begin(), texCoords->end());
    }

    _count += verts->size();

    // Add the new indices to the buffer
    unsigned int startIndex = _indexCount;
    _mutableIndices.insert(_mutableIndices.end(), indices->begin(), indices->end());
    _indexCount += indices->size();

    ModelMeshPart newMesh(indices->size(), startIndex);
    _mutableMeshParts.push_back(newMesh);
    _numMeshes += 1;
}

void ModelFBX::internVectors() {
    _verts     = vector_to_array(_mutableVerts);
    _norms     = vector_to_array(_mutableNorms);
    _texCoords = vector_to_array(_mutableTexCoords);
    _indices   = vector_to_array(_mutableIndices);

    _meshes    = new ModelMesh(vector_to_array(_mutableMeshParts), _mutableMeshParts.size());

    findBounds();
    generateVBOs();
}
