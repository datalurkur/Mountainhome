#include "MeshPartRenderable.h"

MeshPartRenderable::MeshPartRenderable(const Matrix &posMatrix, unsigned int indexCount,
    unsigned int indexBuffer, unsigned int vertexBuffer, unsigned int normalBuffer, unsigned int texCoordBuffer):
    _positionalMatrix(posMatrix), _indexCount(indexCount), 
    _indexBuffer(indexBuffer), _vertexBuffer(vertexBuffer), _texCoordBuffer(texCoordBuffer) {}

MeshPartRenderable::~MeshPartRenderable() {}

void MeshPartRenderable::render(RenderContext *context) {
    Info("Rendering mesh part");
    context->setActiveMaterial(getMaterial());
    context->setModelMatrix(_positionalMatrix);

    // Render stuff!
    context->addToPrimitiveCount(_indexCount / 3);
    context->addToVertexCount(_indexCount);
    context->addToModelCount(1);

    glEnableClientState(GL_VERTEX_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    if (_normalBuffer) {
        glEnableClientState(GL_NORMAL_ARRAY);

        glBindBuffer(GL_ARRAY_BUFFER, _normalBuffer);
        glNormalPointer(GL_FLOAT, 0, NULL);
    }

    if (_texCoordBuffer) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glBindBuffer(GL_ARRAY_BUFFER, _texCoordBuffer);
        glTexCoordPointer(2, GL_FLOAT, 0, NULL);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, NULL);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    context->unsetActiveMaterial(getMaterial());
}

void MeshPartRenderable::setPositionalMatrix(const Matrix &posMatrix) {
    _positionalMatrix = posMatrix;
}
