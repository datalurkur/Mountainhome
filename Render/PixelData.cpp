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

PixelData::PixelData():
    _layout(0),
    _type(0),
    _width(0),
    _height(0),
    _depth(0),
    _bytesPP(0),
    _pixels(),
    _cleanup(false)
{}

PixelData::PixelData(
    void *pixels,
    GLenum layout,
    GLenum type,
    unsigned int w,
    unsigned int h,
    unsigned int d,
    bool cleanup
):
    _layout(layout),
    _type(type),
    _width(w),
    _height(h),
    _depth(d),
    _bytesPP(0),
    _pixels(pixels),
    _cleanup(cleanup)
{
    calcBytesPerPixel();
}

PixelData::~PixelData() {
    deleteOldPixelData();
}

GLenum PixelData::getLayout()              const { return _layout; }
GLenum PixelData::getDataType()            const { return _type; }
unsigned int PixelData::getWidth()         const { return _width; }
unsigned int PixelData::getHeight()        const { return _height; }
unsigned int PixelData::getDepth()         const { return _depth; }
unsigned int PixelData::getBytesPerPixel() const { return _bytesPP; }

void PixelData::deleteOldPixelData() {
    if (_cleanup && _pixels) {
        switch(_type) {
        case GL_UNSIGNED_BYTE: delete[] static_cast<unsigned char* >(_pixels); break;
        case GL_BYTE:          delete[] static_cast<  signed char* >(_pixels); break;
        case GL_UNSIGNED_SHORT:delete[] static_cast<unsigned short*>(_pixels); break;
        case GL_SHORT:         delete[] static_cast<  signed short*>(_pixels); break;
        case GL_UNSIGNED_INT:  delete[] static_cast<unsigned int*  >(_pixels); break;
        case GL_INT:           delete[] static_cast<  signed int*  >(_pixels); break;
        case GL_FLOAT:         delete[] static_cast<float*         >(_pixels); break;
        case GL_DOUBLE:        delete[] static_cast<double*        >(_pixels); break;
        default: THROW(NotImplementedError, "This data type is not supported: " << _type);
        }

        _pixels = NULL;
    }
}

void PixelData::calcBytesPerPixel() {
    int components;
    switch(_layout) {
    case GL_LUMINANCE:
    case GL_RED:
    case GL_GREEN:
    case GL_BLUE:
    case GL_ALPHA: components = 1; break;
    case GL_RGB:   components = 3; break;
    case GL_RGBA:  components = 4; break;
    case GL_BGR:   components = 3; break;
    case GL_BGRA:  components = 4; break;
    default: THROW(NotImplementedError, "This texture type is not supported: " << _layout);
    }

    int size;
    switch(_type) {
    case GL_UNSIGNED_BYTE:
    case GL_BYTE:          size = sizeof(char);   break;
    case GL_UNSIGNED_SHORT:
    case GL_SHORT:         size = sizeof(short);  break;
    case GL_UNSIGNED_INT:
    case GL_INT:           size = sizeof(int);    break;
    case GL_FLOAT:         size = sizeof(float);  break;
    case GL_DOUBLE:        size = sizeof(double); break;
    default: THROW(NotImplementedError, "This data type is not supported: " << _type);
    }

    _bytesPP = size * components;
}

void PixelData::saveToDisk(const std::string &name) const {
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    int colorType;

    if (_type != GL_UNSIGNED_BYTE) {
        // PNG output really only works with unsigned bytes. I'd need to do some translating to support other types.
        THROW(NotImplementedError, "Doesn't support non-unsigned byte pixel types.");
    }

    std::string fullname = name + ".png";
    if (!(fp = fopen(fullname.c_str(), "wb"))) {
        THROW(InternalError, "Error writing out to: " << fullname);
    }

    if (!(png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                            png_voidp_NULL,
                                            NULL, NULL))) {
        fclose(fp);
        THROW(InternalError, "Error creating write struct");
    }

    switch(_layout) {
    case GL_LUMINANCE:
    case GL_RED:
    case GL_GREEN:
    case GL_BLUE:
    case GL_ALPHA: colorType = PNG_COLOR_TYPE_GRAY;      break;
    case GL_RGB:   colorType = PNG_COLOR_TYPE_RGB;       break;
    case GL_RGBA:  colorType = PNG_COLOR_TYPE_RGB_ALPHA; break;
    case GL_BGR:   colorType = PNG_COLOR_TYPE_RGB_ALPHA; png_set_bgr(png_ptr); break;
    case GL_BGRA:  colorType = PNG_COLOR_TYPE_RGB_ALPHA; png_set_bgr(png_ptr); break;
    default:
        fclose(fp);
        png_destroy_write_struct(&png_ptr,  png_infopp_NULL);
        THROW(NotImplementedError, "This texture type is not supported: " << _layout);
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
    png_set_IHDR(png_ptr, info_ptr, _width, _height, 8, colorType,
    PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // This is safe because we only let this function work with unsigned byte types.
    png_bytep* row_pointers = (png_bytep*)png_malloc(png_ptr, _height * sizeof(png_bytep));
    for (int i = 0; i < _height; i++) {
        row_pointers[i] = ((png_byte*)_pixels) + (i * _width * _bytesPP);
    }

    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, 0, png_voidp_NULL);

    png_free(png_ptr, row_pointers);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
}
