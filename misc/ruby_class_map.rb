require 'rgl/dot'
require 'rgl/adjacency'
require 'rgl/traversal'

def draw_class_hierarchy_graph(klass, file = nil)
   g = RGL::DirectedAdjacencyGraph.new
   
   ObjectSpace.each_object(Class) do |k|
      next unless k.ancestors.include?(klass)

      to = k.ancestors[0]
      from = k.ancestors[1]

      g.add_edge(from.to_s, to.to_s) unless((from.nil?) or (to.nil?))
   end

   g.write_to_graphic_file('png', 'rcv')
end

require 'net/http'

draw_class_hierarchy_graph(Net::HTTPHeader)
