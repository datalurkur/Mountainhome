/*
 *  Model.h
 *  Engine
 *
 *  Created by Brent Wilson on 3/29/05.
 *  Copyright 2005 Brent Wilson. All rights reserved.
 *
 */

#include "Model.h"

Model::Model() {}
Model::~Model() {}
const AABB3& Model::getBoundingBox() const { return _boundingBox; }
