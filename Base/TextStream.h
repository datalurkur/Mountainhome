/*
 *  TextStream.h
 *  Base
 *
 *  Created by Brent Wilson on 9/30/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _TEXTSTREAM_H_
#define _TEXTSTREAM_H_ 
#include "IOTarget.h"
#include "Logger.h"

#include <sstream>

class TextStream {
public:
    /*! Creates a TextStream with the given IOTarget and OpenMode. If 0 or None is passed
     *  as the open mode, the file will not be opened by the BinaryStream (and thus not
     *  closed in the d'tor either). The user must open the file manually before any read
     *  or write operations will work.
     * \param target Only a pointer to the IOTarget is maintained. Any changes made to the
     *        actual IOTarget may have an effect on TextStream operations.
     * \param mode The mode to open with. None or 0 implies not to open the IOTarget.
     * \param cleanUpIOTarget If true, upon deletion, the BinaryStream will close the
     *        IOTarget if it is open and delete the pointer. If this is false, management
     *        is left to the user. */
    TextStream(IOTarget *target, IOTarget::OpenMode mode = IOTarget::ReadWrite,
               bool cleanUpIOTarget = true);

    /*! Destroys the TextStream. If the IOTarget was opened by this TextStream, it will
     *  also close it. */
    virtual ~TextStream();

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

    /*! Sets the delimeter for readChunk. readChunk will read until it sees any character
     *  in the delimeter string.
     * \param delim The delimeter string. Default value it " \n" (spaces and newlines).
     * \sa readChunk */
    void setDelimeter(const std::string &delim = " \n");

    /*! Reads the entrire stream into a string.
     * \param result The string to read into.
     * \return the number of bytes read. */
    long long readAll(std::string &result);

    /*! Reads a single line into a string.
     * \param result The string to read into.
     * \return the number of bytes read. */
    long long readLine(std::string &result);

    /*! Reads a single chunk of text into a string. A 'chunk' is defined by the delimeter
     *  string set by the setDelimeter function. When read chunk is called, the stream is
     *  read until any character in the delimeter string is found. The delimeter char is
     *  not included in the result.
     * \param result The string to read into.
     * \return the number of bytes read.
     * \sa setDelimeter */
    long long readChunk(std::string &result);

    /*! Reads from the TextStream into the given buffer.
     * \param buffer The buffer to read into.
     * \param size The maximum number of bytes allowed to be read.
     * \return The number of bytes actually read. */
    long long read(std::string &buffer, long long size);

    /*! \copydoc IOTarget::read */
    long long read(char* buffer, long long size);

    /*! Reads a chunk of data into the given variable.
     * \param rhs The variable to read into.
     * \return A reference to the TextStream for chaining. */
    template <typename T> TextStream& operator>>(T &rhs);

    /*! Reads a chunk of data into the given variable.
     * \param rhs The variable to read into.
     * \return A reference to the BinaryFile for chaining. */    
    TextStream& operator>>(float &f);

    /*! Reads a chunk of data into the given variable.
     * \param rhs The variable to read into.
     * \return A reference to the BinaryFile for chaining. */
    TextStream& operator>>(double &f);

    /*! Reads a chunk of data into the given variable.
     * \param rhs The variable to read into.
     * \return A reference to the BinaryFile for chaining. */
    TextStream& operator>>(std::string &str);

    /*! Reads a chunk of data into the given variable.
     * \param rhs The variable to read into.
     * \return A reference to the BinaryFile for chaining. */
    TextStream& operator>>(char &c);

    /*! Reads a chunk of data into the given variable.
     * \param rhs The variable to read into.
     * \return A reference to the BinaryFile for chaining. */
    TextStream& operator>>(signed short &i);

    /*! Reads a chunk of data into the given variable.
     * \param rhs The variable to read into.
     * \return A reference to the BinaryFile for chaining. */
    TextStream& operator>>(unsigned short &i);

    /*! Reads a chunk of data into the given variable.
     * \param rhs The variable to read into.
     * \return A reference to the BinaryFile for chaining. */
    TextStream& operator>>(signed int &i);

    /*! Reads a chunk of data into the given variable.
     * \param rhs The variable to read into.
     * \return A reference to the BinaryFile for chaining. */
    TextStream& operator>>(unsigned int &i);

    /*! Reads a chunk of data into the given variable.
     * \param rhs The variable to read into.
     * \return A reference to the BinaryFile for chaining. */
    TextStream& operator>>(signed long &i);

    /*! Reads a chunk of data into the given variable.
     * \param rhs The variable to read into.
     * \return A reference to the BinaryFile for chaining. */
    TextStream& operator>>(unsigned long &i);

    /*! \copydoc IOTarget::write */
    long long write(const char* pointer, long long size);

    /*! Helper write function. Outputs a string.
     * \param buffer The buffer to read from.
     * \return The number of bytes actually written. */
    long long write(const std::string &buffer);

    /*! Writes the given variable out to the TextStream.
     * \param rhs The variable to write out.
     * \return A reference to the TextStream for chaining. */
    template <typename T> TextStream& operator<<(const T &rhs);

private:
    /*! Checks to see if the given char is in the delimeter string.
     * \param check The char to look for.
     * \return true if the char is in the delimeter string, false otherwise. */
    bool isDelimeter(char check);

private:
    IOTarget *_target;      /*!< The IOTarget read/write operations are performed on. */
    std::string _delimeter; /*!< The list of delimeter chars to consider in readChunk. */
    bool _cleanup;          /*!< Is the BinaryStream is responsible for IOTarget cleanup. */

};

template <typename T> TextStream& TextStream::operator<<(const T &rhs) {
    std::stringstream buffer;
    buffer << rhs;

    long long written = write(buffer.str());
    if (written != buffer.str().length()) {
        Warn("Error writing text data: " << rhs);
        Warn("Bytes written: " << written << "/" << buffer.str().length());
    }

    return (*this);
}

template <typename T> TextStream& TextStream::operator>>(T &rhs) {
    Error("Can't read in this data type with a TextStream.");
    return (*this);
}

#endif
