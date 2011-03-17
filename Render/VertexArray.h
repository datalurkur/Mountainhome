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

class GenericAttributeBuffer;
class PositionBuffer;
class NormalBuffer;
class TexCoordBuffer;

typedef std::vector<std::string> VertexArrayLayout;

/*! VertexArray represent a set of attribute Buffers, both specific (position, normal, and
 *  texcood) and generic. Aligned elements in each of the child attribute Buffers
 *  represent a single vertex. For example, a VertexArray may have an attribute Buffer for
 *  vertex position, vertex normal, and vertex texture coordinates. All of these combines
 *  form a single vertex, which is used by the graphics hardware to render something to
 *  screen. Vertices are used as the most basic input to the active Shader. VertexArrays
 *  may be associated with a Renderable for rendering.
 * 
 *  GenericAttributeBuffers are also associated with names. These names are used when
 *  binding to a Shader. This simplifies associating GenericAttributeBuffers with
 *  attribute channels and then attribute channels with Shader attributes. Shaders may
 *  simply take a VertexArrayLayout, with the GenericAttributeBuffer names representing
 *  the name of the attribute in the Shader.
 * \note See Renderable for a more complete explanation of how rendering works.
 * \seealso GenericAttributeBuffer
 * \seealso IndexBuffer
 * \seealso PositionBuffer
 * \seealso NormalBuffer
 * \seealso TexCoordBuffer
 * \seealso Renderable
 * \seealso Shader */
class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    /*! Deletes all of the underlying buffers. */
    void deleteAllBuffers();

    /*! Gets the number of vertices in the VertexArray. All child GenericAttributeBuffers must
     *  have the same number of elements to be valid. */
    unsigned int getElementCount() const;

    /*! Get the number of GenericAttributeBuffers in the VertexArray */
    unsigned int getAttributeCount() const;

    /*! Sets the position buffer for this VertexArray. The generic addGenericAttributeBuffer
     *  function may be used for this, but this function allows access to the data using
     *  the position keywords in shaders, which may be preferable.
     * \seealso VertexArray::addGenericAttributeBuffer */
    void setPositionBuffer(PositionBuffer *buffer);

    /*! Gets the current position buffer for this VertexArray. */
    PositionBuffer * getPositionBuffer();

    /*! Sets the position buffer for this VertexArray. The generic addGenericAttributeBuffer
     *  function may be used for this, but this function allows access to the data using
     *  the position keywords in shaders, which may be preferable.
     * \seealso VertexArray::addGenericAttributeBuffer */
    void setNormalBuffer(NormalBuffer *buffer);

    /*! Gets the current position buffer for this VertexArray. */
    NormalBuffer * getNormalBuffer();

    /*! Sets the position buffer for this VertexArray. The generic addGenericAttributeBuffer
     *  function may be used for this, but this function allows access to the data using
     *  the position keywords in shaders, which may be preferable.
     * \seealso VertexArray::addGenericAttributeBuffer */
    void setTexCoordBuffer(int index, TexCoordBuffer *buffer);

    /*! Gets the current position buffer for this VertexArray. */
    TexCoordBuffer * getTexCoordBuffer(int index);

    /*! Adds an GenericAttributeBuffer to the VertexArray with the given name. */
    unsigned int addGenericAttributeBuffer(const std::string &name, GenericAttributeBuffer *buffer);

    /*! Gets the GenericAttributeBuffer at the named index. */
    GenericAttributeBuffer * getGenericAttributeBuffer(unsigned int index) const;

    /*! Gets the named GenericAttributeBuffer. */
    GenericAttributeBuffer * getGenericAttributeBuffer(const std::string &name) const;

    /*! Gets the set of GenericAttributeBuffer names that are stored in this VertexArray. The
     *  index of a given name indicates which channel the accociated GenericAttributeBuffer will
     *  be bound to. */
    const VertexArrayLayout & getVertexArrayLayout() const;

    /*! Enables the VertexArray for rendering. */
    void enable();

    /*! Disables the VertexArray. */
    void disable();

private:
    std::vector<GenericAttributeBuffer*> _buffers;

    VertexArrayLayout _names;

    PositionBuffer *_positions;

    NormalBuffer *_normals;

    std::vector<TexCoordBuffer*> _texCoords;

};

#endif
