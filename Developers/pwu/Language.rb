#!/usr/bin/ruby

# Read input and strip to words
input_file = File.open("jeeves.txt", "r")
input = input_file.read
input_file.close

# kill punctuation dead
punctuation_frei = input.gsub(/[^a-zA-Z \.]+/, " ")

# kill excess whitespace and drop capital letters
wordstr = punctuation_frei.gsub(/\W+/, " ").downcase

# split into individual unique, sorted words
WORD_LIST = wordstr.split(/\W+/).uniq.sort

# generate mutation hash
class Array
    def random_index
        self[Kernel.rand(self.size)]
    end
end

LETTERS = ('a'..'z').to_a
VOWELS = "aeiou".split(//)
CONSONANTS = LETTERS - VOWELS

class Language
    def initialize
        @mutation = Language.mutate
        @words = WORD_LIST.map { |word| Language.mutate_str(@mutation, word) }
        @syllables = Language.generate_syllables(@words)
    end

    # Mutate vowels to other vowels, and consonants to other consonants.
    # This one is fun.
    # mutation = {"v"=>"q", "k"=>"y", "w"=>"p", "l"=>"q", "a"=>"o", "x"=>"w", "m"=>"g", "b"=>"t", "y"=>"d", "n"=>"b", "c"=>"w", "z"=>"z", "o"=>"u", "d"=>"p", "p"=>"t", "e"=>"o", "q"=>"d", "f"=>"p", "r"=>"y", "g"=>"j", "s"=>"z", "h"=>"n", "t"=>"h", "i"=>"e", "u"=>"a", "j"=>"f"}
    def self.mutate
        mutation = Hash.new

        LETTERS.each do |letter|
            if VOWELS.include?(letter)
                mutation[letter] = VOWELS.random_index
            else
                mutation[letter] = CONSONANTS.random_index
            end
        end
        mutation
    end

    # Split the mutated words into syllables.
    def self.generate_syllables(words)
        syllables = []
        words.each do |word|
            while not word.empty? do
                # grab the basic C+V+
                syllable = word.slice!(/^[#{CONSONANTS}]*[#{VOWELS}]*/)

                unless word.empty?
                    # look ahead for consonants
                    next_c = word[/^[#{CONSONANTS}]*/]
                    if next_c and next_c.size == word.size
                        # mash them onto the last syllable if the word ends with these consonants
                        syllable += next_c
                        word = ""
                    elsif next_c
                        # split the consonants between this syllable and the next
                        # FIXME: use random to determine split rather than size / 2
                        cons_num = next_c.size / 2
                        syllable += word.slice!(/^.{#{cons_num}}/)
                        # This next line, which could replace the previous two, doesn't compile, and I have no idea why.
                        #syllable += word.slice!(/^.{#{next_c.size / 2}}/)
                    end
                end

                syllables << syllable
            end
        end
        syllables = syllables.uniq.sort
    end

    # Recompose the syllables together.
    # "derfefmjempjao"
    # "kziezkzef"
    def random_word
        word = ""
        0.upto(rand(4)) do
            word << @syllables.random_index
        end
        word
    end

    def self.mutate_str(mutation, str)
        new_str = String.new
        str.each_char do |c|
            new_str << (mutation[c]? mutation[c] : c)
        end
        new_str
    end
end

dwarven = Language.new
print dwarven.random_word.capitalize
0.upto(rand(20)) do
    print " " + dwarven.random_word
end
puts "."
