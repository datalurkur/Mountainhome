class Weapon
    @types = []
    def self.inherited(subclass)
        @types << subclass
    end
    def self.types
        @types
    end

    def self.min_dmg;  @min_dmg || 1;        end
    def self.max_dmg;  @max_dmg || 1;        end
    def self.dmg_type; @dmg_type || :normal; end

    def self.damage
        [rand(max_dmg - min_dmg + 1) + min_dmg, self.dmg_type]
    end
end

class ShortSword < Weapon
    @max_dmg = 5
end

class Mace < Weapon
    @max_dmg = 10
end

class BattleAxe < Weapon
    @min_dmg = 3
    @max_dmg = 6
end

class Arrow < Weapon
    @max_dmg = 4
    @dmg_type = :piercing
end

class Acid < Weapon
    @max_dmg = 3
    @dmg_type = :corrosive
end

class Armor
    @types = []
    def self.inherited(subclass)
        @types << subclass
    end
    def self.types
        @types
    end

    def self.damage_resistance; @dr || 0; end
    def self.damage_blocking;   @db || 0; end

    # Damage types are:
    #  :piercing  - Ignored DB but not DR
    #  :normal    - Is affected normally by DB and DR
    #  :corrosive - Ignores DR but not DB
    def self.reduce_damage(value, type = :normal)
        if (type != :piercing) && (self.damage_blocking > value)
            # All damage was blocked (damage below blocking threshold)
            return 0
        elsif (type != :corrosive)
            # Damage was reduced by some amount
            return (value * (1.0 - self.damage_resistance)).to_i
        else
            # Damage was unaffected
            return value
        end
    end
end

class Leather < Armor
    @db = 1
    @dr = 0.3
end

class Padded < Armor
    @dr = 0.5
end

class Chainmail < Armor
    @db = 1
    @dr = 0.4
end

class Plate < Armor
    @db = 3
    @dr = 0.2
end

class TestCombatant
    attr_writer :hp, :armor, :weapon, :max_hp

    def initialize(max_hp, weapon, armor)
        self.weapon = weapon
        self.armor = armor
        self.max_hp = max_hp
    end

    def max_hp; @max_hp || 1;        end
    def armor;  @armor ||= Armor     end
    def weapon; @weapon ||= Weapon   end
    def hp;     @hp ||= self.max_hp; end

    def deal_damage
        return self.weapon.damage
    end

    def take_damage(value, type)
        damage  = self.armor.reduce_damage(value, type)
        self.hp = [self.hp - damage, 0].max

        if self.hp == 0
            return [:dead, damage]
        else
            return [:alive, damage]
        end
    end
end
