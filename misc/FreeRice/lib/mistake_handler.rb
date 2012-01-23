class MistakeHandler
  def initialize
    @errors = {}
  end

  def add(question, answer, choices = [])
    if @errors[question].nil?
      # new mistake
      c = choices.dup
      c.delete(answer)
      @errors[question] = c
    else
      @errors[question].delete(answer)
    end
  end

  def get_remaining_options(question)
    @errors[question] || []
  end
end

if __FILE__ == $0
  m = MistakeHandler.new

  m.add("A", "B", ["A", "B", "C", "D"])
  puts m.get_remaining_options("A").inspect
  m.add("A", "C")
  puts m.get_remaining_options("A").inspect
  m.add("A", "D")
  puts m.get_remaining_options("A").inspect
end
