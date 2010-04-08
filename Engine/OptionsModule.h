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
#include <Base/PropertyTree.h>
#include <string>

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

    /*! Saves the current settings list to disk, but does NOT notify registered objects of
     *  any changes made.
     * \seealso apply */
    void save();

protected:
    std::string _filename;
    boost::property_tree::ptree _ptree;

};

#endif

