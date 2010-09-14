/*
 *  ModelMeshPart.h
 *  Render
 *
 *  Created by Andrew Jean on on 8/05/10.
 *  Copyright 2010 Mountainhome Project
 *  All rights reserved.
 *
 */

#ifndef _MODELMESHPART_H_
#define _MODELMESHPART_H_

#include <Base/Vector.h>
#include "Material.h"

class ModelMeshPart {
public:
    ModelMeshPart();
    ModelMeshPart(unsigned int numIndices, unsigned int startIndex);
    ~ModelMeshPart();

    unsigned int getIndexCount();
    unsigned int getStartIndex();

    Material *getMaterial() { return _mat; }

protected:
    Material *_mat;             // The material associated with this mesh

    std::string _tag;           // This mesh's identifier

    unsigned int _startIndex;   // The location in the index buffer to begin reading verts

    unsigned int _numIndices;   // The number of vertices used during a draw call
    unsigned int _primCount;    // The number of primivites rendered
};

#endif
