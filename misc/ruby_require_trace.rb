require 'rgl/dot'
require 'rgl/adjacency'
require 'rgl/traversal'

module Kernel
   alias_method :orig_require, :require

   def name_to_path(name)
      return name if (name =~ /^\//) and (File.exists?(name))

      unless name =~ /\.(rb|so)$/
         name1 = name + ".rb"
         name2 = name + ".so"
      else
         name1 = name2 = name
      end

      $:.each do |p|
         path1 = File.join(p, name1)
         path2 = File.join(p, name2)

         return path1 if File.exists?(path1)
         return path2 if File.exists?(path2)
      end

      nil
   end

   def require(name)
      graph = $GRAPH || $GRAPH = RGL::DirectedAdjacencyGraph.new

      caller_name = name_to_path(caller[0].split(':')[0])
      callee_name = name_to_path(name)
      $GRAPH.add_edge(caller_name, callee_name) unless (caller_name.nil?) or (callee_name.nil?)

      orig_require(name)
   end
end

Kernel.at_exit {$GRAPH && $GRAPH.write_to_graphic_file('png', ENV['GRAPH_FILE_NAME'] || 'ruby')}

require 'net/http'

