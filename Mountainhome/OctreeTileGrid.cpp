/*
 *  OctreeTileGrid.hpp
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

///\todo XXXBMW: Consider making a copy c'tor of time kind to avoid having to pass around w, h, d, pos, type, parent everywhere.
///\todo XXXBMW: Consider a 'dimensions' class to avoid having to pass w/h/d everywhere.

#include <Base/FileSystem.h>

#include "OctreeTileGrid.h"

#define IS_UPPER_X(index) index & 0x4
#define IS_UPPER_Y(index) index & 0x2
#define IS_UPPER_Z(index) index & 0x1

#define USE_POOL 0

#if USE_POOL
#define DELETE_GROUP(group) _pool->putOctreeTileGrid(group)
#define NEW_GROUP(w, h, d, pos, tile, parent) _pool->getOctreeTileGrid((w), (h), (d), (pos), (tile), (parent))
#else
#define DELETE_GROUP(group) do { delete group; group = NULL; } while (0)
#define NEW_GROUP(w, h, d, pos, tile, parent) new OctreeTileGrid((w), (h), (d), (pos), (tile), (parent))
#endif

struct OctreeTileGrid::Chunk {
    OctreeTileGrid group;
    Chunk *next;
};

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark OctreeTileGridPool
//////////////////////////////////////////////////////////////////////////////////////////
OctreeTileGrid::OctreeTileGridPool::OctreeTileGridPool(int initialSize, OctreeTileGrid *parent): _parent(parent), _available(initialSize), _used(0) {
    Info("Creating OctreeTileGridPool with initial size " << _available);
    _basePtr = (Chunk*)malloc(sizeof(Chunk) * _available);
    _freePtr = _basePtr;
    _lastPtr = NULL;

    // Initialize all of our chunks.
    for (int i = 0; i < _available; i++) {
        _basePtr[i].next = (i == (_available - 1)) ? NULL : &_basePtr[i+1];
        _basePtr[i].group.setPool(this);
    }

    printStats();
}


OctreeTileGrid::OctreeTileGridPool::~OctreeTileGridPool() {
    free(_basePtr);
}

OctreeTileGrid* OctreeTileGrid::OctreeTileGridPool::getOctreeTileGrid(int width,
int height, int depth, const Vector3 &pos, Tile tile, OctreeTileGrid* parent)
{
    // If there are no groups available, make one.
    if (_available == 0) {
        THROW(InternalError, "This doesn't work!!!");

        // Add on to the existing chunk and update the count.
        Chunk *newLoc = (Chunk*)realloc(_basePtr, _used * 2);
        _available = _used;

        // The new location ABSOLUTELY MUST be the same as the old!!!!!
        ASSERT_EQ(_basePtr, newLoc);

        // Initialize all of the new chunks.
        Chunk *newChunks = _basePtr + _used;
        for (int i = 0; i < _available; i++) {
            newChunks[i].next = (i == (_available - 1)) ? NULL : &newChunks[i+1];
            newChunks[i].group.setPool(this);
        }

        // And update the free and last pointer values.
        ASSERT(_lastPtr->next == NULL);
        ASSERT(_freePtr == NULL);

        _freePtr = _lastPtr->next = newChunks;
    }

    // Update counts
    _available--;
    _used++;

    // Update the free and last chunk pointers.
    _lastPtr = _freePtr;
    _freePtr = _freePtr->next;

    // Initialize the group and return.
    return _lastPtr->group.initialize(width, height, depth, pos, tile, parent);
}

void OctreeTileGrid::OctreeTileGridPool::putOctreeTileGrid(OctreeTileGrid *&group) {
    // Convert the group to a chunk and nullify the given group.
    Chunk *chunk = (Chunk*)group;
    group = NULL;

    // Update counts
    _available++;
    _used--;

    // Insert the chunk into the free list.
    chunk->next = _freePtr;
    _freePtr = chunk;
}

void OctreeTileGrid::OctreeTileGridPool::printStats() {
    Info("Tile pool stats:");
    LogStream::IncrementIndent();
    Info("Allocated: " << _available + _used);
    Info("Unused:    " << _available);
    Info("Used:      " << _used);
    LogStream::DecrementIndent();
}

OctreeTileGrid *OctreeTileGrid::OctreeTileGridPool::getParent() {
    return _parent;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark OctreeTileGrid c'tors and d'tors
//////////////////////////////////////////////////////////////////////////////////////////

OctreeTileGrid::OctreeTileGrid(int width, int height, int depth, const Vector3 &pos,
Tile tile, OctreeTileGrid* parent):TileGrid(width, height, depth), _tile(0,0), _pool(NULL),
_parent(NULL)
{
#if USE_POOL
    // This constructor is the public constructor, which means it should only be called for the very root node. Create a OctreeTileGridPool here.
    _pool = new OctreeTileGridPool(width * height * depth / 4, this);
#endif

    // And setup the object.
    initialize(width, height, depth, pos, tile, parent);
}

OctreeTileGrid::OctreeTileGrid(): TileGrid(0, 0, 0), _tile(0, 0), _pool(NULL), _parent(NULL) {}

OctreeTileGrid::~OctreeTileGrid() {
#if USE_POOL
    // Only delete the pool if this is the pool's initial parent.
    if (_pool->getParent() == this) {
        clear();
        delete _pool;
    }

    // Note: Non-parent children MAY have children here, as we do NOT clear children when
    // putting tiles back into the pool during optimization. It slightly faster, but we
    // lose this bit of safety code:
    //else if (!isLeaf()) {
    //    THROW(InternalError, "Only the parent group should have any children at deletion "
    //        "time. All other OctreeTileGrids should have cleared their children back into the "
    //        "pool when the parent called clear.");
    //}

    _pool = NULL;
#else
    clear();
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark TileGrid interface definitions
//////////////////////////////////////////////////////////////////////////////////////////

Tile OctreeTileGrid::getTile(int x, int y, int z) {
    return getLowestGroup(Vector3(x, y, z))->_tile;
}

TileType OctreeTileGrid::getTileType(int x, int y, int z) {
    return getLowestGroup(Vector3(x, y, z))->_tile.Type;
}

bool OctreeTileGrid::getTileParameter(int x, int y, int z, TileParameter param) {
    return getLowestGroup(Vector3(x, y, z))->_tile.getParameter(param);
}

void OctreeTileGrid::setTile(int x, int y, int z, Tile newTile) {
    setTile(Vector3(x, y, z), newTile);
}

/*! Sets the tile at the given location. */
void OctreeTileGrid::setTile(const Vector3 &loc, Tile newTile) {
    if (isSmallest()) {
        // Update the tile.
        _tile = newTile;

        // NOTE: This is currently kind of magical as we're possibly deleting tiles we
        // still have to step back through. However, because of GCC's tail recursion
        // optimization, we're saved any horrible explodey death here. As soon as I
        // implement group pools or something, though, the danger should go away.
		#if SYS_COMPILER != COMPILER_GNUC
		#   error This has only been tested on GCC version 4.3 and may cause all sorts of scary issues.
		#endif

        OctreeTileGrid *leaf = this;
        while(leaf->optimizeGroup() && leaf->_parent) {
            leaf = leaf->_parent;
        }

        return;
    }

    int index = coordsToIndex(loc);
    if (!_children[index]) {
        // ASSERT(hasOctant(index));

        // This group contains the tile and is of the correct type.
        if (_tile.Equals(newTile)) { return; }

        // This tile is not the correct type, so we need to descend further.
        Vector3 nPos;
        int nW, nH, nD;
        indexToCoords(index, nPos);
        indexToDims(index, nW, nH, nD);
        _children[index] = NEW_GROUP(nW, nH, nD, nPos, _tile, this);
    }

    // GCC apparently implements tail call recursion, even with O0!
    _children[index]->setTile(loc, newTile);
}

