require 'BasicExtensions'

# Module that, when extended, will track all children of the object that are created after
# this module has been extended. Note that we have to use extend, because the included and
# inherited callbacks don't play well with the 'super' call otherwise. Basically if we
# want to allow children to have their own versions of these, we have to use extend (at
# least that's what my testing seemed to indicate).
#
module RecordChildren
    def inherited(klass)
        self.add_child(klass)
        klass.extend(RecordChildren)
    end

    def included(klass)
        self.add_child(klass)
        klass.extend(RecordChildren)
    end

    def add_child(klass, include_ancestors = true)
        @children ||= []

        unless @children.include?(klass)
            @children << klass

            if include_ancestors
                self.ancestors.each do |ancestor|
                    if ancestor.kind_of?(RecordChildren)
                        ancestor.add_child(klass, false)
                    end
                end
            end
        end
    end

    def ancestors(type = :all)
        ancestors = super()

        case type
        when :all     then ancestors
        when :classes then ancestors.select { |a| a.kind_of?(Class) }
        when :modules then ancestors.reject { |a| a.kind_of?(Class) }
        else nil
        end
    end

    def children(type = :all)
        @children ||= []
        case type
        when :all     then @children
        when :classes then @children.select { |a| a.kind_of?(Class) }
        when :modules then @children.reject { |a| a.kind_of?(Class) }
        else nil
        end
    end
end

###############################
# Mountainhome DSL extensions #
###############################

# These allow us to easily and dynamically extend the base DSL. Here we can inject methods
# into both modules (which can be called in describe blocks) and instantiable classes
# (which can be accessed and used during gameplay). These are kept separate to allow for
# growth of the DSL. The reason we do not directly include these into the relevent MH
# modules is to avoid cluttering their inheritance tree and to provide one easy location
# to see exactly what language extensions are in place (the extensions class method).

# Extensions work using four callback functions:
# module_extends    - Called when a new module that uses the extension is first created.
#                     This can be used to add methods to the new module. These methods
#                     will be directly callable from the describe block. Note that this
#                     should only be called ONCE per module that has a given extension as
#                     it handles things like one time method definitions.
# module_inherits   - Called when a module is set to inherit from another module that also
#                     has the same extension. This allows us to do things like copy state
#                     from the parent module, into the child module. Note that this needs
#                     to be called for each parent / new_module pair, for every extension,
#                     to make sure all state is copied over properly.
# class_created     - Called when a new class is created from a module that uses this
#                     extension. This is used to add new methods to the newly created
#                     class. Note that this should only be called ONCE per class that has
#                     a given extension as it handles things like one time method
#                     definitions.
# class_initialized - Called when a new instance of a class that was created from a module
#                     that uses this extension is created. This allows us to copy relevant
#                     state into the new instance.

module InstanceAttributesExtension
    def self.module_extends(new_module)
        class << new_module
            def instance_attributes; @instance_attributes ||= {}; end
            def set_instance_attributes(attribute_hash)
                attribute_hash.each do |attribute, value|
                    self.instance_attributes[attribute] = value
                end
            end
        end
    end

    def self.module_inherits(new_module, parent_module)
        new_module.set_instance_attributes(parent_module.instance_attributes)
    end

    def self.class_created(new_module, klass)
        class << klass
            def default_instance_attributes
                parent_module.instance_attributes
            end
        end

        # Assumes the @attributes ivar will be set in class_initialized.
        new_module.class_attributes.each_pair do |attribute, value|
            if value.kind_of?(Proc)
                klass.class_eval %{
                    def #{attribute}; instance_eval(&@attributes[:#{attribute}]); end
                }
            else
                klass.class_eval %{
                    def #{attribute};         @attributes[:#{attribute}];         end
                    def #{attribute}=(value); @attributes[:#{attribute}] = value; end
                }
            end
        end
    end

    def self.class_initialized(instance)
        # Make sure all instance variables have a value supplied.
        # Note - This made sense when we had the concept of declaring and defining attrs,
        # but now, this might not be valid.
        nil_attrs = []
        instance.class.default_instance_attributes.each do |k, v|
            nil_attrs << k if v.nil?
        end

        # Raise in a post pass step so we can see everything that is unset at once.
        if !nil_attrs.empty?
            raise RuntimeError, "Cannot make instance of #{self.class}! The " +
                "following attributes are undefined:\n  #{nil_attrs.join("\n  ")}"
        end

        # And setup the @attributes ivar.
        instance.instance_eval do
            @attributes = self.class.default_instance_attributes.dup
        end

    end
end

