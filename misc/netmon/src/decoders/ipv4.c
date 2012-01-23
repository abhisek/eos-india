#include <netmon.h>
#include <decoder.h>
#include <ethernet.h>
#include <ipv4.h>
#include <netinet/in.h>

static int initialize;

int ipv4_do_decode(struct packet *packet)
{
   struct ipv4_header *ip;

   if((packet->l2_data_len + sizeof(*ip)) > (packet->frame_size)) {
      DEBUG_MSG("IPv4 Decoder: Packet too small");

      return DECODER_RET_FAILED;
   }

   ip = (struct ipv4_header*) calloc(sizeof(*ip), 1);
   assert(ip != NULL);

   memcpy(ip, packet->frame + packet->l2_data_len, sizeof(*ip));

   /*
    * scoped debug stuff
    */
   {
      char *src_ip;
      char *dst_ip;
      struct in_addr in1, in2;
      char *tmp;

      in1.s_addr = ip->src_addr;
      in2.s_addr = ip->dst_addr;

      src_ip = strdup((char*)inet_ntoa(in1));
      dst_ip = strdup((char*)inet_ntoa(in2));
      
      IPv4_PROTO_TO_STR(ip->protocol, tmp);
      DEBUG_MSG("IPv4 Decoder: SRC: %s DST: %s PROTO: %s", src_ip, dst_ip, tmp);

      free(src_ip);
      free(dst_ip);
   }

   packet_lock(packet);
   packet->l3_proto = ETH_P_IP;
   packet->l3_data = (void*) ip;
   packet->l3_data_len = sizeof(*ip);
   packet->l4_proto = (unsigned int) ip->protocol;
   packet_unlock(packet);

   return DECODER_RET_SUCCESS;
}

static struct netmon_decoder ipv4_decoder = {
   .name = "ipv4_decoder",
   .protonum = ETH_P_IP,
   .decoder_init = NULL,
   .decoder_deinit = NULL,
   .decoder_func = ipv4_do_decode,
};

int ipv4_decoder_init()
{
   DEBUG_MSG("Registering IPv4 Decoder");

   register_decoder(&ipv4_decoder, 3);
   return 0;
}

void ipv4_decoder_deinit()
{
   return;
}
