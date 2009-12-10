/*
 *  Archive.h
 *  System
 *
 *  Created by loch on 11/30/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _ARCHIVE_H_
#define _ARCHIVE_H_
#include "Zip_Helper.h"
#include "IOTarget.h"

#include <string>
#include <list>
#include <set>

class DataTarget;
class Archive {
struct FileHeader {
    FileHeader(const ZIP_FileHeader &current, const std::string &n):
        compressedSize(current.compressedSize),
        uncompressedSize(current.uncompressedSize),
        offset(current.relativeOffsetOfLocalHeader + sizeof(ZIP_LocalFileHeader) +
               current.fileNameLength + current.extraFieldLength), 
        fullName(n),
        dir("") {
        std::string::size_type pos = n.rfind('/');
        if (pos != std::string::npos) {
            dir = n.substr(0, pos);
        }
    }

    int compressedSize;
    int uncompressedSize;
    int offset;
    std::string fullName;
    std::string dir;
};

public:
    /*! Initialize an archive with the given IOTarget. Note that the IOTarget must NOT be
     *  a sequential target such as a socket stream. The target must be random access to
     *  be correctly supported.
     * \param name The name of the archive.
     * \param target The target to create the Archive with.
     * \param cleanUp If set to true, the Archive d'tor will delete target. */
    Archive(const std::string &name, IOTarget *target, bool cleanUp = true);

    /*! D'tor */
    ~Archive();

    /*! Returns the name of the archive.
     * \return The name of the archive. */
    const std::string& name() const;

    /*! Checks to see if the file exists anywhere in the Archive. The entire archive is
     *  searched and if any match is found, true is returned. If directories are specified
     *  before the filename, then the directories will be matched when searching for the
     *  file as well.
     * \param path The filename to look for.
     * \return true if a file matching the name can be found. */
    bool exists(const std::string &path) const;

    /*! Returns a DataTarget containing the requested file.
     * \note The list must be deleted.
     * \param path The name to lookfor.
     * \return The DataTarget. NULL is returned if a match cannot be found. */
    DataTarget* open(const std::string &path) const;

    /*! Seturns a listing of files in the archive.
     * \note The list must be deleted.
     * \param path The path to start the listing from.
     * \param recurse true if the search should be recursive.
     * \return A pointer to a list of files in the archive. NULL is returned if the
     *         specified path is not a directory. */ 
    std::list<std::string>* listing(const std::string &path = "", bool recurse = true,
                               bool dirs = false) const;

private:
    void loadEndOfCDR();
    void loadFileHeaders();
    void extractDirs();

    void formatPath(std::string &path) const;

private:
    std::string _name;
    IOTarget *_target;
    bool _error;
    bool _cleanUp;

    ZIP_EndOfCDR _endOfCDR;
    std::list<FileHeader> _files;
    std::set<std::string> _dirs;
};

#endif
