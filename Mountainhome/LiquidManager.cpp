/*
 * LiquidManager.cpp
 * Mountainhome
 *
 * Created by Andrew Jean on 6/23/11
 * Copyright 2011 Mountainhome Project. All rights reserved.
 */

#include "LiquidManager.h"

LiquidManager::LiquidManager(): _initialized(false) {
    _vacuumNeighborhood.insert(Vector3( 1,  0,  0));
    _vacuumNeighborhood.insert(Vector3(-1,  0,  0));
    _vacuumNeighborhood.insert(Vector3( 0,  1,  0));
    _vacuumNeighborhood.insert(Vector3( 0, -1,  0));
    _vacuumNeighborhood.insert(Vector3( 0,  0,  1));
    // Vacuums don't affect liquids underneath them

    // Vertical liquid movement is handled separately and doesn't need entries in the neighborhood
    _liquidNeighborhood.insert(Vector3( 1,  0,  0));
    _liquidNeighborhood.insert(Vector3(-1,  0,  0));
    _liquidNeighborhood.insert(Vector3( 0,  1,  0));
    _liquidNeighborhood.insert(Vector3( 0, -1,  0));
}

LiquidManager::~LiquidManager() {
}

void LiquidManager::liquidCreated(const Vector3 &coords) {
    // Liquids taken by world from the _newLiquids set find their way back to us via this call
    // Newly added liquids should then attempt to flow
    //Info("Liquid node created at " << coords);
    ASSERT(_flowingLiquids.find(coords) == _flowingLiquids.end());
    _flowingLiquids.insert(coords);
}

void LiquidManager::vacuumCreated(const Vector3 &coords) {
    // We don't care until the liquid system starts a-flowin'
    if(_initialized) {
        // Check to see if this exposes old, resting liquids to new avenues of flowing
        Vector3Set::iterator itr, finder;
        for(itr = _vacuumNeighborhood.begin(); itr != _vacuumNeighborhood.end(); itr++) {
            finder = _restingLiquids.find((coords + *itr));
            if(finder != _restingLiquids.end()) {
                _flowingLiquids.insert(*finder);
                _restingLiquids.erase(finder);
            }
        }
    }
}

bool LiquidManager::update(int elapsed) {
    Vector3Set stoppedFlowing;
    Vector3Set::iterator itr, localItr;
    
    ASSERT(_initialized);
    
    ASSERT(elapsed >= 0);
    MSSinceLastTick += elapsed;
    if(MSSinceLastTick < MSPerTick)   { return false; }
    if(MSSinceLastTick > MSPerTick*2) { Warn("LiquidManager is lagging, losing ticks."); }
    MSSinceLastTick -= MSPerTick;
    ASSERT(MSSinceLastTick >= 0);

    // Check each flowing node to see if it can flow more
    //Info(_flowingLiquids.size() << " liquids flowing");
    for(itr = _flowingLiquids.begin(); itr != _flowingLiquids.end(); itr++) {
        // Can this liquid flow down?
        Vector3 tileBelow = (*itr) + Vector3(0, 0, -1);
        
        if(_newLiquids.find(tileBelow) == _newLiquids.end()) {
            if(!_terrain->isOutOfBounds(tileBelow) && !_terrain->getVoxel(tileBelow.x, tileBelow.y, tileBelow.z)) {
                // The tile below this location is empty!
                _newLiquids.insert(Vector3(tileBelow));
                //Info("Liquid at " << *itr << " flows down");
                continue;
            } else if(_flowingLiquids.find(tileBelow) != _flowingLiquids.end()) {
                // The tile below is still flowing, wait for it to stop flowing and then begin flowing outwards
                continue;
            }
        }

        // Can this liquid flow outwards?
        for(localItr = _liquidNeighborhood.begin(); localItr != _liquidNeighborhood.end(); localItr++) {
            Vector3 localPosition = (*itr) + (*localItr);
            if(_newLiquids.find(localPosition) != _newLiquids.end()) { continue; }
            if(!_terrain->isOutOfBounds(localPosition) && !_terrain->getVoxel(localPosition.x, localPosition.y, localPosition.z)) {
                _newLiquids.insert(localPosition);
            }
        }

        // Once it has flowed outwards, this liquid has flowed all it can
        stoppedFlowing.insert(*itr);
        //Info("Liquid at " << *itr << " flows out and stops flowing");
    }

    // Take any liquids that have stopped flowing, remove them from the flowing liquid set, and add them to the resting liquid set
    for(itr = stoppedFlowing.begin(); itr != stoppedFlowing.end(); itr++) {
        _flowingLiquids.erase(*itr);
        _restingLiquids.insert(*itr);
    }

    return (_newLiquids.size() > 0);
}

void LiquidManager::setup(Terrain *terrain) {
    _terrain = terrain;
    _initialized = true;
    Info("Liquid manager initialized");
}

void LiquidManager::getNewLiquids(Vector3Set &newLiquids) {
    newLiquids = _newLiquids;
}

void LiquidManager::clearNewLiquids() { _newLiquids.clear(); }

int LiquidManager::MSSinceLastTick = 0;
