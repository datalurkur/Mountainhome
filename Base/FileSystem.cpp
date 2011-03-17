/*
 *  FileSystem.cpp
 *  File
 *
 *  Created by Brent Wilson on 5/31/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "FileSystem.h"
#include "Assertion.h"

#include "File.h"

#include <cassert>
#include <fstream>

#include <Boost/filesystem.hpp>

std::list<Archive*> FileSystem::_resources;

bool FileSystem::IsDirectory(const std::string &path) {
    return boost::filesystem::is_directory(HasLeadingDecorations(path) ? path : CurrentDirectory() + path);
}

bool FileSystem::Exists(const std::string &path) {
    std::string newPath(path);
    FormatPath(newPath);
    return boost::filesystem::exists(newPath);
}

std::string& FileSystem::ExtractDirectory(const std::string &p, std::string &result) {
    std::string path(p);
    FormatPath(path);

    if (IsDirectory(path)) {
        result = path;
    } else {
        string::size_type position = path.find_last_of("/");
        if (position == string::npos) {
            result = "";
        } else {
            result = path.substr(0, position + 1);
        }
    }

    return FormatPath(result);
}

std::string& FileSystem::ExtractExtension(const std::string &p, std::string &result, bool forceLowerCase) {
    std::string path(p);
    FormatPath(path);

    if (IsDirectory(path)) {
        result = "";
    } else {
        string::size_type position = path.find_last_of(".");
        if (position == string::npos || position == path.size()) {
            result = "";
        } else {
            result = path.substr(position + 1);
        }
    }

    if (forceLowerCase) {
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    }

    return result;
}

bool FileSystem::GetReadName(const std::string &file, std::string &result) {
    result = file;
    FormatPath(result);
    return Exists(result) && !IsDirectory(result);
}

std::string& FileSystem::ExtractFilename(const std::string &p, std::string &result) {
    std::string path(p);
    FormatPath(path);

    if (IsDirectory(path)) {
        result = "";
    } else {
        string::size_type position = path.find_last_of("/");
        if (position == path.size()) {
            result = "";
        } else if (position == string::npos) {
            result = path;
        } else {
            result = path.substr(position + 1);
        }
    }
    
    return result;
}

bool FileSystem::CreateDirectory(const std::string &dir) {
    if (Exists(dir)) { return true; }
    return boost::filesystem::create_directory(dir);
}

std::string& FileSystem::FormatPath(std::string &path, bool prependCurrent) {
    int pos;
    while ((pos = path.find("\\")) != std::string::npos) {
        path.replace(pos, 1, "/");
    }

    if (!HasLeadingDecorations(path)) {
        path = prependCurrent ? CurrentDirectory() + path : "./" + path;
    }

    if (*path.rbegin() != '/' && IsDirectory(path)) {
        path += "/";
    }

//    // Attempt to simplify out any '..'s in the path.
//    std::string::size_type current;
//    while((current = path.find("/..")) != string::npos) {
//        std::string::size_type lastSlash = path.rfind("/", current - 1);
//        if (lastSlash == string::npos) {
//            Warn("Cannot 'cd ..' past './'");
//            path.erase(current, 3);
//        } else {
//            path.erase(lastSlash, current - lastSlash + 3);
//        }
//    }

    return path;
}

unsigned long long FileSystem::Length(const std::string &name) {
    if (IsDirectory(name)) {
        return 0;
    }

    std::fstream stream(name.c_str(), std::fstream::binary | std::fstream::in);
    if (!stream) { return 0; }

    stream.seekg(0, std::fstream::end);
    unsigned long long result = static_cast<unsigned long long>(stream.tellg());
    stream.close();
    return result;
}

bool FileSystem::Touch(const std::string &path) {
    if (Exists(path)) {
        return false;
    }

    std::fstream stream(path.c_str(), std::fstream::out | std::fstream::app);
    if (!stream) {
        return false;
    }

    stream.close();
    return true;
}

bool FileSystem::Delete(const std::string &path) {
    if (IsDirectory(path)) {
        Warn("Given name is of a directory: " << path);
        return NULL;
    }

    std::string newPath(path);
    FormatPath(newPath);
    boost::filesystem::remove(newPath);
    return !Exists(path);
}

File* FileSystem::GetFile(const std::string &path, long openFlags) {
    if (IsDirectory(path)) {
        Warn("Given name is of a directory: " << path);
        return NULL;
    }

    std::string dir, file;
    ExtractFilename(path, file);
    ExtractDirectory(path, dir);
    FormatPath(dir);
    return new File(dir, file, openFlags);
}

template<typename Iterator>
std::list<std::string>* listing(const std::string &path, bool dirs) {
    if (!FileSystem::IsDirectory(path)) {
        Warn("Can only make listings of directories. Not a directory: " << path);
        return NULL;
    }

    std::list<std::string> *result = new std::list<std::string>;
    if (dirs) {
        result->push_back(path);
    }

    Iterator itr(path);
    for (; itr != Iterator(); itr++) {
        if (FileSystem::IsDirectory(itr->path().string())) {
            if (dirs) {
                result->push_back(itr->path().string() + "/");
            }
        } else {
            result->push_back(itr->path().string());
        }
    }

    result->sort();
    return result;
}

std::list<std::string>* FileSystem::GetListing(const std::string &path, bool recurse,
                                               bool dirs) {
    std::string newPath(path);
    FileSystem::FormatPath(newPath);

    if (!recurse) {
        return listing<boost::filesystem::directory_iterator>(newPath, dirs);
    }

    return listing<boost::filesystem::basic_recursive_directory_iterator
           <boost::filesystem::path> >(newPath, dirs);
}

void FileSystem::ChangeDirectory(const std::string &dir) {
    std::string newDir(dir);
    FormatPath(newDir);
    if (IsDirectory(newDir)) {
        boost::filesystem::current_path(newDir);
    } else {
        Warn("Attempted to set current directory to a non directory: " << newDir);
    }
}

std::string FileSystem::CurrentDirectory() {
    return boost::filesystem::current_path().string();
}

bool FileSystem::HasLeadingDecorations(const std::string &path) {
    return *path.begin() == '/' || path.substr(0, 2) == "./";
}
