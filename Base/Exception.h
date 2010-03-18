/*
 *  Exception.h
 *  Base
 *
 *  Created by loch on 12/23/09.
 *  Copyright 2009 Brent Wilson. All rights reserved.
 *
 */

#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_
#include "Base.h"

#define THROW(exception, desc) \
do { \
   	std::ostringstream stream; \
	stream << desc; \
    throw exception(stream.str(), SYS_FUNCTION, __FILE__, __LINE__); \
} while(0)

class Exception : public std::exception {
protected:
    unsigned int _line;
    std::string _typeName;
    std::string _description;
    std::string _source;
    std::string _file;
    mutable std::string _fullDesc;

public:
    /*! Advanced constructor. */
    Exception(const std::string &description, const char* type, const char* source, const char* file, unsigned int line);

    /*! Needed for  compatibility with std::exception */
    virtual ~Exception() throw() {}

    /*! Returns a string with the full description of this error.
     * \remarks The description contains the error number, the description
     *          supplied by the thrower, what routine threw the exception,
     *          and will also supply extra platform-specific information
     *          where applicable. For example - in the case of a rendering
     *          library error, the description of the error will include both
     *          the place in which OGRE found the problem, and a text
     *          description from the 3D rendering library, if available. */
    virtual const std::string& getFullDescription(void) const;

    /*! Gets the source function. */
    virtual const std::string &getSource() const { return _source; }

    /*! Gets source file name. */
    virtual const std::string &getFile() const { return _file; }

    /*! Gets line number. */
    virtual unsigned int getLine() const { return _line; }

    /*! Returns a string with only the 'description' field of this exception. Use 
     *  getFullDescriptionto get a full description of the error including line number,
     *  error number and what function threw the exception. */
    virtual const std::string &getDescription(void) const { return _description; }

    /*! Override std::exception::what */
    const char* what() const throw() { return getFullDescription().c_str(); }
};

class DuplicateItemError : public Exception {
public:
    DuplicateItemError(const std::string &desc, const char* source, const char* file, unsigned int line):
        Exception(desc, "Duplicate Item Error", source, file, line) {}
};

class InvalidStateError : public Exception {
public:
    InvalidStateError(const std::string &desc, const char* source, const char* file, unsigned int line):
        Exception(desc, "Invalid State Error", source, file, line) {}
};

class InternalError : public Exception {
public:
    InternalError(const std::string &desc, const char* source, const char* file, unsigned int line):
        Exception(desc, "Internal Error", source, file, line) {}
};

class AssertionFailure : public Exception {
public:
    AssertionFailure(const std::string &desc, const char* source, const char* file, unsigned int line):
        Exception(desc, "Assertion Failure", source, file, line) {}
};

class NotImplementedError : public Exception {
public:
    NotImplementedError(const std::string &desc, const char* source, const char* file, unsigned int line):
        Exception(desc, "Not Implemented Error", source, file, line) {}
};

#endif
