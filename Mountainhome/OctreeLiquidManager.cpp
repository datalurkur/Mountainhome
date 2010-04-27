#include "OctreeLiquidManager.h"
#include <Render/Entity.h>
#include <Render/Node.h>

#include <Render/MaterialManager.h>

#include "OctreeSceneManager.h"

TestLiquidModel::TestLiquidModel(const Vector3 &position, const float &volume): _pos(position), _volume(volume) {
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

OctreeLiquidManager::OctreeLiquidManager(MHTerrain *terrain): MHLiquidManager(terrain) {
    int width = _terrain->getWidth(),
        height = _terrain->getHeight(),
        depth = _terrain->getDepth();
    _rootPool = new LiquidPool(Vector3(0, 0, 0), Vector3(width, height, depth), Vector2(0,0.0), 0);
}

OctreeLiquidManager::~OctreeLiquidManager() {
    if(_rootPool) {
        delete _rootPool;
        _rootPool = NULL;
    }
}

// Returns the type of liquid present in [x,y,z]
short OctreeLiquidManager::getLiquidType(int x, int y, int z) {
    return _rootPool->getTile(Vector3(x,y,z))[0];
}

// Returns the amount of liquid contained in [x,y,z]
float OctreeLiquidManager::getLiquidVolume(int x, int y, int z) {
    return _rootPool->getTile(Vector3(x,y,z))[1];
}

// Sets all variables for a given [x,y,z]
void OctreeLiquidManager::setLiquid(int x, int y, int z, short type, float depth) {
    _rootPool->setTile(Vector3(x,y,z), Vector2(type, depth));
}

// Sets the type of [x,y,z]
void OctreeLiquidManager::setLiquidType(int x, int y, int z, short type) {
    _rootPool->setTile(Vector3(x,y,z), Vector2(type, 1.0));
}

// Sets the depth of [x,y,z]
void OctreeLiquidManager::setLiquidDepth(int x, int y, int z, float depth) {
    short type = _rootPool->getTile(Vector3(x,y,z))[0];
    setLiquid(x,y,z,type,depth);
}

// Adds some amount of liquid to a pool, returning the amount left over if the
// destination pool is filled in the process
float OctreeLiquidManager::addDepth(int x, int y, int z, short type, float amount) {
    Vector2 cPool = _rootPool->getTile(Vector3(x,y,z));

    if(cPool[0] != type) {
        Info("Mixing liquids not yet supported");
        return amount;
    }

    float sum = cPool[1] + amount,
          remainder = 1.0 - sum;

    if(remainder >= 0.0) {
        setLiquid(x,y,z,type,1.0);
        return remainder;
    }
    else {
        setLiquid(x,y,z,type,sum);
        return 0.0;
    }
}

// Attempt to fill to the level specified, returning the difference
float OctreeLiquidManager::fillTo(int x, int y, int z, short type, float depth) {
    Vector2 cPool = _rootPool->getTile(Vector3(x,y,z));

    if(cPool[0] != type) {
        Info("Mixing liquids not yet supported");
        return 0.0;
    }

    float difference = cPool[1] - depth;

    setLiquid(x,y,z,type,depth);

    return difference;
}

void OctreeLiquidManager::populate(OctreeSceneManager *scene, MaterialManager *mManager) {
    Vector3 dims = _rootPool->getDims();

    for(int x=0; x<dims[0]; x++) {
        for(int y=0; y<dims[1]; y++) {
            std::list<Vector2> bounds;
            _rootPool->getTileBoundaries(Vector2(x,y), &bounds);

            std::list<Vector2>::iterator itr;
            for(itr = bounds.begin(); itr != bounds.end(); itr++) {
                int start, end;
                if((*itr)[1] == 1) {
                    start = (*itr)[0];
                    itr++;
                    if((*itr)[1] != 0) {
                        Error("Two ceilings in a row at " << x << "/" << y);
                    }
                    end = (*itr)[0];
                }
                else {
                    start = 0;
                    end = (*itr)[0];
                }

                // Add the water surface at this x/y pair
                TestLiquidModel *model = new TestLiquidModel(Vector3(x,y,end), 1.0);
                std::string entityName = "water" + to_s(x) + "," + to_s(y) + "," + to_s(end);
                Entity *entity = scene->createEntity(model, entityName.c_str());
                entity->setMaterial(mManager->getOrLoadResource("water.material"));
                scene->getRootNode()->attach(entity);

                // Check edges to see if we need to add a "skirt" to prevent gaps in the surface at shorelines
                for(int j=-1; j<=1; j++) {
                    for(int k=-1; k<=1; k++) {
                        // Ignore the local x/y pair
                        if(j==0 && k==0) { continue; }
                        int tX = x+j,
                            tY = y+k;

                        // Check for OOB
                        if(tX<0 || tY<0 || tX>=dims[0] || tY>=dims[1]) { continue; }

                        // Check the height of the terrain
                        int tHeight = _terrain->getSurfaceLevel(tX, tY);

                        // If the terrain is at the level of the water or higher, add a water "skirt"
                        if(tHeight>=end) {
                            std::string skirtName = "skirt" + to_s(tX) + "," + to_s(tY) + "," + to_s(end);

                            // Check to see if a skirt already exists at this location
                            if(scene->hasEntity(skirtName.c_str())) { continue; }

                            TestLiquidModel *sModel = new TestLiquidModel(Vector3(tX, tY, end), 1.0);
                            Entity *sEntity = scene->createEntity(sModel, skirtName.c_str());
                            sEntity->setMaterial(mManager->getOrLoadResource("water.material"));
                            scene->getRootNode()->attach(sEntity);
                        }
                    }
                }
            }
            bounds.clear();
        }
    }
}
