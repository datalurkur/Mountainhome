#include "LiquidSystem.h"

LiquidSystem::LiquidSystem(float msPerTick): _msPerTick(msPerTick) {
    _adjacency.insert(Vector3(-1,  0,  0));
    _adjacency.insert(Vector3( 1,  0,  0));
    _adjacency.insert(Vector3( 0, -1,  0));
    _adjacency.insert(Vector3( 0,  1,  0));
    _adjacency.insert(Vector3( 0,  0, -1));
    _adjacency.insert(Vector3( 0,  0,  1));
}

LiquidSystem::~LiquidSystem() {
    LiquidGlobList::iterator itr;
    for(itr = _globs.begin(); itr != _globs.end(); itr++) {
        delete *itr;
    }
    _globs.clear();
}

void LiquidSystem::processLiquid(const Vector3 &node) {
    if(!_initialized) {
        _freeLiquids.insert(node);
    } else {
        // This is for creating water out of nothing once the world has already been running (springs, wells, rain, etc)
        // TODO - This code needs writing
        ASSERT(0);
    }
}

void LiquidSystem::processVacuum(const Vector3 &node) {
    if(!_initialized) {
        _vacuums.insert(node);
    } else {
        LiquidGlobList::iterator itr;
        LiquidSourceList sources;

        for(itr = _globs.begin(); itr != _globs.end(); itr++) {
            Vector3 abutting;
            if((*itr)->findSource(node, _adjacency, abutting)) {
                sources.push_back(LiquidSource(abutting, (*itr)));
            }
        }
        
        if(!sources.empty()) {
            LiquidGlob *newGlob = new LiquidGlob(node, 0.0);
            LiquidFlow newFlow(node, newGlob);
            newFlow.addSources(sources);
            _flows.push_back(newFlow);
        } else {
            _vacuums.insert(node);
        }
    }
}

void LiquidSystem::setup() {
    LiquidNodeSet remaining;
    LiquidNodeSet::iterator itr;
    LiquidGlob *nextGlob;
    Vector3 startingPoint;
    
    ASSERT(!_initialized);
    
    remaining = _freeLiquids;
    _freeLiquids.clear();
    while(!remaining.empty()) {
        Info("Forming globs with " << remaining.size() << " nodes remaining to be globbed");
        nextGlob = new LiquidGlob(remaining);
        startingPoint = *(remaining.begin());
        remaining.clear();
        
        nextGlob->consolidate(remaining, _adjacency, startingPoint);
        _globs.push_back(nextGlob);
    }

    _initialized = true;

    remaining = _vacuums;
    _vacuums.clear();
    for(itr = remaining.begin(); itr != remaining.end(); itr++) {
        processVacuum(*itr);
    }
}

void LiquidSystem::update(int elapsed) {
    LiquidFlowList::iterator flowItr;
    
    // Move liquid according to each active flow
    for(flowItr = _flows.begin(); flowItr != _flows.end(); flowItr++) {
        SortedLiquidSources sources;

        // Start with the source with the lowest surface level and work our way up
        flowItr->getSortedSources(sources);
        while(!sources.empty()) {
            float levelDifference;
            float flowVolume, amountProvided, amountDrained;
            LiquidGlobList splitGlobs;
            
            LiquidSource source = sources.top();
            sources.pop();
            
            // Find the difference in surface levels and determine how much liquid can flow from the source to the drain
            levelDifference = source.glob()->getSurfaceLevel() - flowItr->glob()->getSurfaceLevel();
            ASSERT(levelDifference > 0.0);
            if(levelDifference > 1.0) { flowVolume = 1.0; }
            else                      { flowVolume = levelDifference; }
            
            // TODO - use the elapsed time to control this value
            //flowVolume *= (elapsed / _msPerTick);
            
            // Attempt to pull the requisite volume from the source
            if(source.glob()->drainFrom(source.point(), flowVolume, amountProvided, splitGlobs)) {
                remapGlob(source.glob(), splitGlobs);
            }
        }
    }
}