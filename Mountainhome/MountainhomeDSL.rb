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
# Mountainhome classes #
########################
module MountainhomeTypeModule
  def self.included(base)
    class << base
      def class_attributes() @attributes ||= {} end
      def class_attributes=(val) @attributes = val end
      def is_an(modules) is_a(modules) end

      def is_a(modules)
        modules.each do |mod|
          mod = "#{mod}_module".constantize
          self.instance_eval { include(mod) }
          if mod.respond_to?(:class_attributes)
            # Make sure we give preference to the subclass's keys and values.
            self.class_attributes = mod.class_attributes.merge(class_attributes)
          end
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
    end # class << base
  end # self.included
end # module

class MountainhomeObject < MHObject
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

module InstantiableModule
  def self.included(base)
    name = base.name.gsub(/Module$/, '')
    Object.class_eval %{
      class #{name} < MountainhomeObject
        include #{base}
        # This will make sure MountainhomeObject's initialize will properly call the correct class_attributes method.
        def self.class_attributes
          #{base}.class_attributes
        end
      end
    }
  end
end

##################################
# The imfamous 'describe' method #
##################################
def describe(name, options = {}, &block)
  # Create the class.
  name = "#{name}_module"
  Object.class_eval "module #{name.camelize}; include MountainhomeTypeModule; end"
  klass = name.constantize

  # extend the proper modules.
  klass.is_a(([options[:is_a]] + [options[:is_an]]).flatten.compact)

  # Evaluate the block properly.
  klass.instance_eval(&block)
  klass
end

###########################
# Some actual definitions #
###########################
describe :natural do
  has_attributes :con, :hd, :level, :hp, :weight
  attribute_values(
    :hp     => Proc.new { (level * hd / 2.0).floor + ((con - 10) / 2) },
    :weight => Proc.new { con * hd },
    :level  => 1)
end

describe :tile do
  has_attributes :rarity, :grouping_type, :material
end

describe :dwarf, :is_a => [:natural, :instantiable] do
  attribute_values(
    :con => 16,
    :hd  => 12)
end

describe :bedrock, :is_a => [:tile, :instantiable] do
  attribute_values(
    :rarity        => :common,
    :grouping_type => :large_expanses,
    :material      => 'bedrock')
end

describe :hardrock, :is_a => [:tile, :instantiable] do
  attribute_values(
    :rarity        => :common,
    :grouping_type => :large_expanses,
    :material      => 'hardrock')
end

describe :softrock, :is_a => [:tile, :instantiable] do
  attribute_values(
    :rarity        => :common,
    :grouping_type => :large_expanses,
    :material      => 'softrock')
end

describe :sediment, :is_a => [:tile, :instantiable] do
  attribute_values(
    :rarity        => :common,
    :grouping_type => :large_expanses,
    :material      => 'sediment')
end

describe :empty, :is_a => [:tile, :instantiable] do
  attribute_values(
    :rarity        => :common,
    :grouping_type => :large_expanses,
    :material      => 'empty')
end

##################
# And an example #
##################
#$logger.info Hardrock.new.inspect
#dwarf = Dwarf.new
#$logger.info (dwarf.methods - Object.new.methods).sort.inspect
#$logger.info dwarf.eval_attributes.inspect
#dwarf.level = 2
#$logger.info dwarf.eval_attributes.inspect
#dwarf.con = 2
#$logger.info dwarf.eval_attributes.inspect
