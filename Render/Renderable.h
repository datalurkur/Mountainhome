/*
 *  Renderable.h
 *  Mountainhome
 *
 *  Created by loch on 7/19/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _RENDERABLE_H_
#define _RENDERABLE_H_
#include <Base/AABB.h>

#include "RenderParameterContainer.h"
#include "ShaderParameter.h"
#include "RenderOperation.h"
#include "Material.h"

class Renderable;
typedef std::list<Renderable*> RenderableList;

class Material;
class RenderContext;

/*! This brings everything together. It represents a single, unique, visible object in the
 *  world. It brings together a Material, per-object ShaderParameters (like the modelview
 *  matrix), and a RenderOperation. Subclasses can be created, which allow for special
 *  pre/post render behavior. This can allow us to handle any logic that only matters if
 *  the object is visible.
 *  
 *  Here is a more complete picture of how rendering works:
 *
 *    When rendering something, you supply a set of arbitrarily complex vertices to the
 *    GPU, which are processed in some user-defined, arbitrarily complex way, transformed
 *    into a space that maps to the currently active viewport on screen, broken into
 *    pixels, processed some more in a final, user-defined, arbitrarily complex way, and
 *    finally displayed on the monitor.
 *
 *    At this level, on the CPU side of things, we have several players:
 *      - Shader
 *      - ShaderParameter
 *      - RenderOperation
 *      - IndexBuffer
 *      - VertexArray
 *      - AttributeBuffer
 *  
 *    These can be split into two groups. The first includes Shader and ShaderParameter.
 *    Shader represents all of the above mentioned "user-defined, arbitrarily complex"
 *    processing. There are many steps in this processing (vertex, geometry, tessellation,
 *    pixel), some of which are often omitted. Shader represents a combination of all of
 *    them. While vertices are the initial input to a Shader, these individual steps often
 *    have variables which are uniform across all input to them, which we call
 *    ShaderParameters. These can be things like lighting coefficients and the like.
 *
 *    The other classes fall into the second group, which represent how we send vertices
 *    to the currently active Shader. Vertices are often thought of as positions in the
 *    world, but really they're better defined as arbitrarily complex input to a Shader
 *    Vertices can have many attributes associated with them. Position is probably the
 *    most common attribute, but it is not required, nor is it the only one. Other common
 *    attributes include things like normal and texture coordinates. These individual
 *    attributes are grouped together in something called an AttributeBuffer, with each
 *    index in an AttributeBuffer representing a single attribute of a vertex. To fully
 *    describe a vertex, several AttributeBuffers are typically needed and can be grouped
 *    together, with names associated with them (these are used when binding to a Shader),
 *    in a VertexArray.
 *
 *    More than just vertices are needed, though. To rasterize properly, vertices need
 *    to be grouped together to form primitives (points, lines, triangles, etc...). A
 *    RenderOperation is what combines the VertexArray with a PrimitiveType for rendering.
 *    It also provides an optional spot for an IndexBuffer (contains a set of indices into
 *    a VertexArray), which is a special buffer used to avoid sending duplicate data to
 *    the graphics card when a single vertex is needed many times. If an IndexBuffer is
 *    specified, primitives are built by reading indices out of the IndexBuffer, one after
 *    the next, and looking up into the VertexArray. If no IndexBuffer is specified,
 *    primitives are built by reading vertices out of the VertexArray, one after the next.
 *
 *    All of this gives you everything you need to setup custom logic on the graphics
 *    hardware, specify uniform variables, and send vertices down to be processed. This is
 *    a very low level way of working with things, though, and a more natural interface is
 *    provided with the following classes:
 *      - Renderable
 *      - Material
 *
 *    In a typical world, there will be many unique objects in the world. These unique
 *    objects often share many qualities with each other, however. These two classes
 *    provide a natural way to set ShaderParameters based on this idea. The Material
 *    contains ShaderParameters that are shared across a set of objects in the world,
 *    while Renderable contains any other parameters that are unique to just the object.
 *
 *    The Renderable does more than handle unique ShaderParameters. It also maintains a
 *    RenderOperation, which allows for sending vertices to the appropriate program, and
 *    it contains a Material, which allows it to have access to its set of shared
 *    ShaderParameters and the Shader that will be used to render it. In this way, the
 *    Renderable brings everything together into one neat, customizable package.
 *
 *  An example, to help clarify:
 *
 *    Imagine a relatively simple world, where we're rendering a forest scene with some
 *    terrain, trees, and grass. Because this is simple, we'll only have a single Shader,
 *    which provides us with some per-pixel lighting and the ability to set a texture and
 *    some lighting qualities like shininess, diffuse and ambient coefficients, etc...
 *    This Shader will be used to render absolutely everything.
 *
 *    In this world, we'll only worry about one kind of tree (oak), one kind of grass
 *    (crab). To handle this, we'll want three Materials, which represent how sets of
 *    things in the world should appear. We can have one Material for our oak tree,
 *    another for our crab grass, and a final one for the terrain. Each of these will set
 *    the appropriate, shader ShaderParameters, such as texture and lighting parameters.
 *
 *    From here, each tree, each clump of grass, and the terrain will have an associated
 *    Renderable. These Renderables will allow unique parameters to be set, such as the
 *    modelview matrix, which describes where in the world a tree is. Even though all of
 *    our trees will share the same texture and lighting params, obviously each one will
 *    need to be rendered in a different place with hopefully with a different
 *    orientation.
 *
 *    Renderables also require RenderOperation. The RenderOperation is what sends vertices
 *    to the graphics card. While we may only have one type of tree (oak), we may want
 *    some variety to the shape of these trees. If we have two different tree shapes
 *    (short and tall?), then we'll need two different RenderOperations to describe these
 *    different shapes (and one RenderOperation for the terrain and one for the grass).
 *    These RenderOperations will likely just be drawing everything with triangles. We
 *    could set the two tree types up to share VertexArrays and have different
 *    IndexBuffers if we want, or they could each have their own VertexArray and their own
 *    IndexBuffer (or no IndexBuffer at all!). It doesn't really matter how this is
 *    handled. We WILL need to ensure the VertexArrays each have certain attributes
 *    required by our Shader, though. Likely, three will be enough: position, normal,
 *    texture coordinate. Each of these attributes will need an associated AttributeBuffer
 *    in the VertexArray.
 * \seealso VertexArray
 * \seealso IndexBuffer
 * \seealso AttributeBuffer
 * \seealso Material
 * \seealso Shader
 * \seealso RenderOperation */
class Renderable : public RenderParameterContainer {
public:
    Renderable(RenderOperation *op, Material *mat);
    virtual ~Renderable();

    RenderOperation *getRenderOperation();

    void setMaterial(Material *newMat);

    Material *getMaterial();

    void setModelMatrix(const Matrix &mat);

    const Matrix & getModelMatrix();

    /*! Called before this Renderable is rendered. By default, it sets any local
     *  ShaderParameters in its Material's Shader. */
    virtual void preRenderNotice();

    /*! Called after this Renderable is rendered. */
    virtual void postRenderNotice();

    /*! Used for sorting lists of renderables for rendering speed.
     * \todo Needs to be setup to consider alpha blending. */
    bool operator < (const Renderable& rhs);

protected:
    Renderable();

private:
    ShaderParameterMap _params;
    RenderOperation *_renderOp;
    Material *_material;
    Matrix _modelMatrix;

};

#endif
