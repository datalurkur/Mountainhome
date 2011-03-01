/*
 *  PixelData.h
 *  Mountainhome
 *
 *  Created by loch on 3/18/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _PIXELDATA_H_
#define _PIXELDATA_H_
#include <Base/Assertion.h>
#include "GL_Helper.h"
#include <string>

/*! \todo Make this more powerful by including width/height.
 *  \todo Remove references to GL_Helper */
class PixelData {
public:
    PixelData();
    PixelData(void *pixels, GLenum layout, GLenum type, unsigned int w, unsigned int h, unsigned int d = 1, bool cleanup = false);
    ~PixelData();

    /*! Sets the data for a three dimensional image. */
    template <typename T>
    void setPixelData(T *pixels, GLenum layout, unsigned int w, unsigned int h, unsigned int d, bool cleanup = false);

    /*! Sets the data for a two dimensional image. */
    template <typename T>
    void setPixelData(T *pixels, GLenum layout, unsigned int w, unsigned int h, bool cleanup = false);

    /*! Sets the data for a one dimensional image. */
    template <typename T>
    void setPixelData(T *pixels, GLenum layout, unsigned int w, bool cleanup = false);

    /*! Returns a pointer to the pixel data, cast to the requested type. You can only cast
     *  to the type the PixelData was originally set to, otherwise a DataType assertion
     *  will fail. */
    template <typename T>
    T * getPixelData() const;

    /*! Saves the internal pixel data to disk as a png. The '.png' will automatically be
     *  added to the end of the given name.
     * \note Only works with GL_UNSIGNED_BYTE. */
    void saveToDisk(const std::string &name) const;

    /*! Gets the GL pixel layout for the PixelData. */
    GLenum getLayout() const;

    /*! Gets the GL data type for the PixelData. Attempting to call getPixelData with a
     *  type that doesn't match the data type returned here will cause an assertion
     *  failure. */
    GLenum getDataType() const;

    /*! Gets the number of bytes in a single pixel. This is a function of the DataType and
     *  pixel layout. */
    unsigned int getBytesPerPixel() const;

    /*! Returns the width of the image data. */
    unsigned int getWidth() const;

    /*! Returns the height of the image data. */
    unsigned int getHeight() const;

    /*! Returns the depth of the image data. */
    unsigned int getDepth() const;

private:
    /*! Cleans up the old pixel data if it is set and the cleanup flag is set. */
    void deleteOldPixelData();

    /*! Calculates the number of bytes in a single pixel and sets _bytesPP. */
    void calcBytesPerPixel();

private:
    GLenum _layout;
    GLenum _type;

    unsigned int _width;
    unsigned int _height;
    unsigned int _depth;

    unsigned int _bytesPP;

    void* _pixels;

    bool _cleanup;
};

template <typename T>
void PixelData::setPixelData(T *pixels, GLenum layout, unsigned int w, unsigned int h, unsigned int d, bool cleanup) {
    deleteOldPixelData();

    _layout = layout;
    _width = w;
    _height = h;
    _depth = d;
    _pixels = pixels;
    _cleanup = cleanup;

    // Set the type using some clever template magic.
         if (has_type<T, char          >::value) { _type = GL_BYTE;           }
    else if (has_type<T, unsigned char >::value) { _type = GL_UNSIGNED_BYTE;  }
    else if (has_type<T, short         >::value) { _type = GL_SHORT;          }
    else if (has_type<T, unsigned short>::value) { _type = GL_UNSIGNED_SHORT; }
    else if (has_type<T, int           >::value) { _type = GL_INT;            }
    else if (has_type<T, unsigned int  >::value) { _type = GL_UNSIGNED_INT;   }
    else if (has_type<T, float         >::value) { _type = GL_FLOAT;          }
    else if (has_type<T, double        >::value) { _type = GL_DOUBLE;         }
    else { THROW(NotImplementedError, "SetPixelData called with an unsupported type."); }

    calcBytesPerPixel();
}

template <typename T>
void PixelData::setPixelData(T *pixels, GLenum layout, unsigned int w, unsigned int h, bool cleanup) {
    setPixelData(pixels, layout, w, h, 1, cleanup);
}

template <typename T>
void PixelData::setPixelData(T *pixels, GLenum layout, unsigned int w, bool cleanup) {
    setPixelData(pixels, layout, w, 1, 1, cleanup);
}

template <typename T>
T * PixelData::getPixelData() const {
    // Set the type using some clever template magic.
         if (has_type<T, void          >::value) {}
    else if (has_type<T, char          >::value) { ASSERT_EQ(_type, GL_BYTE          ); }
    else if (has_type<T, unsigned char >::value) { ASSERT_EQ(_type, GL_UNSIGNED_BYTE ); }
    else if (has_type<T, short         >::value) { ASSERT_EQ(_type, GL_SHORT         ); }
    else if (has_type<T, unsigned short>::value) { ASSERT_EQ(_type, GL_UNSIGNED_SHORT); }
    else if (has_type<T, int           >::value) { ASSERT_EQ(_type, GL_INT           ); }
    else if (has_type<T, unsigned int  >::value) { ASSERT_EQ(_type, GL_UNSIGNED_INT  ); }
    else if (has_type<T, float         >::value) { ASSERT_EQ(_type, GL_FLOAT         ); }
    else if (has_type<T, double        >::value) { ASSERT_EQ(_type, GL_DOUBLE        ); }
    else { THROW(NotImplementedError, "SetPixelData called with an unsupported type."); }

    return static_cast<T*>(_pixels);
}

#endif
