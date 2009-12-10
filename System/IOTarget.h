/*
 *  IOTarget.h
 *  System
 *
 *  Created by loch on 9/30/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _IOTARGET_H_
#define _IOTARGET_H_

class IOTarget {
public:
    /*! Allows certain flags to be set upon ReadFile and WriteFile creation. */
    enum OpenFlags {
        None    = 0,         /*!< No particular behavior. */
        Append  = 1 << 0,    /*!< Append to existing content. */
        Write   = 1 << 1,    /*!< Enable write access to the IOTarget */
        Read    = 1 << 2,    /*!< Enable read  access to the IOTarget */
        ReadWrite = Write | Read
    };

    /*! Used when setting the stream pointer position in subclasses. */
    enum OffsetBase {
        Current,      /*!< Offset from the current position.    */
        Beginning,    /*!< Offset from the start of the stream. */
        End           /*!< Offset from the end of the stream.   */
    };

    /*! Used for convenience */
    typedef long OpenMode;

    /*! Create an IOTarget. */
    IOTarget();

    /*! Destroys an IOTarget. */
    virtual ~IOTarget();

    /*! Opens the IOTarget for read/write operations.
     * \return True if the IOTarget was opened successfully, false otherwise. */
    virtual bool open(OpenMode openFlags = ReadWrite) = 0;

    /*! Closes the IOTarget. */
    virtual void close() = 0;

    /*! Checks to see if the IOTarget is open at the moment.
     * \return True if the IOTarget is open, false otherwise. */
    virtual bool isOpen() = 0;

    /*! Returns the number of bytes the current position is from the end of the IOTarget.
     * \return The number of bytes from the end of the IOTarget */
    virtual long long bytesLeft() = 0;

    /*! Returns the next char on the IOTarget without advancing the stream position.
     * \return The next char on the IOTarget. */
    virtual char peek() = 0;

    /*! Returns the next char on the IOTarget and advances the position by 1.
     * \return The next char on the IOTarget. */
    virtual char getc() = 0;

    /*! Reads from the IOTarget into the given buffer.
     * \param buffer The buffer to read into.
     * \param size The maximum number of bytes allowed to be read.
     * \return The number of bytes actually read. */
    virtual long long read(void* buffer, long long size) = 0;

    /*! Writes to the IOTarget from the given buffer.
     * \param buffer The buffer to read from.
     * \param size The maximum number of bytes allowed to be written.
     * \return The number of bytes actually written. */
    virtual long long write(const void* buffer, long long size) = 0;

    /*! Returns the value of the internal error flag. The error flag is set if some sort
     *  of problem arises while using the the IOTarget.
     * \return The error flag. */
    virtual int error();

    /*! Clears the internal error flag. */
    virtual void clearError();

    /*! Returns whether or not the IOTarget is at the end.
     * \note Not compatible with sequential targets. 
     * \return true if the IOTarget is at its end, false otherwise. */
    virtual bool atEnd();

    /*! Sets the current read/write position based on the offset and base.
     * \note Not compatible with sequential targets.
     * \param offset The offset to seek to, depends on the base.
     * \param base The point to offset from.
     * \return True if no error occured, false otherwise. */
    virtual bool seek(long long offset, OffsetBase base = Beginning);

    /*! Returns the current read/write position of the IOTarget.
     * \note Not compatible with sequential targets.
     * \return the position of the stream. */
    virtual long long position();

    /*! Returns the length of the IOTarget in bytes.
     * \note Not compatible with sequential targets.
     * \return The length of the IOTarget in bytes. */
    virtual long long length();

protected:
    int _error;

};

#endif
