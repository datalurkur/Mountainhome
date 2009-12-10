/*
 *  CG_Helper.h
 *  System
 *
 *  Created by loch on 11/29/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CG_HELPER_H_
#define _CG_HELPER_H_

#if defined(__APPLE__) && defined(__MACH__)
#   include <Cg/cgGL.h>
#   include <Cg/cg.h>
#else
#   include <cgGL.h>
#   include <cg.h>
#endif

#define CheckCGErrors(X) checkCGErrorsFL(X, __FILE__, __LINE__)
void checkCGErrorsFL(CGcontext &context, const char *file, int line);
void initCGProgram(CGcontext &context, CGprofile &profile, const char* filename,
                   const char* functionname, CGprogram &result);

#endif
