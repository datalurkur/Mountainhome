/*
 *  GL_Helper.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "GL_Helper.h"
#include <Base/Math3D.h>
#include <Base/Logger.h>

#include <string>

bool AreExtensionsSupported(const char* extensionList) {
    char buffer[1024];
    char *list = buffer, *extension;
    strncpy(buffer, extensionList, 1024);
    do {
        extension = strchr(list, ' ');
        Math::Swap(list, extension);
        if(list) { list[0] = '\0'; list++; }
        if (!IsExtensionSupported(extension)) { return false; }
    } while(list);
    return true;
}

bool IsExtensionSupported(const char* target) {
    const char *extensions = (const char*) glGetString(GL_EXTENSIONS);
    unsigned int length = strlen(target);
    char *start, *end;
    
    if (!extensions) { return false; }
    if(strchr(target, ' ') || *target == '\0' ) { return false; }
    
    while(true) {
        if(!(start = strstr(extensions, target))) { break; }
        end = start + length;
        if(start == extensions || *(start - 1) == ' ') {
            if(*end == ' ' || *end == '\0') { return true; }
        }
        extensions = (const char*)end;
    }

    Info(target << " not a supported openGL extenstion!");
    return false;
}

void CheckGLErrorsFL(const char* file, int line) {
    GLenum errCode;
    const GLubyte *errStr;
    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errStr = gluErrorString(errCode);
        THROW(InternalError, "OpenGL error at " << file << ":" << line << " - " << (char*)errStr);
    }
}

void CheckFramebufferStatusFL(const char *file, int line) {
    GLenum status = (GLenum)glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status == GL_FRAMEBUFFER_COMPLETE_EXT) { return; }
    
    Error(file << " : " << line);
    switch(status) {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            Error("Framebuffer incomplete,incomplete attachment");
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            Error("Unsupported framebuffer format");
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            Error("Framebuffer incomplete,missing attachment");
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            Error("Framebuffer incomplete,attached images "
            << "must have same dimensions");
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
             Error("Framebuffer incomplete,attached images"
            << "must have same format");
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            Error("Framebuffer incomplete,missing draw buffer");
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            Error("Framebuffer incomplete,missing read buffer");
    }
}

GLenum TranslatePrimitiveType(PrimitiveType type) {
    switch(type) {
    case POINTS:         return GL_POINTS;
    case LINES:          return GL_LINES;
    case LINE_STRIP:     return GL_LINE_STRIP;
    case TRIANGLES:      return GL_TRIANGLES;
    case TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
    case QUADS:          return GL_QUADS;
    }
    
    THROW(InvalidStateError, "Invalid primitive type.");
    
    return 0;
}

void GetPolygonMode(PolygonMode *mode, bool *fill) {
    GLint currentMode[2];
    glGetIntegerv(GL_POLYGON_MODE, currentMode);

    switch(currentMode[0]) {
    case GL_FRONT:          *mode = FRONT; break;
    case GL_BACK:           *mode = BACK; break;
    case GL_FRONT_AND_BACK: *mode = FRONT_AND_BACK; break;
        THROW(InvalidStateError, "Invalid polygon mode.");
    }

    switch(currentMode[1]) {
    case GL_POINT:
    case GL_LINE: *fill = false; break;
    case GL_FILL: *fill = true;  break;
    default:
        THROW(InvalidStateError, "Invalid polygon fill.");
    }
}

void SetPolygonMode(PolygonMode mode, bool fill) {
    GLint glMode;

    switch(mode) {
    case FRONT:          glMode = GL_FRONT; break;
    case BACK:           glMode = GL_BACK; break;
    case FRONT_AND_BACK: glMode = GL_FRONT_AND_BACK; break;
    default:
        THROW(InvalidStateError, "Invalid polygon face.");
    }

    glPolygonMode(glMode, fill ? GL_FILL : GL_LINE);

}
