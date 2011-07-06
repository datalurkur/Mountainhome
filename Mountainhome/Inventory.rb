module InventoryExtension
    def self.module_extends(new_module)
        class << new_module
            def slots; @slots ||= []; end
            def has_slots(*slots)
                @slots ||= []
                @slots += slots
                @slots.uniq
            end
        end
    end

    def self.module_inherits(new_module, parent_module)
        new_module.add_slots(parent_module.slots)
    end

    def self.class_created(new_module, klass)
        class << klass
            def slots; parent_module.slots; end
        end

        new_module.slots.each do |slot|
            klass.class_eval %{
                def #{slot}; self.inventory[#{slot}]; end
                def #{slot}=(value)
                    unless self.inventory[#{slot}].nil?
                        # Drop the current item in the slot.
                        self.inventory[#{slot}].position = self.position
                    end
                    self.inventory[#{slot}] = value
                end
            }
        end

        klass.class_eval %{
            def inventory; @inventory; end
        }
    end

    # Start wearing and carrying nothing.
    def self.class_initialized(instance)
        instance.instance_eval do
            @inventory = Inventory.new
            self.class.slots.each do |slot|
                @inventory[slot] = slot.new
            end
        end
    end
end

class Inventory < Hash
    def contains?(object)
        return true if self.values.find { |slot| slot.contents.include?(object) }
        return false
    end
end

class Container
    attr_reader :contents, :max_count, :max_size, :allowed_types
    def initialize(allowed_types = :all)
        @contents = []
        @allowed_types = allowed_types
        @count = 0
        @size = 0
    end

    # Check whether the object will fit in the container.
    def can_fit(obj)
#        $logger.info "can_fit: #{(@max_count.nil? || @count < @max_count)} && " +
#                    "#{(@max_size.nil? || (obj.respond_to?(:size) && @size + obj.size > @max_size))} && " +
#                    "#{(@allowed_types == :all || @allowed_types.any? { |type| obj.is_a?(type) })}"

        (@max_count.nil? || @count < @max_count) &&
        (@max_size.nil? || (obj.respond_to?(:size) && @size + obj.size > @max_size)) &&
        (@allowed_types == :all || @allowed_types.any? { |type| obj.is_a?(type) })
    end

    def add_object(world, obj)
        if !can_fit(obj)
            $logger.error "Container error! #{obj.inspect} (size #{obj.size}) does not fit or isn't accepted type of #{self.inspect}!"
            return
        end
        # Stop rendering the object.
        obj.visible = false
        # Remove the object from the freefloating items list.
        world.actors.delete(obj)

        @contents << obj
        @count += 1
        @size += obj.size if obj.respond_to?(:size)
    end

    def remove_object(world, obj, position)
        # Start rendering the object.
        obj.visible = true
        # Add the object to the freefloating items list, and place it.
        world.actors << obj
        obj.position = position

        @contents.delete(obj)
        @count -= 1
        @size -= obj.size if obj.respond_to?(:size)
    end

    def empty?() @contents.empty?; end
end

class Slot < Container
    def initialize(allowed_types = :all)
        @max_count = 1
        super(allowed_types)
    end
end

class CarryingSlot < Slot; end

class Body < Slot; end
class Right_Hand < CarryingSlot; end
class Left_Hand < CarryingSlot; end
class Mouth < CarryingSlot; end