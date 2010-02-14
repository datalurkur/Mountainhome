/*
 *  Billboard.h
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_
#include "Model.h"

class Billboard : public Model {
public:
    Billboard();
    virtual ~Billboard();

    virtual void render(RenderContext *context);
};

#endif
