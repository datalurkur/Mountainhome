#include "MeshPartRenderable.h"
#include "RenderContext.h"
#include "ModelMeshPart.h"
#include "Model.h"

MeshPartRenderable::MeshPartRenderable(Model *model, ModelMeshPart *meshPart)
: _model(model), _meshPart(meshPart)
{
    Material *mat = NULL;

    if(meshPart) {
        mat = meshPart->getMaterial();
    }

    if(mat == NULL) {
        mat = model->getDefaultMaterial();
    }

    setMaterial(mat);
}

MeshPartRenderable::~MeshPartRenderable() {}

void MeshPartRenderable::render(RenderContext *context) {
    // Need to have at least a vertex buffer!
    ASSERT(_model->getVertexBuffer());

    // Setup the index count and start index.
    unsigned int indexCount;
    unsigned int startIndex;
    if (_meshPart) {
        indexCount = _meshPart->getIndexCount();
        startIndex = _meshPart->getStartIndex();
    } else {
        indexCount = _model->getIndexCount();
        startIndex = 0;
    }

    Material *material = getMaterial();
    if(material == NULL) {
        material = _model->getDefaultMaterial();
    }
    context->setActiveMaterial(material);
    context->setModelMatrix(_positionalMatrix);

    // Render stuff!
    context->addToPrimitiveCount(indexCount / 3);
    context->addToVertexCount(indexCount);
    context->addToModelCount(1);

    glEnableClientState(GL_VERTEX_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, _model->getVertexBuffer());
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    if (_model->getNormalBuffer()) {
        glEnableClientState(GL_NORMAL_ARRAY);

        glBindBuffer(GL_ARRAY_BUFFER, _model->getNormalBuffer());
        glNormalPointer(GL_FLOAT, 0, NULL);
    }

    if (_model->getTexCoordBuffer()) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glBindBuffer(GL_ARRAY_BUFFER, _model->getTexCoordBuffer());
        glTexCoordPointer(2, GL_FLOAT, 0, NULL);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _model->getIndexBuffer());
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, NULL);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    context->unsetActiveMaterial(material);
}

void MeshPartRenderable::setPositionalMatrix(const Matrix &posMatrix) {
    _positionalMatrix = posMatrix;
}
