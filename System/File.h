/*
 *  File.h
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _FILE_H_
#define _FILE_H_
#include "IOTarget.h"

#include <fstream>
#include <string>
#include <list>

using std::string;

/*! \brief Represents a File and allows for easy manipulation of the Filesystem.
 *  \todo Should open happen automatically upon construction like fstream?
 *  \todo This class is just started. It works for now, but will need work later on.
 *  \todo Add zipFile support
 *  \todo Support for '\\'??
 *  \todo Look at making a super class for this to share functionality with sockets. I
 *        believe I would pull out things like open, close, atEnd, etc... At that point
 *        I may consider having a ReadStream and a WriteStream, but then I have to start
 *        asking where the common File functionaliy goes. Multiple inheritance or
 *        interfaces would be my only real choices then (I like neither).
 *                                                       |--> ReadStream  --|
 *        As another though, I could have stream Stream--|                  |--> File
 *                                                       |--> WriteStream --|
 *        This would be MUCH more like std's way of doing it. Note the need for virtual
 *        inheritence though...
 *  \todo Move delete impl to FileSystem.
 *  \todo Add move and copy functions, making use of FileSystem.
 *  \author Brent Wilson
 *  \date 4/22/07 */
class File : public IOTarget {
protected:
    /*! Creates a File with the given path.
     * \param dir The directory of the File to open.
     * \param File The name of the File to open. */
    File(const string &dir, const string &File);

    /*! Creates a File with the given path and immediately opens it with the given mode.
     * \param dir The directory of the File to open.
     * \param File The name of the File to open.
     * \param openFlags The mode flags to use during the open call. */
    File(const string &dir, const string &File, OpenMode openFlags);

    // This enforces the use of the factory methods in FileSystem.
    friend class FileSystem;

public:
    /*! Basic File destructor. */
    virtual ~File();

    /*! \copydoc IOTarget::open */
    virtual bool open(OpenMode openFlags = ReadWrite);

    /*! \copydoc IOTarget::close */
    virtual void close();

    /*! \copydoc IOTarget::isOpen */
    virtual bool isOpen();

    /*! \copydoc IOTarget::atEnd */
    virtual bool atEnd();

    /*! \copydoc IOTarget::seek */
    virtual bool seek(long long offset, OffsetBase base = Beginning);

    /*! \copydoc IOTarget::position */
    virtual long long position();

    /*! \copydoc IOTarget::length */
    virtual long long length();

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

    /*! Checks to see if the File exists on the File system. Generally the only time this
     *  will be the case is when a WriteFile has been created, but not opened yet. A
     *  ReadFile MUST point at something that exists.
     * \return True is the File exists, false otherwise. */
    bool exists();

    /*! Attempts to delete a File from the File system. To do this, the File must be
     *  closed (by EVERYTHING, not just this object) and the user must have the correct
     *  permissions.
     * \return True if the File was deleted, false otherwise. */
    bool deleteFile();

    /*! Returns the full path of the File (directory and Filename).
     * \return The full path of the File. */
    const string& fullName();

    /*! Returns the name of the File (no directory).
     * \return The name of the File. */
    const string& fileName();

    /*! Returns the name of the directory of the File.
     * \return The directory name. */
    const string& dirName();

protected:
    string _fullName;
    string _fileName;
    string _dirName;

    std::fstream _internal;
    OpenMode _mode;
};

#endif
