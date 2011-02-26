/*
 *  PositionBuffer.h
 *  Mountainhome
 *
 *  Created by loch on 2/25/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _POSITIONBUFFER_H_
#define _POSITIONBUFFER_H_
#include "Buffer.h"

class PositionBuffer : public Buffer {
public:
    PositionBuffer(
        GLenum accessType,
        GLenum dataType,
        unsigned int elementSize,
        unsigned int elementCount,
        void *data
    );

    void enable();

    void disable();

};

#endif
