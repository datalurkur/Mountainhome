#ifndef _MESHPARTRENDERABLE_H_
#define _MESHPARTRENDERABLE_H_

#include "Renderable.h"
#include "RenderContext.h"

class MeshPartRenderable : public Renderable {
public:
    MeshPartRenderable(const Matrix &posMatrix, unsigned int indexCount, unsigned int indexBuffer, unsigned int vertexBuffer, unsigned int normalBuffer, unsigned int texCoordBuffer);
    ~MeshPartRenderable();

public:
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
