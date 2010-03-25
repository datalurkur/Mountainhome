/*
 *  Archive.cpp
 *  Base
 *
 *  Created by Brent Wilson on 11/30/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "Archive.h"
#include "DataTarget.h"
#include "BinaryStream.h"
#include "Assertion.h"

Archive::Archive(const std::string &n, IOTarget *t, bool cleanUp): _name(n), _target(t),
_error(false), _cleanUp(cleanUp) {
    loadEndOfCDR();
    if (_endOfCDR.sizeOfCDR          == 0xFFFFFFFF ||
        _endOfCDR.offsetOfStartOfCDR == 0xFFFFFFFF) {
        Error("Unsupported ZIP format loaded!!!");
        _error = true;
        return;
    }

    loadFileHeaders();
    extractDirs();
}

Archive::~Archive() {
    if (_cleanUp) {
        delete _target;
    }
}

void Archive::extractDirs() {
    std::list<std::string> topDirs;
    std::list<FileHeader>::iterator topItr;
    for (topItr = _files.begin(); topItr != _files.end(); topItr++) {
        if (topItr->dir.size()) {
            topDirs.push_front(topItr->dir);
        }
    }

    // Remove the duplicates in the list.
    topDirs.sort();
    topDirs.unique();

    // Because we want to add ALL directories, not just those containing files, we need to
    // loop through all of the directories containing files (obtained above) and then add
    // both them and all of their parents to the _dirs set. We break down the directories
    // here rather than in the above loop because there will generally be far fewer
    // directories than files in a ZIP and there will be at worst the same number.
    std::list<std::string>::iterator itr;
    for (itr = topDirs.begin(); itr != topDirs.end(); itr++) {
        while(itr->size()) {
            // Insert the current string.
            _dirs.insert(*itr);

            // Set pos to the last '/' or 0 if there are no more.
            std::string::size_type pos = itr->rfind('/');
            pos = pos == std::string::npos ? 0 : pos;

            // Erase everything after pos.
            itr->erase(pos);
        }
    }
}

void Archive::loadFileHeaders() {
    BinaryStream bin(_target, IOTarget::Read, false);
    bin.seek(_endOfCDR.offsetOfStartOfCDR, IOTarget::Beginning);

    ZIP_FileHeader current;
    while(bin.position() - _endOfCDR.offsetOfStartOfCDR < _endOfCDR.sizeOfCDR) {
        bin.read(&current, sizeof(ZIP_FileHeader));
        if (current.signature != ZIP_FileSig) {
            break;
        }

        char* cname = new char[current.fileNameLength + 1];
        cname[current.fileNameLength] = '\0';

        bin.read(cname, current.fileNameLength);
        bin.seek(current.extraSize(), IOTarget::Current);
        std::string name(cname);
        delete[] cname;

        _files.push_front(FileHeader(current, name));
    }

    _target->close();
}

const int ChunkSize = sizeof(ZIP_EndOfCDR) * 2;
void Archive::loadEndOfCDR() {
    BinaryStream bin(_target, IOTarget::Read, false);
    bin.seek(0, IOTarget::End);
    char chunk[ChunkSize];
    bool found = false;

    while(!found && !_error && bin.position() >= ChunkSize) {
        bin.seek(-ChunkSize, IOTarget::Current);
        bin.read(chunk, ChunkSize);
        bin.seek(-ChunkSize, IOTarget::Current);
        for (int i = 0; i < ChunkSize; i++) {
            if (*((int*)(chunk + i)) == ZIP_EndOfCDRSig) {
                bin.seek(i, IOTarget::Current);
                bin.read(&_endOfCDR, sizeof(ZIP_EndOfCDR));
                if (_endOfCDR.commentLength == bin.bytesLeft()) {
                    found = true;
                } else {
                    bin.seek(-ChunkSize, IOTarget::Current);
                }
            }
        }    
    }

    if (!found || _error) {
        Warn("The given IOTarget does not point at a legitimate ZIP archive.");
        _error = true;
    }

    _target->close();
}

const std::string& Archive::name() const {
    return _name;
}

bool Archive::exists(const std::string &p) const {
    if (_error) {
        Warn("Cannot make function calls on an invalid archive.");
        return false;
    }

    std::string path(p);
    formatPath(path);

    std::list<FileHeader>::const_iterator itr;
    for (itr = _files.begin(); itr != _files.end(); itr++) {
        if (itr->fullName == path) {
            return true;
        }
    }

    return false;
}

DataTarget* Archive::open(const std::string &p) const {
    if (_error) {
        Warn("Cannot make function calls on an invalid archive.");
        return NULL;
    }

    std::string path(p);
    formatPath(path);

    DataTarget *result = NULL;
    std::list<FileHeader>::const_iterator itr;
    for (itr = _files.begin(); result == NULL && itr != _files.end(); itr++) {
        if (itr->fullName == path) {
            ASSERT_EQ(itr->compressedSize, itr->uncompressedSize);
            BinaryStream bin(_target, IOTarget::Read, false);
            bin.seek(itr->offset, IOTarget::Beginning);

            unsigned char *data = new unsigned char[itr->uncompressedSize];
            bin.read(data, itr->uncompressedSize);

            result = new DataTarget(data, itr->uncompressedSize);
            
            _target->close();
        }
    }
    return result;
}

std::list<std::string>* Archive::listing(const std::string &p, bool recurse,
                                         bool dirs) const {
    std::string path(p);
    if (_error) {
        Warn("Cannot make function calls on an invalid archive.");
        return NULL;
    }

    formatPath(path);

    // Add the needed files to the list.
    std::list<std::string>* result = new std::list<std::string>;
    std::list<FileHeader>::const_iterator itr;
    for (itr = _files.begin(); itr != _files.end(); itr++) {
        if ((!recurse && itr->dir == path) ||
            ( recurse && itr->dir.find(path) == 0)) {
            result->push_back(itr->fullName);
        }
    }

    // Add the needed directories to the list.
    if (dirs) {
        std::set<std::string>::const_iterator itr;
        for (itr = _dirs.begin(); itr != _dirs.end(); itr++) {
            std::string containing = "";
            int pos = itr->rfind('/', itr->size() - 2);
            if (pos != std::string::npos) {
                containing = itr->substr(0, pos);
            }

            if ((!recurse && containing == path) ||
                ( recurse && containing.find(path) == 0)) {
                // Directories are not stored with and internal '/'. This adds one.
                result->push_back(*itr + "/");
            }
        }
    }

    return result;
}

void Archive::formatPath(std::string &path) const {
    // The internal directories have no leading or trailing symbols. Clean off any from
    // the path or the search will always fail.
    while (path.size() && (*path.begin() == '.' || *path.begin() == '/')) {
        path.erase(path.begin());
    }

    while (path.size() && *path.rbegin() == '/') {
        path.erase(path.size() - 1, 1);
    }
}

/*
In case I ever decide that listing w/ dirs is too slow.
struct Directory {
    Directory(const std::string &n):
        fullName(n),
        containing("") {
        std::string::size_type pos = n.rfind('/', n.size() - 2);
        if (pos != std::string::npos) {
            containing = n.substr(0, pos);
        }
    }

    std::string fullName;
    std::string containing;
};
*/

/*! Finds files matching the given pattern and returns a list of them.
 * \note The list must be deleted.
 * \param pattern The pattern to use when collecting filenames.
 * \param recurse true if the search should be recursive.
 * \return The list of matching files. */
//std::list<std::string>* find(const std::string &pattern, bool recurse = true) const;
/*
std::list<std::string>* Archive::find(const std::string &pattern, bool recurse) const {
    if (_error) {
        Warn("Cannot make function calls on an invalid archive.");
        return NULL;
    }

    return NULL;
}*/