void OctreeTileGrid::setTileType(int x, int y, int z, TileType newType) {
    setTileType(Vector3(x, y, z), newType);
}

void OctreeTileGrid::setTileType(const Vector3 &loc, TileType newType) {
    // Get the current tile.
    Tile replacement = getLowestGroup(loc)->_tile;
    // If it doesn't match already, replace the tile.
    if(newType != replacement.Type) {
        replacement.Type = newType;
        setTile(loc, replacement);
    }
}

void OctreeTileGrid::setTileParameter(int x, int y, int z, TileParameter param, bool value) {
    // Get the current tile.
    Tile replacement = getLowestGroup(Vector3(x, y, z))->_tile;
    // If it doesn't match perfectly, replace the tile.
    if(replacement.getParameter(param) != value) {
        replacement.setParameter(param, value);
        setTile(Vector3(x, y, z), replacement);
    }
}

int OctreeTileGrid::getSurfaceLevel(int x, int y) {
    // Calculate the maximum z level of the lower and upper octants.
    int upperHeight = midZ() + upperDepth() - 1;
    int lowerHeight = lowZ() + lowerDepth() - 1;

    // If this is a leaf, we can short circuit.
    if (isLeaf()) { return _tile.Type != defaultType() ? upperHeight : OutOfBounds; }

    // Check the upper half, first, ensuring the upper octant even exists.
    int upperIndex = coordsToIndex(x, y, midZ());
    if (hasOctant(upperIndex)) {
        OctreeTileGrid *upper = _children[upperIndex];

        if (!upper && _tile.Type != defaultType()) {
            return upperHeight;
        } else if (upper) {
            int zLevel = upper->getSurfaceLevel(x, y);
            if (zLevel != OutOfBounds) { return zLevel; }
        }
    }

    // Then check the lower half. No need to check the octant here as the lower size
    // ALWAYS exists.
    OctreeTileGrid *lower = _children[coordsToIndex(x, y, lowZ())];

    if (!lower && _tile.Type != defaultType()) {
        return lowerHeight;
    } else if (lower) {
        int zLevel = lower->getSurfaceLevel(x, y);
        if (zLevel != OutOfBounds) { return zLevel; }
    }

    return OutOfBounds;
}

