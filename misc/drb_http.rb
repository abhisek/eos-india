module DRb
   class DRbHttpSocket
      require 'uri'
      require 'net/http'

      #
      # Implements the required class methods which are used by DRbProtocol
      # to associate correct URI with this module
      #
      def self.parse_uri(uri)
         # /^drbhttp:\/\/(.*?):(\d+)(\?(.*))?$/
         # /^drbhttp:\/\/(.*):([0-9]+)\/?(.*)\??(.*)/
         if uri =~ /^drbhttp:\/\/(.*):([0-9]+)\/?([^\?]+)\??(.*)/
            host = $1
            port = $2
            path = $3
            params = $4
            
            unless params.nil? or params.empty?
               params = params.split('&').collect{|v| v.split('=')}.collect{|v| {"#{v[0]}" => "#{v[1]}"}}
               h = Hash.new
               params.each {|v| h.merge!(v)}
               params = h
            end

            [host, port, path, params]
         else
            raise(DRbBadScheme, uri) unless uri =~ /^drbhttp:/
            raise(DRbBadURI, 'can\'t parse uri:' + uri)
         end
      end

      def self.uri_option(uri, config = nil)
         [uri, nil]
      end

      def self.open(uri, config)
         host, port, path, params = DRbHttpSocket.parse_uri(uri)
         DRbHttpSocket.new("drbhttp://#{host}:#{port}/#{path}", params, config)
      end

      def self.open_server
         raise(DRbConnError, 'unimplemented')
      end

      #
      # Implement the DRbHttp instance
      # 
      # Expects config to be a Hash containing different options.
      # Currently uses only config[:post_params] which should be
      # a Hash containing key-value pairs which is sent to the server
      # during POST along with the Marshalled data
      #
      def initialize(url, params, config)
         @url = url
         @params = params
         @msg = DRbMessage.new(config)
         @response_buffer = String.new
         @config = config
      end

      def send_request(ref, msg_id, arg, b)
         @msg.send_request(self, ref, msg_id, arg, b)
      end

      def recv_reply
         @msg.recv_reply(self)
      end

      def alive?
         true
      end

      def close
      end

      def write(str)
         url = ::URI.parse(@url.sub(/drbhttp/, 'http'))
         options = @params.merge(:data => str)
         res = Net::HTTP.post_form(url, options)
         @response_buffer << res.body
         
         str.length
      end

      def read(size)
         @response_buffer.slice!(0, size)
      end
   end

   DRbProtocol.add_protocol(DRbHttpSocket)
end
