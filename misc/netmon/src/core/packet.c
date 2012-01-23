#include <netmon.h>
#include <packet.h>
#include <pcap.h>
#include <pthread.h>
#include <packet.h>
#include <decoder.h>
#include <ethernet.h>

static int initialized;
static pcap_t *cap_handle;
static pthread_t cap_thread;
static LIST_HEAD(_packet_predecoders, packet_hook) packet_predecoders;
static LIST_HEAD(_packet_postdecoders, packet_hook) packet_postdecoders;
static LIST_HEAD(_packet_postanalyzers, packet_hook) packet_postanalyzers;

/* 
 * We are going to do link layer decoding here only since it is
 * machine dependent
 */
static int packet_link_type;
static int (*packet_decode_link_layer)(struct packet *p);

int packet_lock(struct packet *p)
{
   return 0;
}

int packet_unlock(struct packet *p)
{
   return 0;
}

static int packet_decode_link_ether(struct packet *packet)
{
   struct ethernet_header *e;
   char *tmp;

   if(packet->frame_size < sizeof(struct ethernet_header)) {
      DEBUG_MSG("Ethernet Decoder: Packet too small (frame size: %d)", packet->frame_size);

      return DECODER_RET_FAILED;
   }

   e = (struct ethernet_header*) calloc(sizeof(*e), 1);
   assert(e != NULL);

   memcpy(e, packet->frame, sizeof(*e));
   e->proto = ntohs(e->proto);
   
   ETHERNET_PROTONUM_TO_STR(e->proto, tmp);
   DEBUG_MSG("Ethernet Decoder: SRC: %02x:%02x:%02x:%02x:%02x:%02x "
                        "DST: %02x:%02x:%02x:%02x:%02x:%02x "
                        "PROTO: %s", 
                        e->source[0], 
                        e->source[1], 
                        e->source[2], 
                        e->source[3], 
                        e->source[4],
                        e->source[5],
                        e->dest[0],
                        e->dest[1],
                        e->dest[2],
                        e->dest[3],
                        e->dest[4],
                        e->dest[5],
                        tmp);

   packet_lock(packet);
   packet->l2_proto = 0;   /* TODO: Protocol Number for Ethernet */
   packet->l2_data = (void*) e;
   packet->l2_data_len = sizeof(*e);
   packet->l3_proto = e->proto;
   packet_unlock(packet);

   return 0;
}

static void packet_prepare_handle(pcap_t *cap)
{
   packet_link_type = pcap_datalink(cap);

   switch(packet_link_type) {
      case DLT_NULL:
         DEBUG_MSG("Packet Link type: BSD loop back encapsulation");
         packet_decode_link_layer = NULL; /* not supported */
         break;

      case DLT_EN10MB:
         DEBUG_MSG("Packet Link type: Ethernet (10Mb, 100Mb, 1000Mb and up)");
         packet_decode_link_layer = packet_decode_link_ether;
         break;

      case DLT_IEEE802:
         DEBUG_MSG("Packet Link type: IEEE 802.5 Token Ring");
         packet_decode_link_layer = NULL; /* not supported */
         break;

      case DLT_LINUX_SLL:
         DEBUG_MSG("Packet Link type: Linux cooked sockets");
         packet_decode_link_layer = packet_decode_link_ether;
         break;

      default:
         DEBUG_MSG("Packet Link type: %d", packet_link_type);
         packet_decode_link_layer = NULL; /* not supported */
         break;
   }
}

/*
 * Hook callbacks
 */
static int packet_call_predecode_hooks(struct packet *p)
{
   return 0;
}

static int packet_call_postdecode_hooks(struct packet *p)
{
   return 0;
}

static int packet_call_postanalysis_hooks(struct packet *p)
{
   return 0;
}

/*
 * The looper thread responsible for call-back of decoders
 * and analysis plugins.
 *
 * Entire packet life cycle is handled by this function
 */
