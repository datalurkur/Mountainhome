#ifndef _MESHPARTRENDERABLE_H_
#define _MESHPARTRENDERABLE_H_
#include <Base/Matrix.h>
#include "Renderable.h"

class Model;
class ModelMeshPart;
class RenderContext;
class MeshPartRenderable : public Renderable {
public:
    MeshPartRenderable(Model *model, ModelMeshPart *meshPart);
    ~MeshPartRenderable();

    /*! Renders the underlying buffers.
     * \todo Support index buffer offsets and null index buffers (vertex based rendering).
     * \todo Normals and tex coords should be supported as generic attribute buffers. */
    void render(RenderContext *context);

    void setPositionalMatrix(const Matrix &posMatx);

protected:
    Matrix _positionalMatrix;

    unsigned int _indexCount;
    unsigned int _startIndex;

    // Buffers
    unsigned int _indexBuffer;
    unsigned int _vertexBuffer;
    unsigned int _normalBuffer;
    unsigned int _texCoordBuffer;
};

#endif
