/*
 *  RenderContext.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/4/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "GL_Helper.h"
#include "RenderContext.h"
#include "Viewport.h"
#include "RenderTarget.h"
#include "Texture.h"

RenderContext::RenderContext() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glShadeModel(GL_SMOOTH);

    glClearDepth(1.0f);    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

#if SYS_PLATFORM != PLATFORM_APPLE
    glewExperimental = true;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        Error() << glewGetErrorString(err);
    }
#endif

    // build the renderer info string
    char version[256], renderer[256];
    int i;
    strncpy(renderer, (const char *)glGetString(GL_RENDERER), 255);
    if (0 == strcmp(renderer, "Generic")) {
        strncpy(renderer, "Apple Generic", 255);
    }
    else if ((strlen(renderer) > 14) && (0 == strcmp(renderer+strlen(renderer)-14, " OpenGL Engine"))) {
        renderer[strlen(renderer)-14] = 0;
    }
    strncat(renderer, " ", 255);
    strncpy(version, (const char *)glGetString(GL_VERSION), 255);
    for (i = 0; i < 256; i++) {
        char c[2] = { version[i], 0};
        if (((c[0] >= '0' ) && (c[0] <= '9')) || c[0] == '.') strncat(renderer, c, 255);
        else break;
    }
    Info("Renderer: " << renderer);

}

RenderContext::~RenderContext() {}

void RenderContext::setWireFrame() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void RenderContext::setFilled() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RenderContext::enableAlphaBlend() {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
}

void RenderContext::disableAlphaBlend() {
    glDisable(GL_BLEND);
}

void RenderContext::renderTexture(Texture *src, RenderTarget *dest) {
    dest->enable();

    clearBuffers(Color4(1, 1, 1, 1));
    setOrtho2D(0, 1, 0, 1);
    resetModelviewMatrix();

    if (src) {
        src->bindAndEnable();
    }

    draw2DRect(0, 0, 1, 1);

    if (src) {
        src->releaseAndDisable();
    }
}

/// \todo Make this use not immediate mode!!!!!!!!!!
void RenderContext::drawTriangles(Vector3 *vertices, int number, const Color4 &color) {
    _primitiveCount += number;
    _vertexCount += number * 3;
    _modelCount++;

    glColor4f(color.r, color.g, color.b, color.a);
    glBegin(GL_TRIANGLES); {
        for (int i = 0; i < number * 3; i++) {
            glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
        }
    } glEnd();
}

void RenderContext::draw2DRect(Real x, Real y, Real w, Real h,
                               const Color3 &color) const {
    draw2DRect(x, y, w, h, color.r, color.g, color.b, 1.0);
}

void RenderContext::draw2DRect(Real x, Real y, Real w, Real h,
                               const Color4 &color) const {
    draw2DRect(x, y, w, h, color.r, color.g, color.b, color.a);
}

void RenderContext::draw2DRect(Real x, Real y, Real w, Real h,
                               Real r, Real g, Real b, Real a) const {
    glColor4f(r, g, b, a);
    draw2DRect(x, y, w, h);
}

void RenderContext::draw2DRect(Real x, Real y, Real w, Real h) const {
    _primitiveCount++;
    _vertexCount += 4;
    _modelCount++;
    
    glBegin(GL_QUADS); {
        glTexCoord2f(0, 0); glVertex2f(x    , y    );
        glTexCoord2f(1, 0); glVertex2f(x + w, y    );
        glTexCoord2f(1, 1); glVertex2f(x + w, y + h);
        glTexCoord2f(0, 1); glVertex2f(x    , y + h);
    } glEnd();
}

void RenderContext::drawBoundingBox(const AABB3 &boundingBox, const Color4 &color) {
    _primitiveCount += 12;
    _vertexCount += 18;
    _modelCount++;

    glColor4f(color.r, color.g, color.b, color.a);
    const Vector3 &min = boundingBox.getMin();
    const Vector3 &max = boundingBox.getMax();

    glLineWidth(1);
    glBegin(GL_LINE_STRIP); {
        glVertex3f(min[0], min[1], max[2]);
        glVertex3f(max[0], min[1], max[2]);
        glVertex3f(max[0], max[1], max[2]);
        glVertex3f(min[0], max[1], max[2]);
        glVertex3f(min[0], min[1], max[2]);
    } glEnd();

    glBegin(GL_LINE_STRIP); {
        glVertex3f(min[0], min[1], min[2]);
        glVertex3f(max[0], min[1], min[2]);
        glVertex3f(max[0], max[1], min[2]);
        glVertex3f(min[0], max[1], min[2]);
        glVertex3f(min[0], min[1], min[2]);
    } glEnd();

    glBegin(GL_LINES); {
        glVertex3f(min[0], min[1], min[2]);
        glVertex3f(min[0], min[1], max[2]);

        glVertex3f(max[0], max[1], min[2]);
        glVertex3f(max[0], max[1], max[2]);

        glVertex3f(min[0], max[1], min[2]);
        glVertex3f(min[0], max[1], max[2]);

        glVertex3f(max[0], min[1], min[2]);
        glVertex3f(max[0], min[1], max[2]);
    } glEnd();
}

void RenderContext::setViewport(int x, int y, int width, int height) const {
    //Info("Setting viewport to: " << x << " " << y << " " << width << " " << height);
    glViewport(x, y, width, height);
}

void RenderContext::clearBuffers(const Color4 &clearColor) const {
    //Info("Clearing buffers to: " << clearColor);
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int RenderContext::getPrimitiveCount() const { return _primitiveCount; }
int RenderContext::getVertexCount() const { return _vertexCount; }
int RenderContext::getModelCount() const { return _modelCount; }

///\todo Get rid of these!
void RenderContext::addToPrimitiveCount(int count) const { _primitiveCount += count; }
void RenderContext::addToVertexCount(int count) const { _vertexCount += count; }
void RenderContext::addToModelCount(int count) const { _modelCount += count; }

void RenderContext::resetMetrics() {
    _primitiveCount = 0;
    _vertexCount = 0;
    _modelCount = 0;
}

void RenderContext::setPerspective(int width, int height, Real fov, Real n, Real f) const {
    Real ratio = width;
    if (height) { ratio /= static_cast<Real>(height); }
    setPerspective(fov, ratio, n, f);
}

void RenderContext::setPerspective(Real fov, Real ratio, Real n, Real f) const {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, ratio, n, f);
    glMatrixMode(GL_MODELVIEW);
}

void RenderContext::setOrtho2D(Real left, Real right, Real bottom, Real top) const {
    setOrtho(left, right, bottom, top, 1, -1);
}

void RenderContext::setDepthTest(bool setTo) {
	if (setTo) { glEnable(GL_DEPTH_TEST);  }
	else       { glDisable(GL_DEPTH_TEST); }
}

void RenderContext::setLighting(bool setTo) {
	if (setTo) { glEnable(GL_LIGHTING); }
	else	   { glDisable(GL_LIGHTING); }
}

void RenderContext::setOrtho(Real left, Real right, Real bottom,
                             Real top, Real n, Real f) const {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(left, right, bottom, top, n, f);
    glMatrixMode(GL_MODELVIEW);
}


void RenderContext::resetModelviewMatrix() {
    glLoadIdentity();
    _modelviewMatrix.loadIdentity();
    _modelMatrix.loadIdentity();
    _viewMatrix.loadIdentity();
}

void RenderContext::setProjectionMatrix(const Matrix &projection) {
    _projectionMatrix = projection;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(projection.getMatrix());
    glMatrixMode(GL_MODELVIEW);
}

const Matrix& RenderContext::getModelviewMatrix() {
    return _modelviewMatrix;
}

void RenderContext::setViewMatrix(const Matrix &viewMat) {
    _viewMatrix = viewMat;
    _modelviewMatrix = _viewMatrix * _modelMatrix;
    glLoadMatrixf(_modelviewMatrix.getMatrix());
}

void RenderContext::setModelMatrix(const Matrix &modelMat) {
    _modelMatrix = modelMat;
    _modelviewMatrix = _viewMatrix * _modelMatrix;
    glLoadMatrixf(_modelviewMatrix.getMatrix());
}

void RenderContext::setActiveMaterial(const Material *mat) {
    mat->enableMaterial();
}

void RenderContext::unsetActiveMaterial(const Material *mat) {
    mat->disableMaterial();
}
