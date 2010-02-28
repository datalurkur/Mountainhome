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

enum TileTypes {TILE_WALL=0, TILE_CONVEX_CORNER, TILE_PINCH, TILE_RAMP, TILE_CONCAVE_CORNER};

class Tile : public Model {
public:
    Tile(char type);
    virtual ~Tile();

    virtual void render(RenderContext *context);

private:
    char _type;
};

#endif
