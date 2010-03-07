/*
 *  Engine.h
 *  Base
 *
 *  Created by Brent Wilson on 11/11/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#if !defined(_ENGINE_H_) && defined(__cplusplus)
#define _ENGINE_H_

#include <Base/Base.h>
#include <Render/Render.h>

#if SYS_PLATFORM == PLATFORM_APPLE
#   include <Carbon/Carbon.h>
    inline std::string macBundlePath() {
        char path[1024];
        CFBundleRef mainBundle = CFBundleGetMainBundle();
        assert(mainBundle);

        CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
        assert(mainBundleURL);

        CFStringRef cfStringRef = CFURLCopyFileSystemPath(mainBundleURL, kCFURLPOSIXPathStyle);
        assert(cfStringRef);

        CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);

        CFRelease(mainBundleURL);
        CFRelease(cfStringRef);

        return std::string(path);
    }
#endif

#endif