static int __attribute__((noreturn)) packet_looper(void *args)
{
   char errbuf[PCAP_ERRBUF_SIZE];
   struct packet *p;
   struct pcap_pkthdr *phdr;
   unsigned char *pdata;
   int ret;

   DEBUG_MSG("Preparing pcap and entering packet capture loop");
   
   /*
   ret = pcap_setnonblock(cap_handle, 1, errbuf);
   if(ret == -1) {
      DEBUG_MSG("pcap: %s", errbuf);
      abort();
   }
   */

   for(;;) {
      ret = pcap_next_ex(cap_handle, &phdr, &pdata);
      
      if(phdr->len != phdr->caplen)
         DEBUG_MSG("Packet length and Capture length mismatch (caplen=%d len=%d)",
                                                            phdr->caplen,
                                                            phdr->len);

      switch(ret) {
         case 1:  /* We got a packet from pcap */
            /* we got a frame, lets process it */
            p = (struct packet*) calloc(sizeof(*p), 1);
            assert(p != NULL);

            p->frame = calloc(phdr->caplen, 1);
            assert(p->frame != NULL);
            memcpy(p->frame, pdata, phdr->caplen);

            p->frame_size = phdr->caplen;
            p->state = PACKET_STATE_DECODE_L1;

            /* pre-decode hooks */
            packet_call_predecode_hooks(p);

            /* decode L1 */
            if(packet_decode_link_layer == NULL) {
               DEBUG_MSG("No link layer decoder found for packet, skipping packet");

               free(p->frame);
               free(p);

               break;
            }
            ret = packet_decode_link_layer(p);

            if(ret != DECODER_RET_SUCCESS) {
               DEBUG_MSG("Link Layer decoding failed, skipping packet");

               free(p->frame);
               free(p);

               break;
            }

            decode_packet(p);

            /* post decode hooks */
            packet_call_postdecode_hooks(p);

            //analyze_packet(p);

            /* post analysis hooks */
            packet_call_postanalysis_hooks(p);

            free(p->frame);
            
            if(p->l1_data)
               free(p->l1_data);

            if(p->l2_data)
               free(p->l2_data);

            if(p->l3_data)
               free(p->l3_data);

            free(p);

            break;

         case 0:
            /* timeout occurred during a live capture */
            break;

         default:
            DEBUG_MSG("Error occurred while reading from pcap_next_ex (ret=%d)", ret);
            break;
      }
   }
}

/* 
 * packet hook routines
 */

int packet_add_predecode_hook(struct packet_hook *p)
{
   return 0;
}

int packet_del_predecode_hook(struct packet_hook *p)
{
   return 0;
}

int packet_add_postdecode_hook(struct packet_hook *p)
{
   return 0;
}

int packet_del_postdecode_hook(struct packet_hook *p)
{
   return 0;
}

int packet_add_postanalysis_hook(struct packet_hook *p)
{
   return 0;
}

int packet_del_postanalysis_hook(struct packet_hook *p)
{
   return 0;
}

/*
 * generic routine for modules to check for packet capture
 * subsystem initialization
 * 
 * return
 *    1: initialized
 *    0: not initialized
 */
int packet_initialized()
{
   return initialized;
}

void packet_init()
{
   const char *dev;
   int ret;
   char errbuf[PCAP_ERRBUF_SIZE];

   if(initialized) {
      DEBUG_MSG("Packet subsystem already initialized");
      return;
   }

   memset(errbuf, '\0', sizeof(errbuf));
   ret = conf_get_opt("interface", &dev);

   if(ret == -1)
      dev = NULL;

   cap_handle = pcap_open_live(dev, 65535, 1, -1, errbuf);
   assert(cap_handle != (pcap_t*) NULL);

   if(strlen(errbuf) > 0)
      DEBUG_MSG("pcap: %s", errbuf);

   /* basic pre init stuff */
   packet_prepare_handle(cap_handle);

   /* initialize the packet hook lists */
   LIST_INIT(&packet_predecoders);
   LIST_INIT(&packet_postdecoders);
   LIST_INIT(&packet_postanalyzers);

   DEBUG_MSG("Starting packet capture and dispatcher thread");
   ret = pthread_create(&cap_thread, NULL, (void*) &packet_looper, NULL);
   assert(ret == 0);

   initialized = 1;
   DEBUG_MSG("Packet capture initialized");
}

void packet_deinit()
{

}
