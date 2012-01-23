#!/usr/bin/ruby

#
# Automated Rice Donator script.. for your eye's pleasure.. hehe :PpPpp
#
# Copright (C) - Humans only.. Aliens stay away...
# Be sure to (e)mail me 1% of the Salvation you earn by using this script :P
#
# Hell is hot and it doesn't have an AC.. I don't like it there..
#

$:.unshift(File.dirname(__FILE__) + '/lib')
require 'freerice_com'
require 'dict_org'
require 'mistake_handler'

def find_by_simple_existance_matching(answers, words)
  h = {}
  answers.each do |a|
    h[a] = 0
  end

  answers.each do |a|
    words.each do |w|
      h[a] += 1 if w == a
    end
  end

  sols = h.sort_by {|k, v| v}.reverse

  if sols.empty?
    puts "FSEM: No match.."
    return nil
  end

  puts "FSEM: #{sols.size} possible matches, selecting first"
  return sols[0][0]
end

def find_best_sol(word, words = [])
  find_by_simple_existance_matching(word, words)  
end

def do_the_thing(agent = nil)
  old_score = 0
  memory = {}
  m = MistakeHandler.new
  wrongs = [] # to store the wrong answers for manual effort
  freerice = FreeRice.new(agent)
  dict = Dict.new

  freerice.get_question
  loop do
    puts "Freerice: Getting next word"
    word = freerice.get_question_word
    puts "Freerice: Word is \"#{word}\""
    words = freerice.get_answers
    puts "Freerice: Answers (#{words.keys.inspect})"

    if wrongs.include?(word)
      puts "EEE: In wrong list, selecting from memory"
      answer = memory[word]

      if answer.nil?
         puts "EEE: In wrong list, selecting from mistake handler"
         opts = m.get_remaining_options(word)

         if opts.empty?
            puts "EEE: No remaining option, this should not happen.. skipping"
            freerice.get_question
            next
         end

         puts "EEE: Remianing options #{opts.size}"
         answer = opts[0]
      end
      puts "EEE: Selected answer \"#{answer}\""
      n = words[answer]
    else 
      puts "Dict: Searching for #{word}"
      dict.search(word.to_s)
      tokens = dict.stripped_tokenized_body
      puts "Dict: Got #{tokens.size} tokens"

      answer = find_best_sol(words.keys, tokens)
      if answer.nil?
        #puts "EEE: Failed to guess answer, manual insert.."
        #print "Enter number: "
        #n = gets.strip.to_i
        puts "EEE: Failed to guess answer, skipping.."
        freerice.get_question
        next
      else
        puts "EEE: Answer found: #{answer} (#{words[answer]})"

        n = words[answer].to_i
      end
    end

    puts "Freerice: Submiting answer"
    freerice.submit_answer(n)

    score = freerice.get_score # score only increases
    if (score == 0) or (score == old_score)
      puts "Freerice: \033[01;31mWRONG answer\033[00m"
      wrongs << word unless wrongs.include?(word)
      m.add(word, answer, words.keys) # TODO: deprecate
      memory[word] = freerice.get_correct_answer
    else
      puts "Freerice: \033[01;32mScore is #{score}\033[00m"
      old_score = score
    end

    sleep(5)  # TODO: add random delay
  end
end

if __FILE__ == $0
  a = WWW::Mechanize.new
  loop do
    begin
      do_the_thing(a)
    rescue Timeout::Error => e
      puts "Exception: #{e.message}"
      sleep(1)
    rescue => e
      sleep(1)
    end
  end
end
