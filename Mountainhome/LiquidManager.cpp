/*
 * LiquidManager.cpp
 * Mountainhome
 *
 * Created by Andrew Jean on 6/23/11
 * Copyright 2011 Mountainhome Project. All rights reserved.
 */

#include "LiquidManager.h"

LiquidManager::LiquidManager() {
}

LiquidManager::~LiquidManager() {
}

void LiquidManager::addLiquidType(VALUE typeValue, int flowRate) {
    std::vector<Liquid>::iterator itr = _liquidTypes.begin();
    for(; itr != _liquidTypes.end(); itr++) {
        if(typeValue == (*itr).first) { return; }
    }
    _liquidTypes.push_back(std::make_pair(typeValue, flowRate));
}

bool LiquidManager::isLiquid(const Voxel *type) {
    VALUE typeValue = type->getType();
    std::vector<Liquid>::iterator itr = _liquidTypes.begin();
    for(; itr != _liquidTypes.end(); itr++) {
        if(typeValue == (*itr).first) { return true; }
    }
    return false;
}

int LiquidManager::getFlowRate(const Voxel *type) {
    VALUE typeValue = type->getType();
    std::vector<Liquid>::iterator itr = _liquidTypes.begin();
    for(; itr != _liquidTypes.end(); itr++) {
        if(typeValue == (*itr).first) { return (*itr).second; }
    }
    return 0;
}

void LiquidManager::setFlow(Vector3 source, Vector3 dest, int offset) {
    //Info("Flow set: " << source << " -> " << dest << " (" << offset << ")");
    ASSERT(_outflows.find(source) == _outflows.end() && _inflows.find(dest) == _inflows.end());
    _outflows[source] = std::make_pair<Vector3,int>(dest, offset);
    _inflows[dest] = source;
}

void LiquidManager::rerouteFlowDest(Vector3 source, Vector3 dest, int offset, Terrain *terrain) {
    Vector3 oldDest = _outflows[source].first;
    int oldOffset = _outflows[source].second;
    
    //Info("Rerouting destination of " << source << " from " << oldDest << " to " << dest);

    _inflows.erase(oldDest);
    ASSERT(_inflows.find(oldDest) == _inflows.end());

    _outflows[source] = std::make_pair(dest, oldOffset);
    _inflows[dest] = source;

    processVacuum(oldDest, offset, terrain);
}

void LiquidManager::rerouteFlowSource(Vector3 source, Vector3 dest, int offset, Terrain *terrain) {
    Vector3 oldSource = _inflows[dest];
    int oldOffset = _outflows[oldSource].second;

    //Info("Rerouting source of " << dest << " from " << oldSource << " to " << source);
    if(oldOffset <= 0) {
        _readyToFlow.remove(oldSource);
        _readyToFlow.push_back(source);
    }

    _outflows.erase(oldSource);
    ASSERT(_outflows.find(oldSource) == _outflows.end());

    _outflows[source] = std::make_pair<Vector3,int>(dest, offset);
    _inflows[dest] = source;

    processLiquid(oldSource, oldOffset, terrain);
}

void LiquidManager::processLiquid(Vector3 coords, int offset, Terrain *terrain) {
    ASSERT(_outflows.find(coords) == _outflows.end());
/*
    if(_outflows.find(coords) != _outflows.end()) {
        Warn("Liquid at " << coords << " is already scheduled to flow");
        printOutflows();
        return;
    }
*/

    const Voxel *sourceType = terrain->getVoxel(coords[0], coords[1], coords[2]);

    // Check voxel below
    Vector3 belowCoords = coords - Vector3(0,0,1);
    if(!terrain->isOutOfBounds(belowCoords)) {
        const Voxel *belowType = terrain->getVoxel(belowCoords[0], belowCoords[1], belowCoords[2]);
        if(belowType == NULL) {
            if(_inflows.find(belowCoords) == _inflows.end()) {
                setFlow(coords, belowCoords, getFlowRate(sourceType) + offset);
            } else {
                rerouteFlowSource(coords, belowCoords, getFlowRate(sourceType) + offset, terrain);
            }
            return;
        }
    }

    // Check neighboring voxels
    for(int xLocal = -1; xLocal <= 1; xLocal++) {
        for(int yLocal = -1; yLocal <= 1; yLocal++) {
            Vector3 localCoords = coords + Vector3(xLocal, yLocal, 0);
            if(terrain->isOutOfBounds(localCoords)) { continue; }

            const Voxel *localType = terrain->getVoxel(localCoords[0], localCoords[1], localCoords[2]);
            if(localType == NULL && _inflows.find(localCoords) == _inflows.end()) {
                setFlow(coords, localCoords, getFlowRate(sourceType) + offset);
                return;
            }
        }
    }
}

