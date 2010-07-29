#############################
# Built in class extensions #
#############################

class Symbol
    def constantize () self.to_s.constantize end
    def camelize    () self.to_s.camelize    end
end

class String
    def constantize
        name = self.camelize
        Object.const_defined?(name) ?
            Object.const_get(name) :
            Object.const_missing(name)
    end

    def camelize
        self.gsub(/\/(.?)/) { "::#{$1.upcase}" }.gsub(/(?:^|_)(.)/) { $1.upcase }
    end
end

########################
# Mountainhome modules #
########################
module MountainhomeTypeModule
    def self.included(base)
        class << base
            def class_attributes()     @attributes ||= {} end
            def class_attributes=(val) @attributes = val end

            def base_class()     @base_class ||= nil end
            def base_class=(val) @base_class = val end

            def is_an(modules) is_a(modules) end

            def is_a(modules)
                modules.each do |mod|
                    mod = "#{mod}_module".constantize
                    self.instance_eval { include(mod) }

                    # Make sure we give preference to the subclass's keys and values.
                    self.class_attributes = mod.class_attributes.merge(class_attributes) if mod.respond_to?(:class_attributes)

                    # And don't forget to copy over the base type (though only do it if it's been set).
                    self.base_class = mod.base_class if mod.respond_to?(:base_class) && mod.base_class
                end
            end

            def has_attributes(*attrs)
                attrs.each do |attribute|
                    class_attributes[attribute] = nil
                end
            end

            def attribute_values(pairs)
                pairs.each do |attribute, value|
                    unless class_attributes.has_key?(attribute)
                        raise RuntimeError, "Cannot set value for undefined attribute: #{attribute}"
                    end

                    class_attributes[attribute] = value
                    if value.kind_of?(Proc)
                        define_method(attribute) { instance_eval(&@attributes[attribute]) }
                    else
                        define_method(attribute) { @attributes[attribute] }
                        define_method("#{attribute}=") { |value| @attributes[attribute] = value }
                    end
                end # pairs.each
            end # attribute_values

            def managed_by(singleton)
                @manager = singleton.instance
            end
        end # class << base
    end # self.included
end # module

module InstantiableModule
    def self.included(base)
        name = base.name.gsub(/Module$/, '')

        # Error our if no base type has been specified.
        if base.base_class.nil?
            raise RuntimeError, "Module '#{base}' does not have a defined base class."
        end

        Object.class_eval %{
            class #{name} < #{base.base_class}
                include #{base}
                # This will make sure MountainhomeObject's initialize will properly call the correct class_attributes method.
                def self.class_attributes
                    #{base}.class_attributes
                end
            end
        }
    end
end

###########################
# Mountainhome base types #
###########################
class MountainhomeObject
    include MountainhomeTypeModule
    def verify_attributes_are_filled_in
        nil_attrs = []
        self.class.class_attributes.each { |k, v| nil_attrs << k if v.nil? }
        return if nil_attrs.size == 0
        raise RuntimeError, "Cannot make instance of #{self.class}! The " +
            "following attributes are undefined:\n  #{nil_attrs.join("\n  ")}"
    end

    def eval_attributes
        map = Hash.new
        @attributes.keys.each { |key| map[key] = self.send(key) }
        map
    end

    def initialize(*args)
        verify_attributes_are_filled_in
        @attributes = self.class.class_attributes.dup
        super(*args)
    end
end

require 'ExtraModules'

class Actor < MountainhomeObject
    attr_accessor :entity, :name, :world
    include Mining
end

class Tile < MountainhomeObject
end

#######################
# And some setup code #
#######################
require 'Event'

require 'StateEventCreator'
require 'LoadingState'
require 'GameState'
require 'MenuState'

require 'Terrain'
require 'LiquidManager'
require 'PlantManager'

#########################################################
# Static class to track descriptions and their managers #
#########################################################

class MountainhomeDSL
    @managers = Hash.new
    def self.register_manager(klass)
        @managers[klass] ||= klass.new
    end

    def self.describe(name, options = {}, &block)
        # Create the class.
        name = "#{name}_module"
        Object.class_eval "module #{name.camelize}; include MountainhomeTypeModule; end"
        $logger.info("Creating #{name}")
        klass = name.constantize

        # extend the proper modules.
        klass.is_a(([options[:is_a]] + [options[:is_an]]).flatten.compact)

        # Set the base type if we need to.
        klass.base_class = options[:base] if options[:base]

        # Evaluate the block properly.
        klass.instance_eval(&block)

        # Register the manager
        if options.has_key?(:managed_by)
            @managers[options[:managed_by]] ||= options[:managed_by].new
            @managers[options[:managed_by]].register(klass)
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
$mhcore.register_state(GameState.new($mhcore),    "GameState")
$mhcore.register_state(MenuState.new($mhcore),    "MenuState")
$mhcore.register_state(LoadingState.new($mhcore), "LoadingState")
$mhcore.set_state("MenuState")
