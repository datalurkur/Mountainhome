/*
 *  AttributeBuffer.h
 *  Render
 *
 *  Created by loch on 12/15/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _ATTRIBUTEBUFFER_H_
#define _ATTRIBUTEBUFFER_H_
#include "GL_Helper.h"
#include "Buffer.h"

/*! A buffer that contains a single attribute associated with a vertex. Multiple
 *  attributes can be combined in a VertexArray. Aligned elements in each of the active
 *  AttributeBuffers represent a single vertex. For example, a VertexArray may have an
 *  AttributeBuffer for vertex position, vertex normal, and vertex texture coordinates.
 *  All of these combines form a single vertex, which is used by the graphics hardware to
 *  render something to screen. AttributeBuffer can be enabled and disabled individually,
 *  but it is probably easiest to work with VertexArrays. Remember, when binding to
 *  channels, the channels need to be associated with attributes in the active Shader.
 * \note See Renderable for a more complete explanation of how rendering works.
 * \todo Remove GL references.
 * \todo Allow for editting of data that is stored in gl buffers.
 * \seealso Renderable
 * \seealso VertexArray
 * \seealso Shader */
class AttributeBuffer : public Buffer {
public:
    /*! Creates a new AttributeBuffer
     * \param accessType Defines how the buffer should be stored internally. Some are
     *  slower to update, but are faster to render, while others are quicker to update,
     *  but slower to render. Here are the standard options:
     *      GL_STREAM_DRAW    GL_STATIC_DRAW    GL_DYNAMIC_DRAW
     *      GL_STREAM_READ    GL_STATIC_READ    GL_DYNAMIC_READ
     *      GL_STREAM_COPY    GL_STATIC_COPY    GL_DYNAMIC_COPY
     *  Note that passing NULL has special behavior. A GL side buffer will not be created
     *  and the data will be read out from the data pointer every read call.
     * \param dataType Specifies the type of data in the data buffer.
     *      GL_BYTE     GL_UNSIGNED_BYTE
     *      GL_SHORT    GL_UNSIGNED_SHORT
     *      GL_INT      GL_UNSIGNED_INT
     *      GL_FLOAT    GL_DOUBLE
     * \param elementSize The size of an individual element in the buffer. An element in a
     *  normal attribute buffer will have 3 elements (x, y, z). A color attribute may have
     *  3 (rgb) or 4 (rgba). Only byte, short, and int types can have a non-1 sizes.
     * \param elementCount The number of elements in the buffer.
     * \param data A pointer to the data. This will be stored but NOT deleted. */
    AttributeBuffer(GLenum accessType, GLenum dataType, unsigned int elementSize, unsigned int elementCount, void *data);
    ~AttributeBuffer();

    void enable(int channel);
    void disable();

private:
    /*! The number of components per element. Must be either 1, 2, 3, or 4. */
    unsigned int _elementSize;

    /*! The channel the Buffer is currently bound to. Used for easy disabling. */
    int _activeChannel;

};

#endif
