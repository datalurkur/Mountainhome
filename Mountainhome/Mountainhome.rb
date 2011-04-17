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

########################
# Mountainhome modules #
########################
module MountainhomeTypeModule
    extend RecordChildren

    def self.included(base)
        class << base
            def attributes()     @attributes ||= {} end
            def attributes=(val) @attributes = val end

            def class_attrs()     @class_attrs ||= {} end
            def class_attrs=(val) @class_attrs = val end

            def base_class()     @base_class ||= nil end
            def base_class=(val) @base_class = val end

            def manager()     @manager ||= nil end
            def manager=(val) @manager = val end

            def uses(modules)
                modules.each do |mod|
                    self.module_eval { include mod.constantize }
                end
            end

            def extensions()     @extensions ||= [] end
            def extends(modules) self.extensions.concat(modules) end

            def is_an(modules) is_a(modules) end

            def is_a(modules)
                modules.each do |mod|
                    mod = "#{mod}_module".constantize
                    self.instance_eval { include(mod) }

                    # Make sure we give preference to the subclass's keys and values.
                    self.attributes = mod.attributes.merge(attributes) if mod.respond_to?(:attributes)
                    self.class_attrs = mod.class_attrs.merge(class_attrs) if mod.respond_to?(:class_attrs)

                    # Include the parent's extensions as extensions
                    self.extensions.concat(mod.extensions) if mod.respond_to?(:extensions)

                    # And don't forget to copy over the base type (though only do it if it's been set).
                    self.base_class = mod.base_class if mod.respond_to?(:base_class) && mod.base_class

                    # Also copy the manager
                    self.manager = mod.manager if mod.respond_to?(:manager) && mod.manager
                end
            end

            # Define attributes (instance attributes)
            def has_attributes(*attrs)
                attrs.each do |attribute|
                    attributes[attribute] = nil
                end
            end

            def has_class_attr(attr, value)
                # If self is instantiable, add the class variables to it directly
                if self.include?(InstantiableModule)
                    self.inst_class.class_eval %{
                        class << self
                            attr_accessor :#{attr}
                        end
                    }
                    self.inst_class.send("#{attr}=", value)
                end
                # Add it to the list of class attrs in the module on the off-chance that this instantiable class
                #  is the parent of another
                class_attrs[attr] = value
            end
            def has_class_attrs(attr_hash={})
                attr_hash.each_pair { |k,v| has_class_attr(k,v) }
            end

            def default_value(pair) default_values(pair) end
            def default_values(pairs)
                pairs.each do |attribute, value|
                    unless attributes.has_key?(attribute)
                        raise RuntimeError, "Cannot set value for undefined attribute: #{attribute}"
                    end

                    attributes[attribute] = value
                    if value.kind_of?(Proc)
                        define_method(attribute) { instance_eval(&@inst_attributes[attribute]) }
                    else
                        define_method(attribute) { @inst_attributes[attribute] }
                        define_method("#{attribute}=") { |value| @inst_attributes[attribute] = value }
                    end
                end # pairs.each
            end # default_values
        end # class << base

        super

    end # self.included
end # module

# Contains logic specific to all instantiable objects, as opposed to
# MountainhomeTypeModule, which contains logic to include in non-modules, instead.
module InstantiableModule
    def self.included(base)
        # Provide a way to get the instantiable class without using String ops
        class << base
            attr_accessor :inst_class
        end

        # Error out if no base type has been specified.
        if base.base_class.nil?
            raise RuntimeError, "Module '#{base}' does not have a defined base class."
        end

        name = base.name.gsub(/Module$/, '')

        Object.class_eval %{
            class #{name} < #{base.base_class}
                include #{base}
                # Pass through attributes from the KlassModule to the Klass.
                def self.default_attributes() #{base}.attributes end
            end
        }

        # Set up class variables and their accessors
        klass = name.constantize
        base.class_attrs.each_pair do |k, v|
            klass.class_eval %{
                class << self
                    attr_accessor :#{k}
                end
            }
            klass.send("#{k}=", v)
        end

        # Remember this class in the module that created it
        base.inst_class = name.constantize
    end

    def verify_attributes_are_filled_in
        nil_attrs = []
        @inst_attributes.each { |k, v| nil_attrs << k if v.nil? }
        unless nil_attrs.empty?
            raise RuntimeError, "Cannot make instance of #{self.class}! The " +
                "following attributes are undefined:\n  #{nil_attrs.join("\n  ")}"
        end
    end

    def eval_attributes
        map = Hash.new
        @inst_attributes.keys.each { |key| map[key] = self.send(key) }
        map
    end

    def initialize(*args)
        @inst_attributes = self.class.default_attributes.dup
        verify_attributes_are_filled_in
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
    def z_offset() @z_offset || 0.4; end

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

module TileParameters
    def has_parameter(param, default)
        self.attributes[:parameters] ||= []
        # Check to see if this parameter exists already
        self.attributes[:parameters].each do |p, d|
            if p == param
                d = default
                return
            end
        end
        # This parameter doesn't exist, add it
        self.attributes[:parameters] << [param, default]
    end

    def has_parameters(*params)
        params.each_pair do |param, default|
            self.has_parameter(param, default)
        end
    end
end

###########################
# Mountainhome base types #
###########################

class Actor < MHActor
    include MountainhomeTypeModule
    include TranslatePosition

    attr_accessor :name

    def to_s; @name; end
end

class Tile
    include MountainhomeTypeModule

    def has_parameter?(param)
        !(@inst_attributes[:parameters].find { |p| p[0] == param }).nil?
    end
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
        klass = name.constantize

        # Extend the proper modules.
        klass.uses(options[:uses]) if options[:uses]
        klass.is_a(([options[:is_a]] + [options[:is_an]]).flatten.compact)

        # Include extensions defined explicitly and from ancestors
        klass.extends(options[:extends]) if options[:extends]
        klass.extensions.each do |mod|
            klass.module_eval { extend mod.constantize }
        end

        # Set the base type if we need to.
        klass.base_class = options[:base] if options[:base]

        # Evaluate the block properly.
        klass.instance_eval(&block) if block_given?

        # Register the manager.
        if options[:managed_by]
            klass.manager = options[:managed_by]
            self.register_manager(klass.manager)
        end

        # Register the module with its manager, which may have been set from a superclass.
        if klass.manager && (klass.include? InstantiableModule)
            self.managers[klass.manager].register(klass.inst_class)
        end

        klass
    end
end

require 'Descriptions'

##################
# Global Objects #
##################

# MHCore objects cannot go out of scope.
$mhcore = MHCore.new
# Disable the menu until Andrew can get the menus working again.
$mhcore.register_state(MenuState.new($mhcore),    "MenuState")
$mhcore.register_state(LoadingState.new($mhcore), "LoadingState")
$mhcore.register_state(GameState.new($mhcore),    "GameState")
$mhcore.set_state("MenuState")
# $mhcore.set_state("LoadingState", :empty)

# $mhcore.set_state("LoadingState", :generate, {:width => 9, :height => 9, :depth => 9})
