/*
 *  OptionsModule.h
 *  Engine
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _OPTIONSMODULE_H_
#define _OPTIONSMODULE_H_
#include <Content/PropertyTree.h>
#include <string>

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Special type definitions
//////////////////////////////////////////////////////////////////////////////////////////

/*! \brief Represents a resolution, which can be written to and read from disk. */
struct Resolution {
    Resolution(): width(-1), height(-1) {}
    Resolution(int w, int h): width(w), height(h) {}
    int width, height;
};

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Special type translators
//////////////////////////////////////////////////////////////////////////////////////////

namespace boost {
    namespace property_tree {
        struct resolution_translator {
            Resolution get_value(const std::string &input) {
                // Break the string into tokens.
                std::vector<std::string> tokens;
                tokenize(input, "x", tokens);

                // Verify the input, must be in form: 1024x768
                if (tokens.size() != 2) {
                    THROW(InternalError, "Invalid resolution read from file: " << input);
                }

                // Fill in our new Resolution.
                return Resolution(
                    strtol(tokens[0].c_str(), (char **)NULL, 10),
                    strtol(tokens[1].c_str(), (char **)NULL, 10));
            }

            std::string put_value(const Resolution &res) {
                std::ostringstream desc;
                desc <<  res.width << "x" << res.height;
                return desc.str();
            }

            typedef std::string internal_type;
            typedef Resolution  external_type;
        };


        // Define the translators.
        template <> struct translator_between<std::string, Resolution> { typedef resolution_translator type; };
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark OptionsModule declarations
//////////////////////////////////////////////////////////////////////////////////////////

/*! This is an options persistence module that can be used to store various game settings.
 *  Individual options can be sorted into groups and interested objects can register to be
 *  notified of changes to various groups, allowing them to act on those changes
 *  accordingly. Actual options are accessed by group and key and changes to options are
 *  NOT immediately stored to disk. Rather, changes are buffered until 'apply' is called,
 *  at which point changes are flushed to disk and the appropriate registered objects are
 *  notified of the changes.
 * \brief Stores grouped game options in a persistent property tree. */
class OptionsModule {
public:
    class Listener {
    public:
        virtual ~Listener() {}
        virtual void optionsUpdated(const std::string &section, OptionsModule *module) = 0;

    };

public:
    /*! Creates a new options module that will be saved to and loaded from the given
     *  directory under the specified name. */
    OptionsModule(const std::string &homeDirectory, const std::string &name = "options.ini");

    /*! Deletes the options module. */
    virtual ~OptionsModule();

    /*! Takes any recent changes to the settings and saves them to disk as official. This
     *  also notifies any registered objects of the changes made. */
    void apply();

    /*! Loads the settings from disk, clearing any unsaved changes. */
    void load();

    /*! Registers the given listener with the options module to be informed whenever
     *  changes take place in the given section. */
    void registerListener(const std::string &section, Listener *listener);

    /*! Puts the given value at the specified path. Paths are specified as follows:
     *
     *  <section>.<key>
     *
     *  So, 'video.resolution' is an example of a valid path.
     * \param path The path to put the value at.
     * \param value The value to store at the given path. */
    template <typename T>
    void put(const std::string &path, const T &value);

    /*! Gets value at the specified path. Paths are specified as follows:
     *
     *  <section>.<key>
     *
     *  So, 'video.resolution' is an example of a valid path.
     * \param path The path fro which to get the value. */
    template <typename T>
    T get(const std::string &path);

protected:
    /*! Notifies all of registered listeners that the named section has updated. */
    void notifySection(const std::string &section);

    template <typename T>
    void put_default(const std::string &path, const T &value);

protected:
    typedef std::list<Listener*> ListenerList;
    typedef std::map<std::string, ListenerList> ListenerMap;

protected:
    boost::property_tree::ptree _realOptions;
    boost::property_tree::ptree _tempOptions;
    ListenerMap _listeners;
    std::string _filename;

};

template <typename T>
T OptionsModule::get(const std::string &path) {
    return _realOptions.get<T>(path);
}

template <typename T>
void OptionsModule::put(const std::string &path, const T &value) {
    _tempOptions.put(path, value);
}

template <typename T>
void OptionsModule::put_default(const std::string &path, const T &value) {
    if (!_tempOptions.get_optional<T>(path)) {
        put(path, value);
    }
}

#endif

