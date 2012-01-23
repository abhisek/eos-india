#ifndef _NETMON_DECODE_H
#define _NETMON_DECODE_H

#include <netmon.h>
#include <packet.h>

#define DEFINE_NETMON_DECODER_FUNC(name) \
   int name(struct packet *packet)

struct netmon_decoder {
   char *name;
   int protonum;
   int (*decoder_init)(void *ptr);
   int (*decoder_deinit)(void *ptr);
   DEFINE_NETMON_DECODER_FUNC((*decoder_func));

   LIST_ENTRY(netmon_decoder) link;
};

#define DECODER_RET_SUCCESS    0x00  /* decode successful */
#define DECODER_RET_CONTINUE   0x01  /* decode must continue */
#define DECODER_RET_FAILED     0x02  /* decode failed */

#endif
