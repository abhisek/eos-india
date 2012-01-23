require 'rubygems'
gem 'mechanize'
gem 'hpricot'
require 'mechanize'
require 'hpricot'

class FreeRice
  attr_reader :agent

  def initialize(agent = nil)
    @agent = agent || WWW::Mechanize.new
    @agent.user_agent = "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)"
  end

  def get_question
    @agent.get("http://www.freerice.com/index.php")
  end

  def get_question_word
    @agent.page.body =~ /<li><strong>(.*)<\/strong> means:<\/li>/

    return $1
  end

  def get_answers
    answers = {}
    @agent.page.body =~ /<li><noscript><input type="submit" name="SELECTED" value=" 1 "> <\/noscript><a href=".*">(.*)<\/a><\/li>/
    answers.merge! $1.to_s.gsub("&nbsp;", "") => 1
    @agent.page.body =~ /<li><noscript><input type="submit" name="SELECTED" value=" 2 "> <\/noscript><a href=".*">(.*)<\/a><\/li>/
    answers.merge! $1.to_s.gsub("&nbsp;", "") => 2
    @agent.page.body =~ /<li><noscript><input type="submit" name="SELECTED" value=" 3 "> <\/noscript><a href=".*">(.*)<\/a><\/li>/
    answers.merge! $1.to_s.gsub("&nbsp;", "") => 3
    @agent.page.body =~ /<li><noscript><input type="submit" name="SELECTED" value=" 4 "> <\/noscript><a href=".*">(.*)<\/a><\/li>/
    answers.merge! $1.to_s.gsub("&nbsp;", "") => 4
  end

  def get_score
    ret = nil
    
    if ret.nil?
      ret = @agent.page.body =~ /<p id="donatedAmount">.*\s([0-9]+)\s.*<\/p>/
    end
    if ret.nil?
      ret = @agent.page.body =~ /<p id="donatedAmount">.*>([0-9]+)\s.*<\/p>/
    end

    return $1.to_i
  end

  def get_correct_answer
    @agent.page.body =~ /<div id="incorrect">(.*)<\/div>/
    str = $1

    unless str.nil?
      a = str.split('=')[1]
      return a.strip unless a.nil?
    end

    return nil
  end

  def submit_answer(answer_number)
    submit_form = @agent.page.form('form1')
    submit_form.SELECTED = answer_number.to_s

    @agent.submit(submit_form)
  end
end

if __FILE__ == $0
  f = FreeRice.new
  f.get_question

  puts f.get_question_word.inspect
  puts f.get_answers.inspect
  
  print "Enter choice: "
  choice = gets.strip.to_i
  f.submit_answer(choice)
  puts f.get_score
  puts f.get_correct_answer

  y = File.open("./body.txt", "w")
  y.write(f.agent.page.body)
  y.close
end
