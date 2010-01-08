/*
 *  main.cpp
 *  HL2-BSPTree
 *
 *  Created by Brent Wilson on 11/10/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "Mountainhome.h"

#if defined(__APPLE__) && defined(__MACH__)
#   include "SDL.H"
#endif

int main(int argc, char *argv[]) {
    Mountainhome app;
    app.startMainLoop();
	return 0;
}