module ClassAttributesExtension
    def self.module_extends(new_module)
        class << new_module
            def class_attributes; @class_attributes ||= {}; end
            def set_class_attributes(attribute_hash)
                attribute_hash.each do |attribute, value|
                    self.class_attributes[attribute] = value
                end
            end
        end
    end

    def self.module_inherits(new_module, parent_module)
        new_module.set_class_attributes(parent_module.class_attributes)
    end

    def self.class_created(new_module, klass)
        class << klass
            def attributes; parent_module.class_attributes; end
        end

        new_module.class_attributes.each_pair do |attribute, value|
            if value.kind_of?(Proc)
                klass.instance_eval %{
                    def #{attribute}; instance_eval(&self.attributes[:#{attribute}]); end
                }
            else
                klass.instance_eval %{
                    def #{attribute}; self.attributes[:#{attribute}]; end

                    # Lets leave them immutable, for now.
                    # def #{attribute}=(value); self.attributes[:#{attribute}] = value; end
                }
            end
        end
    end

    def self.class_initialized(instance); end
end

module TileParametersExtension
    def self.module_extends(new_module)
        class << new_module
            def parameters; @parameters; end
            def set_parameter(new_params = {}); set_parameters(new_params); end
            def set_parameters(new_params = {})
                @parameters ||= {}
                new_params.each do |name, default|
                    @parameters[name] = default
                end
            end
            def remove_parameter(to_remove); remove_parameters(to_remove); end
            def remove_parameters(*to_remove)
                @parameters ||= {}
                to_remove.each do |current|
                    @parameters.delete(current)
                end
            end
        end
    end

    def self.module_inherits(new_module, parent_module)
        new_module.set_parameters(parent_module.parameters)
    end

    def self.class_created(new_module, klass)
        klass.class_eval do
            def self.default_parameters; parent_module.parameters; end
            def self.has_parameter?(name); self.default_parameters.has_key?(name); end
            def parameters; @parameters; end
            def has_parameter?(name); @parameters.has_key?(name); end
        end
    end

    def self.class_initialized(instance)
        instance.instance_eval do
            @parameters = self.class.default_parameters.dup
        end
    end
end

module ManagerExtension
    def self.module_extends(new_module)
        class << new_module
            def manager()     @manager ||= nil end
            def manager=(val) @manager = val end
        end
    end

    def self.module_inherits(new_module, parent_module)
        if parent_module.manager
            if new_module.manager && new_module.manager != parent_module.manager
                raise RuntimeError, "Attempting to override a base class value, which " +
                    "is invalid. Current value [#{new_module.manager}] New value " +
                    "[#{parent_module.manager}]"
            end

            new_module.manager = parent_module.manager
        end
    end

    def self.class_created(new_module, klass)
        # Register the module with its manager, which may have been set from a superclass.
        if new_module.manager
            MountainhomeDSL.managers[new_module.manager].register(klass)
        end
    end

    def self.class_initialized(instance); end
end

########################
# Mountainhome modules #
########################
module MountainhomeTypeModule
    extend RecordChildren

    def self.included(base)
        class << base
            def base_class; @base_class; end
            def base_class=(value)
                if value
                    if @base_class && @base_class != value
                        raise RuntimeError, "Attempting to override a base class value, " +
                            "which is invalid. Current value [#{@base_class}] New value " +
                            "[#{value}]"
                    end

                    @base_class = value
                end
            end

            def extensions; @extensions ||= [] end
            def extends(*new_extensions)
                new_extensions.each do |ext|
                    # Make sure we're working with a module, and not just a name.
                    ext = "#{ext}_extension".constantize unless ext.kind_of?(Module)

                    if !self.extensions.include?(ext)
                        ext.module_extends(self)
                        self.extensions << ext
                    end
                end
            end

            # XXXBMW: FIXME - Why do we need 'uses'? This should almost certainly be nuked from orbit.
            def uses(modules)
                modules.each do |mod|
                    self.module_eval { include mod.constantize }
                end
            end

            def is_an(modules) is_a(modules) end

            def is_a(modules)
                modules.each do |parent|
                    parent = "#{parent}_module".constantize unless parent.kind_of?(Module)
                    self.instance_eval { include(parent) }

                    # NOTE: We should only have MountainhomeTypeModules in here, so don't
                    # worry about safety checks.

                    # Inherit the base class, if there is one set.
                    self.base_class = parent.base_class

                    # Inherit all extensions from the parent. Note that extends properly
                    # deals with duplicate extensions and that module_inherits needs to be
                    # called for each module / extension pair to make sure all state is
                    # copied properly.
                    parent.extensions.each do |ext|
                        self.extends(ext)
                        ext.module_inherits(self, parent)
                    end
                end
            end

        end # class << base

        super

    end # self.included
end # module

