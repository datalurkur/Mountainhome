/*
 *  Model.h
 *  Engine
 *
 *  Created by Brent Wilson on 3/29/05.
 *  Copyright 2005 Brent Wilson. All rights reserved.
 *
 */

#include <Base/Plane.h>
#include "RenderContext.h"
#include "GL_Helper.h"
#include "Model.h"

Model::Model(): _texCoords(NULL), _verts(NULL), _norms(NULL), _count(0),
_indices(NULL), _indexCount(0), _indexBuffer(0), _vertexBuffer(0), _normalBuffer(0),
_texCoordBuffer(0), _drawVerts(false), _drawNormals(false), _drawAABB(false),
_numMeshes(0), _defaultMaterial(NULL)
{}

Model::Model(Vector3 *verts, Vector3 *norms, Vector2 *texCoords, int vertexCount,
unsigned int *indices, int indexCount): _texCoords(texCoords), _verts(verts),
_norms(norms), _count(vertexCount), _indices(indices), _indexCount(indexCount),
_indexBuffer(0), _vertexBuffer(0), _normalBuffer(0), _texCoordBuffer(0),
_drawVerts(false), _drawNormals(false), _drawAABB(false),
_numMeshes(0), _defaultMaterial(NULL)
{
    findBounds();
    generateVBOs();
}

Model::~Model() {
    clear();
}

const AABB3& Model::getBoundingBox() const { return _boundingBox; }

ModelMesh* Model::getMesh(int index) { return &_meshes[index]; }

void Model::clear() {
    if (_verts)     { delete []_verts;     _verts     = NULL; }
    if (_norms)     { delete []_norms;     _norms     = NULL; }
    if (_texCoords) { delete []_texCoords; _texCoords = NULL; }
    if (_indices)   { delete []_indices;   _indices   = NULL; }

    if (_indexBuffer   ) { glDeleteBuffers(1, &_indexBuffer   ); _indexBuffer    = 0; }
    if (_vertexBuffer  ) { glDeleteBuffers(1, &_vertexBuffer  ); _vertexBuffer   = 0; }
    if (_normalBuffer  ) { glDeleteBuffers(1, &_normalBuffer  ); _normalBuffer   = 0; }
    if (_texCoordBuffer) { glDeleteBuffers(1, &_texCoordBuffer); _texCoordBuffer = 0; }

    _indexCount = 0;
    _count = 0;
}

void Model::findBounds() {
    _boundingBox.setRadius(Vector3(0, 0, 0));
    for (int i = 0; i < (_indices ? _indexCount : _count); i++) {
        Vector3 &vector = _indices ? _verts[_indices[i]] : _verts[i];
        if (i == 0) { _boundingBox.setCenter(vector); }
        else        { _boundingBox.encompass(vector); }
    }
}

void Model::generateVBOs() {
    if (_verts) {
        glGenBuffers(1, &_vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, _count * sizeof(Vector3), _verts, GL_STATIC_DRAW);
    }

    if (_norms) {
        glGenBuffers(1, &_normalBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _normalBuffer);
        glBufferData(GL_ARRAY_BUFFER, _count * sizeof(Vector3), _norms, GL_STATIC_DRAW);
    }

    if (_texCoords) {
        glGenBuffers(1, &_texCoordBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _texCoordBuffer);
        glBufferData(GL_ARRAY_BUFFER, _count * sizeof(Vector2), _texCoords, GL_STATIC_DRAW);
    }

    if (_indices) {
        glGenBuffers(1, &_indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexCount * sizeof(unsigned int), _indices, GL_STATIC_DRAW);
    }
}

void Model::setDefaultMaterial(Material *mat) {
    _defaultMaterial = mat;
}

