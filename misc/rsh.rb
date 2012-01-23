#!/usr/bin/ruby
require 'readline'

class RShell

   def initialize()
      Signal.trap('INT') {
         puts "Enter exit/quit to exit shell"
      }
   end

   def unixcmd?(cmd = nil)
      return nil unless cmd

      paths = ENV["PATH"].split(':')
      paths.each do |p|
         file = "#{p}/#{cmd.split(' ',2)[0]}"
         return true if File.exists? file
      end

      false
   end

   def run

      loop do
         cmd = Readline::readline("rsh #{ENV['PWD']}> $ ", true)
         exit if cmd =~ /exit|quit/
         if unixcmd? cmd
            system(cmd)
         else
            begin
               eval cmd
            rescue
               puts "Command not found"
            end
         end
         
      end
      
   end

end

RShell.new.run
