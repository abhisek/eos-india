
def bits(n, bits = 32)
   n = n.to_i
   b = bits.to_i
   
   bbs = []
   0.upto(b - 1) do |i|
      bit = (n >> i) & 0x01
      bbs.push(bit)
   end

   return bbs.reverse
end
