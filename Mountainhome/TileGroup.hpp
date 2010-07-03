/*
 *  TileGroup<TileData>.hpp
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

#if USE_POOL
#define NEW_GROUP(pos, dims, type, parent) _pool->getTileGroup<TileData>(pos, dims, type, parent)
#define DELETE_GROUP(group) _pool->putTileGroup<TileData>(group)
#else
#define NEW_GROUP(pos, dims, type, parent) new TileGroup<TileData>(pos, dims, type, parent)
#define DELETE_GROUP(group) do { delete group; group = NULL; } while (0)
#endif

template <class TileData>
struct TileGroup<TileData>::Chunk {
    TileGroup<TileData> group;
    Chunk *next;
};

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark TileGroupPool
//////////////////////////////////////////////////////////////////////////////////////////
template <class TileData>
TileGroup<TileData>::TileGroupPool::TileGroupPool(int initialSize, TileGroup<TileData> *parent): _parent(parent), _available(initialSize), _used(0) {
    Info("Creating TileGroupPool with initial size " << _available);
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

template <class TileData>
TileGroup<TileData>::TileGroupPool::~TileGroupPool() {
    free(_basePtr);
}

template <class TileData>
TileGroup<TileData>* TileGroup<TileData>::TileGroupPool::getTileGroup(const Vector3 &pos, const Vector3 &dims, TileData type, TileGroup<TileData>* parent) {
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
    return _lastPtr->group.initialize(pos, dims, type, parent);
}

template <class TileData>
void TileGroup<TileData>::TileGroupPool::putTileGroup(TileGroup<TileData> *&group) {
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

template <class TileData>
void TileGroup<TileData>::TileGroupPool::printStats() {
    Info("Tile pool stats:");
    LogStream::IncrementIndent();
    Info("Allocated: " << _available + _used);
    Info("Unused:    " << _available);
    Info("Used:      " << _used);
    LogStream::DecrementIndent();
}

template <class TileData>
TileGroup<TileData> *TileGroup<TileData>::TileGroupPool::getParent() {
    return _parent;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark TileGroup
//////////////////////////////////////////////////////////////////////////////////////////

template <class TileData>
TileGroup<TileData>::TileGroup(const Vector3 &pos, const Vector3 &dims, TileData type, TileGroup<TileData>* parent): _pool(NULL), _parent(NULL) {
#if USE_POOL
    // This constructor is the public constructor, which means it should only be called for the very root node. Create a TileGroupPool here.
    _pool = new TileGroupPool(dims.x * dims.y * dims.x / 4, this);
#endif

    // And setup the object.
    initialize(pos, dims, type, parent);
}

template <class TileData>
TileGroup<TileData>::TileGroup(): _type(0), _pool(NULL), _parent(NULL) {}

template <class TileData>
TileGroup<TileData>::~TileGroup() {
#if USE_POOL
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
    //        "time. All other TileGroup<TileData>s should have cleared their children back into the "
    //        "pool when the parent called clearChildren.");
    //}

    _pool = NULL;
#else
    clearChildren();
#endif
}

template <class TileData>
void TileGroup<TileData>::setPool(TileGroupPool *pool) {
    _pool = pool;
}

template <class TileData>
inline bool TileGroup<TileData>::isSmallest() {
    return ((_dims[0] == 1) && (_dims[1] == 1) && (_dims[2] == 1));
}

template <class TileData>
void TileGroup<TileData>::clearChildren() {
    for (int c = 0; c < 8; c++) {
        if (_children[c]) {
#if USE_POOL
            _children[c]->clearChildren();
#endif

            DELETE_GROUP(_children[c]);
        }
    }

    // ASSERT(isLeaf());
}

template <class TileData>
void TileGroup<TileData>::printStats() {
    Info("Tile group stats:");
    LogStream::IncrementIndent();
    Info("Dimensions: " << _dims);
    Info("Position:   " << _pos );
    Info("Type:       " << getType());
    _pool->printStats();
    LogStream::DecrementIndent();
}

template <class TileData>
TileGroup<TileData>* TileGroup<TileData>::initialize(const Vector3 &position, const Vector3 &dimensions, TileData type, TileGroup<TileData>* parent) {
    _pos    = position;
    _dims   = dimensions;
    _type   = type;
    _parent = parent;

    // ASSERT_GE(dimensions, Vector3(1, 1, 1));
    // ASSERT_GE(position,   Vector3(0, 0, 0));
    // ASSERT_GE(_type, 0);

    // Set all of the children to NULL.
    memset(_children, 0, sizeof(_children));
    return this;
}

template <class TileData>
bool TileGroup<TileData>::hasOctant(int index) {
    if (_dims.x == 1 && IS_UPPER_X(index)) { return false; }
    if (_dims.y == 1 && IS_UPPER_Y(index)) { return false; }
    if (_dims.z == 1 && IS_UPPER_Z(index)) { return false; }

    return true;
}

template <class TileData>
inline int TileGroup<TileData>::coordsToIndex(const Vector3 &coords) {
    return coordsToIndex(coords.x, coords.y, coords.z);
}

template <class TileData>
inline int TileGroup<TileData>::coordsToIndex(int x, int y, int z) {
    return ((x >= midX()) << 2) | ((y >= midY()) << 1) | (z >= midZ());
}

template <class TileData>
inline void TileGroup<TileData>::indexToCoords(int index, Vector3 &coords) {
    if (_children[index]) {
        coords = _children[index]->_pos;
    } else {
        coords.x = IS_UPPER_X(index) ? midX() : lowX();
        coords.y = IS_UPPER_Y(index) ? midY() : lowY();
        coords.z = IS_UPPER_Z(index) ? midZ() : lowZ();
    }
}

template <class TileData>
inline void TileGroup<TileData>::indexToDims(int index, Vector3 &dims) {
    if (_children[index]) {
        dims = _children[index]->_dims;
    } else {
        dims.x = IS_UPPER_X(index) ? upperWidth()  : lowerWidth();
        dims.y = IS_UPPER_Y(index) ? upperHeight() : lowerHeight();
        dims.z = IS_UPPER_Z(index) ? upperDepth()  : lowerDepth();
    }
}

// Inline breaks getDims?!?!
template <class TileData>
Vector3 TileGroup<TileData>::getDims() { return _dims; }

template <class TileData>
inline TileType TileGroup<TileData>::getType() { return 0; }




template <>
inline TileType TileGroup<TileType>::defaultType() { return 0; }

template <>
inline TileType TileGroup<Vector2>::defaultType() { return 0; }

template <>
inline TileType TileGroup<TileType>::getType() { return _type; }
template <>
inline TileType TileGroup<Vector2>::getType() {
//    if(_type[1] >= 1.0) {
        return _type[0];
//    }
//    else {
//        return defaultType();
//    }
}

template <>
inline TileType TileGroup<TileType>::getType(TileType tData) { return tData; }
template <>
inline TileType TileGroup<Vector2>::getType(Vector2 tData) {
    //if(tData[1] >= 1.0) {
        return tData[0];
    //}
    //else {
    //    return defaultType();
    //}
}

template <class TileData>
inline void TileGroup<TileData>::setType(short type) { }

template <>
inline void TileGroup<short>::setType(short type) { _type = type; }
template <>
inline void TileGroup<Vector2>::setType(short type) { _type[0] = type; }

template <class TileData>
bool TileGroup<TileData>::isLeaf() {
    for (int c = 0; c < 8; c++) {
        if (_children[c]) {
            return false;
        }
    }

    return true;
}

// The iterative solution seems to be the fastest, though not the nicest, solution here,
// especially for larger world sizes.
template <class TileData>
TileGroup<TileData>* TileGroup<TileData>::getLowestGroup(const Vector3 &loc) {
    TileGroup<TileData> *lowest = this;
    int index = coordsToIndex(loc);

    while(lowest->_children[index]) {
        lowest = lowest->_children[index];
        index = lowest->coordsToIndex(loc);
    }

    return lowest;
}

template <class TileData>
int TileGroup<TileData>::getSurfaceLevel(const Vector2 &loc) {
    // Calculate the maximum z level of the lower and upper octants.
    int upperHeight = midZ() + upperDepth() - 1;
    int lowerHeight = lowZ() + lowerDepth() - 1;

    // If this is a leaf, we can short circuit.
    if (isLeaf()) { return getType() != defaultType() ? upperHeight : -1; }

    // Check the upper half, first, ensuring the upper octant even exists.
    int upperIndex = coordsToIndex(loc.x, loc.y, midZ());
    if (hasOctant(upperIndex)) {
        TileGroup<TileData> *upper = _children[upperIndex];

        if (!upper && getType() != defaultType()) {
            return upperHeight;
        } else if (upper) {
            int zLevel = upper->getSurfaceLevel(loc);
            if (zLevel != -1) { return zLevel; }
        }
    }

    // Then check the lower half. No need to check the octant here as the lower size
    // ALWAYS exists.
    TileGroup<TileData> *lower = _children[coordsToIndex(loc.x, loc.y, lowZ())];

    if (!lower && getType() != defaultType()) {
        return lowerHeight;
    } else if (lower) {
        int zLevel = lower->getSurfaceLevel(loc);
        if (zLevel != -1) { return zLevel; }
    }

    return -1;
}

template <class TileData>
int TileGroup<TileData>::ceiling(const Vector2 &loc) {
    int index;
    if(_dims[2]>1) {
        index = coordsToIndex(loc.x, loc.y, midZ());
    }
    else {
        index = coordsToIndex(loc.x, loc.y, lowZ());
    }

    if(isSmallest() || !hasOctant(index) || !_children[index]) {
        return getType();
    }
    else {
        return _children[index]->ceiling(loc);
    }
}

template <class TileData>
int TileGroup<TileData>::floor(const Vector2 &loc) {
    int index = coordsToIndex(loc.x, loc.y, lowZ());

    if(isSmallest() || !hasOctant(index) || !_children[index]) {
        return getType();
    }
    else {
        return _children[index]->floor(loc);
    }
}

template <class TileData>
void TileGroup<TileData>::getTileBoundaries(const Vector2 &loc, std::list<Vector2> *boundaries) {
    // If this is a leaf, there are obviously no transitions
    if(_dims[2]>1 && !isLeaf()) {
        short lowerType, upperType;

        // Find type transitions inside the upper and lower octants
        int upperIndex = coordsToIndex(loc.x, loc.y, midZ()),
            lowerIndex = coordsToIndex(loc.x, loc.y, lowZ());

        if(_children[upperIndex]) {
            upperType = _children[upperIndex]->floor(loc);
        }
        else {
            upperType = getType();
        }

        if(_children[lowerIndex]) {
            _children[lowerIndex]->getTileBoundaries(loc, boundaries);
            lowerType = _children[lowerIndex]->ceiling(loc);
        }
        else {
            lowerType = getType();
        }

        // Also check for a transition at the boundary of the upper and lower octants
        if(lowerType != upperType) {
            if(upperType == 0) {
                boundaries->push_back(Vector2(midZ()-1, 0));
            }
            else if(lowerType == 0) {
                boundaries->push_back(Vector2(midZ(), 1));
            }
        }

        // Put off checking the upper octant until after we've added any boundary transitions
        // This way, the list is properly ordered
        if(_children[upperIndex]) {
            _children[upperIndex]->getTileBoundaries(loc, boundaries);
        }
    }
}

template <class TileData>
TileData TileGroup<TileData>::getTile(const Vector3 &loc) {
    return getLowestGroup(loc)->_type;
}

template <class TileData>
bool TileGroup<TileData>::optimizeGroup() {
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
        if (_children[c]->isLeaf() && _children[c]->getType() == getType()) {
            DELETE_GROUP(_children[c]);
            continue;
        }

        // Valid child. Increment the count.
        childCount++;

        // Keep track of the type of children in this group.
        if (childCount == 1) {
            baseType = _children[c]->getType();
        }

        // Further optimization can only be done if all children are leaves of the same type.
        if (_children[c]->getType() != getType(baseType) || !_children[c]->isLeaf()) {
            canOptimize = false;
        }
    }

    // If type is -1, we have multiple types among the children and can't do more here.
    if (!canOptimize) { return false; }

    // If all of the children are of the same type and every child slot is filled, we can
    // just clear all of the children and set this group's type.
    if (childCount == octantCount) {
        clearChildren();
        setType(getType(baseType));
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
                    Vector3 nPos, nDims;
                    indexToCoords(c, nPos),
                    indexToDims(c, nDims);
                    _children[c] = NEW_GROUP(nPos, nDims, _type, this);
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

template <class TileData>
void TileGroup<TileData>::setTile(const Vector3 &loc, TileData type) {
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

        TileGroup<TileData> *leaf = this;
        while(leaf->optimizeGroup() && leaf->_parent) {
            leaf = leaf->_parent;
        }

        return;
    }

    int index = coordsToIndex(loc);
    if (!_children[index]) {
        // ASSERT(hasOctant(index));

        // This group contains the tile and is of the correct type.
        if (TileGroup::getType(type) == getType()) { return; }

        // This tile is not the correct type, so we need to descend further.
        Vector3 nPos, nDims;
        indexToDims(index, nDims);
        indexToCoords(index, nPos);
        _children[index] = NEW_GROUP(nPos, nDims, _type, this);
    }

    // GCC apparently implements tail call recursion, even with O0!
    _children[index]->setTile(loc, type);
}

template <class TileData>
void TileGroup<TileData>::examineOctree(int depth) {
    std::string space = std::string(depth, ' ');
    Info(space << "Node@" << depth << " " << _pos << " " << _dims << " " << getType());
    for (int c=0; c<8; c++) {
        if (_children[c]) {
            _children[c]->examineOctree(depth+1);
        } else {
            Info(space << " Node@" << depth+1 << " NULL");
        }
    }
}

template <class TileData>
int TileGroup<TileData>::write(File *tFile) {
    int octantCounter=0;

    // Write this tilegroup's representative data
    tFile->write(&_pos,  3*sizeof(int));
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

template <class TileData>
void TileGroup<TileData>::addOctant(const Vector3 &position, const Vector3 &dimensions, TileData type) {
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
            _children[index] = NEW_GROUP(position, dimensions, type, this);
            return;
        }

        THROW(InternalError, "Terrain file appears incomplete, octant failed to load");
    }

    if (hasOctant(index)) {
        _children[index]->addOctant(position, dimensions, type);
        return;
    }

    // The place where this octant should go doesn't exist
    THROW(InternalError, "Octant with index " << index << " out of place");
}

// Nanosecond accurate timing code:
//    uint64_t start, end;
//    start = mach_absolute_time();
//    TileGroup<TileData> *lowestBranch = getLowestGroup(loc);
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
