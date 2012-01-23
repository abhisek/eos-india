#!/usr/bin/ruby

module VimEnvApp

   class Installer
      def initialize
         prefix = "/usr/share/vim/vimfiles"

         unless File.exists? prefix
            print "Please enter vim share directory: "
            prefix = gets

            raise "PathNotFound" unless File.exists? prefix
         end

         install_dir = File.join(prefix, 'vim-env')

         Dir.mkdir(install_dir) unless File.exists? install_dir

         @install_dir = install_dir
         @profiles = File.join(File.dirname(__FILE__), 'profiles')
	      @bin = File.join(File.dirname(__FILE__), 'src/vim-env.rb')
	      @install_bin = "/usr/bin/vim-env"
         
         raise "ProfilesNotFound" unless File.exists? @profiles
      end

      def run
         Dir.entries(@profiles).each do |profile|
            next if profile =~ /^\./
            path = File.join(@profiles, profile)

            unless File.directory? path
               f = File.open(path)
               data = f.read
               f.close

               f = File.open(File.join(@install_dir, profile), "w")
               f.write data
               f.close
            end
         end

	      f = File.open(@bin)
	      data = f.read
	      f.close

	      f = File.open(@install_bin, "w")
	      f.write data
	      f.close
      end
   end

end

if __FILE__ == $0
   VimEnvApp::Installer.new.run
end
