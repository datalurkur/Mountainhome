/*
 *  TileGroup.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _TILEGROUP_H_
#define _TILEGROUP_H_

// Tile is a basic octree implementation
template <class TileData>
class TileGroup {
public:
    TileGroup(Vector3 pos, Vector3 dims, TileData type, TileGroup<TileData>* parent);
    virtual ~TileGroup();

    // Private member accessors (for *this* TileGroup)
    TileData getType() { return _type; }
    void setType(TileData type) { _type = type; }
    Vector3 getDims() { return _dims; }

    // External interface functions (for general use)
    TileGroup<TileData>* getGroup(Vector3 loc);
    int getSurfaceLevel(Vector2 loc);
    TileData getTile(Vector3 loc);
    void setTile(Vector3 loc, TileData type);
    
    // Debugging function to see the current state of the octree
    void examineOctree(int depth);

protected:
    int coordsToIndex(Vector3 coords);
    bool isLeaf();
    bool isSmallest();
    bool hasChild(int index);
        
    void spawnLeaf(Vector3 loc, TileData type);
    bool prune();

private:
    // Utility functions used by TileGroup
    // Function for generating child index
    Vector3 indexToCoords(int index);
    Vector3 indexToDims(int index);
    void clearChildren();

    // Halfway points and dimensions
    int halfX() { return _dims[0]/2; }
    int halfY() { return _dims[1]/2; }
    int halfZ() { return _dims[2]/2; }
    int midX() { return _pos[0] + halfX(); }
    int midY() { return _pos[1] + halfY(); }
    int midZ() { return _pos[2] + halfZ(); }

    // These are used to determine the width in the case of odd-sized groups
    // Ex: A group of size 9,9,9 gets broken down into groups of 4,4,4 and 5,5,5
    int oHalfX() { return _dims[0] - (int)(_dims[0]/2); }
    int oHalfY() { return _dims[1] - (int)(_dims[1]/2); }
    int oHalfZ() { return _dims[2] - (int)(_dims[2]/2); }

private:
    TileGroup<TileData>* _children[8];
    TileGroup<TileData>* _parent; // Normally, we wouldn't use a parent, but it's useful for optimising the pruning algorithm
    Vector3 _pos, _dims;          // Place in the world, dimensions
    TileData _type;               // Storage for tile information
};

#include "TileGroup.hpp"

#endif
