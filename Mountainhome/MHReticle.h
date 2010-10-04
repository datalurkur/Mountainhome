/*
 *  MHReticle.h
 *  Mountainhome
 *
 *  Created by apple on 9/29/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHRETICLE_H_
#define _MHRETICLE_H_
#include "Model.h"

class MHReticle : public Model {
public:
    MHReticle(Real radius);
    virtual ~MHReticle();

    static void SetupBindings();

private:
    Real _size;
};

#endif
