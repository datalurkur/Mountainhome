/*
 *  VertexArray.h
 *  Mountainhome
 *
 *  Created by loch on 12/17/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _VERTEXARRAY_H_
#define _VERTEXARRAY_H_
#include <vector>
#include <string>

#include "AttributeBuffer.h"

typedef std::vector<std::string> VertexArrayLayout;

/*! VertexArray represent a set of AttributeBuffers. Aligned elements in each of the child
 *  AttributeBuffers represent a single vertex. For example, a VertexArray may have an
 *  AttributeBuffer for vertex position, vertex normal, and vertex texture coordinates.
 *  All of these combines form a single vertex, which is used by the graphics hardware to
 *  render something to screen. Vertices are used as the most basic input to the active
 *  Shader. VertexArrays may be associated with a Renderable for rendering.
 * 
 *  AttributeBuffers are also associated with names. These names are used when binding to
 *  a Shader. This simplifies associating AttributeBuffers with attribute channels and
 *  then attribute channels with Shader attributes. Shaders may simply take a
 *  VertexArrayLayout, with the AttributeBuffer names representing the name of the
 *  attribute in the Shader.
 * \note See Renderable for a more complete explanation of how rendering works.
 * \seealso AttributeBuffer
 * \seealso IndexBuffer
 * \seealso Renderable
 * \seealso Shader */
class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    /*! Gets the number of vertices in the VertexArray. All child AttributeBuffers must
     *  have the same number of elements to be valid. */
     unsigned int getElementCount() const;

    /*! Get the number of AttributeBuffers in the VertexArray */
    unsigned int getAttributeCount() const;

    /*! Adds an AttributeBuffer to the VertexArray with the given name. */
    unsigned int addAttributeBuffer(const std::string &name, AttributeBuffer *buffer);

    /*! Gets the AttributeBuffer at the named index. */
    AttributeBuffer * getAttributeBuffer(unsigned int index) const;

    /*! Gets the named AttributeBuffer. */
    AttributeBuffer * getAttributeBuffer(const std::string &name) const;

    /*! Gets the set of AttributeBuffer names that are stored in this VertexArray. The
     *  index of a given name indicates which channel the accociated AttributeBuffer will
     *  be bound to. */
    const VertexArrayLayout & getVertexArrayLayout() const;

    /*! Enables the VertexArray for rendering. */
    void enable();

    /*! Disables the VertexArray. */
    void disable();

private:
    std::vector<AttributeBuffer*> _buffers;
    VertexArrayLayout _names;

};

#endif