# Contains logic specific to all instantiable objects, as opposed to
# MountainhomeTypeModule, which contains logic to include in non-modules, instead.
module InstantiableModule
    def self.included(base)
        # Error out if no base type has been specified. Note that everything should have
        # the BaseClassExtension.
        if base.base_class.nil?
            raise RuntimeError, "Module '#{base}' does not have a defined base class."
        end

        # Create the actual new class and include the parent module.
        name = base.name.gsub(/Module$/, '')
        Object.class_eval %{
            class #{name} < #{base.base_class}
                include #{base}

                # Give a way to access the parent module for the class_initialized callback.
                def self.parent_module; #{base}; end
            end
        }

        klass = name.constantize

        # Make the extension callbacks.
        base.extensions.each do |ext|
            ext.class_created(base, klass)
        end

        # Provide a way to get the instantiable class without using String ops.
        base.instance_eval %{
            def inst_class; #{base}; end
        }
    end

    def initialize(*args)
        # Handle the extension callbacks.
        self.class.parent_module.extensions.each do |ext|
            ext.class_initialized(self)
        end

        super(*args)
    end
end

# Deal with offsets to the graphics positioning.
# self.position, x, y and z are all in 'game' coordinates.
# set_position translates this into 'graphics' coordinates
# when informing an entity.
module TranslatePosition
    attr_reader :x, :y, :z

    # Set full position for every change.
    def x=(x) set_position( x,@y,@z); end
    def y=(y) set_position(@x, y,@z); end
    def z=(z) set_position(@x,@y, z); end

    attr_accessor :x_offset, :y_offset, :z_offset

    # By default, offset to the center of the tile.
    def x_offset() @x_offset || 0.5; end
    def y_offset() @y_offset || 0.5; end
    # And floating above (0.4 works well for spheres.)
    def z_offset() @z_offset || 0.0; end

    def set_position(x, y, z)
        @x = x
        @y = y
        @z = z
        # Can be included in MHActor (an entity) or an object with an @entity.
        if self.is_a?(MHActor)
            super(@x + self.x_offset, @y + self.y_offset, @z + self.z_offset)
        elsif @entity
            @entity.set_position(@x + self.x_offset, @y + self.y_offset, @z + self.z_offset)
        end
    end

    def position=(position)
        set_position(*position) if position.is_a?(Array) and position.size == 3
    end

    def position
        [@x, @y, @z]
    end
end

###########################
# Mountainhome base types #
###########################

class Actor < MHActor
    include MountainhomeTypeModule
    include TranslatePosition

    attr_accessor :name, :world

    def to_s; @name; end
end

class Tile
    include MountainhomeTypeModule
end

#######################
# And some setup code #
#######################
require 'Event'

require 'LoadingState'
require 'GameState'
require 'MenuState'

require 'Jobs'

require 'PlantManager'

#########################################################
# Static class to track descriptions and their managers #
#########################################################

class MountainhomeDSL
    # XXXBMW: FIXME - These should probably be done elsewhere. The ManagerExtension module?
    @managers = Hash.new

    def self.register_manager(klass)
        @managers[klass] ||= klass.new
    end

    def self.managers
        @managers ||= Hash.new
    end

    def self.describe(name, options = {}, &block)
        # Create the class.
        name = "#{name}_module"
        Object.class_eval "module #{name.camelize}; include MountainhomeTypeModule; end"
        $logger.info("Creating #{name}")
        new_mh_module = name.constantize

        # Add in basic and specified extensions. Inherited extensions will come with the 'is_a' call.
        new_mh_module.extends :instance_attributes, :class_attributes, :manager
        new_mh_module.extends(*options[:extends]) if options[:extends]

        # Handle the is_a and uses directives.
        # XXXBMW: FIXME - Why do we need 'uses'? This should almost certainly be nuked from orbit.
        new_mh_module.uses(options[:uses]) if options[:uses]
        new_mh_module.is_a(([options[:is_a]] + [options[:is_an]]).flatten.compact)

        # Set the base type if we need to.
        new_mh_module.base_class = options[:base] if options[:base]

        # Now that everything else is setup, send the module_extends callback.
        new_mh_module.extensions.each do |ext|
            ext.module_extends(new_mh_module)
        end

        # Evaluate the block properly.
        new_mh_module.instance_eval(&block) if block_given?

        # Register the manager.
        if options[:managed_by]
            new_mh_module.manager = options[:managed_by]
            self.register_manager(new_mh_module.manager)
        end

        # Once everything else is handled, we can safely create our real Class, if needed.
        if options[:instantiable]
            new_mh_module.instance_eval { include InstantiableModule }
        end

        new_mh_module
    end
end

require 'Descriptions'

##################
# Global Objects #
##################
# MHCore objects cannot go out of scope.
$mhcore = MHCore.new

$mhcore.register_state(MenuState.new($mhcore),    "MenuState")
$mhcore.register_state(LoadingState.new($mhcore), "LoadingState")
$mhcore.register_state(GameState.new($mhcore),    "GameState")
$mhcore.set_state("MenuState")
