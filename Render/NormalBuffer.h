/*
 *  NormalBuffer.h
 *  Mountainhome
 *
 *  Created by loch on 2/25/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _NORMALBUFFER_H_
#define _NORMALBUFFER_H_
#include "Buffer.h"

class NormalBuffer : public Buffer {
public:
    NormalBuffer(
        GLenum accessType,
        GLenum dataType,
        unsigned int elementCount,
        void *data
    );

    void enable();

    void disable();
};

#endif
