#ifndef _MHTERRAIN_H_
#define _MHTERRAIN_H_

#include <Base/Vector.h>
#include "RubyBindings.h"
#include "TileGroup.h"

// TEMPORARY DEFINES UNTIL WE GET SOME ACTUAL TILE TYPES
#define TILE_EMPTY      0
#define TILE_SEDIMENT   1
// =====================================================


class MHTerrain: public ManyObjectBinding<MHTerrain> {
public:
    MHTerrain(int w, int h, int d);
    virtual ~MHTerrain();

    static void SetupBindings();
    static VALUE Initialize(VALUE self, VALUE w, VALUE h, VALUE d);
    static VALUE GetTile(VALUE self, VALUE x, VALUE y, VALUE z);
    static VALUE SetTile(VALUE self, VALUE x, VALUE y, VALUE z, VALUE type);
    static VALUE SurfaceTile(VALUE self, VALUE x, VALUE y);

    short getTile(int x, int y, int z);
    void setTile(int x, int y, int z, short type);

    int getSurfaceLevel(int x, int y);

private:
    TileGroup<short> *_rootGroup;
};

#endif
