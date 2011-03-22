require 'pp'

class Logger
    def info(a)
        puts a
    end
end
$logger = Logger.new

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

module MountainhomeTypeModule
    def self.included(base)
        class << base
            def instance_attributes()     @inst_attributes ||= {} end
            def instance_attributes=(val) @inst_attributes = val end

            def base_class()     @base_class ||= nil end
            def base_class=(val) @base_class = val end

            def has_attributes(*attrs)
                attrs.each do |attribute|
                    instance_attributes[attribute] = nil
                end
            end

            def default_values(pairs)
                pairs.each do |attribute, value|
                    puts "adding #{attribute} with value #{value} to #{self.to_s} (#{self.class})"
                    unless instance_attributes.has_key?(attribute)
                        raise RuntimeError, "Cannot set value for undefined attribute: #{attribute}"
                    end

                    instance_attributes[attribute] = value
                    if value.kind_of?(Proc)
                        define_method(attribute) { instance_eval(&(@inst_attributes || {})[attribute]) }
                    else
                        define_method(attribute) {
                            puts "#{self.to_s} (#{self.class}) instance attributes: #{@inst_attributes.inspect} "
                            (@inst_attributes || {})[attribute]
                        }
                        define_method("#{attribute}=") { |value|
                            (@inst_attributes || {})[attribute] = value
                            puts "#{self.to_s} (#{self.class}) instance attributes: #{@inst_attributes.inspect} "
                        }
                    end
                end # pairs.each
            end # default_values
        end # class << base

        super

    end # self.included
end # module

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
                # Only MountainhomeObject's initialize uses this.
                def self.default_attributes() #{base}.instance_attributes end
                # Allow a class reference to be used to access class attributes. 
                def self.class_attributes() #{base}.class_attributes end
                def self.class_attributes=(val) #{base}.class_attributes = val end
            end
        }

        base.inst_class = name.constantize
    end
end

module MountainhomeObjectModule
    #    include MountainhomeTypeModule
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
        $logger.info "#{"*" * 80}\nTHIS IS NEVER CALLED ACK!\n#{"*" * 80}"
        @inst_attributes = self.class.default_attributes.dup
        $logger.info "default @inst_attributes are #{@inst_attributes.inspect}"
        verify_attributes_are_filled_in
        super(*args)
    end
end

class Actor
    include MountainhomeObjectModule

    attr_accessor :name

    def to_s; @name; end
end

def describe(name, options = {}, &block)
    $logger.info("Creating #{name}")

    name = "#{name}_module"

    # Create the class_module.
    Object.class_eval "module #{name.camelize}; include MountainhomeTypeModule; end"

    klass = name.constantize
    $logger.info "Using #{klass.to_s} of type #{klass.class}"
    # Set the base type if we need to.
    klass.base_class = options[:base] if options[:base]

    Object.class_eval "module #{name.camelize}; include InstantiableModule; end"

    # Evaluate the block properly.
    klass.instance_eval(&block) if block_given?
end

describe(:dwarf, :base => "Actor") { has_attributes(:hp); default_values(:hp => 1) }

=begin
module DwarfModule
    include MHTM
    self.instance_eval { default_values(:hp => 1) }
end

class Dwarf
    include MHTM
end
=end

DwarfModule.respond_to?(:instance_attributes)

DwarfModule.instance_attributes[:hp]

Dwarf.respond_to?(:instance_attributes)

d = Dwarf.new; d.name = "d"
e = Dwarf.new; e.name = "e"
d.hp
e.hp
d.hp = 4
e.hp
e.hp = 5
d.hp
f = Dwarf.new; f.name = "f"
f.hp