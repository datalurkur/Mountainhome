/*
 *  TexCoordBuffer.h
 *  Mountainhome
 *
 *  Created by loch on 2/25/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _TEXCOORDBUFFER_H_
#define _TEXCOORDBUFFER_H_
#include "Buffer.h"

class TexCoordBuffer : public Buffer {
public:
    TexCoordBuffer(
        GLenum accessType,
        GLenum dataType,
        unsigned int elementSize,
        unsigned int elementCount,
        void *data
    );

    void enable(int channel);

    void disable();

private:
    int _activeChannel;

};

#endif
