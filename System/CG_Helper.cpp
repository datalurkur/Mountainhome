/*
 *  CG_Helper.cpp
 *  System
 *
 *  Created by loch on 11/29/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "CG_Helper.h"

#include <Log.h>
#include <FileSystem.h>

void checkCGErrorsFL(CGcontext &context, const char *file, int line) {
    CGerror error;
    const char *string = cgGetLastErrorString(&error);
    
    if (error != CG_NO_ERROR) {
        Error(file << ":" << line << ":" << string);
        if (error == CG_COMPILER_ERROR) {
            Error(cgGetLastListing(context));
        }
    }
}

void initCGProgram(CGcontext &context, CGprofile &profile, const char* filename,
                   const char* functionname, CGprogram &result) {
    std::string fullname;
    if (!FileSystem::GetReadName(filename, fullname)) {
        Error("Could not find the given file.");
        Error("Given name :" << filename);
        Error("Search name:" << fullname);
        return;
    }

      result = cgCreateProgramFromFile(context, CG_SOURCE, fullname.c_str(), profile,
                                     functionname, NULL);
    CheckCGErrors(context);
    cgGLLoadProgram(result);
    CheckCGErrors(context);
}
