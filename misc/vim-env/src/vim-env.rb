#!/usr/bin/ruby

module VimEnvApp
   NAME     = "Vim-Env"
   VERSION  = "0.1"

# global settings
class Settings
   @@vim_share    = "/usr/share/vim/vimfiles/vim-env"
   @@vim_env_dir  = File.join(@@vim_share, 'vimenv')
   @@vim_profile  = File.join(ENV['HOME'], '.vimrc')
   
   # readers
   def self.vim_profile
      @@vim_profile
   end
   def self.vim_env_dir
      @@vim_env_dir
   end
   def self.vim_share
      @@vim_share
   end

   # writers
end

# vim profile abstraction
class Profile
   attr_reader :name
   attr_reader :description
   attr_reader :body

   def initialize(name = nil, desc = nil, body = nil)
      @name          = name
      @description   = desc
      @body          = body
   end

   def self.load_from_file(file)
      f = File.open(file)
      name_match = /^"\s+name:\s+([-_A-Za-z0-9]+)/
      desc_match = /^"\s+description:\s+([-\s_A-Za-z0-9]+)/ 

      name_line = f.readline.to_s
      desc_line = f.readline.to_s

      name_line.strip!
      name_line.chomp!

      desc_line.strip!
      desc_line.chomp!

      if (name_line =~ name_match) and (desc_line =~ desc_match)
         matches = name_line.scan(name_match)
         name = matches[0][0]
         matches = desc_line.scan(desc_match)
         description = matches[0][0]
         body = f.read
         f.close

         Profile.new(name, description, body)
      else
         f.close
         nil
      end
   end

   def write_to_file(file)
      if (self.name) and (self.description) and (self.body)
         f = File.open(file, "w")
         f.write("\" name: %s\n" % [self.name])
         f.write("\" description: %s\n" % [self.description])
         f.write(self.body)
         f.close
      end
   end

   def load
      write_to_file(Settings.vim_profile)
   end
end

# main class
class VimEnv
   def initialize
      @profiles = []
      @current = Profile.new('default', 'Vim default environment', nil)
   end

   # controllers
   def load_profile(name)
      render("Loading profile: %s" % [name])
      load_profiles

      @profiles.each do |profile|
         if (profile.name == name)
            profile.load
            return true
         end
      end
      
      render("Not found")
      false
   end

   def show_current
      load_current

      render("Current profile: %s\n\n" % [@current.name])
   end

   def show_profiles
      load_profiles

      render("Available profiles: ")
      @profiles.each do |profile|
         render("\t%s:\t\t%s" % [profile.name, profile.description])
      end
   end

   def show_banner
      render "%s v%s\n\n" % [NAME, VERSION]
   end

   def show(msg)
      render(msg)
   end

private
   # models
   def load_profiles
      return unless File.exists? Settings.vim_share

      Dir.entries(Settings.vim_share).each do |entry|
         next if (entry =~ /^\./)

         path = File.join(Settings.vim_share, entry)
         unless File.directory? path
            profile = Profile.load_from_file(path)
            @profiles << profile unless profile.nil?
         end
      end
   end

   def load_current
      if File.exists? Settings.vim_profile
         profile = Profile.load_from_file(Settings.vim_profile)
         @current = profile unless profile.nil?
      end
   end

   # views
   def render(msg)
      puts "\t %s" % [msg]
   end
end

end

if __FILE__ == $0
   app = VimEnvApp::VimEnv.new
   
   if ARGV.size == 0
      app.show_banner
      app.show_current
      app.show_profiles
   else
      app.load_profile(ARGV[0].chomp)
   end
end
