class DTNode; end

class Decision < DTNode
    Branches = {}

    attr_writer :trueNode, :falseNode
    def trueNode()  @trueNode  || Branches[true]; end
    def falseNode() @falseNode || Branches[false]; end

    def get_branch(decider)
        Branches[self.class.condition?(decider)]
    end

    def self.condition?(decider); false; end

    def make_decision(decider)
        node = get_branch(decider)
        case(node)
        when nil; nil#$logger.info "From #{self.class}, #{decider} chooses to do nothing."
        when Decision; return node.make_decision(decider)
        when Class
            $logger.info "#{decider.name} decides to #{node.to_s}"
            decider.task = TaskFactory.create(node, decider.world, :position => decider.position)
        end
    end
end

class FleeDecision < Decision
    Branches[true] = FleeTask
#    def self.condition?(decider); false; end
end

class RoamDecision < Decision
    Branches[true] = MoveTask
#    def self.condition?(decider); false; end
end

class GrazeDecision < Decision
    Branches[true] = GrazeTask
    def self.condition?(decider)
        decider.nutrition < 95
    end
end

class EatDecision < Decision
    Branches[true] = EatTask
    def self.condition?(decider)
#        decider.hungry? && decider.world.food_available?(decider)
#        decider.nutrition < (decider.glutton? ? 80 : 50)
        #decider.nutrition < 95
        false
    end
end

class DecisionTree
    def initialize(*node_types)
        @root = (Class === node_types.first) ? node_types.shift.new : node_types.shift

        old_node = @root
        # Attach each node in the initial list to the previous one
        node_types.each do |node|
            old_node.falseNode = (Class === node_types.first) ? node_types.shift.new : node_types.shift
            old_node = node
        end
    end

    def make_decision(decider)
       @root.make_decision(decider)
    end
end

# This is a straight decision tree.
class ForagerAI < DecisionTree
    def initialize
        super(FleeDecision, GrazeDecision, RoamDecision)
    end
end

class DwarfAI < DecisionTree
    def initialize
        super(EatDecision)
    end
end