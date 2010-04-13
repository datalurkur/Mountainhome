/*
 *  TileGroup.hpp
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "TileGroup.h"
#include <mach/mach_time.h>

template <class TileData>
TileGroup<TileData>::TileGroup(const Vector3 &pos, const Vector3 &dims, TileData type, TileGroup<TileData>* parent): _pos(pos), _dims(dims), _type(type), _parent(parent) {
    memset(_children, 0, sizeof(_children));
}

template <class TileData>
TileGroup<TileData>::~TileGroup() {}

template <class TileData>
bool TileGroup<TileData>::hasOctant(int index) {
    if (_dims.x == 1 && IS_UPPER_X(index)) { return false; }
    if (_dims.y == 1 && IS_UPPER_Y(index)) { return false; }
    if (_dims.z == 1 && IS_UPPER_Z(index)) { return false; }

    return true;
}

template <class TileData> inline
int TileGroup<TileData>::coordsToIndex(const Vector3 &coords) {
    return ((coords[0] >= midX()) << 2) | ((coords[1] >= midY()) << 1) | (coords[2] >= midZ());
}

template <class TileData> inline
void TileGroup<TileData>::indexToCoords(int index, Vector3 &coords) {
    if (_children[index]) {
        coords = _children[index]->_pos;
    } else {
        coords.x = IS_UPPER_X(index) ? midX() : lowX();
        coords.y = IS_UPPER_Y(index) ? midY() : lowY();
        coords.z = IS_UPPER_Z(index) ? midZ() : lowZ();
    }
}

template <class TileData> inline
void TileGroup<TileData>::indexToDims(int index, Vector3 &dims) {
    if (_children[index]) {
        dims = _children[index]->_dims;
    } else {
        dims.x = IS_UPPER_X(index) ? upperWidth()  : lowerWidth();
        dims.y = IS_UPPER_Y(index) ? upperHeight() : lowerHeight();
        dims.z = IS_UPPER_Z(index) ? upperDepth()  : lowerDepth();
    }
}

template <class TileData> inline
TileData TileGroup<TileData>::getType() { return _type; }

template <class TileData> inline
void TileGroup<TileData>::setType(TileData type) { _type = type; }

template <class TileData> inline
Vector3  TileGroup<TileData>::getDims() { return _dims; }

template <class TileData>
bool TileGroup<TileData>::isLeaf() {
    for (int c = 0; c < 8; c++) {
        if (_children[c]) {
            return false;
        }
    }

    return true;
}

template <class TileData> inline
bool TileGroup<TileData>::isSmallest() {
    return ((_dims[0] == 1) && (_dims[1] == 1) && (_dims[2] == 1));
}

template <class TileData>
void TileGroup<TileData>::clearChildren() {
    for (int c = 0; c < 8; c++) {
        delete _children[c];
        _children[c] = NULL;
    }
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
    if (isLeaf()) { return _type != 0 ? upperHeight : -1; }

    // Check the upper half, first.
    TileGroup<TileData> *upper = _children[coordsToIndex(Vector3(loc.x, loc.y, midZ()))];

    if (!upper && _type != 0) {
        return upperHeight;
    } else if (upper) {
        int zLevel = upper->getSurfaceLevel(loc);
        if (zLevel != -1) { return zLevel; }
    }

    // Then check the lower half.
    TileGroup<TileData> *lower = _children[coordsToIndex(Vector3(loc.x, loc.y, lowZ()))];

    if (!lower && _type != 0) {
        return lowerHeight;
    } else if (lower) {
        int zLevel = lower->getSurfaceLevel(loc);
        if (zLevel != -1) { return zLevel; }
    }

    return -1;
}

template <class TileData>
TileData TileGroup<TileData>::getTile(const Vector3 &loc) {
    return getLowestGroup(loc)->getType();
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
        if (_children[c]->isLeaf() && _children[c]->getType() == _type) {
            delete _children[c];
            _children[c] = NULL;
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
                    delete _children[c];
                    _children[c] = NULL;
                } else {
                    Vector3 nPos, nDims;
                    indexToCoords(c, nPos),
                    indexToDims(c, nDims);
                    _children[c] = new TileGroup(nPos, nDims, _type, this);
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
        // This group contains the tile and is of the correct type.
        if (type == _type) { return; }

        // This tile is not the correct type, so we need to descend further.
        Vector3 nPos, nDims;
        indexToDims(index, nDims);
        indexToCoords(index, nPos);
        _children[index] = new TileGroup(nPos, nDims, _type, this);
    }

    // GCC apparently implements tail call recursion, even with O0!
    _children[index]->setTile(loc, type);
}

template <class TileData>
int TileGroup<TileData>::write(File *tFile) {
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

template <class TileData>
bool TileGroup<TileData>::addOctant(const Vector3 &position, const Vector3 &dimensions, TileData type) {
    Info("Attempting to add octant to (" << _type << ") at " << _pos << " of size " << _dims);

    // First check to see if the new octant is small enough to fit inside this one
    if(dimensions[0] > _dims[0] || dimensions[1] > _dims[1] || dimensions[2] > _dims[2] || dimensions == _dims) {
        Error("Octant is too large");
        return false;
    }

    int index = coordsToIndex(position);
    if(hasOctant(index) && (!_children[index])) {
        // The octant should go here
        // First, make sure it's the correct size
        Vector3 cDims;
        indexToDims(index, cDims);
        if(cDims == dimensions) {
            _children[index] = new TileGroup(position, dimensions, type, this);
            return true;
        }
        else {
            Error("Terrain file appears incomplete, octant failed to load");
            return false;
        }
    }
    else if(hasOctant(index)) {
        return _children[index]->addOctant(position, dimensions, type);
    }
    else {
        // The place where this octant should go doesn't exist
        Error("Octant out of place");
        return false;
    }
}

template <class TileData>
void TileGroup<TileData>::examineOctree(int depth) {
    std::string space = std::string(depth, ' ');
    Info(space << "Node@" << depth << " " << _pos << " " << _dims << " " << _type);
    for (int c=0; c<8; c++) {
        if (_children[c]) {
            _children[c]->examineOctree(depth+1);
        }
    }
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
