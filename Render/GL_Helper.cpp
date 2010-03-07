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
        Error("OpenGL error at " << file << ":" << line << " - " << (char*)errStr);
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

