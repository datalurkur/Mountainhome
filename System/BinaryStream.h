/*
 *  BinaryStream.h
 *  Base
 *
 *  Created by Brent Wilson on 9/30/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _BINARYSTREAM_H_
#define _BINARYSTREAM_H_
#include "IOTarget.h"
#include "Logger.h"

class BinaryStream {
public:
    /*! Creates a BinaryStream with the given IOTarget and OpenMode. If 0 or None is
     *  passed as the open mode, the file will not be opened by the BinaryStream (and thus
     *  not closed in the d'tor either). The user must open the file manually before any
     *  read or write operations will work.
     * \param target Only a pointer to the IOTarget is maintained. Any changes made to the
     *        actual IOTarget may have an effect on BinaryStream operations.
     * \param mode The mode to open with. None or 0 implies not to open the IOTarget.
     * \param cleanUpIOTarget If true, upon deletion, the BinaryStream will close the
     *        IOTarget if it is open and delete the pointer. If this is false, management
     *        is left to the user. */
    BinaryStream(IOTarget *target, IOTarget::OpenMode mode = IOTarget::ReadWrite,
                 bool cleanUpIOTarget = true);

    /*! Destroys the BinaryStream. If the IOTarget was opened by this BinaryStream, it
     *  will also close it. */
    virtual ~BinaryStream();

    /*! Returns whether or not it is safe to read from the underlying IOTarget. If there
     *  is an error on the underlying IOTarget, the IOTarget is not open, or the IOTarget
     *  is set to NULL, false will be returned.
     * \return If the underlying IOTarget is safe for operations or not. */
    bool isValid();

    /*! \copydoc IOTarget::seek */
    bool seek(long long offset, IOTarget::OffsetBase base = IOTarget::Beginning);

    /*! \copydoc IOTarget::position */
    long long position();

    /*! \copydoc IOTarget::length */
    long long length();

    /*! \copydoc IOTarget::atEnd */
    bool atEnd();

    /*! \copydoc IOTarget::bytesLeft */
    long long bytesLeft();

    /*! \copydoc IOTarget::read */
    long long read(void* pointer, long long size);

    /*! Reads a chunk of data into the given variable.
     * \param rhs The variable to read into.
     * \return A reference to the BinaryStream for chaining. */
    template <typename T> BinaryStream& operator>>(T &rhs);

    /*! \copydoc IOTarget::write */
    long long write(const void* buffer, long long size);

    /*! Helper write function. Outputs a string.
     * \param buffer The buffer to read from.
     * \return The number of bytes actually written. */
    long long write(const std::string &buffer);

    /*! Writes the given variable out to the BinaryStream.
     * \param rhs The variable to write out.
     * \return A reference to the BinaryStream for chaining. */
    template <typename T> BinaryStream& operator<<(const T &rhs);

private:
    IOTarget *_target; /*!< The IOTarget read/write operations are performed on. */
    bool _cleanup;     /*!< Is the BinaryStream is responsible for IOTarget cleanup. */
};

template <typename T> BinaryStream& BinaryStream::operator<<(const T &rhs) {
    long long written = write((char*)&rhs, sizeof(T));
    if (written != sizeof(T)) {
        Warn("Error writing data with BinaryStream: " << rhs);
        Warn("Bytes written: " << written << "/" << sizeof(T));
    }

    return (*this);
}

template <typename T> BinaryStream& BinaryStream::operator>>(T &rhs) {
    read((char*)&rhs, sizeof(T));
    return (*this);
}

#endif
