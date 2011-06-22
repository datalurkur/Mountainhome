class DTNode; end

class Decision < DTNode
    attr_writer :trueNode, :falseNode
    Branches = {}

    def trueNode()  @trueNode  || Branches[true]; end
    def falseNode() @falseNode || Branches[false]; end

    def self.condition?(decider)
    end

    def getBranch(decider)
        Branches[self.class.condition?(decider)]
    end

    def makeDecision(decider)
        node = getBranch(decider)
        case(node)
        when nil; $logger.info "nil decision reached from #{self.inspect}"
        when Decision; return node.makeDecision(decider)
        when Action
            $logger.info "#{decider.name} decides to #{Action}"
            retun nil # TODO: set the decider to do whatever action
        end
    end
end

class Action < DTNode
end

class HungerNode < Decision
    Branches[true] = EatNode
    def self.condition?(decider)
       decider.nutrition < (decider.glutton? ? 80 : 50)
    end
end 

class EatNode < Action
    # store/set up Eat task and assign somehow
    def self.callback
        Eat.new()
    end
end

class DecisionTree
    def initialize(*node_types)
        @root = node_types.shift.new

        old_node = @root
        # Attach each node in the initial list to the previous one
        node_types.each do |node|
            old_node.falseNode = node_types.shift.new
            old_node = node
        end
    end

    def makeDecision(decider)
       @root.makeDecision(decider)
    end
end

# This is a straight decision tree.
forager_behavior = DecisionTree.new(FleeNode, GrazeNode, RoamNode)
# This doesn't preclude the possibility of setting up a tree under a node
# and then setting up DecisionTree and giving it that node.

# Equivalent code to above line:
node  = FleeNode.new
node2 = GrazeNode.new
node3 = RoamNode.new
node.falseNode = node2
node2.falseNode = node3
forager_behavior = DecisionTree.new(node)