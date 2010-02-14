/*
 *  Teapot.h
 *  System
 *
 *  Created by loch on 1/29/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _TEAPOT_H_
#define _TEAPOT_H_
#include "Model.h"

class Teapot : public Model {
public:
    Teapot(Real size);
    virtual ~Teapot();

    virtual void render(RenderContext *context);

private:
    Real _size;

};

#endif
