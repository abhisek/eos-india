#include <netmon.h>
#include <decoder.h>
#include <ethernet.h>
#include <arp.h>
#include <netinet/in.h>

static int intialized;

int arp_do_decode(struct packet *packet)
{
   struct arp_header *arp;

   if((packet->l2_data_len + sizeof(*arp)) > (packet->frame_size)) {
      DEBUG_MSG("ARP Decoder: Packet too small");

      return DECODER_RET_FAILED;
   }

   arp = (struct arp_header*) calloc(sizeof(*arp), 1);
   assert(arp != NULL);

   memcpy(arp, packet->frame + packet->l2_data_len, sizeof(*arp));
   arp->arp_op = ntohs(arp->arp_op);

   /*
    * scoped debug stuff
    */
   {
      char *src_ip;
      char *dst_ip;
      char *arpop;
      struct in_addr in1, in2;

      in1.s_addr = *((unsigned long*) arp->src_proto_addr);
      in2.s_addr = *((unsigned long*) arp->dst_proto_addr);

      src_ip = strdup((char*)inet_ntoa(in1));
      dst_ip = strdup((char*)inet_ntoa(in2));

      ARP_OPCODE_TO_STR(arp->arp_op, arpop);
      DEBUG_MSG("ARP Decoder: SRC IP: %s DST IP: %s OP: %s", 
                  src_ip, dst_ip, arpop);

      free(src_ip);
      free(dst_ip);
   }

   packet_lock(packet);
   packet->l3_proto = ETH_P_ARP;
   packet->l3_data = (void*) arp;
   packet->l3_data_len = sizeof(*arp);
   packet_unlock(packet);
   
   return DECODER_RET_SUCCESS;
}

static struct netmon_decoder arp_decoder = {
   .name = "arp_decoder",
   .protonum = ETH_P_ARP,
   .decoder_init = NULL,
   .decoder_deinit = NULL,
   .decoder_func = arp_do_decode,
};

int arp_decoder_init()
{
   DEBUG_MSG("Registering ARP decoder");

   register_decoder(&arp_decoder, 3);
   return 0;
}

void arp_decoder_deinit()
{
   return;
}
