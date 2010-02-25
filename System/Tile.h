/*
 *  Tile.h
 *  System
 *
 *  Created by datalurkur on 2/24/10.
 *
 */

#ifndef _TILE_H_
#define _TILE_H_
#include "Model.h"

enum TileTypes {ALLUP = 0, ONEUP, TWOADJUP, TWOOPPUP, THREEUP, NONEUP, WALL};

class Tile : public Model {
public:
    Tile(char type);
    virtual ~Tile();

    virtual void render(RenderContext *context);

private:
    char _type;
};

#endif
