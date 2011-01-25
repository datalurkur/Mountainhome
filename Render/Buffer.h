/*
 *  Buffer.h
 *  Mountainhome
 *
 *  Created by loch on 12/17/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _BUFFER_H_
#define _BUFFER_H_
#include "GL_Helper.h"

/*! A generic graphics hardware buffer. This class is absolutely not meant to be used by
 *  itself. it JUST provides a place to put shared interface logic between the IndexBuffer
 *  class and the AttributeBuffer class.
 * \seealso AttributeBuffer
 * \seealso IndexBuffer */
class Buffer {
public:
    Buffer(GLenum bufferType, GLenum accessType, GLenum dataType, unsigned int elementCount, void *data);
    virtual ~Buffer();

    GLenum getAccessType();

    GLenum getDataType();

    unsigned int getElementCount();

    unsigned int getByteCount();

    template <typename T>
    T * getData() {
        return static_cast<T*>(_data);
    }
    
protected:
    /*! Specifies the type of the buffer. */
    GLenum _bufferType;

    /*! How the data in the buffer will be accessed. Determines where the data will live
     *  in memory. Either vidmem, sysmem, or both. */
    GLenum _accessType;

    /*! Represents the type of data contained in the buffer. */
    GLenum _dataType;

    /*! The number of elements in the buffer. */
    unsigned int _elementCount;

    /*! A sysmem copy of the data. */
    void *_data;

    /*! The unique identifier for the buffer. */
    GLuint _handle;

    /*! The number of bytes in the buffer. */
    unsigned int _byteCount;

};

#endif
