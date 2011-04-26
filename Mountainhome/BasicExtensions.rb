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