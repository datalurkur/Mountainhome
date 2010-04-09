/*
 *  TileGroup.hpp
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "TileGroup.h"

template <class TileData>
TileGroup<TileData>::TileGroup(Vector3 pos, Vector3 dims, TileData type, TileGroup<TileData>* parent): _pos(pos), _dims(dims), _type(type), _parent(parent) {
    int c;
    for(c=0; c<8; c++) {
        _children[c] = NULL;
    }
}

template <class TileData>
TileGroup<TileData>::~TileGroup() {}

template <class TileData>
bool TileGroup<TileData>::isLeaf() {
    int c;
    for(c=0; c<8; c++) {
        if(_children[c]) {
            return false;
        }
    }

    return true;
}

template <class TileData>
bool TileGroup<TileData>::isSmallest() {
    return ((_dims[0] == 1) && (_dims[1] == 1) && (_dims[2] == 1));
}

template <class TileData>
bool TileGroup<TileData>::hasChild(int index) {
    bool lX = index & 0x4,
         lY = index & 0x2,
         lZ = index & 0x1;

    if(_dims[0]==1 && lX) {
        return false;
    }

    if(_dims[1]==1 && lY) {
        return false;
    }

    if(_dims[2]==1 && lZ) {
        return false;
    }

    return true;
}

template <class TileData>
int TileGroup<TileData>::coordsToIndex(Vector3 coords) {
    return ((coords[0] < midX()) << 2) | ((coords[1] < midY()) << 1) | (coords[2] < midZ());
}

template <class TileData>
Vector3 TileGroup<TileData>::indexToCoords(int index) {
    Vector3 retVal;
    bool lX = index & 0x4,
         lY = index & 0x2,
         lZ = index & 0x1;

    if(lX) {
        retVal[0]  = _pos[0];
    }
    else {
        retVal[0]  = midX();
    }

    if(lY) {
        retVal[1]  = _pos[1];
    }
    else {
        retVal[1]  = midY();
    }

    if(lZ) {
        retVal[2]  = _pos[2];
    }
    else {
        retVal[2]  = midZ();
    }
    
    return retVal;
}

template <class TileData>
Vector3 TileGroup<TileData>::indexToDims(int index) {
    Vector3 retVal;
    bool lX = index & 0x4,
         lY = index & 0x2,
         lZ = index & 0x1;
         
    if(lX) {
        retVal[0] = halfX(); 
    }
    else {
        retVal[0] = oHalfX();
    }

    if(lY) {
        retVal[1] = halfY();
    }
    else {
        retVal[1] = oHalfY();
    }

    if(lZ) {
        retVal[2] = halfZ();
    }
    else {
        retVal[2] = oHalfZ();
    }

    return retVal;
}

template <class TileData>
void TileGroup<TileData>::clearChildren() {
    int c;
    for(c=0; c<8; c++) {
        delete _children[c];
        _children[c] = NULL;
    }
}

template <class TileData>
TileGroup<TileData>* TileGroup<TileData>::getGroup(Vector3 loc) {
    int index = coordsToIndex(loc);
    if(_children[index]) {
        return _children[index]->getGroup(loc);
    }
    else {
        return this;
    }
}

template <class TileData>
int TileGroup<TileData>::getSurfaceLevel(Vector2 loc) {
    if(isLeaf()) {
        if(_type != 0) {
            return _pos[2];
        }
        else {
            return -1;
        }
    }
    
    int iLower = coordsToIndex(Vector3(loc[0], loc[1], _pos[2])),
        iUpper = coordsToIndex(Vector3(loc[0], loc[1], _pos[2] + _dims[2] - 1));
    TileGroup *lower = _children[iLower],
              *upper = _children[iUpper];
              
    int rUpper = -1, 
        rLower = -1,
        retVal = -1;
              
    if(!upper && !lower) {
        if(_type != 0) {
            retVal = (_pos[2] + _dims[2] - 1);
            //Info("(!U !L) Node [" << _pos << "] [" << _dims << "] returns " << retVal);
            return retVal;
        }
        else {
            return -1;
        }
    }
    else if(!upper) {
        if(_type != 0) {
            retVal = (midZ() + oHalfZ() - 1);
            //Info("(!U) Node [" << _pos << "] [" << _dims << "] returns " << retVal);
            return retVal;
        }
    }
    else {
        rUpper = upper->getSurfaceLevel(loc);
    }
    
    if(rUpper != -1) {
        return rUpper;
    }
    else if(!lower) {
        if(_type != 0) {
            retVal = (_pos[2] + halfZ() - 1);
            //Info("(!L) Node [" << _pos << "] [" << _dims << "] returns " << retVal);
            return retVal;
        }
    }
    else {
        rLower = lower->getSurfaceLevel(loc);
    }
    
    return rLower;
}

template <class TileData>
TileData TileGroup<TileData>::getTile(Vector3 loc) {
    return getGroup(loc)->getType();
}

template <class TileData>
bool TileGroup<TileData>::prune() {
    TileData defaultType;
    int c, numLeaves = 0, maxLeaves = 0;

    // Check to see if we've hit a leaf (which obviously won't have any children to prune)
    if(isSmallest() || isLeaf()) {
        return true;
    }

    // Check children for similarity
    for(c=0; c<8; c++) {
        // Ignore NULL pointers
        if(hasChild(c)) {
            if(_children[c]) {
                if(!_children[c]->isLeaf()) {
                    // We can't prune a child that has children
                    return false;
                }

                if(numLeaves==0) {
                    // This is the first non-NULL leaf,
                    //  use it as the initial type for comparison
                    defaultType = _children[c]->getType();
                }
                else {
                    // Check to see if this leaf is of the same type as the rest
                    if(_children[c]->getType() != defaultType) {
                        //  If any one type is different, no pruning can occur
                        return false;
                    }
                }
                numLeaves++;
            }
            maxLeaves++;
        }
    }

    // Make sure we have at least one leaf to work with, otherwise, none of this is necessary
    if(numLeaves == 0) {
        return true;
    }

    // If we've made it this far, the leaves that exist are homogenous
    if(defaultType == _type) {
        //Info("Pruning homogenous children");
        // The leaves are not only homogenous, but already accurately represented by their parent
        clearChildren();

        return true;
    }
    else if(numLeaves == maxLeaves) {
        //Info("Pruning odd children, modifying parent.");
        // All 8 of the leaves are accounted for, 
        //  meaning they can be grouped without regards to the parent
        _type = defaultType;
        clearChildren();

        return true;
    }
    else if(numLeaves > (maxLeaves/2)) {
        //Info("Pruning incomplete odd children, re-filling parent");
        // The leaves that are present have become the majority and are at odds with the parent type
        // Example: If a parent is of type "empty" and has 6 leaves of type "sediment", we can save
        //  memory by setting the parent type to "sediment", removing the leaves, and adding
        //  new leaves of type "empty" where there were none previously
        // Before: Parent (Empty)    : 6 x Leaves (Sediment) at indices [0,1,2,3,  5,  7,8]
        // After : Parent (Sediment) : 2 x Leaves (Empty)    at indices [        4,  6,   ]
        
        TileData pType = _type;
        _type = defaultType;
        for(c=0; c<8; c++) {
            if(hasChild(c)) {
                if(_children[c]) {
                    delete _children[c];
                    _children[c] = NULL;
                }
                else {
                    Vector3 nPos  = indexToCoords(c),
                            nDims = indexToDims(c);
                    _children[c] = new TileGroup(nPos, nDims, pType, this);
                }
            }
        }
    }

    return false;
}

template <class TileData>
void TileGroup<TileData>::spawnLeaf(Vector3 loc, TileData type) {
    //Info("-----------------");
    //Info("Spawning leaf for node with attrs " << _pos << " and " << _dims);
    //Info("Spawn's final location is " << loc << " with type " << type);
    int index = coordsToIndex(loc);
    Vector3 nPos, nDims;

    if(loc[0] < midX()) {
        nPos[0]  = _pos[0];
        nDims[0] = halfX(); 
    }
    else {
        nPos[0]  = midX();
        nDims[0] = oHalfX();
    }

    if(loc[1] < midY()) {
        nPos[1]  = _pos[1];
        nDims[1] = halfY();
    }
    else {
        nPos[1]  = midY();
        nDims[1] = oHalfY();
    }

    if(loc[2] < midZ()) {
        nPos[2]  = _pos[2];
        nDims[2] = halfZ();
    }
    else {
        nPos[2]  = midZ();
        nDims[2] = oHalfZ();
    }

    //Info("Spawn's attrs are " << nPos << " and " << nDims);

    _children[index] = new TileGroup(nPos, nDims, _type, this);
    if(_children[index]->isSmallest()) {
        _children[index]->setType(type);
    }
    else {
        _children[index]->spawnLeaf(loc, type);
    }
}

template <class TileData>
void TileGroup<TileData>::setTile(Vector3 loc, TileData type) {
    TileGroup<TileData> *lowestBranch = getGroup(loc);
    if(lowestBranch->getType() != type) {
        if(lowestBranch->isSmallest()) {
            lowestBranch->setType(type);
        }
        else {
            lowestBranch->spawnLeaf(loc, type);
        }
        while(lowestBranch->prune()) {
            if(lowestBranch->_parent) {
                lowestBranch = lowestBranch->_parent;
            }
            else {
                break;
            }
        }
    }
}

template <class TileData>
void TileGroup<TileData>::examineOctree(int depth) {
    std::string space = std::string(depth, ' ');
    Info(space << "Node@" << depth << " " << _pos << " " << _dims << " " << _type);
    for(int c=0; c<8; c++) {
        if(_children[c]) {
            _children[c]->examineOctree(depth+1);
        }
    }
}
