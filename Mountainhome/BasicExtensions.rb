#############################
# Built in class extensions #
#############################

# Zip together and add, with size checking.
class Array
    def piecewise_add(array)
        if array.size != self.size
            raise ArgumentError, "Array sizes #{[array.size,self.size].inspect} don't match."
        else
            self.zip(array).map { |i,j| i+j }
        end
    end
    def x; self[0]; end
    def x=(val); self[0]=val; end
    def y; self[1]; end
    def y=(val); self[1]=val; end
    def z; self[2]; end
    def z=(val); self[2]=val; end
    def normalize
        magnitude = (self.inject(0) { |sum,i| sum + (i**2) }) ** 0.5
        self.collect { |i| i / magnitude }
    end
end

class Integer
    def power_of_two?
        Math.log2(self).denominator == 1
    end
end

class Symbol
    def constantize () self.to_s.constantize end
    def camelize    () self.to_s.camelize    end
end

class String
    def humanize
        # Way braindead, atm. Could be made much nicer.
        self.gsub(/_/, " ")
    end

    # Called on "String" or "Dwarf", returns the class types String or Dwarf
    def constantize
        name = self.camelize
        Object.const_defined?(name) ?
            Object.const_get(name) :
            Object.const_missing(name)
    end

    # called on "foo/bar" yields Foo::Bar, or "dwarf_module" yields DwarfModule
    def camelize
        self.gsub(/\/(.?)/) { "::#{$1.upcase}" }.gsub(/(?:^|_)(.)/) { $1.upcase }
    end
end

class Object
    def self.delegate_to(obj, *methods)
        methods.each do |method|
            class_eval %{
                def #{method}(*args)
                    @#{obj}.#{method}(*args)
                end
            }
        end
    end
end
