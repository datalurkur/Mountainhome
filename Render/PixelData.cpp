/*
 *  PixelData.cpp
 *  Mountainhome
 *
 *  Created by loch on 3/18/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "PixelData.h"

#ifdef USEPNGLIB
#include <png.h>
void Texture::saveToDisk(const std::string &name) {
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;

    if (!(fp = fopen(name.c_str(), "wb"))) {
        Error("Error writing out to: " << name);
        return;
    }

    if (!(png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                            png_voidp_NULL,
                                            NULL, NULL))) {
        fclose(fp);
        Error("Error creating write struct");
        return;
    }

    if (!(info_ptr = png_create_info_struct(png_ptr))) {
        fclose(fp);
        png_destroy_write_struct(&png_ptr,  png_infopp_NULL);
        Error("Error creating info struct");
        return;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        Error("Error setting up error handling");
        return;
    }

    // Init with use of streams.
    png_init_io(png_ptr, fp);
    png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA,
    PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_bytep* row_pointers = (png_bytep*)png_malloc(png_ptr, height * sizeof(png_bytep));
    for (int i = 0; i < height; i++) {
        row_pointers[i] = &(pixels[i * width * 4]);
    }

    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, 0, png_voidp_NULL);

    png_free(png_ptr, row_pointers);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
}
#endif
