require 'libnet'

l = Libnet.new

100.times do
   l.build_icmpv4_echo(Libnet::ICMP_ECHO, 0, 0, 1, 1, nil, 0, 0)
   l.auto_build_ipv4(Libnet::LIBNET_ICMPV4_ECHO_H + Libnet::LIBNET_IPV4_H, Libnet::IPPROTO_ICMP, "192.168.0.2")
   l.auto_build_ethernet(l.hex_aton("00:11:11:94:D4:F5"), Libnet::ETHERTYPE_IP)
   ret = l.write
   l.clear_packet
   puts l.geterror if ret < 0
end

sent,error,written = l.stats
puts ""
puts "Sent: #{sent}"
puts "Error: #{error}"
puts "Bytes Written: #{written}"
