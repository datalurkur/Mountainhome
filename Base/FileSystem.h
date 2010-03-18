
#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_
#include "IOTarget.h"
#include "Archive.h"
#include "Logger.h"

#include <fstream>
#include <string>
#include <list>

class TextFile;
class BinaryFile;
class DataTarget;
class Archive;
class File;

/*! This class acts as a virtual representation of the file system. It gives the user
 *  direct access to the FS in several different ways. Common functions are implemented as
 *  either static members or members accessable via the singleton. Static members make no
 *  use of local variables whereas standard member functions do, having a concept of the
 *  current directory and a list of resource locations to make finding and opening files
 *  easier. The FileSystem also provides factory methods for creation of either a TextFile
 *  or a BinaryFile.
 *  
 * \brief a class giving the user access to files on the computer.
 * \note Users can supply a path name in any form, but as a standard convention, all
 *       internal path information will be stored with a trailing '/' and either '/' or
 *       './' at the front. This is for clarity and consistency.
 * \note When passed to the library, or returned from it, a leading './' indicates the
 *       path is relative to the working directory of the actual file system.
 * \note When passed to the library, or returned from it, a leading '/' indicates the
 *       file is relative to the root of the actual file system.
 * \note If a path is given to the library without a leading './' or '/' the library's
 *       current directory and resource locations will be used. If the operation requires
 *       some kind of search to be performed (existence or opening files) the current dir
 *       will be checked followed by resource locations. For other operations, the
 *       current directory will be prepended to the passed directory. This does not apply
 *       to static member functions!
 * \note A path retreived from the library will always have a leading './' or '/'.
 * \todo Implement support for archives as resource locations.
 * \todo Add Delete, Move, Copy, Link, etc...
 * \todo Look into adding temp files.
 * \todo Add XML files?
 * \author Brent Wilson
 * \date May 31, 2007 */
class FileSystem {
public:
    /*! Checks the given path to see if it is a directory.
     * \return True if the given path is a directory. */
    static bool IsDirectory(const std::string &path);

    /*! Looks at the file system and checks to see if the given path exists.
     * \param path The name of the path to check for.
     * \return True if the path exists, false otherwise. */
    static bool Exists(const std::string &path);

    /*! Checks a given file to see how long it is in bytes.
     * \param path The name of the file to check.
     * \return The length of the file, or zero is it doesn't exist or is a directory. */
    static unsigned long long Length(const std::string &path);

    /*! Acts as an analog to the standard unix touch command.
     * \param path The path to touch.
     * \return True on success, false if the file was not created or something else
     *         already existed. */
    static bool Touch(const std::string &path);

    /*! Attempts to delete a path from the file system.
     * \param path The path to delete.
     * \return True if the path was deleted, false otherwise. */
    static bool Delete(const std::string &path);

    /*! Takes the given path and constructs a File object out of it. If the given path is
     *  a directory, NULL is returned. If the file does not exist, a File object pointing
     *  at the given location is created anyways. This is so you can create new files.
     * \note The returned value must be deleted by the user.
     * \param path The path to use when creating the File object.
     * \return A pointer to a File object. This must be deleted by the user (or given to a
     *         BinaryStream or TextStream to delete). */
    static File* GetFile(const std::string &path, long openFlags = IOTarget::None);

    /*! This looks at the given path and gets a list of files inside of it. The result may
     *  optionally include directory and may optionally be recursive as well.
     * \note The returned value must be deleted by the user.
     * \param path The path to list out.
     * \param recurse true if the listing should be recursive.
     * \param dirs true if directories should be included in the listing.
     * \return A pointer to the listing. NULL is returned if the given path is not a
     *         directory */
    static std::list<std::string>* GetListing(const std::string &path = ".",
                                              bool recurse = false,
                                              bool dirs = false);

    /*! Sets the current directory for the library. The current directory is prepended to
     *  the beggining of path arguments if no leading './' or '/'.
     *       i.e. The current dir is "./foo/" and you call setCurrentDirectory("bar"), the
     *       new current directory will be ./foo/bar/.
     * \param dir */
    static void ChangeDirectory(const std::string &dir);

