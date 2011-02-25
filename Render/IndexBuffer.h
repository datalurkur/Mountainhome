/*
 *  IndexBuffer.h
 *  Render
 *
 *  Created by loch on 12/15/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _INDEXBUFFER_H_
#define _INDEXBUFFER_H_
#include "Buffer.h"

/*! A buffer of indices into a VertexArray (or any arbitrary set of attribute Buffers).
 *  This basically allows us to reuse elements in an attribute Buffers. Think of it as a
 *  buffer of Vertex pointers, where each pointer is really just an index into the
 *  currently active set of attribute Buffers (which is typically represented by a
 *  VertexArray). By reusing vertices, we send less data down to the graphics card,
 *  speeding things up significantly. Alternatively, IndexBuffers can be used to skip
 *  around and leave out certain elements in a VertexArray. This could allow us to have a
 *  single VertexArray shared by many different RenderOperations, each of which have thier
 *  own IndexBuffers and draw different things.
 * \note IndexBuffers can be quite helpful, but they're not required.
 * \note See Renderable for a more complete explanation of how rendering works.
 * \todo Remove direct GL references.
 * \todo Allow for editting of data that is stored in gl buffers.
 * \seealso Renderable
 * \seealso RenderOperation
 * \seealso GenericAttributeBuffer
 * \seealso VertexArray */
class IndexBuffer : public Buffer {
public:
    /*! Creates a new IndexBuffer
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
     * \param count The number of indices in the buffer.
     * \param data A pointer to the indices. This will be stored but NOT deleted. */
    IndexBuffer(
        GLenum accessType,
        GLenum dataType,
        unsigned int count,
        void *data
    );

private:
    friend class RenderOperation;
    void render(PrimitiveType type);

};

#endif
