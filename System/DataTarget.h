/*
 *  DataTarget.h
 *  System
 *
 *  Created by loch on 11/30/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _DATASTREAM_H_
#define _DATASTREAM_H_
#include "IOTarget.h"

class DataTarget : public IOTarget {
public:
    DataTarget(unsigned char *data, int length);
    ~DataTarget();

    /*! \copydoc IOTarget::open */
    virtual bool open(OpenMode openFlags = ReadWrite);

    /*! \copydoc IOTarget::close */
    virtual void close();

    /*! \copydoc IOTarget::isOpen */
    virtual bool isOpen();

    /*! \copydoc IOTarget::bytesLeft */
    virtual long long bytesLeft();

    /*! \copydoc IOTarget::peek */
    virtual char peek();

    /*! \copydoc IOTarget::getc */
    virtual char getc();

    /*! \copydoc IOTarget::read */
    virtual long long read(void* buffer, long long size);

    /*! \copydoc IOTarget::write */
    virtual long long write(const void* buffer, long long size);

    /*! \copydoc IOTarget::error */
    virtual int error();

    /*! \copydoc IOTarget::clearError */
    virtual void clearError();

    /*! \copydoc IOTarget::atEnd */
    virtual bool atEnd();

    /*! \copydoc IOTarget::seek */
    virtual bool seek(long long offset, OffsetBase base = Beginning);

    /*! \copydoc IOTarget::position */
    virtual long long position();

    /*! \copydoc IOTarget::length */
    virtual long long length();

private:
    unsigned char *_data;
    long long _length;

    long long _pos;
};

#endif