void LiquidManager::processVacuum(Vector3 coords, int offset, Terrain *terrain) {
    ASSERT(_inflows.find(coords) == _inflows.end());
/*
    if(_inflows.find(coords) != _inflows.end()) {
        Warn("Vacuum at " << coords << " already has a scheduled inflow");
        return;
    }
*/

    // Check voxel above
    Vector3 aboveCoords = coords + Vector3(0,0,1);
    if(!terrain->isOutOfBounds(aboveCoords)) {
        const Voxel *aboveType = terrain->getVoxel(aboveCoords[0], aboveCoords[1], aboveCoords[2]);
        if(aboveType && isLiquid(aboveType)) {
            if(_outflows.find(aboveCoords) == _outflows.end()) {
                setFlow(aboveCoords, coords, getFlowRate(aboveType) + offset);
            } else {
                rerouteFlowDest(aboveCoords, coords, offset, terrain);
            }
            return;
        }
    }

    // Check neighboring voxels
    for(int xLocal = -1; xLocal <= 1; xLocal++) {
        for(int yLocal = -1; yLocal <= 1; yLocal++) {
            Vector3 localCoords = coords + Vector3(xLocal, yLocal, 0);
            if(terrain->isOutOfBounds(localCoords)) { continue; }

            const Voxel *localType = terrain->getVoxel(localCoords[0], localCoords[1], localCoords[2]);
            if(localType && isLiquid(localType) && _outflows.find(localCoords) == _outflows.end()) {
                setFlow(localCoords, coords, getFlowRate(localType) + offset);
                return;
            }
        }
    }
}

void LiquidManager::updateFlows(int elapsed) {
/*
    Info("Updating flows");
    printOutflows();
    printInflows();
*/
    // Determine which flows are about to execute by subtracting the elapsed time from the time remaining until execution
    OutflowMap::iterator itr = _outflows.begin();
    for(; itr != _outflows.end(); itr++) {
        itr->second.second -= elapsed;
        if((*itr).second.second <= 0) {
            _readyToFlow.push_back((*itr).first);
        }
    }
}

bool LiquidManager::hasNextFlow() {
    return !_readyToFlow.empty();
}

int LiquidManager::getNextFlow(Vector3 &source, Vector3 &dest) {
    Vector3 nextSource = _readyToFlow.back();
    _readyToFlow.pop_back();

    source = nextSource;
    dest   = _outflows[nextSource].first;
    return _outflows[nextSource].second;
}

void LiquidManager::deleteInflow(Vector3 coords) { _inflows.erase(coords); }
void LiquidManager::deleteOutflow(Vector3 coords) { _outflows.erase(coords); }

void LiquidManager::printOutflows() {
    Info("Printing outflows:");
    OutflowMap::iterator itr = _outflows.begin();
    for(; itr != _outflows.end(); itr++) {
        Info("Outflow originating at " << itr->first << " goes to " << itr->second.first << " in " << itr->second.second);
    }
}

void LiquidManager::printInflows() {
    Info("Printing inflows:");
    InflowMap::iterator itr = _inflows.begin();
    for(; itr != _inflows.end(); itr++) {
        Info("Inflow into " << itr->first << " comes from " << itr->second);
    }
}

