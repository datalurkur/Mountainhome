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
//    glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
//    glBegin(GL_QUADS);
//       glVertex3f(0.0f,   0.0f,   0.0f);
//       glVertex3f(100.0f, 0.0f,   0.0f);
//       glVertex3f(100.0f, 100.0f, 0.0f);
//       glVertex3f(0.0f,   100.0f, 0.0f);
//    glEnd();

    glDisable(GL_TEXTURE_2D);
    glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(_pos[0], _pos[1], _pos[2]);
    glBegin(GL_LINES);
       glVertex3f(0.0f, 0.0f, 0.0f);
       glVertex3f(0.0f, 0.0f, _volume);
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
    for(int x=0; x<_rootPool->getDims()[0]; x++) {
        for(int y=0; y<_rootPool->getDims()[1]; y++) {
            std::list<Vector2> bounds;
            _rootPool->getTileBoundaries(Vector2(x,y), &bounds);

            std::list<Vector2>::iterator itr;
            for(itr = bounds.begin(); itr != bounds.end(); itr++) {
                int start, end;
                if((*itr)[1] == 1) {
                    start = (*itr)[0];
                    itr++;
                    if((*itr)[1] != 0) {
                        Error("Two ceilings in a row!  Faile!");
                    }
                    end = (*itr)[0];
                }
                else {
                    start = 0;
                    end = (*itr)[0];
                }

                for(int c=start; c<=end; c++) {
                    Info("Adding model");
                    TestLiquidModel *model = new TestLiquidModel(Vector3(x,y,c), 0.5);
                    std::string entityName = "drop" + to_s(x) + to_s(y) + to_s(c);
                    Entity *entity = scene->createEntity(model, entityName.c_str());
                    entity->setMaterial(mManager->getOrLoadResource("white"));
                    scene->getRootNode()->attach(entity);
                }
            }
            bounds.clear();
        }
    }
}
