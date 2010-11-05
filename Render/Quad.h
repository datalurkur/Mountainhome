/*
 *  Quad.h
 *  System
 *
 *  Created by loch on 2/23/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _QUAD_H_
#define _QUAD_H_
#include "Model.h"

class Quad : public Model {
public:
    Quad(Real width, Real height);
    virtual ~Quad();
private:
    void buildQuad();

    Real _width, _height;

};

#endif
