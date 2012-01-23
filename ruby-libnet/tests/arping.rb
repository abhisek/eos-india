require 'libnet'
require 'socket'

l = Libnet.new

100.times do
l.auto_build_arp(Libnet::ARPOP_REQUEST, l.gethwaddr, Socket.gethostbyname(l.getip)[-1], "\xff\xff\xff\xff\xff\xff", Socket.gethostbyname("192.168.0.2")[-1])

l.auto_build_ethernet("\xff\xff\xff\xff\xff\xff", Libnet::ETHERTYPE_ARP)
l.write
l.clear_packet
end

sent,error,written = l.stats
puts ""
puts "Sent: #{sent}"
puts "Error: #{error}"
puts "Bytes Written: #{written}"

