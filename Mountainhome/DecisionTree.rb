class Decision
    def initialize
        @branches = {}
    end

    def set_branch(result, node)
        $logger.info "setting @branches[#{result}] to #{node.inspect}"
        @branches[result] = node
    end

    def get_branch(decider)
        @branches[self.class.condition?(decider)]
    end

    def make_decision(decider)
        node = get_branch(decider)
        case(node)
        when nil; $logger.info "From #{self.class}, #{decider} chooses to do nothing."
        when Decision; return node.make_decision(decider)
        when Class
            $logger.info "#{decider.name} decides to #{node.to_s}"
            decider.task = TaskFactory.create(node, decider.world, self.class.parameters(decider) )
        end
    end

    # Defaults.
    def self.condition?(decider) false; end
    def self.parameters(decider) {}; end

    def to_s() self.class.name + " " + @branches.inspect; end
end

class FleeDecision < Decision
    def initialize() super; @branches[true] = FleeTask; end
#    def self.condition?(decider); false; end
end

class RoamDecision < Decision
    def initialize() super; @branches[true] = MoveTask; end
    def self.condition?(decider) true; end
    def self.parameters(decider) { :position => decider.world.pathable_in_distance(decider.position, 2).rand }; end
end

class GrazeDecision < Decision
    def initialize() super; @branches[true] = GrazeTask; end
    def self.condition?(decider)
        decider.nutrition < 95
    end
end

class EatDecision < Decision
    def initialize() super; @branches[true] = EatTask; end
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
            node = (Class === node) ? node.new : node
            old_node.set_branch(false, node)
            $logger.info old_node.to_s
            old_node = node
        end
    end

    delegate_to :root, :make_decision
end

# This is a straight decision tree.
class ForagerAI < DecisionTree
    def initialize
        super(FleeDecision, GrazeDecision, RoamDecision)
    end
end

class DwarfAI < DecisionTree
    def initialize
        super(EatDecision, RoamDecision)
    end
end