void Model::generateNormals() {
    ASSERT(_count);
    ASSERT(_indices);
    ASSERT(_verts);

    if (_norms) {
        delete[] _norms;
    }

    _norms = new Vector3[_count];
    memset(_norms, 0, sizeof(Vector3) * _count);

    for (int i = 0; i < _indexCount; i+=3) {
        Vector3 one = _verts[_indices[i+1]] - _verts[_indices[i+0]];
        Vector3 two = _verts[_indices[i+2]] - _verts[_indices[i+1]];
        Vector3 polyNormal = one.crossProduct(two);
        polyNormal.normalize();

        for (int j = 0; j < 3; j++) {
            _norms[_indices[i+j]] += polyNormal;
        }
    }

    for (int i = 0; i < _count; i++) {
        _norms[i].normalize();
    }
}


//class LODIndexArray {
//    struct Face;
//    typedef          int IndexArrayIndex; // To accommodate -1
//    typedef unsigned int VertexArrayIndex;
//    typedef std::list<Face*>  FaceList;
//    typedef std::map<VertexArrayIndex, FaceList> FaceMap;
//
//    class Face {
//        VertexArrayIndex *_indices;
//        Vector3 *_verts;
//        bool _isFlat;
//
//    public:
//        VertexArrayIndex vertexArrayIndices[3];
//        IndexArrayIndex   indexArrayIndices[3];
//        Vector3 normal;
//
//        Face(int index, VertexArrayIndex *indices, Vector3 *verts): _verts(verts), _indices(indices) {
//            for (int i = 0; i < 3; i++) {
//                indexArrayIndices[i] = index+i;
//            }
//
//            update();
//        }
//
//        inline int indexOfVertexIndex(VertexArrayIndex vertIndex) {
//            for (int i = 0; i < 3; i++) {
//                if (vertexArrayIndices[i] == vertIndex) {
//                    return i;
//                }
//            }
//
//            // Couldn't find it.
//            return -1;
//        }
//
//        inline bool isFlat() {
//            return _isFlat;
//        }
//
//        inline bool hasVertexArrayIndex(VertexArrayIndex index) {
//            return vertexArrayIndices[0] == index ||
//                   vertexArrayIndices[1] == index ||
//                   vertexArrayIndices[2] == index;
//        }
//
//        void update() {
//            // Update the vertexArrayIndices.
//            for (int i = 0; i < 3; i++) {
//                vertexArrayIndices[i] = _indices[indexArrayIndices[i]];
//            }
//
//            // Calculate the normal.
//            Vector3 one = _verts[vertexArrayIndices[1]] - _verts[vertexArrayIndices[0]];
//            Vector3 two = _verts[vertexArrayIndices[2]] - _verts[vertexArrayIndices[1]];
//            normal = one.crossProduct(two);
//            normal.normalize();
//
//            // Calculate the isFlat component.
//            _isFlat = normal == Vector3(0.0);
//        }
//    };
//
//private:
//    FaceList _allFaces;
//    FaceMap  _faceMapping;
//
//    int _indexCount;
//    VertexArrayIndex *_indices;
//
//    int _vertCount;
//    Vector3 *_verts;
//    Vector3 *_normals;
//
//    AABB3 _aabb;
//
//private:
//    void getFacesUsingIndexFromList(VertexArrayIndex index, const FaceList &from, FaceList &to) {
//        FaceList::const_iterator itr = from.begin();
//        for (; itr != from.end(); itr++) {
//            if ((*itr)->hasVertexArrayIndex(index)) {
//                to.push_back(*itr);
//            }
//        }
//    }
//
//    float calculateCurvature(VertexArrayIndex indexA, VertexArrayIndex indexB) {
//        FaceList aFaces, bFaces;
//        aFaces = _faceMapping[indexA];
//        getFacesUsingIndexFromList(indexB, aFaces, bFaces);
//        // ASSERT(bFaces.size() > 0);
//
//        float totalCurvature = 0;
//        FaceList::iterator aItr, bItr;
//        for (aItr = aFaces.begin(); aItr != aFaces.end(); aItr++) {
//            // Compare each of the shared faces to each of indexA's faces, looking for the
//            // smallest curvature. This will be the cost of the individual face that will
//            // be disapearing. Note, this is what allows us to drop edges that are
//            // actually on edges. Consider the cube example, where there is a vertex
//            // sitting on a cube edge. We know we can make it go away, at zero cost, and
//            // this is how that is accounted for.
//            float minCurvature = 1;
//            for (bItr = bFaces.begin(); bItr != bFaces.end(); bItr++) {
//                //Info("    Comparing for merge " << _verts[_indices[indexA]] << " to " << _verts[_indices[indexB]]);
//                float twoFaceCurvature = 0;
//
//                // Determine the visible impact this merge would have by examining all of
//                // the normals associated with the two faces.
//                for (int i = 0; i < 3; i++) {
//                    for (int j = 0; j < 3; j++) {
//                        Vector3 *normalA = _normals + (*aItr)->vertexArrayIndices[i];
//                        Vector3 *normalB = _normals + (*bItr)->vertexArrayIndices[j];
//                        float twoNormalCurvature = (1 - normalA->dotProduct(*normalB)) / 2.0;
//                        twoFaceCurvature = std::max(twoFaceCurvature, twoNormalCurvature);
//                        //Info("        " << *normalA << " x " << *normalB << " = " << ((1 - normalA->dotProduct(*normalB)) / 2.0) << " [" << twoFaceCurvature << "]");
//                    }
//                }
//
//                minCurvature = std::min(minCurvature, twoFaceCurvature);
//            }
//            totalCurvature = std::max(totalCurvature, minCurvature);
//        }
//
//        return totalCurvature;
//    }
//
//    /*! Gets a foating point value representing the cost of collapsing the vertex at
//     *  indexA into the vertex at indexB. This value is computed as a function of the
//     *  length of the edge we'd be collapsing (longer edges mean smaller differences will
//     *  have a greater impact) and a value representing the curvature of the polygons
//     *  around indexA. */
//    float determineCost(VertexArrayIndex indexA, VertexArrayIndex indexB) {
//        float edgeLength = (_verts[indexA] - _verts[indexB]).lengthSquared();
//        return edgeLength * calculateCurvature(indexA, indexB);
//    }
//
//    int findIndexToMergeWith(IndexArrayIndex index, float maxCost = 0.0) {
//        //Info("findIndexToMergeWith: " << _verts[_indices[index]]);
//        // Never merge corners into anything.
//        if (isOnCorner(index)) { return -1; }
//
//        FaceList &faceList = _faceMapping[_indices[index]];
//        float minCost = maxCost;
//        int minIndex = -1;
//
//        FaceList::iterator itr = faceList.begin();
//        for (; itr != faceList.end(); itr++) {
//            if ((*itr)->isFlat()) { continue; }
//            for (int i = 0; i < 3; i++) {
//                int curIndex = (*itr)->indexArrayIndices[i];
//                if (_indices[index] != _indices[curIndex]) {
//                    //Info("findIndexToMergeWith - trying: " << _verts[_indices[index]] << " => " << _verts[_indices[curIndex]]);
//
//                    // Never merge boundry verts that don't share a boundry.
//                    if (!canMerge(index, curIndex)) { continue; }
//
//                    float curCost = determineCost(_indices[index], _indices[curIndex]);
//                    //Info("    Cost: " << curCost);
//
//                    if (curCost >= 0 && curCost <= minCost) {
//                        minIndex = curIndex;
//                        minCost = curCost;
//                    }
//                }
//            }
//        }
//
//        return minIndex;
//    }
//
//    void calculateStuff() {
//        clear_list(_allFaces);
//        _faceMapping.clear();
//
//        // Build all of the faces.
////        Info("Index Count: " << _indexCount);
//        for (int i = 0; i < _indexCount; i+=3) {
//            Face *face = new Face(i, _indices, _verts);
//            if (!face->isFlat()) {
//                _allFaces.push_back(face);
//            } else {
//                delete face;
//            }
//        }
//
//        delete[] _indices;
//        _indexCount = _allFaces.size() * 3;
//        _indices = new VertexArrayIndex[_indexCount];
//        FaceList::iterator itr = _allFaces.begin();
//        for (int i = 0; itr != _allFaces.end(); itr++, i+=3) {
//            for (int j = 0; j < 3; j++) {
//                _indices[i+j] = (*itr)->vertexArrayIndices[j];
//                (*itr)->indexArrayIndices[j] = i+j;
//            }
//        }
//
//        // Map all of the vertices to faces.
//        FaceList tempList;
//        for (int i = 0; i < _vertCount; i++) {
//            getFacesUsingIndexFromList(i, _allFaces, tempList);
//            _faceMapping[i] = tempList;
//            tempList.clear();
//        }
//    }
//
//    bool isOnBoundry(IndexArrayIndex index) {
//        Vector3 vert = _verts[_indices[index]];
//        Vector3 min = _aabb.getMin();
//        Vector3 max = _aabb.getMax();
//        return Math::eq(vert.x, min.x) || Math::eq(vert.x, max.x) ||
//               Math::eq(vert.y, min.y) || Math::eq(vert.y, max.y);
//    }
//
//    bool shareBoundry(IndexArrayIndex indexA, IndexArrayIndex indexB) {
//        Vector3 vertA = _verts[_indices[indexA]];
//        Vector3 vertB = _verts[_indices[indexB]];
//        Vector3 min = _aabb.getMin();
//        Vector3 max = _aabb.getMax();
//
//        return (Math::eq(vertA.x, min.x) && Math::eq(vertB.x, min.x)) ||
//               (Math::eq(vertA.x, max.x) && Math::eq(vertB.x, max.x)) ||
//               (Math::eq(vertA.y, min.y) && Math::eq(vertB.y, min.y)) ||
//               (Math::eq(vertA.y, max.y) && Math::eq(vertB.y, max.y));
//    }
//
//    bool wouldResultInFold(IndexArrayIndex startingIndex, IndexArrayIndex endingIndex) {
//        FaceList &faces = _faceMapping[_indices[startingIndex]];
//
////        Info("wouldResultInFold: " << _verts[_indices[startingIndex]] << " => " << _verts[_indices[endingIndex]]);
////        Info("Faces to check: " << faces.size());
//
//        FaceList::iterator itr = faces.begin();
//        for (; itr != faces.end(); itr++) {
//            // Get the vertices associated with the starting and ending points.
//            Vector3 *starting = _verts + _indices[startingIndex];
//            Vector3 *ending   = _verts + _indices[endingIndex];
//
//            // Get the vertices associated with the two static points.
//            int i = (*itr)->indexOfVertexIndex(_indices[startingIndex]);
//            Vector3 *otherA = _verts + (*itr)->vertexArrayIndices[(i+1) % 3];
//            Vector3 *otherB = _verts + (*itr)->vertexArrayIndices[(i+2) % 3];
//            ASSERT_GE(i, 0); // All faces here should definitely contain the starting vertex.
//
//            // Transform everything so that otherA is at the origin (simplifies plane calculations).
//            Vector3 tOtherB   = (*otherB) - (*otherA);
//            Vector3 tStarting = (*starting) - (*otherA);
//            Vector3 tEnding   = (*ending)   - (*otherA);
//
//            // Find the normal of the plane we're comparing to.
//            Vector3 line = tOtherB.getNormalized();
//            Vector3 normal = tStarting - (line * line.dotProduct(tStarting));
//            normal.normalize();
//
//
////            Info("From: " << *starting << ", " << *otherA << ", " << *otherB);
////            Info("To:   " << *ending   << ", " << *otherA << ", " << *otherB);
////
////
////            Info("Details");
////            LogStream::IncrementIndent();
////            Info(line);
////            Info(tStarting);
////            Info(line.dotProduct(tStarting));
////            Info(line * (line.dotProduct(tStarting)));
////            Info(tStarting - (line * (line.dotProduct(tStarting))));
////            Info(normal);
////            LogStream::DecrementIndent();
//
//
//            // Check to see if the new end point will cross the plane.
//            if (!Plane(normal, 0).isInFrontOrOn(tEnding)) {
//                return true;
//            }
//        }
//
//        return false;
//    }
//
//    bool canMerge(IndexArrayIndex indexA, IndexArrayIndex indexB) {
////        LogStream::IncrementIndent();
////        Info("canMerge " << _verts[_indices[indexA]] << " into " << _verts[_indices[indexB]]);
////        LogStream::IncrementIndent();
//
//        bool a = !isOnBoundry(indexA);
//        bool b = shareBoundry(indexA, indexB);
//        bool c = wouldResultInFold(indexA, indexB);
//
////        Info("Got: (" << a << " || " << b << ") && !" << c);
////        LogStream::DecrementIndent();
////        LogStream::DecrementIndent();
//
//        return (a || b) && !c;
//    }
//
//    bool isOnCorner(IndexArrayIndex index) {
//        Vector3 vert = _verts[_indices[index]];
//        Vector3 min = _aabb.getMin();
//        Vector3 max = _aabb.getMax();
//        return (Math::eq(vert.x, min.x) || Math::eq(vert.x, max.x)) &&
//               (Math::eq(vert.y, min.y) || Math::eq(vert.y, max.y));
//    }
//public:
//    LODIndexArray(int indexCount, VertexArrayIndex *indices, int vertCount, Vector3 *verts, Vector3 *normals, AABB3 aabb):
//    _indexCount(indexCount), _indices(indices), _vertCount(vertCount), _verts(verts), _normals(normals), _aabb(aabb) {
//        ASSERT(_indexCount % 3 == 0);
//    }
//
//    virtual ~LODIndexArray() {
//        // _indices is expected to be managed by the creating entity.
//        clear_list(_allFaces);
//    }
//
//    VertexArrayIndex* getPtr(int lod = 0) {
//        return _indices;
//    }
//
//    int getCount(int lod = 0) {
//        return _indexCount;
//    }
//
//    void reduce() {
//        int initialIndexCount = _indexCount;
//        int mergeCount = 0;
//
//        calculateStuff();
//
//        Info("Reducing poly count! Starting at " << _indexCount / 3);
//        for (IndexArrayIndex indexA = 0; indexA < _indexCount; indexA++) {
//
////            if (mergeCount >= 30) { return false; }
//
//            IndexArrayIndex indexB = findIndexToMergeWith(indexA);
//            if (indexB >= 0) {
//                Info("Merging " << indexA << " -> " << _indices[indexA] << " " << _verts[_indices[indexA]] << " "
//                     "into "    << indexB << " -> " << _indices[indexB] << " " << _verts[_indices[indexB]]);
//
//                // Update the indices directly.
//                VertexArrayIndex toMerge = _indices[indexA];
//                for (int i = 0; i < _indexCount; i++) {
//                    if (_indices[i] == toMerge) {
//                        _indices[i] = _indices[indexB];
//                    }
//                }
//
//                // Move all associated faces over from A to B, updating them as we go.
//                FaceList &toUpdate = _faceMapping[toMerge];
//                FaceList::iterator itr = toUpdate.begin();
//                for (; itr != toUpdate.end(); itr++) {
//                    _faceMapping[_indices[indexB]].push_back(*itr);
//                    (*itr)->update();
//                }
//
//                toUpdate.clear();
//
//                // And update the merge count.
//                mergeCount++;
//            }
//        }
//
//        calculateStuff();
//
//        Info("Made " << mergeCount << " merges this run.");
//        Info("Final poly count: " << _indexCount / 3 << " (" <<
//            100.0 * _indexCount / initialIndexCount << "% of starting value)");
//    }
//};
