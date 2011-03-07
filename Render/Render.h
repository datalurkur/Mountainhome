/*
 *  Render.h
 *  Base
 *
 *  Created by Brent Wilson on 12/22/09.
 *  Copyright 2009 NVIDIA. All rights reserved.
 *
 */

#ifndef _RENDER_H_
#define _RENDER_H_

#include <Base/Base.h>

enum PrimitiveType {
    POINTS,
    LINES,
    LINE_STRIP,
    TRIANGLES,
    TRIANGLE_STRIP,
    QUADS
};

enum CullMode {
    NONE,
    FRONT,
    BACK,
    FRONT_AND_BACK
};

#endif
