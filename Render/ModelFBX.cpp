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
    _scene = KFbxScene::Create(_sdkManager, "");
    status = _importer->Import(_scene);

    // Convert the materials
    KFbxMaterialConverter matConverter(*_sdkManager);
    matConverter.ConnectTexturesToMaterials(*_scene);

    if(status == false) {
        Error("Failed to import FBX " << name);
        return NULL;
    }

    // Get the root node
    KFbxNode *rootNode = _scene->GetRootNode();
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

    // ...and attribute type
    // If this node as an attribute, it might have meshes/materials/data we care about
    KFbxNodeAttribute *attr = node->GetNodeAttribute();
    if(attr != NULL) {
        // Get the type of attribute present, and branch accordingly
        KFbxNodeAttribute::EAttributeType type = attr->GetAttributeType();

        if(type == KFbxNodeAttribute::eMESH) {
            KFbxMesh *mesh = (KFbxMesh*)attr;
            parseMesh(mesh, model);
        }
        else if(type == KFbxNodeAttribute::eSKELETON) {
            Info("Skipping eSKELETON node");
            // For now, we're ignoring skeleton stuff
        }
        else if(type == KFbxNodeAttribute::eMARKER) {
            Info("Skipping eMARKER node");
            // Not sure what these are, or if they're significant
        }
        else {
            Info("Skipping node of type " << type);
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

bool ModelFBX::Factory::parseMesh(KFbxMesh *mesh, ModelFBX *model) {
    unsigned int i, j;
    KFbxNode *node = mesh->GetNode();

    // Get the transformation matrix for this node
    KFbxAnimEvaluator *evaluator = _scene->GetEvaluator();
    KFbxXMatrix& globalTransform = evaluator->GetNodeGlobalTransform(node);

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
    std::vector <Vector3> normals;
    std::vector <Vector2> texCoords;
    for(i = 0; i < vertCount; i++) {
        verts.push_back(Vector3(vertex_data[i].GetAt(0), vertex_data[i].GetAt(1), vertex_data[i].GetAt(2)));
    }

    // Get the layer count
    unsigned int layers = mesh->GetLayerCount();
    if(layers > 1) {
        Info("Warning: Multiple layers found in mesh, Only one will be used.");
    }

    // Get the UV coordinates
    KFbxLayerElementUV *fbxTexCoords = mesh->GetLayer(0)->GetUVs();
    KFbxLayerElementNormal *fbxNormals = mesh->GetLayer(0)->GetNormals();

    // Count the indices
    unsigned int indexCount=0;
    for(i = 0; i < mesh->GetPolygonCount(); i++) {
        indexCount += mesh->GetPolygonSize(i);
    }

    // Get the indices for each face
    std::vector <unsigned int> indices;
    for(i = 0; i < mesh->GetPolygonCount(); i++) {
        for(j = 0; j < mesh->GetPolygonSize(i); j++) {
            unsigned int index = mesh->GetPolygonVertex(i,j);
            indices.push_back(index);

            // Get the normal
            if(fbxNormals) {
                KFbxVector4 normal;

                KFbxLayerElement::EMappingMode mappingMode = fbxNormals->GetMappingMode();
                if(mappingMode == KFbxLayerElement::eBY_CONTROL_POINT) {
                    unsigned int normalIndex = 0;

                    KFbxLayerElement::EReferenceMode referenceMode = fbxNormals->GetReferenceMode();
                    switch(referenceMode) {
                    case KFbxLayerElement::eDIRECT:
                        normalIndex = index;
                        break;
                    case KFbxLayerElement::eINDEX_TO_DIRECT:
                        normalIndex = fbxNormals->GetIndexArray().GetAt(index);
                        break;
                    default:
                        THROW(NotImplementedError, "Unsupported normal reference mode " << referenceMode);
                        break;
                    }
                    normal = fbxNormals->GetDirectArray().GetAt(normalIndex);
                }
                else if(mappingMode == KFbxLayerElement::eBY_POLYGON_VERTEX) {
                    mesh->GetPolygonVertexNormal(i,j,normal);
                }
                else {
                    THROW(NotImplementedError, "Unsupported normal mapping mode " << mappingMode);
                }
                normals.push_back(Vector3(normal.GetAt(0), normal.GetAt(1), normal.GetAt(2)));
            }

            // Get the texcoord
            if(fbxTexCoords) {
                KFbxVector2 texCoord;
                unsigned int texCoordIndex = 0;

                KFbxLayerElement::EMappingMode mappingMode = fbxTexCoords->GetMappingMode();
                if(mappingMode == KFbxLayerElement::eBY_CONTROL_POINT) {

                    KFbxLayerElement::EReferenceMode referenceMode = fbxTexCoords->GetReferenceMode();
                    switch(referenceMode) {
                    case KFbxLayerElement::eDIRECT:
                        texCoordIndex = index;
                        break;
                    case KFbxLayerElement::eINDEX_TO_DIRECT:
                        texCoordIndex = fbxTexCoords->GetIndexArray().GetAt(index);
                    default:
                        THROW(NotImplementedError, "Unsupported texCoord reference mode " << referenceMode);
                        break;
                    }
                }
                else if(mappingMode == KFbxLayerElement::eBY_POLYGON_VERTEX) {
                    texCoordIndex = mesh->GetTextureUVIndex(i,j,KFbxLayerElement::eDIFFUSE_TEXTURES);
                }
                else {
                    THROW(NotImplementedError, "Unsupported texture mapping mode " << mappingMode);
                }
                texCoord = fbxTexCoords->GetDirectArray().GetAt(texCoordIndex);
                texCoords.push_back(Vector2(texCoord.GetAt(0), texCoord.GetAt(1)));
            }
        }
    }

    // Get the materials from the parent layer
    std::vector <Material*> matList;
    if(!parseMaterials(node, model, &matList)) {
        Error("Error parsing materials for " << node->GetName());
        return false;
    }

    if(matList.size() > 1) {
        Info("Warning: Multitexturing is currently not supported in Mountainhome, only one material will be used.");
    }

    // Add the prim data to the model
    model->addMeshPart(&verts, &normals, &texCoords, &indices, matList.front());

    return true;
}

bool ModelFBX::Factory::parseMaterials(KFbxNode *node, ModelFBX *model, std::vector<Material*> *matList) {
    unsigned int i, matCount = 0;
    matCount = node->GetMaterialCount();

    for(i = 0; i < matCount; i++) {
        KFbxSurfaceMaterial *fbxMat = node->GetMaterial(i);
        Material *mat = new Material();

        // Get the textures for this layer
        KFbxMesh *mesh = node->GetMesh();
        KFbxLayerElementTexture *textures = mesh->GetLayer(0)->GetTextures(KFbxLayerElement::eDIFFUSE_TEXTURES);
        // TODO Textures

        // Check to see if this material is a hardware shader
        const KFbxImplementation *implementation = GetImplementation(fbxMat, ImplementationHLSL);
        if(implementation) {
            THROW(NotImplementedError, "HLSL Shaders are not supported by ModelFBX");
        }
        else if(fbxMat->GetClassId().Is(KFbxSurfaceLambert::ClassId)) {
            Info("Found a Lambert surface");
            KFbxSurfaceLambert *lSurface = (KFbxSurfaceLambert*)fbxMat;

            // Temporary vars
            KFbxPropertyDouble3 tDouble3;
            KFbxPropertyDouble1 tDouble1;

            // Set the material color
            tDouble1 = lSurface->GetTransparencyFactor();
            //mat->setColor(tDouble3.Get()[0], tDouble.Get()[1], tDouble.Get()[2], 1.0-tDouble1.Get());
            mat->setColor(1.0, 1.0, 1.0, 1.0-tDouble1.Get());

            // Set the ambient color
            tDouble3 = lSurface->GetAmbientColor();
            mat->setAmbient(tDouble3.Get()[0], tDouble3.Get()[1], tDouble3.Get()[2]);

            // Set the diffuse color
            tDouble3 = lSurface->GetDiffuseColor();
            mat->setDiffuse(tDouble3.Get()[0], tDouble3.Get()[1], tDouble3.Get()[2]);

            // TODO - Support emissive color
            //tDouble3 = lSurface->GetEmissiveColor();
        }
        else if(fbxMat->GetClassId().Is(KFbxSurfacePhong::ClassId)) {
            THROW(NotImplementedError, "Phong surfaces are not supported by ModelFBX");
        }
        else {
            THROW(NotImplementedError, "Unhandled material class found in ModelFBX");
        }

        matList->push_back(mat);
    }

    return true;
}

// =============================
// ModelFBX Function Definitions
// =============================

ModelFBX::ModelFBX(): Model() {}

void ModelFBX::addMeshPart(std::vector<Vector3> *verts, std::vector<Vector3> *norms, std::vector<Vector2> *texCoords, std::vector<unsigned int> *indices, Material *mat) {
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
    newMesh.setMaterial(mat);
    _mutableMeshParts.push_back(newMesh);
}

void ModelFBX::internVectors() {
    _verts     = vector_to_array(_mutableVerts);
    _norms     = vector_to_array(_mutableNorms);
    _texCoords = vector_to_array(_mutableTexCoords);
    _indices   = vector_to_array(_mutableIndices);

    _meshes    = new ModelMesh(vector_to_array(_mutableMeshParts), _mutableMeshParts.size());
    _numMeshes = 1;

    for(int i = 0; i < _meshes[0].getPartCount(); i++) {
        ModelMeshPart *part = _meshes[0].getPart(i);
    }

    findBounds();
    generateVBOs();
}
