/* packet decoder abstraction */
#include <netmon.h>
#include <decoder.h>
#include <packet.h>

#define DECODER_TOWER_SIZE    7  /* OSI Layer */
LIST_HEAD(_netmon_decoder_head, netmon_decoder);
static struct _netmon_decoder_head decoder_tower[DECODER_TOWER_SIZE + 1]; /* 1 ... 7 */

/*
 * after this function, the packet must be fully decoded
 */
void decode_packet(struct packet *p)
{
   struct netmon_decoder *d;
   int ret;
   int break_loop;

   if(p == NULL)
      return;
   
   if((p->state < 1) || (p->state > PACKET_STATE_DECODE_L7))
      return;

   LIST_FOREACH(d, &decoder_tower[p->state], link) {
      /*
       * heuristics
       */
      if(p->state == PACKET_STATE_DECODE_L3) {
         /*
          * in L3 we *always* have protonum from L2 decoder
          */
         if(d->protonum != p->l3_proto)
            continue;
      }
      if(p->state == PACKET_STATE_DECODE_L4) {
         /*
          * in L4 we *always* have protonum from L3 decoder
          */
         if(d->protonum != p->l4_proto)
            continue;
      }

      ret = d->decoder_func(p);
      
      break_loop = 0;
      switch(ret) {
         case DECODER_RET_SUCCESS:
            break_loop = 1;
            break;   /* we are done in this level */

         case DECODER_RET_CONTINUE:
            break;   /* not fully done, lets continue */

         case DECODER_RET_FAILED:
            break;   /* not appropriate decoder */

         default: 
            break;
      }

      if(break_loop)
         break;
   }

   p->state = p->state + 1; /* elevate the state */
   decode_packet(p); /* constant recurse */
}

/*
 * Register a new decoder to the decoder tower
 * d must point to allocated and initialized block
 */
void register_decoder(struct netmon_decoder *d, int layer)
{
   if(d == NULL) {
      DEBUG_MSG("Cannot register NULL decoder");
      return;
   }

   if((layer < 1) || (layer > DECODER_TOWER_SIZE)) {
      DEBUG_MSG("Invalid layer (%d) for decoder registration", layer);
      return;
   }

   LIST_INSERT_HEAD(&decoder_tower[layer], d, link);

   DEBUG_MSG("Decoder %s (%d) registered", d->name, d->protonum);
}

void decoder_init()
{
   int i;

   for(i = 1; i <= DECODER_TOWER_SIZE; i++) {
      LIST_INIT(&decoder_tower[i]);
   }

   /*
    * Hardcoding decoder inits here, need to make it configurable in future
    */
   arp_decoder_init();
   ipv4_decoder_init();

   return;
}

void decoder_deinit()
{
   arp_decoder_deinit();
   ipv4_decoder_deinit();

   return;
}
