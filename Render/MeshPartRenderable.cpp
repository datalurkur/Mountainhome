#include "MeshPartRenderable.h"
#include "RenderContext.h"
#include "ModelMeshPart.h"
#include "Model.h"

MeshPartRenderable::MeshPartRenderable(Model *model): _model(model), _meshPart(0)
{
    setMaterial(model->getDefaultMaterial());
}

MeshPartRenderable::MeshPartRenderable(Model *model, const Matrix &position, ModelMeshPart *meshPart)
: _model(model), _meshPart(meshPart)
{
    setPositionalMatrix(position);
    setMaterial(meshPart->getMaterial());
}

MeshPartRenderable::~MeshPartRenderable() {}

void MeshPartRenderable::render(RenderContext *context) {
    // Need to have at least a vertex buffer!
    ASSERT(_model->getVertexBuffer());


    // Setup a few mesh based variables..
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
    if (material == NULL) {
        material = _model->getDefaultMaterial();
    }

	// Get the context ready.
    if(_model->wireframeMode()) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    } else {
        context->setActiveMaterial(material);
    }

    context->setModelMatrix(_positionalMatrix);

    context->addToPrimitiveCount(indexCount / 3);
    context->addToVertexCount(indexCount);
    context->addToModelCount(1);

    // Render stuff!
    glEnableClientState(GL_VERTEX_ARRAY);

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

    glBindBuffer(GL_ARRAY_BUFFER, _model->getVertexBuffer());
    glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));

    if (_model->getNormalBuffer()) {
        glEnableClientState(GL_NORMAL_ARRAY);

        glBindBuffer(GL_ARRAY_BUFFER, _model->getNormalBuffer());
        glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(0));
    }

    if (_model->getTexCoordBuffer()) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glBindBuffer(GL_ARRAY_BUFFER, _model->getTexCoordBuffer());
        glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _model->getIndexBuffer());
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, BUFFER_OFFSET(startIndex));

#undef BUFFER_OFFSET

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    if(_model->wireframeMode()) {
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else {
        context->unsetActiveMaterial(material);
    }

    if(_model->shouldDrawNormals() && _model->getNormals()) {
        Vector3 *verts = _model->getVertices();
        Vector3 *norms = _model->getNormals();

        glDisable(GL_LIGHTING);
        glUseProgram(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, NULL);
        glDisable(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, NULL);
        glDisable(GL_TEXTURE_2D);

        glBegin(GL_LINES);
        for(int c = 0; c < indexCount; c++) {
            float color = pow(norms[c].z, 5.0);

            glColor4f(1.0, color, color, 1.0);
            glVertex3fv(verts[c].array);
            glVertex3fv((verts[c] + norms[c]).array);
        }
        glEnd();
    }
}

void MeshPartRenderable::setPositionalMatrix(const Matrix &posMatrix) {
    _positionalMatrix = posMatrix;
}
