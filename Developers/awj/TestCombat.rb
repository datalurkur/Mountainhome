#!/opt/local/bin/ruby
require 'Combat'

USAGE = "\n"+
    "Usage:\n" +
    "-verbose           Enable verbose logging\n" + 
    "-i [iterations]    Set the number of combats to simulate\n" +
    "-hp [max]          Set maximum hitpoints\n" +
    "-turns [max]       The maximum number of turns to run combat, after which it is considered a tie\n"

$verbose = false
def say(message)
    puts message if $verbose
end

$combat_results_hash = {}

def log_results(weapon_a, armor_a, weapon_b, armor_b, result)
    log_a = get_log_for(weapon_a, armor_a, weapon_b, armor_b)
    log_b = get_log_for(weapon_b, armor_b, weapon_a, armor_a)
    if result == :victor_a
        mod_a = 1
        mod_b = -1
    elsif result == :victor_b
        mod_a = -1
        mod_b = 1
    else
        mod_a = mod_b = 0
    end
    log_a[0] += mod_a
    log_a[1] += 1
    log_b[0] += mod_b
    log_b[1] += 1
end

def get_log_for(weapon_a, armor_a, weapon_b, armor_b)
    lookup_a = [weapon_a, armor_a]
    lookup_b = [weapon_b, armor_b]

    $combat_results_hash[lookup_a]           ||= {}
    $combat_results_hash[lookup_a][lookup_b] ||= [0,0]

    return $combat_results_hash[lookup_a][lookup_b]
end

iterations = 1_000
max_turns = 1_000
max_hp = 10
num_weapons = Weapon.types.size
num_armors = Armor.types.size

while arg = ARGV.shift
    case arg
    when "-hp"
        max_hp = ARGV.shift.to_i
    when "-i"
        iterations = ARGV.shift.to_i
    when "-verbose"
        $verbose = true
    when "-turns"
        max_turns = ARGV.shift.to_i
    else
        puts USAGE
        exit
    end
end

puts "Simulating #{iterations.inspect} fights"
iterations.times do |i|
    weapon_a = Weapon.types[rand(num_weapons)]
    weapon_b = Weapon.types[rand(num_weapons)]
    armor_a = Armor.types[rand(num_armors)]
    armor_b = Armor.types[rand(num_armors)]
    combatant_a = TestCombatant.new(20, weapon_a, armor_a)
    combatant_b = TestCombatant.new(20, weapon_b, armor_b)
    say "=" * 50
    say "Fight #{i} consists of #{weapon_a}/#{armor_a} vs #{weapon_b}/#{armor_b}"

    turns = 0
    while true
        turns += 1
        damage_a = combatant_a.deal_damage
        damage_b = combatant_b.deal_damage

        taken_a = combatant_a.take_damage(*damage_b)
        taken_b = combatant_b.take_damage(*damage_a)

=begin
        say " - Combatant A deals #{damage_a.inspect} damage with his #{weapon_a}"
        say " - Combatant B takes #{taken_b.last} damage, reduced by his #{armor_b} armor"
        say " - Combatant B deals #{damage_b.inspect} damage with his #{weapon_b}"
        say " - Combatant A takes #{taken_a.last} damage, reduced by his #{armor_a} armor"
=end

        if taken_a.first == :dead || taken_b.first == :dead
            if taken_a.first == taken_b.first
                say "Both combatants have died!"
                log_results(weapon_a, armor_a, weapon_b, armor_b, :tie)
            elsif taken_a.first == :dead
                say "Combatant A has died!"
                log_results(weapon_a, armor_a, weapon_b, armor_b, :victor_b)
            else
                say "Combatant B has died!"
                log_results(weapon_a, armor_a, weapon_b, armor_b, :victor_a)
            end
            break
        elsif turns > max_turns
            say "Neither combatant has died after #{max_turns} turns."
            log_results(weapon_a, armor_a, weapon_b, armor_b, :tie)
            break
        end
    end
end

puts "RESULTS\n" + "="*30
column_width = 10
$combat_results_hash.each_pair do |key, hashes|
    puts key.inspect

    # Print the list of armor types
    armor_string = "".center(column_width) + "|"
    Armor.types.each do |armor|
        armor_string += armor.to_s.center(column_width)
    end
    puts armor_string
    puts ("-" * column_width) + "|" + ("-" * column_width * num_armors)

    Weapon.types.each do |weapon|
        weapon_hashes = hashes.reject { |k,v| !(k.include? weapon) }
        weapon_string = weapon.to_s.center(column_width) + "|"

        Armor.types.each do |armor|
            armor_hash = weapon_hashes.reject { |k,v| !(k.include? armor) }
            result = if armor_hash.size > 0
                result = armor_hash.values.first
                (100 * result.first / result.last).to_i.to_s + "%"
            else
                "X"
            end.center(column_width)
            weapon_string += result
        end
        puts weapon_string
    end
    puts "\n"
end

f = File.open("combat_test_results", "w")
Marshal.dump($combat_results_hash, f)
f.close
