/*
 *  Viewport.cpp
 *  Render
 *
 *  Created by Brent Wilson on 4/6/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "Viewport.h"

Viewport::Viewport(int x, int y, int w, int h):
    width(w), height(h), xPos(x), yPos(y)
{}

Viewport::Viewport(const Viewport &other):
    width(other.width), height(other.height),
    xPos(other.xPos), yPos(other.yPos)
{}

Viewport::~Viewport() {}

const Viewport & Viewport::operator=(const Viewport rhs) {
    width = rhs.width;
    height = rhs.height;
    xPos = rhs.xPos;
    yPos = rhs.yPos;
    return *this;
}
