/*
 *  TileGroup.hpp
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include <Base/FileSystem.h>

#include "TileGroup.h"
#include <mach/mach_time.h>

#define IS_UPPER_X(index) index & 0x4
#define IS_UPPER_Y(index) index & 0x2
#define IS_UPPER_Z(index) index & 0x1

#define USE_POOL 0

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark TileGroupPool
//////////////////////////////////////////////////////////////////////////////////////////

TileGroup::TileGroupPool::TileGroupPool(int initialSize, TileGroup *parent): _parent(parent), _currentCount(0), _maxCount(0) {
#if USE_POOL
    Info("Creating TileGroupPool with initial size " << initialSize);
    for (int i = 0; i < initialSize; i++) {
        createTileGroup();
    }

    printStats();
#endif
}

TileGroup::TileGroupPool::~TileGroupPool() {
    ASSERT_EQ(_pool.size(), _currentCount);
    ASSERT_EQ(_maxCount - _currentCount, 0);
    clear_list(_pool);
}

TileGroup* TileGroup::TileGroupPool::getTileGroup(const Vector3 &pos, const Vector3 &dims, TileData type, TileGroup* parent) {
#if USE_POOL
    // If there are no groups available, make one.
    if (_currentCount == 0) { createTileGroup(); }

    // And pull a group out of our list.
    TileGroup *group = _pool.back();
    _pool.pop_back();
    _currentCount--;
#else
    // Just make a new group.
    TileGroup *group = new TileGroup(this);
#endif

    // Initialize the group and return.
    group->initialize(pos, dims, type, parent);
    return group;
}

void TileGroup::TileGroupPool::createTileGroup() {
    _pool.push_back(new TileGroup(this));
    _currentCount++;
    _maxCount++;
}

void TileGroup::TileGroupPool::putTileGroup(TileGroup **group) {
    // ASSERT(*group);
#if USE_POOL
    _currentCount++;
    _pool.push_back(*group);
#else
    delete *group;
#endif
    *group = NULL;
}

void TileGroup::TileGroupPool::printStats() {
    Info("Tile pool stats:");
    LogStream::IncrementIndent();
    Info("Allocated: " << _maxCount);
    Info("Unused:    " << _pool.size());
    Info("Used:      " << _maxCount - _currentCount);
    LogStream::DecrementIndent();
}

TileGroup *TileGroup::TileGroupPool::getParent() {
    return _parent;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark TileGroup
//////////////////////////////////////////////////////////////////////////////////////////

TileGroup::TileGroup(const Vector3 &pos, const Vector3 &dims, TileData type, TileGroup* parent): _type(0), _pool(NULL), _parent(NULL) {
    // This constructor is the public constructor, which means it should only be called for the very root node. Create a TileGroupPool here.
    _pool = new TileGroupPool(dims.x * dims.y * 2, this);

    // And setup the object.
    initialize(pos, dims, type, parent);
}

TileGroup::TileGroup(TileGroupPool *pool): _type(0), _pool(pool), _parent(NULL) {}

TileGroup::~TileGroup() {
    // Only delete the pool if this is the pool's initial parent.
    if (_pool->getParent() == this) {
        clearChildren();
        delete _pool;
    }

    // Note: Non-parent children MAY have children here, as we do NOT clear children when
    // putting tiles back into the pool during optimization. It slightly faster, but we
    // lose this bit of safety code:
    //else if (!isLeaf()) {
    //    THROW(InternalError, "Only the parent group should have any children at deletion "
    //        "time. All other TileGroups should have cleared their children back into the "
    //        "pool when the parent called clearChildren.");
    //}

    _pool = NULL;
}

inline bool TileGroup::isSmallest() {
    return ((_dims[0] == 1) && (_dims[1] == 1) && (_dims[2] == 1));
}

void TileGroup::clearChildren() {
    for (int c = 0; c < 8; c++) {
        if (_children[c]) {
            _children[c]->clearChildren();
            _pool->putTileGroup(&_children[c]);
        }
    }

    // ASSERT(isLeaf());
}

void TileGroup::printStats() {
    Info("Tile group stats:");
    LogStream::IncrementIndent();
    Info("Dimensions: " << _dims);
    Info("Position:   " << _pos );
    Info("Type:       " << _type);
    _pool->printStats();
    LogStream::DecrementIndent();
}

void TileGroup::initialize(const Vector3 &position, const Vector3 &dimensions, TileData type, TileGroup* parent) {
    _pos    = position;
    _dims   = dimensions;
    _type   = type;
    _parent = parent;

    // ASSERT_GE(dimensions, Vector3(1, 1, 1));
    // ASSERT_GE(position,   Vector3(0, 0, 0));
    // ASSERT_GE(_type, 0);

    // Set all of the children to NULL.
    memset(_children, 0, sizeof(_children));
}

bool TileGroup::hasOctant(int index) {
    if (_dims.x == 1 && IS_UPPER_X(index)) { return false; }
    if (_dims.y == 1 && IS_UPPER_Y(index)) { return false; }
    if (_dims.z == 1 && IS_UPPER_Z(index)) { return false; }

    return true;
}

inline int TileGroup::coordsToIndex(const Vector3 &coords) {
    return coordsToIndex(coords.x, coords.y, coords.z);
}

inline int TileGroup::coordsToIndex(int x, int y, int z) {
    return ((x >= midX()) << 2) | ((y >= midY()) << 1) | (z >= midZ());
}

inline void TileGroup::indexToCoords(int index, Vector3 &coords) {
    if (_children[index]) {
        coords = _children[index]->_pos;
    } else {
        coords.x = IS_UPPER_X(index) ? midX() : lowX();
        coords.y = IS_UPPER_Y(index) ? midY() : lowY();
        coords.z = IS_UPPER_Z(index) ? midZ() : lowZ();
    }
}

inline void TileGroup::indexToDims(int index, Vector3 &dims) {
    if (_children[index]) {
        dims = _children[index]->_dims;
    } else {
        dims.x = IS_UPPER_X(index) ? upperWidth()  : lowerWidth();
        dims.y = IS_UPPER_Y(index) ? upperHeight() : lowerHeight();
        dims.z = IS_UPPER_Z(index) ? upperDepth()  : lowerDepth();
    }
}

// Inline breaks getDims?!?!
       Vector3             TileGroup::getDims()              { return _dims; }
inline TileGroup::TileData TileGroup::getType()              { return _type; }
inline void                TileGroup::setType(TileData type) { _type = type; }

bool TileGroup::isLeaf() {
    for (int c = 0; c < 8; c++) {
        if (_children[c]) {
            return false;
        }
    }

    return true;
}

// The iterative solution seems to be the fastest, though not the nicest, solution here,
// especially for larger world sizes.
TileGroup* TileGroup::getLowestGroup(const Vector3 &loc) {
    TileGroup *lowest = this;
    int index = coordsToIndex(loc);

    while(lowest->_children[index]) {
        lowest = lowest->_children[index];
        index = lowest->coordsToIndex(loc);
    }

    return lowest;
}

int TileGroup::getSurfaceLevel(const Vector2 &loc) {
    // Calculate the maximum z level of the lower and upper octants.
    int upperHeight = midZ() + upperDepth() - 1;
    int lowerHeight = lowZ() + lowerDepth() - 1;

    // If this is a leaf, we can short circuit.
    if (isLeaf()) { return _type != 0 ? upperHeight : -1; }

    // Check the upper half, first, ensuring the upper octant even exists.
    int upperIndex = coordsToIndex(loc.x, loc.y, midZ());
    if (hasOctant(upperIndex)) {
        TileGroup *upper = _children[upperIndex];

        if (!upper && _type != 0) {
            return upperHeight;
        } else if (upper) {
            int zLevel = upper->getSurfaceLevel(loc);
            if (zLevel != -1) { return zLevel; }
        }
    }

    // Then check the lower half. No need to check the octant here as the lower size
    // ALWAYS exists.
    TileGroup *lower = _children[coordsToIndex(loc.x, loc.y, lowZ())];

    if (!lower && _type != 0) {
        return lowerHeight;
    } else if (lower) {
        int zLevel = lower->getSurfaceLevel(loc);
        if (zLevel != -1) { return zLevel; }
    }

    return -1;
}

TileGroup::TileData TileGroup::getTile(const Vector3 &loc) {
    return getLowestGroup(loc)->getType();
}

bool TileGroup::optimizeGroup() {
    // Check to see if we've hit a leaf, which can't be optimized.
    if (isSmallest() || isLeaf()) {
        return true;
    }

    TileData baseType;
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
        if (_children[c]->isLeaf() && _children[c]->getType() == _type) {
            _pool->putTileGroup(&_children[c]);
            continue;
        }

        // Valid child. Increment the count.
        childCount++;

        // Keep track of the type of children in this group.
        if (childCount == 1) {
            baseType = _children[c]->getType();
        }

        // Further optimization can only be done if all children are leaves of the same type.
        if (_children[c]->getType() != baseType || !_children[c]->isLeaf()) {
            canOptimize = false;
        }
    }

    // If type is -1, we have multiple types among the children and can't do more here.
    if (!canOptimize) { return false; }

    // If all of the children are of the same type and every child slot is filled, we can
    // just clear all of the children and set this group's type.
    if (childCount == octantCount) {
        clearChildren();
        _type = baseType;
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
                    _pool->putTileGroup(&_children[c]);
                } else {
                    Vector3 nPos, nDims;
                    indexToCoords(c, nPos),
                    indexToDims(c, nDims);
                    _children[c] = Pool->getTileGroup(nPos, nDims, _type, this);
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

void TileGroup::setTile(const Vector3 &loc, TileData type) {
    if (isSmallest()) {
        // Update the type.
        _type = type;

        // NOTE: This is currently kind of magical as we're possibly deleting tiles we
        // still have to step back through. However, because of GCC's tail recursion
        // optimization, we're saved any horrible explodey death here. As soon as I
        // implement group pools or something, though, the danger should go away.
        #if SYS_COMPILER != COMPILER_GNUC
        #   error This has only been tested on GCC version 4.3 and may cause all sorts of scary issues.
        #endif

        TileGroup *leaf = this;
        while(leaf->optimizeGroup() && leaf->_parent) {
            leaf = leaf->_parent;
        }

        return;
    }

    int index = coordsToIndex(loc);
    if (!_children[index]) {
        // ASSERT(hasOctant(index));

        // This group contains the tile and is of the correct type.
        if (type == _type) { return; }

        // This tile is not the correct type, so we need to descend further.
        Vector3 nPos, nDims;
        indexToDims(index, nDims);
        indexToCoords(index, nPos);
        _children[index] = _pool->getTileGroup(nPos, nDims, _type, this);
    }

    // GCC apparently implements tail call recursion, even with O0!
    _children[index]->setTile(loc, type);
}

void TileGroup::examineOctree(int depth) {
    std::string space = std::string(depth, ' ');
    Info(space << "Node@" << depth << " " << _pos << " " << _dims << " " << _type);
    for (int c=0; c<8; c++) {
        if (_children[c]) {
            _children[c]->examineOctree(depth+1);
        } else {
            Info(space << " Node@" << depth+1 << " NULL");
        }
    }
}

int TileGroup::write(File *tFile) {
    int octantCounter=0;

    // Write this tilegroup's representative data
    tFile->write(&_pos, 3*sizeof(int));
    tFile->write(&_dims, 3*sizeof(int));
    tFile->write(&_type, sizeof(TileData));

    // Recursively write each octant's data
    for(int c=0; c<8; c++) {
        if(hasOctant(c) && _children[c]) {
            octantCounter += _children[c]->write(tFile);
        }
    }

    // Info("Tilegroup and " << octantCounter << " octants written.");

    return octantCounter+1;
}

void TileGroup::addOctant(const Vector3 &position, const Vector3 &dimensions, TileData type) {
    // Info("Attempting to add octant to (" << _type << ") at " << _pos << " of size " << _dims);

    // First check to see if the new octant is small enough to fit inside this one
    if (dimensions >= _dims) {
        THROW(InternalError, "Octant is too large: " << dimensions << " > " << _dims);
    }

    int index = coordsToIndex(position);
    if (hasOctant(index) && (!_children[index])) {
        // The octant should go here
        // First, make sure it's the correct size
        Vector3 cDims;
        indexToDims(index, cDims);
        if (cDims == dimensions) {
            _children[index] = _pool->getTileGroup(position, dimensions, type, this);
            return;
        }

        THROW(InternalError, "Terrain file appears incomplete, octant failed to load");
    }

    if (hasOctant(index)) {
        _children[index]->addOctant(position, dimensions, type);
    }

    // The place where this octant should go doesn't exist
    THROW(InternalError, "Octant out of place");
}

// Nanosecond accurate timing code:
//    uint64_t start, end;
//    start = mach_absolute_time();
//    TileGroup *lowestBranch = getLowestGroup(loc);
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
