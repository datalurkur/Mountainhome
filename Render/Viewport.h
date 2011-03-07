/*
 *  Viewport.h
 *  Render
 *
 *  Created by Brent Wilson on 4/6/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_

struct Viewport {
    Viewport(int x, int y, int w, int h);
    Viewport(const Viewport &other);
    virtual ~Viewport();

    const Viewport & operator=(const Viewport rhs);

    int width, height;
    int xPos, yPos;
};

#endif
