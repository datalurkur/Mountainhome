/*
 *  PixelData.cpp
 *  Mountainhome
 *
 *  Created by loch on 3/18/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "PixelData.h"
#include <Base/Logger.h>
#include <png.h>

void PixelData::saveToDisk(const std::string &name, int width, int height) const {
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    int colorType, bpp;

    if (type != GL_UNSIGNED_BYTE) {
        THROW(NotImplementedError, "Doesn't support non-unsigned byte pixel types.");
    }

    if (!(fp = fopen(name.c_str(), "wb"))) {
        THROW(InternalError, "Error writing out to: " << name);
    }

    if (!(png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                            png_voidp_NULL,
                                            NULL, NULL))) {
        fclose(fp);
        THROW(InternalError, "Error creating write struct");
    }

    switch(layout) {
    case GL_LUMINANCE:
    case GL_RED:
    case GL_GREEN:
    case GL_BLUE:
    case GL_ALPHA: colorType = PNG_COLOR_TYPE_GRAY;      bpp = 1; break;
    case GL_RGB:   colorType = PNG_COLOR_TYPE_RGB;       bpp = 3; break;
    case GL_RGBA:  colorType = PNG_COLOR_TYPE_RGB_ALPHA; bpp = 4; break;
    case GL_BGR:   colorType = PNG_COLOR_TYPE_RGB_ALPHA; bpp = 3; png_set_bgr(png_ptr); break;
    case GL_BGRA:  colorType = PNG_COLOR_TYPE_RGB_ALPHA; bpp = 4; png_set_bgr(png_ptr); break;
    default:
        fclose(fp);
        png_destroy_write_struct(&png_ptr,  png_infopp_NULL);
        THROW(NotImplementedError, "This texture type is not supported: " << layout);
    }

    if (!(info_ptr = png_create_info_struct(png_ptr))) {
        fclose(fp);
        png_destroy_write_struct(&png_ptr,  png_infopp_NULL);
        THROW(InternalError, "Error creating info struct");
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        THROW(InternalError, "Error setting up error handling");
    }

    // Init with use of streams.
    png_init_io(png_ptr, fp);
    png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, colorType,
    PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_bytep* row_pointers = (png_bytep*)png_malloc(png_ptr, height * sizeof(png_bytep));
    for (int i = 0; i < height; i++) {
        row_pointers[i] = ((png_byte*)pixels) + (i * width * bpp);
    }

    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, 0, png_voidp_NULL);

    png_free(png_ptr, row_pointers);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
}
