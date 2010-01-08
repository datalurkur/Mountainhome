/*
 *  GL_Helper.h
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _GL_HELPER_H_
#define _GL_HELPER_H_

#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
#    if defined(_MSC_VER)
#        pragma comment(lib, "OpenGL32.lib")
#        pragma comment(lib, "GLEW32.lib")
#        pragma comment(lib, "GLu32.lib")
#    endif // _MSC_VER
#endif // _WIN32

#if defined(__APPLE__) && defined(__MACH__)
#    include <OpenGL/gl.h>
#    include <OpenGL/glu.h>
#else
#    include <GL/glew.h>
#endif


#define CheckFramebufferStatus() CheckFramebufferStatusFL(__FILE__, __LINE__)
#define CheckGLErrors() CheckGLErrorsFL(__FILE__, __LINE__)

bool AreExtensionsSupported(const char* extensionList);
bool IsExtensionSupported(const char* target);
void CheckGLErrorsFL(const char* file, int line);
void CheckFramebufferStatusFL(const char *file, int line);

#endif