/* FORMAT DESCRIPTOR
    - # Tilegroups  (int      )
    - OctreeTileGrids
        - Position  (float [3])
        - Dimension (int   [3])
        - Type      (OctreeTileGrid::TileData)
        Note : Tilegroups are ordered heirarchically
             : OctreeTileGrid, OctreeTileGrid's children (recursive)
*/
void OctreeTileGrid::save(IOTarget *target) {
    // Write a placeholder for the number of tilegroups
    int numGroups = 0;
    long groupCountPos = target->position();
    target->write(&numGroups, sizeof(int));

    // Recursively write the tilegroups, tracking the number written along the way
    numGroups = saveGroup(target);

    // Rewind and write the actual number of tilegroups
    long endPos = target->position();
    target->seek(groupCountPos);
    target->write(&numGroups, sizeof(int));
    target->seek(endPos);
}

void OctreeTileGrid::load(IOTarget *target) {
    Vector3 pos;
    TileType type;
    int width, height, depth;
    int numGroups;

    // Read in the number of tilegroups to follow
    target->read(&numGroups, sizeof(int));

    // Read in the individual groups.
    for (int c = 0; c < numGroups; c++) {
        target->read(&width,   sizeof(int));
        target->read(&height,  sizeof(int));
        target->read(&depth,   sizeof(int));
        target->read(&pos, 3 * sizeof(float));
        target->read(&type,    sizeof(TileType));

        if (c == 0) {
            initialize(width, height, depth, pos, NEW_TILE(type), NULL);
        } else {
            addOctant(width, height, depth, pos, type);
        }
    }
}

