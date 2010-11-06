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

ModelFBX::Factory::Factory(ResourceGroupManager *manager, TextureManager *tManager):
    ResourceFactory<Model>(manager), _textureManager(tManager)
{
    _sdkManager = KFbxSdkManager::Create();
    _importer = KFbxImporter::Create(_sdkManager, "");
}

ModelFBX::Factory::~Factory() {
    _importer->Destroy();
    _sdkManager->Destroy();
}

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
    ModelBone *rootBone = new ModelBone();
    model->setRootBone(rootBone);

    parseSceneNode(rootNode, model);
    model->internVectors();

    // Clean up a bit
    _scene->Destroy();

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
        else if(type == KFbxNodeAttribute::eLIGHT) {
            Info("Skipping eLIGHT node");
            // Not sure what these are, or if they're significant
        }
        else if(type == KFbxNodeAttribute::eCAMERA) {
            Info("Skipping eCAMERA node");
            // Not sure what these are, or if they're significant
        }
        else if(type == KFbxNodeAttribute::eBOUNDARY) {
            Info("Skipping eBOUNDARY node");
            // Not sure what these are, or if they're significant
        }
        else if(type == KFbxNodeAttribute::eTRIM_NURBS_SURFACE) {
            Info("Skipping eTRIM_NURBS_SURFACE node");
            // Not sure what these are, or if they're significant
        }
        else {
            Info("Skipping node of type " << type);
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

    Info("Parsing mesh " << mesh->GetName());

    // Get the transformation matrix for this node
    KFbxAnimEvaluator *evaluator = _scene->GetEvaluator();
    KFbxXMatrix& globalTransform = evaluator->GetNodeGlobalTransform(node);
    Matrix transform;
    convertMatrix(&globalTransform, transform);

    // Convert non-triangular meshes
    if(!mesh->IsTriangleMesh()) {
        KFbxGeometryConverter converter(_sdkManager);
        mesh = converter.TriangulateMesh(mesh);
    }

    // Get the vertices from the mesh
    unsigned int vertCount   = mesh->GetControlPointsCount();
    KFbxVector4 *vertex_data = mesh->GetControlPoints();

    // Prepare the vertex data
    std::vector <Vector3> verts;
    std::vector <Vector3> normals;
    std::vector <Vector2> texCoords;

    for(i = 0; i < vertCount; i++) {
        KFbxVector4 currentVert = vertex_data[i];
        verts.push_back(Vector3(currentVert.GetAt(0), currentVert.GetAt(1), currentVert.GetAt(2)));
    }

    // Get the layer count
    unsigned int layers = mesh->GetLayerCount();
    if(layers > 1) {
        Info("Warning: Multiple layers found in mesh, Only one will be used.");
    }

    // Get the normals and UV coordinates
    KFbxLayerElementNormal *fbxNormals = mesh->GetLayer(0)->GetNormals();
    KFbxLayerElementUV *fbxTexCoords = mesh->GetLayer(0)->GetUVs();

    // Prepare our other arrays for data
    if(fbxNormals) { normals.resize(verts.size()); }
    if(fbxTexCoords) { texCoords.resize(verts.size()); }

    // Count the indices
    unsigned int indexCount=0;
    for(i = 0; i < mesh->GetPolygonCount(); i++) {
        indexCount += mesh->GetPolygonSize(i);
    }

    // DEBUG ====
    bool *normalSet   = (bool*)calloc(sizeof(bool), verts.size());
    bool *texCoordSet = (bool*)calloc(sizeof(bool), verts.size());
    // ==========

    // Get the indices for each face
    std::vector <unsigned int> indices;
    for(i = 0; i < mesh->GetPolygonCount(); i++) {
        for(j = 0; j < mesh->GetPolygonSize(i); j++) {
            unsigned int index = mesh->GetPolygonVertex(i,j);
            Vector3 indexNormal;
            Vector2 indexTexCoord;

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

                indexNormal = Vector3(normal.GetAt(0), normal.GetAt(1), normal.GetAt(2));
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

                indexTexCoord = Vector2(texCoord.GetAt(0), texCoord.GetAt(1));
            }

            // DEBUG ====
            // THIS IS NOT PERMANENT CODE
            // There is a corner case in which this code produces duplicate vertices; namely,
            //  if a vertex exists with two sets of texcoords and said vertex exists in many places,
            //  every time after it is instantiated in which it is referred to by texcoords or normals
            //  which do not match the initial instantiation, a new duplicate will be created
            // That being said, this is much better than the case of brute-force loading each index as unique,
            //  which, in the case of the drunken master, means a difference between 12k and 3k unique 
            //  Vertex/Normal/TexCoord sets, where there are 2k unique vertices
            if((fbxNormals   && normalSet[index]   && indexNormal  != normals[index]  ) ||
               (fbxTexCoords && texCoordSet[index] && indexTexCoord != texCoords[index])) {
                // Keep track of the old vertex and get our new index
                Vector3 oldVertex = verts[index];
                index = verts.size();

                // Add a new copy of this vertex
                verts.push_back(oldVertex);

                // Resize the arrays
                normals.resize(verts.size());
                texCoords.resize(verts.size());
            }
            else {
                normalSet[index]   = true;
                texCoordSet[index] = true;
            }
            // ==========

            if(fbxNormals) {
                normals[index]   = indexNormal;
            }

            if(fbxTexCoords) {
                texCoords[index] = indexTexCoord;
            }

            indices.push_back(index);
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

    Info("Total vertex array size: " << verts.size());

    // Add the prim data to the model
    model->addMeshPart(&verts, &normals, &texCoords, &indices, matList.front(), transform);

    // DEBUG ====
    free(normalSet);
    free(texCoordSet);
    // ==========

    return true;
}

bool ModelFBX::Factory::parseMaterials(KFbxNode *node, ModelFBX *model, std::vector<Material*> *matList) {
    unsigned int i, matCount = 0;
    matCount = node->GetMaterialCount();

    for(i = 0; i < matCount; i++) {
        KFbxSurfaceMaterial *fbxMat = node->GetMaterial(i);
        Material *mat = new Material();

        // Get the textures for this layer
        std::vector <std::string> textureNames;
        int textureIndex;
        FOR_EACH_TEXTURE(textureIndex) {
            KFbxProperty prop = fbxMat->FindProperty(KFbxLayerElement::TEXTURE_CHANNEL_NAMES[textureIndex]);
            if(prop.IsValid()) {
                // Check for layered textures
                int layeredTextureCount = prop.GetSrcObjectCount(KFbxLayeredTexture::ClassId);
                if(layeredTextureCount > 0) {
                    THROW(NotImplementedError, "Layered textures not supported in ModelFBX");
                }

                // Check for typical textures
                int textureCount = prop.GetSrcObjectCount(KFbxTexture::ClassId);
                for(unsigned int j = 0; j < textureCount; j++) {
                    KFbxTexture *fbxTexture = KFbxCast <KFbxTexture> (prop.GetSrcObject(KFbxTexture::ClassId,j));
                    if(fbxTexture) {
                        textureNames.push_back(fbxTexture->GetRelativeFileName());
                    }
                }
            }
        }

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

        if(textureNames.size() > 1) {
            Info("Warning: Multitexturing not supported in MH, only one will be used.");
        }

        if(textureNames.size() >= 1) {
            Info("Found texture " << textureNames.front());
            Texture *textureToBind = _textureManager->getOrLoadResource(textureNames.front());
            mat->setTexture(textureToBind);
        }
        matList->push_back(mat);
    }

    return true;
}

void ModelFBX::Factory::convertMatrix(KFbxXMatrix *matrix, Matrix &mhMatrix) {
    float *values = new float[16];

    for(int i = 0; i < 4; i++) {
        KFbxVector4 row = matrix->GetRow(i);
        for(int j = 0; j < 4; j++) {
            values[(i*4) + j] = row.GetAt(j);
        }
    }

    mhMatrix = Matrix(values);
    delete values;
}

// =============================
// ModelFBX Function Definitions
// =============================

ModelFBX::ModelFBX(): Model() {}

void ModelFBX::addMeshPart(std::vector<Vector3> *verts, std::vector<Vector3> *norms, std::vector<Vector2> *texCoords, std::vector<unsigned int> *indices, Material *mat, Matrix const &transform) {
    // Add the new verts/norms/texCoords to the buffers
    // TODO - Eventually, we'll want to check for redundant verts in the array and cull them out as necessary
    unsigned int bufferOffset = _mutableVerts.size();
    _mutableVerts.insert(_mutableVerts.end(), verts->begin(), verts->end());

    if(norms != NULL) {
        _mutableNorms.insert(_mutableNorms.end(), norms->begin(), norms->end());
    }

    if(texCoords != NULL) {
        _mutableTexCoords.insert(_mutableTexCoords.end(), texCoords->begin(), texCoords->end());
    }

    _count += verts->size();

    // Add an offset to each new index, since its index value is offset by previously existing indices
    std::vector<unsigned int>::iterator indexItr;
    for(indexItr = indices->begin(); indexItr != indices->end(); indexItr++) {
        (*indexItr)+=bufferOffset;
    }
 
    // Add the new indices to the buffer
    unsigned int startIndex = _indexCount;
    _mutableIndices.insert(_mutableIndices.end(), indices->begin(), indices->end());
    _indexCount += indices->size();

    // Create the ModelMeshPart
    ModelMeshPart *newMeshPart = new ModelMeshPart(indices->size(), startIndex);
    newMeshPart->setMaterial(mat);

    // Create the bone
    ModelBone *newBone = new ModelBone(transform, getRootBone());

    // Create the ModelMesh
    ModelMesh newMesh(newMeshPart, 1);
    newMesh.setBone(newBone);

    _mutableMeshes.push_back(newMesh);
}

void ModelFBX::internVectors() {
    _verts     = vector_to_array(_mutableVerts);
    _norms     = vector_to_array(_mutableNorms);
    _texCoords = vector_to_array(_mutableTexCoords);
    _indices   = vector_to_array(_mutableIndices);

    _meshes    = vector_to_array(_mutableMeshes);
    _numMeshes = _mutableMeshes.size();

    findBounds();
    generateVBOs();
}