    /*! Resets the current directory to its initial value. */
    static void ClearCurrentDirectory();

    /*! Returns the name of the current directory.
     * \return The name of the current directory. */
    static const std::string& CurrentDirectory();

    /*! Formalizes the given file name and checks to see if the file exists for reading in
     *  the first place. This will give a decorated filename with the current directory
     *  prepended to it and return if the file is good to open for reading.
     * \param file The file to check for.
     * \param result The resulting, decorated string.
     * \return If the given name is pointing at a file or does not exist, false will be
     *         returned. Otherwise, true is returned. */
    static bool GetReadName(const std::string &file, std::string &result);

    /*! Searches through the loaded resource packs and looks for the named file. If it is
     *  found, a DataTarget is constructed and returned.
     * \note The returned value must be deleted by the user.
     * \param The name of the file to look for.
     * \return NULL if the named file cannot be found, otherwise a DataTarget is
     *         returned. */
    static DataTarget* getOrLoadResource(const std::string &path);

    /*! Adds a new ResourcePack to the search stack. The most recently added Archives
     *  are searched first.
     * \param pack The ResourcePack to push on the stack. */
    static void pushResourcePack(Archive *pack);

    /*! Remove the most recently added Archive from the stack. Once it has been removed,
     *  the Archive is deleted. */
    static void popResourcePack();

    /*! Looks for the named file in the current directory. If it is found, it returns a
     *  stream of the specified type that can read the file. If it is not found, all
     *  loaded resource packs are searched. If it is found in one of these, a stream of
     *  the correct type that is capable of reading the resource is returned. If neither
     *  of these pass, then NULL is returned and the error is logged.
     * \note The returned value must be deleted by the user.
     * \note Remember, the stream returned is read only.
     * \param path The path to look for when creating the read stream.
     * \return NULL if the file can't be found, a pointer to a read stream otherwise. */
    template <typename T>
    static T* GetReadStream(const std::string &path);

    /*! Takes the given path and extracts the prepended directory.
     * \param path The std::string to extract the path from.
     * \param result The extracted path.
     * \return A reference to the extracted path. */
    static std::string& ExtractDirectory(const std::string &path, std::string &result);

    /*! Takes the given file and extracts the extension name
     * \param path The std::string to extract the extension from.
     * \param result The extracted extension.
     * \return A reference to the extracted extension. */
    static std::string& ExtractExtension(const std::string &path, std::string &result);

    /*! Takes the given file name and removes the prepended path.
     * \param path The std::string to remove the file name from.
     * \param result The resulting file name with no prepended directory.
     * \return A reference to the result. */
    static std::string& ExtractFilename(const std::string &path, std::string &result);

    /*! Adds a leading './' if neither a leading './' or '/' is present. It also adds a
     *  trailing '/' to the path if it is a directory.
     * \param path The path to add decorations to.
     * \param addCurrentDir If no leading '/' or './' is present, the current dir will
     *        also be prepended.
     * \return A reference to the given path. */
    static std::string& FormatPath(std::string &path, bool addCurrentDir = true);
private:
    static bool HasLeadingDecorations(const std::string &path);

    static std::list<Archive*> _resources;
    static std::string _currentDir; /*!< The current directory. */

    friend class TestFileSystem;
};

#include "DataTarget.h"
#include "File.h"

template <typename T>
T* FileSystem::GetReadStream(const std::string &path) {
    std::string fullName;
    if (FileSystem::GetReadName(path, fullName)) {
        return new T(GetFile(fullName), IOTarget::Read, true);
    }

    DataTarget *res = NULL;
    if (res = getOrLoadResource(path)) {
        return new T(res, IOTarget::Read, true);
    }
    
    Warn("FileSystem: Could not find the requested file.");
    LogStream::IncrementIndent();
    Warn("Given name: " << path);
    Warn("Searched:");
    LogStream::IncrementIndent();
    Warn(_currentDir);
    std::list<Archive*>::iterator itr;
    for (itr = _resources.begin(); itr != _resources.end(); itr++) {
        Warn((*itr)->name());
    }

    LogStream::DecrementIndent();
    LogStream::DecrementIndent();
    return NULL;
}

#endif
