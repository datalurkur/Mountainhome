/*
 *  TerrainBindings.h
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _TerrainBINDINGS_H_
#define _TerrainBINDINGS_H_
#include "RubyBindings.h"
#include "Terrain.h"

class TerrainBindings : public RubyBindings<Terrain, false> {
public:
    static VALUE SetVoxelType(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE rVoxel);
    static VALUE GetVoxelType(VALUE rSelf, VALUE x, VALUE y, VALUE z);
    static VALUE SetVoxelParameter(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE rParameter, VALUE rParamValue);
    static VALUE GetVoxelParameter(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE rParameter);

    static VALUE OutOfBounds(VALUE self, VALUE x, VALUE y, VALUE z);

    static VALUE GetSurfaceLevel(VALUE self, VALUE x, VALUE y);
    static VALUE GetSurfaceType(VALUE self, VALUE x, VALUE y);

    static VALUE EachEmptyRange(VALUE self, VALUE x, VALUE y);
    static VALUE EachFilledRange(VALUE self, VALUE x, VALUE y);

    static VALUE Clear(VALUE self);

    /*! Gets the world's width. */
    static VALUE GetWidth(VALUE self);

    /*! Gets the world's height. */
    static VALUE GetHeight(VALUE self);

    /*! Gets the world's depth. */
    static VALUE GetDepth(VALUE self);

    static VALUE SetPolyReduction(VALUE self, VALUE val);

    static VALUE SetAutoUpdate(VALUE self, VALUE val);

protected:
    static ID parametersKey;

public:
    TerrainBindings();
    virtual ~TerrainBindings() {}

private:
    static bool ConvertRubyParameter(VALUE rParameter, ParameterData &cParameter);
    static bool ConvertCParameter(const ParameterData &cParameter, VALUE &rParameter);

    static void SetAndRegisterVoxel(Terrain *cSelf, int x, int y, int z, const Voxel &cVoxel);

    // Used in the foreach loop inside of GenerateCVoxelFromRVoxelType to set a single
    // parameter for a voxel.
    static int FillInCParam(VALUE key, VALUE value, Voxel *voxel);

    // If the index is not found, we need to create the material and register it. We do
    // this here because there are several variables in the ruby class object that we need
    // access to but do not want to expose elsewhere.
    static PaletteIndex RegisterVoxel(Terrain *cSelf, const Voxel &cVoxel);

    static void GenerateCVoxelFromRVoxelType(Terrain *cSelf, VALUE rVoxel, Voxel &outCVoxel);
};

#endif
