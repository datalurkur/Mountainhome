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
#include <Render/VertexArray.h>
#include <Render/IndexBuffer.h>
#include <Render/ModelMesh.h>
#include <Render/RenderOperation.h>

#include <algorithm>

#include "TextureManager.h"
#include "LambertMaterial.h"

// ======================================
// ModelFBX::Factory Function Definitions
// ======================================

// Have a function that takes an array of Vector3s and a size , or a vertex, and generates
// a bounding box, to encapsulate the functionality. Should probably be in AABB and be
// generic for different sizes. Need to update VectorArray to support a standard position
// setter and to use the non-generic buffer setter for position in OpenGL. Might want to
// do the same for normals and texcoords. This should simplify writing shaders and absolve
// the need for special and arbitrary buffer attribute names. Might be a great place to
// call the generate bounds function I mentioned above. Make ModelFBX not a model. It
// should be a factory and capable of building a Model externally. VectorArray might be a
// great place to put generic polygon reduction, too?

// I might want to turn ModelFBX into a partial model, to aid in thread safety when
// building, that gets thrown away when finished, but I don't like it being an actual
// Model that we return. Need to consider this.

ModelFBXFactory::ModelFBXFactory(
    ResourceGroupManager *manager,
    TextureManager *tManager
):
    ResourceFactory<Model>(manager),
    _textureManager(tManager),
    _sdkManager(NULL),
    _importer(NULL),
    _scene(NULL)
{
    _sdkManager = KFbxSdkManager::Create();
    _importer = KFbxImporter::Create(_sdkManager, "");
}

ModelFBXFactory::~ModelFBXFactory() {
    _importer->Destroy();
    _sdkManager->Destroy();
}

bool ModelFBXFactory::canLoad(const std::string &name) {
    std::string ext;
    FileSystem::ExtractExtension(name, ext, true);
    return ext == "fbx";
}

Model *ModelFBXFactory::load(const std::string &name) {
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

    std::vector<ModelMesh *> meshes;
    buildModelMeshesFromScene(rootNode, meshes);
    _scene->Destroy();

    return new Model(name, meshes);
}

void ModelFBXFactory::buildModelMeshesFromScene(KFbxNode *node, std::vector<ModelMesh *> &meshes) {
    // Query the node name ...
    KString nodeName = node->GetName();

    // ... and attribute type
    // If this node as an attribute, it might have meshes/materials/data we care about
    KFbxNodeAttribute *attr = node->GetNodeAttribute();
    if(attr != NULL) {
        // Get the type of attribute present, and branch accordingly
        switch (attr->GetAttributeType()) {
        case KFbxNodeAttribute::eMESH:      meshes.push_back(fbxMeshToModelMesh((KFbxMesh*)attr)); break;
        case KFbxNodeAttribute::eSKELETON: Info("Skipping eSKELETON node"); break; // FIXME: build some bones, eventually.
        case KFbxNodeAttribute::eMARKER:   Info("Skipping eMARKER node");   break; // Don't care.
        case KFbxNodeAttribute::eLIGHT:    Info("Skipping eLIGHT node");    break; // Don't care.
        case KFbxNodeAttribute::eCAMERA:   Info("Skipping eCAMERA node");   break; // Don't care.
        case KFbxNodeAttribute::eBOUNDARY: Info("Skipping eBOUNDARY node"); break; // Don't care.
        default: Info("Skipping node of type " << attr->GetAttributeType());
        }
    }

    // Parse the child nodes
    for(int i = 0; i < node->GetChildCount(); i++) {
        buildModelMeshesFromScene(node->GetChild(i), meshes);
    }
}

ModelMesh * ModelFBXFactory::fbxMeshToModelMesh(KFbxMesh *mesh) {
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

    // DEBUG ====
    free(normalSet);
    free(texCoordSet);
    // ==========

    // Get the materials from the parent layer
    std::vector <Material*> matList;
    parseMaterialsFromNode(node, matList);
    if(matList.size() > 1) {
        Warn("Multitexturing is currently not supported in Mountainhome, "
             "only one material will be used.");
    }

    Info("Total vertex array size: " << verts.size());

    // Create the ModelMesh and return the result.
    IndexBuffer *indexBuffer = new IndexBuffer(GL_STATIC_DRAW, GL_UNSIGNED_INT, indices.size(), &indices[0]);
    VertexArray *vertexArray = new VertexArray();
    vertexArray->addAttributeBuffer("position", new AttributeBuffer(GL_STATIC_DRAW, GL_FLOAT, 3, verts.size(), &verts[0]));
    vertexArray->addAttributeBuffer("normals",  new AttributeBuffer(GL_STATIC_DRAW, GL_FLOAT, 3, normals.size(), &normals[0]));
    vertexArray->addAttributeBuffer("texcoord", new AttributeBuffer(GL_STATIC_DRAW, GL_FLOAT, 2, texCoords.size(), &texCoords[0]));

    AABB3 bounds = AABB3::FindBounds(&verts[0], verts.size());
    RenderOperation *op = new RenderOperation(TRIANGLES, vertexArray, indexBuffer);
    return new ModelMesh(mesh->GetName(), op, matList.front(), NULL, bounds);
}

void ModelFBXFactory::parseMaterialsFromNode(KFbxNode *node, std::vector<Material*> &matList) {
    unsigned int i, matCount = 0;
    matCount = node->GetMaterialCount();

    for(i = 0; i < matCount; i++) {
        KFbxSurfaceMaterial *fbxMat = node->GetMaterial(i);
        LambertMaterial *mat = new LambertMaterial();

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
            KFbxPropertyDouble3 ambient = lSurface->GetAmbientColor();
            KFbxPropertyDouble3 diffuse = lSurface->GetDiffuseColor();
            float alpha = 1.0 - lSurface->GetTransparencyFactor().Get();
            mat->setAmbient(Vector4(ambient.Get()[0], ambient.Get()[1], ambient.Get()[2], alpha));
            mat->setDiffuse(Vector4(diffuse.Get()[0], diffuse.Get()[1], diffuse.Get()[2], alpha));
        }
        else if(fbxMat->GetClassId().Is(KFbxSurfacePhong::ClassId)) {
            THROW(NotImplementedError, "Phong surfaces are not supported by ModelFBX");
        }
        else {
            THROW(NotImplementedError, "Unhandled material class found in ModelFBX");
        }

        if(textureNames.size() > 1) {
            Warn("Multitexturing not supported by lambert shader, yet.");
        }

        if(textureNames.size() >= 1) {
            Warn("Texturing not supported by lambert shader, yet.");
            // Info("Found texture " << textureNames.front());
            // Texture *textureToBind = _textureManager->getOrLoadResource(textureNames.front());
            // mat->setTexture(textureToBind);
        }

        matList.push_back(mat);
    }
}

void ModelFBXFactory::convertMatrix(KFbxXMatrix *matrix, Matrix &mhMatrix) {
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
