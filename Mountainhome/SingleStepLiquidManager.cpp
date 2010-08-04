/*
 *  SingleStepLiquidManager.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "SingleStepLiquidManager.h"
#include <Render/Entity.h>
#include <Render/SceneNode.h>

#include <Render/MaterialManager.h>

#include "OctreeSceneManager.h"
#include "OctreeTileGrid.h"

TestLiquidModel::TestLiquidModel(const Vector3 &position, const float &volume)
: _pos(position), _volume(volume)
{
    _boundingBox = AABB3(Vector3(0.0), Vector3(100.0));

}
TestLiquidModel::~TestLiquidModel() {}

void TestLiquidModel::render(RenderContext *context) {
    float t_x = (float)_pos[0] * 0.1,
          t_y = (float)_pos[1] * 0.1;

    glPushMatrix();
    glTranslatef(_pos[0], _pos[1], _pos[2]);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(t_x,      t_y);      glVertex3f(0.0f, 0.0f, -0.1f);
        glTexCoord2f(t_x+0.1f, t_y);      glVertex3f(1.0f, 0.0f, -0.1f);
        glTexCoord2f(t_x+0.1f, t_y+0.1f); glVertex3f(1.0f, 1.0f, -0.1f);
        glTexCoord2f(t_x,      t_y+0.1f); glVertex3f(0.0f, 1.0f, -0.1f);
    glEnd();
    glPopMatrix();
}

SingleStepLiquidManager::SingleStepLiquidManager(MHTerrain *terrain,
OctreeSceneManager *scene, MaterialManager *manager)
: _rootPool(NULL), _terrain(terrain), _sceneManager(scene), _materialManager(manager)
{
    _rootPool = new OctreeTileGrid(_terrain->getWidth(), _terrain->getHeight(),
        _terrain->getDepth(), Vector3(0, 0, 0), LIQUID_EMPTY, NULL);
}

SingleStepLiquidManager::~SingleStepLiquidManager() {
    delete _rootPool;
    _rootPool = NULL;
}

TileType SingleStepLiquidManager::getLiquidType(int x, int y, int z) {
    return _rootPool->getTile(x, y, z);
}

float SingleStepLiquidManager::getLiquidVolume(int x, int y, int z) {
    return _rootPool->getTile(x, y, z) ? 1.0 : 0.0;
}

void SingleStepLiquidManager::setLiquid(int x, int y, int z, TileType type, float depth) {
    _rootPool->setTile(x, y, z, type);
}

void SingleStepLiquidManager::setLiquidType(int x, int y, int z, TileType type) {
    _rootPool->setTile(x, y, z, type);
}

void SingleStepLiquidManager::setLiquidDepth(int x, int y, int z, float depth) {}

void SingleStepLiquidManager::populate() {
//    for(int x = 0; x < _rootPool->getWidth(); x++) {
//        for(int y = 0; y < _rootPool->getHeight(); y++) {
//            std::list<Vector2> bounds;
//            _rootPool->getTileBoundaries(Vector2(x,y), &bounds);
//
//            std::list<Vector2>::iterator itr;
//            for(itr = bounds.begin(); itr != bounds.end(); itr++) {
//                int start, end;
//                if((*itr)[1] == 1) {
//                    start = (*itr)[0];
//                    itr++;
//                    if((*itr)[1] != 0) {
//                        Error("Two ceilings in a row at " << x << "/" << y);
//                    }
//                    end = (*itr)[0];
//                }
//                else {
//                    start = 0;
//                    end = (*itr)[0];
//                }
//
//                // Add the water surface at this x/y pair
//                TestLiquidModel *model = new TestLiquidModel(Vector3(x,y,end), 1.0);
//                std::string entityName = "water" + to_s(x) + "," + to_s(y) + "," + to_s(end);
//                Entity *entity = _sceneManager->createEntity(model, entityName.c_str());
//                entity->setMaterial(_materialManager->getOrLoadResource("water.material"));
//
//                // Check edges to see if we need to add a "skirt" to prevent gaps in the surface at shorelines
//                for(int j=-1; j<=1; j++) {
//                    for(int k=-1; k<=1; k++) {
//                        // Ignore the local x/y pair
//                        if(j==0 && k==0) { continue; }
//                        int tX = x+j,
//                            tY = y+k;
//
//                        // Check for OOB
//                        if(tX<0 || tY<0 || tX>=dims[0] || tY>=dims[1]) { continue; }
//
//                        // Check the height of the terrain
//                        int tHeight = _terrain->getSurfaceLevel(tX, tY);
//
//                        // If the terrain is at the level of the water or higher, add a water "skirt"
//                        if(tHeight>=end) {
//                            std::string skirtName = "skirt" + to_s(tX) + "," + to_s(tY) + "," + to_s(end);
//
//                            // Check to see if a skirt already exists at this location
//                            if(_sceneManager->hasEntity(skirtName.c_str())) { continue; }
//
//                            TestLiquidModel *sModel = new TestLiquidModel(Vector3(tX, tY, end), 1.0);
//                            Entity *sEntity = _sceneManager->createEntity(sModel, skirtName.c_str());
//                            sEntity->setMaterial(_materialManager->getOrLoadResource("water.material"));
//                        }
//                    }
//                }
//            }
//            bounds.clear();
//        }
//    }
}
