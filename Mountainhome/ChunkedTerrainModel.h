/*
 *  ChunkedTerrainModel.h
 *  Mountainhome
 *
 *  Created by loch on 7/12/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

///\todo XXXBMW: Needs cleanup!!!!

#ifndef _CHUNKEDTERRAINMODEL_H_
#define _CHUNKEDTERRAINMODEL_H_
#include <Render/Model.h>

#include "ChunkedTerrainGroup.h"
#include "TileGrid.h"

class TranslationMatrix {
public:
    TranslationMatrix(int width, int height);
    virtual ~TranslationMatrix();
    void setIndex(int x, int y, int z, int index);
    int getIndex(int x, int y, int z);

private:
    typedef std::map<int, int> ZMap;
    ZMap *_matrix;
    int _width;
    int _height;
};

class DynamicModel {
public:
    DynamicModel(int width, int height, int xOffset = 0, int yOffset = 0, int zOffset = 0);
    ~DynamicModel();

    void addVertex(Real x, Real y, Real z);
    int getVertexCount();
    int getIndexCount();

    Vector3 *buildStaticVertexArray();
    Vector2 *buildStaticTexCoordArray();
    unsigned int *buildStaticIndexArray();
    Vector3 *buildStaticNormalArray();

private:
    std::vector<Vector3> _vertsArray;
    std::vector<Vector2> _texCoordsArray;
    std::vector<unsigned int> _indexArray;
    TranslationMatrix *_matrix;
    int _xOffset;
    int _yOffset;
    int _zOffset;

};

class ChunkedTerrainModel : public Model {
public:
    ChunkedTerrainModel(TileGrid *grid, TileType type,
        int xChunkIndex, int yChunkIndex, int zChunkIndex);

    virtual ~ChunkedTerrainModel();

    std::string getName();

    virtual int update(bool doPolyReduction) = 0;

protected:
    static const int ChunkSize = ChunkedTerrainGroup::ChunkSize;

protected:
    TileGrid *_grid;
    TileType _type;

    int _xLoc, _yLoc, _zLoc;

    std::string _name;
};

#endif
