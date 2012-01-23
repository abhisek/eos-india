require 'rubygems'
gem 'mechanize'
gem 'hpricot'
require 'mechanize'
require 'hpricot'

class Dict
  attr_reader :agent

  def initialize
    @agent = WWW::Mechanize.new
  end

  def search(str)
    page = @agent.get("http://www.dict.org/bin/Dict")

    search_form = page.form('DICT')
    search_form.Query = str

    @agent.submit(search_form)
  end

  def stripped_body
    @agent.page.body.gsub(/<\/?[^>]*>/, "")
  end

  def stripped_tokenized_body
    body = stripped_body()
    words = body.split(' ')

    # got to remove special chars like (;[{
    words.size.times do |i|
      w1 = words[i]
      w2 = w1.to_s.strip.gsub(/(\(|\)|\{|\}|\[|\]|\.|;|\"|\\|&|#)/,"")
      w2 = w2.to_s.strip.gsub("&nbsp;", "")

      words[i] = w2
    end

    return words
  end
end

if __FILE__ == $0
  d = Dict.new
  d.search("falciform")
  puts d.stripped_tokenized_body.inspect

  f = File.new("search.txt", "w")
  f.write(d.stripped_body)
  f.close
end
