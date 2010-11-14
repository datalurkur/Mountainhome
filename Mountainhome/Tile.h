/*
 *  Tile.h
 *  Mountainhome
 *
 *  Created by Paul Wuersig on 11/13/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 *  Tiles consist of two parts - a type and parameters, which are many boolean values.
 *  Parameter definitions are hard-coded here, and passed into Ruby in TileBindings.h.
 */

#ifndef _TILE_H_
#define _TILE_H_
#include <stdlib.h>

// Some basic parameters.

#define LIQUID      1
#define UNKNOWN1    2
#define UNKNOWN2    4
#define UNKNOWN3    8

// Typedefs, as these types may change.

typedef char TileType;
typedef unsigned int TileParameter;

struct Tile {
    TileType Type;
    TileParameter Parameters;
    
    Tile() {
        // Create an empty tile with no parameters by default.
        Type = NULL;
        Parameters = NULL;
    }
    
    Tile(TileType type, TileParameter params) {
        Type = type;
        Parameters = params;
    }
    
    bool getParameter(TileParameter param) {
        return Parameters & param != 0;
    }
    
    void setParameter(TileParameter param, bool value) {
        if(value) {
            AddParameter(param);
        } else {
            RemoveParameter(param);
        }
    }
    
    void AddParameter(TileParameter param) {
        Parameters |= param;
    }
    
    void RemoveParameter(TileParameter param) {
        Parameters ^= param;
    }
    
    bool Equals(Tile other)
    {
        return this->Type == other.Type && this->Parameters == other.Parameters;
    }
};

#pragma mark Tile Creation Macros

#define NEW_TILE(type) (*(new Tile(type, NULL)))
#define NEW_LIQUID(type) (*(new Tile(type, LIQUID)))
//#define NEW_THINGIE(type) (*(new Tile(type, LIQUID | NEW_THINGIE)))

#endif
