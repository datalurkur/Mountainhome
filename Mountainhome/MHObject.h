/*
 *  MHObject.h
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _MHOBJECT_H_
#define _MHOBJECT_H_
#include "RubyBindings.h"
#include <string>

class SceneManager;
class Material;
class MHWorld;
class Entity;
class Model;

/*! Represents the basic in game, viewable object of Mountainhome. This contains all of
 *  the hooks into the engine and handles updating position, dealing with animations,
 *  setting up the material and models, etc... It also provides the ruby bindings
 *  necessary to interact with the C++ object from within ruby.
 * \note This class should remain generally barebones, leaving much of the higher level
 *  logic to the ruby class. */
class MHObject : public ManyObjectBinding<MHObject> {
public:
#pragma mark MHObject ruby bindings
    static void SetupBindings();
	
    static VALUE Initialize(VALUE self, VALUE name, VALUE world, VALUE model, VALUE mat);
	static VALUE SetPosition(VALUE self, VALUE x, VALUE y, VALUE z);
	static VALUE Rotate(VALUE self, VALUE angle, VALUE x_axis, VALUE y_axis, VALUE z_axis);

public:
#pragma mark MHObject declarations
    /*! Creates a new entity in the world!
     * \param name The name of the object, used for easy referencing.
     * \param world The world in which this object belongs.
     * \param model The name of the model to use when rendering this object.
     * \param material The name of the material to use when rendering this object. */
    MHObject(const std::string name, MHWorld *world,
        const std::string model, const std::string material);

    /*! Destroys the object. */
    virtual ~MHObject();

private:
    MHWorld *_world; /*!< The world this object belongs to.                 */
    Entity *_entity; /*!< The Entity representing this object in the scene. */

};

#endif
