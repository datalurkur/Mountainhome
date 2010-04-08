#!/usr/bin/ruby

class EventTranslator
    def initialize
        @map = {}
        @before_mapping = nil
        @after_mapping = nil
    end

    def set_mapping(event, action)
        @map[event] = action
    end

    def set_mapping(event, &block)
        @map[event] = block
    end

    def default_before_action(&block)
        @before_mapping = block
    end

    def default_after_action(&block)
        @after_mapping = block
    end

    def translate(event)
        if @before_mapping != nil
            status = @before_mapping.call(event)
            return status if status == :handled
        end

        if @map[event] != nil and @map[event].is_a?(Proc)
            @map[event].call
            return :handled
        end

        if @after_mapping != nil
            return @after_mapping.call(event)
        end

        :unhandled
    end
end
