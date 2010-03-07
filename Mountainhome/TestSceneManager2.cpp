/*
 *  TestSceneManager2.cpp
 *  Mountainhome
 *
 *  Created by loch on 3/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include <Render/MaterialManager.h>
#include <Render/Entity.h>
#include <Render/Model.h>
#include <Render/Node.h>

#include "TestSceneManager2.h"
#include "MHWorld.h"

class WorldEntity : public Model {
public:
    WorldEntity(unsigned short *indices, int indexCount, Vector3 *verts, Vector3 *norms, int vertexCount):
    _indices(indices), _indexCount(indexCount), _verts(verts), _norms(norms),
    _vertexCount(vertexCount) {
        for (int i = 0; i < _vertexCount; i++) {
            if (i == 0) { _boundingBox.setCenter(verts[i]); }
            else        { _boundingBox.encompass(verts[i]); }
        }
    }

    virtual ~WorldEntity() {
        delete []_verts; _verts = NULL;
        delete []_norms; _norms = NULL;
    }

    void render(RenderContext *context) {
        context->addToPrimitiveCount(_indexCount / 3 * 2);
        context->addToVertexCount(_vertexCount * 2);
        context->addToModelCount(1);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, _verts);
        glNormalPointer(GL_FLOAT, 0, _norms);

        context->setFilled();
        glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_SHORT, _indices);
        context->setWireFrame();
        glColor3f(0,0,0);
        glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_SHORT, _indices);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
    }

private:
    unsigned short *_indices;
    int _indexCount;

    Vector3 *_verts;
    Vector3 *_norms;
    int _vertexCount;
};

TestSceneManager2::TestSceneManager2(MHWorld *world): MHSceneManager(world) {}

TestSceneManager2::~TestSceneManager2() {}

int TestSceneManager2::getLowestAtCorner(int x, int y) {
    int min = _world->getHeight();
    MHWorld::Tile *t = NULL;

    // Base off of south west corner.
    if (t = _world->getTopTile(x,     y    )) { min = std::min(min, t->z); }
    if (t = _world->getTopTile(x - 1, y    )) { min = std::min(min, t->z); }
    if (t = _world->getTopTile(x,     y - 1)) { min = std::min(min, t->z); }
    if (t = _world->getTopTile(x - 1, y - 1)) { min = std::min(min, t->z); }

    return min;
}

void TestSceneManager2::populate() {
    // Build the vertex array by choosing the lowest z value of the 4 top level tiles
    // around every corner in the map.
    std::vector<Vector3> vertsArray;
    for (int x = 0; x <= _world->getWidth(); x++) {
        for (int y = 0; y <= _world->getHeight(); y++) {
            int z = getLowestAtCorner(x, y);
            vertsArray.push_back(Vector3(x * _tileWidth, y * _tileHeight, z * _tileDepth));
        }    
    }

    int vertexCount = vertsArray.size();
    Vector3 *vertices = vector_to_array(vertsArray);

    // Build the index array
    std::vector<unsigned short> indexArray;
    for (int x = 0; x < _world->getWidth(); x++) {
        for (int y = 0; y < _world->getHeight(); y++) {
            #define TRANSLATE(x, y) ((x) * (_world->getWidth() + 1) + (y))
            // South west triangle
            indexArray.push_back(TRANSLATE(x + 0, y + 0));
            indexArray.push_back(TRANSLATE(x + 1, y + 0));
            indexArray.push_back(TRANSLATE(x + 0, y + 1));

            // North east triangle
            indexArray.push_back(TRANSLATE(x + 0, y + 1));
            indexArray.push_back(TRANSLATE(x + 1, y + 0));
            indexArray.push_back(TRANSLATE(x + 1, y + 1));
            #undef TRANSLATE
        }    
    }

    int indexCount = indexArray.size();
    unsigned short *indices = vector_to_array(indexArray);


    // Calculate the normal for each vertex in the world by averaging the normal of all of
    // the faces a vertex is shared between.
    int *dependantTriangles = (int*)calloc(vertexCount, sizeof(int));
    Vector3 *normals = new Vector3[vertexCount];

    for (int i = 0; i < indexCount; i+=3) {
		Vector3 one = vertices[indexArray[i+1]] - vertices[indexArray[i+0]];
		Vector3 two = vertices[indexArray[i+2]] - vertices[indexArray[i+1]];
		Vector3 polyNormal = one.crossProduct(two);
        polyNormal.normalize();

		for (int j = 0; j < 3; j++) {
			dependantTriangles[indexArray[i+j]]++; 
			normals[indexArray[i+j]] += polyNormal;
		}
    }

	for (int i = 0; i < vertexCount; i++) {
        Info(i << ": " << dependantTriangles[i]);
		if (dependantTriangles[i] > 0) {
			normals[i] /= dependantTriangles[i];
		}
	}

    Entity *entity = createEntity(new WorldEntity(indices, indexCount, vertices, normals, vertexCount), "world");
    entity->setMaterial(MaterialManager::Get()->loadResource("white"));
    getRootNode()->attach(entity);
}
