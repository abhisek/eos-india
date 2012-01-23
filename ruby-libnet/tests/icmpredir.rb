require 'libnet'
require 'socket'

l = Libnet.new

10.times do 
   
   l.build_icmpv4_redirect(Libnet::ICMP_REDIRECT, 2, 0, "192.168.0.30", nil, 0, 0)
   l.auto_build_ipv4(Libnet::LIBNET_ICMPV4_REDIRECT_H + Libnet::LIBNET_IPV4_H, Libnet::IPPROTO_ICMP, "192.168.0.255")
   l.auto_build_ethernet(l.hex_aton("FF:FF:FF:FF:FF:FF"), Libnet::ETHERTYPE_IP)

   l.write
   l.clear_packet

end

sent,error,written = l.stats
puts ""
puts "Sent: #{sent}"
puts "Error: #{error}"
puts "Bytes Written: #{written}"