void OctreeTileGrid::clear() {
    for (int c = 0; c < 8; c++) {
        if (_children[c]) {

#if USE_POOL
            // XXXBMW: I don't remember why, but it's fuckin important!!!
            _children[c]->clear();
#endif

            DELETE_GROUP(_children[c]);
        }
    }

    // ASSERT(isLeaf());
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Other octree tile grid function definitions.
//////////////////////////////////////////////////////////////////////////////////////////

int OctreeTileGrid::saveGroup(IOTarget *target) {
    int octantCounter=0;

    // Write this tilegroup's representative data
    target->write(&_pos, 3 * sizeof(float));
    target->write(&_width,   sizeof(int));
    target->write(&_height,  sizeof(int));
    target->write(&_depth,   sizeof(int));
    target->write(&_tile,    sizeof(Tile));

    // Recursively write each octant's data
    for(int c = 0; c < 8; c++) {
        if(hasOctant(c) && _children[c]) {
            octantCounter += _children[c]->saveGroup(target);
        }
    }

    // Info("Tilegroup and " << octantCounter << " octants written.");

    return octantCounter + 1;
}

void OctreeTileGrid::setPool(OctreeTileGridPool *pool) {
    _pool = pool;
}

inline bool OctreeTileGrid::isSmallest() {
    return ((_width == 1) && (_height == 1) && (_depth == 1));
}

void OctreeTileGrid::printStats() {
    Info("Tile group stats:");
    LogStream::IncrementIndent();
    Info("Dimensions: " << _width << "x" << _height << "x" << _depth);
    Info("Position:   " << _pos);
    Info("Tile:       " << _tile.Type);
    _pool->printStats();
    LogStream::DecrementIndent();
}

OctreeTileGrid* OctreeTileGrid::initialize(int width, int height, int depth,
    const Vector3 &position, Tile tile, OctreeTileGrid* parent)
{
    _width  = width;
    _height = height;
    _depth  = depth;
    _pos    = position;
    _tile   = tile;
    _parent = parent;

    // ASSERT_GE(dimensions, Vector3(1, 1, 1));
    // ASSERT_GE(position,   Vector3(0, 0, 0));
    // ASSERT_GE(_type, 0);

    // Set all of the children to NULL.
    memset(_children, 0, sizeof(_children));
    return this;
}

bool OctreeTileGrid::hasOctant(int index) {
    if (_width  == 1 && IS_UPPER_X(index)) { return false; }
    if (_height == 1 && IS_UPPER_Y(index)) { return false; }
    if (_depth  == 1 && IS_UPPER_Z(index)) { return false; }

    return true;
}

int OctreeTileGrid::coordsToIndex(const Vector3 &coords) {
    return coordsToIndex(coords.x, coords.y, coords.z);
}

int OctreeTileGrid::coordsToIndex(int x, int y, int z) {
    return ((x >= midX()) << 2) | ((y >= midY()) << 1) | (z >= midZ());
}

void OctreeTileGrid::indexToCoords(int index, Vector3 &coords) {
    if (_children[index]) {
        coords = _children[index]->_pos;
    } else {
        coords.x = IS_UPPER_X(index) ? midX() : lowX();
        coords.y = IS_UPPER_Y(index) ? midY() : lowY();
        coords.z = IS_UPPER_Z(index) ? midZ() : lowZ();
    }
}

void OctreeTileGrid::indexToDims(int index, int &width, int &height, int &depth) {
    if (_children[index]) {
        width  = _children[index]->_width;
        height = _children[index]->_height;
        depth  = _children[index]->_depth;
    } else {
        width  = IS_UPPER_X(index) ? upperWidth()  : lowerWidth();
        height = IS_UPPER_Y(index) ? upperHeight() : lowerHeight();
        depth  = IS_UPPER_Z(index) ? upperDepth()  : lowerDepth();
    }
}

TileType OctreeTileGrid::defaultType() { return 0; }

bool OctreeTileGrid::isLeaf() {
    for (int c = 0; c < 8; c++) {
        if (_children[c]) {
            return false;
        }
    }

    return true;
}

// The iterative solution seems to be the fastest, though not the nicest, solution here,
// especially for larger world sizes.
OctreeTileGrid* OctreeTileGrid::getLowestGroup(const Vector3 &loc) {
    OctreeTileGrid *lowest = this;
    int index = coordsToIndex(loc);

    while(lowest->_children[index]) {
        lowest = lowest->_children[index];
        index = lowest->coordsToIndex(loc);
    }

    return lowest;
}

bool OctreeTileGrid::optimizeGroup() {
    // Check to see if we've hit a leaf, which can't be optimized.
    if (isSmallest() || isLeaf()) {
        return true;
    }

    TileType baseType;
    bool canOptimize = true;
    int childCount = 0, octantCount = 0;
    for (int c = 0; c < 8; c++) {
        // Make sure we're examining a valid octant.
        if (!hasOctant(c)) { continue; }

        // Valid octant. Increment the count.
        octantCount++;

        // Make sure we're examining an actual child.
        if (!_children[c]) { continue; }

        // If the child is a leaf and of the same type as the parent, we can prune it.
        if (_children[c]->isLeaf() && _children[c]->_tile.Type == _tile.Type) {
            DELETE_GROUP(_children[c]);
            continue;
        }

        // Valid child. Increment the count.
        childCount++;

        // Keep track of the type of children in this group.
        if (childCount == 1) {
            baseType = _children[c]->_tile.Type;
        }

        // Further optimization can only be done if all children are leaves of the same type.
        if (_children[c]->_tile.Type != baseType || !_children[c]->isLeaf()) {
            canOptimize = false;
        }
    }

    // If type is OutOfBounds, we have multiple types among the children and can't do more here.
    if (!canOptimize) { return false; }

    // If all of the children are of the same type and every child slot is filled, we can
    // just clear all of the children and set this group's type.
    if (childCount == octantCount) {
        clear();
        _tile.Type = baseType;
        return true;
    }

// This code is currently broken.
#if 0
    // The leaves that are present have become the majority.
    // Example: If a parent is of type "empty" and has 6 leaves of type "sediment", we can
    // save memory by setting the parent type to "sediment", removing the leaves, and
    // adding new leaves of type "empty" where there were none previously.
    // Before: Parent (Empty)    : 6 x Leaves (Sediment) at indices [0,1,2,3,  5,  7,8]
    // After : Parent (Sediment) : 2 x Leaves (Empty)    at indices [        4,  6,   ]    
    if (childCount > (octantCount / 2)) {
        for (int c = 0; c < 8; c++) {
            if (hasOctant(c)) {
                if (_children[c]) {
                    DELETE_GROUP(_children[c]);
                } else {
                    Vector3 nPos;
                    int nW, nH, nD;
                    indexToCoords(c, nPos),
                    indexToDims(c, nW, nH, nD);
                    _children[c] = NEW_GROUP(nW, nH, nD, nPos, _type, this);
                }
            }
        }

        // And set this group to the correct type.
        _type = baseType;
    }
#endif

    // Further optimization can be done only if this group it a leaf.
    return isLeaf();
}

void OctreeTileGrid::examineOctree(int depth) {
    std::string space = std::string(depth, ' ');
    Info(space << "Node@" << depth << " " << _pos << " " << _width << "x" << _height <<
         "x" << _depth << " " << _tile.Type);

    for (int c=0; c<8; c++) {
        if (_children[c]) {
            _children[c]->examineOctree(depth+1);
        } else {
            Info(space << " Node@" << depth+1 << " NULL");
        }
    }
}

void OctreeTileGrid::addOctant(int width, int height, int depth, const Vector3 &position, TileType type) {
    // Info("Attempting to add octant to (" << _type << ") at " << _pos << " of size " << _width << "x" << _height << "x" << _depth);

    // First check to see if the new octant is small enough to fit inside this one
    if (width >= _width || height >= _height || depth >= _depth) {
        THROW(InternalError, "Octant is too large: " << 
              width << "x" << height << "x" << depth << " > " <<
              _width << "x" << _height << "x" << _depth);
    }

    int index = coordsToIndex(position);
    if (hasOctant(index) && (!_children[index])) {
        // The octant should go here
        // First, make sure it's the correct size
        int nW, nH, nD;
        indexToDims(index, nW, nH, nD);
        if (nW == width && nH == height && nD == depth) {
            _children[index] = NEW_GROUP(nW, nH, nD, position, NEW_TILE(type), this);
            return;
        }

        THROW(InternalError, "Terrain file appears incomplete, octant failed to load");
    }

    if (hasOctant(index)) {
        _children[index]->addOctant(width, height, depth, position, type);
        return;
    }

    // The place where this octant should go doesn't exist
    THROW(InternalError, "Octant with index " << index << " out of place");
}

//void OctreeTileGrid::getTileBoundaries(const Vector2 &loc, std::list<Vector2> *boundaries) {
//    // If this is a leaf, there are obviously no transitions
//    if(_depth > 1 && !isLeaf()) {
//        short lowerType, upperType;
//
//        // Find type transitions inside the upper and lower octants
//        int upperIndex = coordsToIndex(loc.x, loc.y, midZ()),
//            lowerIndex = coordsToIndex(loc.x, loc.y, lowZ());
//
//        if(_children[upperIndex]) {
//            upperType = _children[upperIndex]->floor(loc);
//        }
//        else {
//            upperType = _type;
//        }
//
//        if(_children[lowerIndex]) {
//            _children[lowerIndex]->getTileBoundaries(loc, boundaries);
//            lowerType = _children[lowerIndex]->ceiling(loc);
//        }
//        else {
//            lowerType = _type;
//        }
//
//        // Also check for a transition at the boundary of the upper and lower octants
//        if(lowerType != upperType) {
//            if(upperType == 0) {
//                boundaries->push_back(Vector2(midZ()-1, 0));
//            }
//            else if(lowerType == 0) {
//                boundaries->push_back(Vector2(midZ(), 1));
//            }
//        }
//
//        // Put off checking the upper octant until after we've added any boundary transitions
//        // This way, the list is properly ordered
//        if(_children[upperIndex]) {
//            _children[upperIndex]->getTileBoundaries(loc, boundaries);
//        }
//    }
//}

//int OctreeTileGrid::ceiling(const Vector2 &loc) {
//    int index = coordsToIndex(loc.x, loc.y, _depth > 1 ? midZ() : lowZ());
//    if(isSmallest() || !hasOctant(index) || !_children[index]) {
//        return _type;
//    }
//
//    return _children[index]->ceiling(loc);
//}
//
//int OctreeTileGrid::floor(const Vector2 &loc) {
//    int index = coordsToIndex(loc.x, loc.y, lowZ());
//    if(isSmallest() || !hasOctant(index) || !_children[index]) {
//        return _type;
//    }
//
//    return _children[index]->floor(loc);
//}

//int OctreeTileGrid::getSurfaceLevel(const Vector2 &loc) {
//    // Calculate the maximum z level of the lower and upper octants.
//    int upperHeight = midZ() + upperDepth() - 1;
//    int lowerHeight = lowZ() + lowerDepth() - 1;
//
//    // If this is a leaf, we can short circuit.
//    if (isLeaf()) { return _type != defaultType() ? upperHeight : OutOfBounds; }
//
//    // Check the upper half, first, ensuring the upper octant even exists.
//    int upperIndex = coordsToIndex(loc.x, loc.y, midZ());
//    if (hasOctant(upperIndex)) {
//        OctreeTileGrid *upper = _children[upperIndex];
//
//        if (!upper && _type != defaultType()) {
//            return upperHeight;
//        } else if (upper) {
//            int zLevel = upper->getSurfaceLevel(loc);
//            if (zLevel != OutOfBounds) { return zLevel; }
//        }
//    }
//
//    // Then check the lower half. No need to check the octant here as the lower size
//    // ALWAYS exists.
//    OctreeTileGrid *lower = _children[coordsToIndex(loc.x, loc.y, lowZ())];
//
//    if (!lower && _type != defaultType()) {
//        return lowerHeight;
//    } else if (lower) {
//        int zLevel = lower->getSurfaceLevel(loc);
//        if (zLevel != OutOfBounds) { return zLevel; }
//    }
//
//    return OutOfBounds;
//}

// Nanosecond accurate timing code:
//    uint64_t start, end;
//    start = mach_absolute_time();
//    OctreeTileGrid *lowestBranch = getLowestGroup(loc);
//    end = mach_absolute_time();
//
//    // Calculate elapsed time and ditch.
//    timespec delta;
//    uint64_t difference = end - start;  
//    mach_timebase_info_data_t info = {0,0};  
//
//    if (info.denom == 0) {
//        mach_timebase_info(&info);
//    }
//
//    uint64_t elapsednano = difference * (info.numer / info.denom);  
//
//    delta.tv_sec  = elapsednano * 1e-9;  
//    delta.tv_nsec = elapsednano - (delta.tv_sec * 1e9);
//
//    Info("     Seconds elapsed: " << delta.tv_sec );
//    Info("Nano seconds elapsed: " << delta.tv_nsec);
//    exit(0);
