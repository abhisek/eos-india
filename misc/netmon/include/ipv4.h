#ifndef _NETMON_IPV4_H
#define _NETMON_IPV4_H

#include <netmon.h>
#include <bits/endian.h>

/*
 * mostly ripped from glibc
 */

struct ipv4_header {
#if __BYTE_ORDER == __LITTLE_ENDIAN
   unsigned int ihl:4;
   unsigned int version:4;
#elif __BYTE_ORDER == __BIG_ENDIAN
   unsigned int version:4;
   unsigned int ihl:4;
#else
# error  "Please fix <bits/endian.h>"
#endif
   unsigned char tos;
   unsigned short total_len;
   unsigned short id;
   unsigned short frag_off;
   unsigned char ttl;
   unsigned char protocol;
   unsigned short check;
   unsigned int src_addr;
   unsigned int dst_addr;
};

#define  IPVERSION   4               /* IP version number */
#define  IP_MAXPACKET   65535    /* maximum packet size */

/*
 *  * Definitions for IP type of service (ip_tos)
 *   */
#define  IPTOS_TOS_MASK    0x1E
#define  IPTOS_TOS(tos)    ((tos) & IPTOS_TOS_MASK)
#define  IPTOS_LOWDELAY    0x10
#define  IPTOS_THROUGHPUT  0x08
#define  IPTOS_RELIABILITY 0x04
#define  IPTOS_LOWCOST     0x02
#define  IPTOS_MINCOST     IPTOS_LOWCOST

/*
 *  * Definitions for IP precedence (also in ip_tos) (hopefully unused)
 *   */
#define  IPTOS_PREC_MASK         0xe0
#define  IPTOS_PREC(tos)                ((tos) & IPTOS_PREC_MASK)
#define  IPTOS_PREC_NETCONTROL      0xe0
#define  IPTOS_PREC_INTERNETCONTROL 0xc0
#define  IPTOS_PREC_CRITIC_ECP      0xa0
#define  IPTOS_PREC_FLASHOVERRIDE   0x80
#define  IPTOS_PREC_FLASH     0x60
#define  IPTOS_PREC_IMMEDIATE    0x40
#define  IPTOS_PREC_PRIORITY     0x20
#define  IPTOS_PREC_ROUTINE      0x00

/*
 *  * Definitions for options.
 *   */
#define  IPOPT_COPY     0x80
#define  IPOPT_CLASS_MASK  0x60
#define  IPOPT_NUMBER_MASK 0x1f

#define  IPOPT_COPIED(o)      ((o) & IPOPT_COPY)
#define  IPOPT_CLASS(o)    ((o) & IPOPT_CLASS_MASK)
#define  IPOPT_NUMBER(o)      ((o) & IPOPT_NUMBER_MASK)

#define  IPOPT_CONTROL     0x00
#define  IPOPT_RESERVED1      0x20
#define  IPOPT_DEBMEAS     0x40
#define  IPOPT_MEASUREMENT       IPOPT_DEBMEAS
#define  IPOPT_RESERVED2      0x60

#define  IPOPT_EOL      0     /* end of option list */
#define  IPOPT_END      IPOPT_EOL
#define  IPOPT_NOP      1     /* no operation */
#define  IPOPT_NOOP     IPOPT_NOP

#define  IPOPT_RR    7     /* record packet route */
#define  IPOPT_TS    68    /* timestamp */
#define  IPOPT_TIMESTAMP      IPOPT_TS
#define  IPOPT_SECURITY    130      /* provide s,c,h,tcc */
#define  IPOPT_SEC      IPOPT_SECURITY
#define  IPOPT_LSRR     131      /* loose source route */
#define  IPOPT_SATID    136      /* satnet id */
#define  IPOPT_SID      IPOPT_SATID
#define  IPOPT_SSRR     137      /* strict source route */
#define  IPOPT_RA    148      /* router alert */

/*
 *  * Offsets to fields in options other than EOL and NOP.
 *   */
#define  IPOPT_OPTVAL      0     /* option ID */
#define  IPOPT_OLEN     1     /* option length */
#define  IPOPT_OFFSET      2     /* offset within option */
#define  IPOPT_MINOFF      4     /* min value of above */

#define  MAX_IPOPTLEN      40

/* flag bits for ipt_flg */
#define  IPOPT_TS_TSONLY      0     /* timestamps only */
#define  IPOPT_TS_TSANDADDR   1     /* timestamps and addresses */
#define  IPOPT_TS_PRESPEC  3     /* specified modules only */

/* bits for security (not byte swapped) */
#define  IPOPT_SECUR_UNCLASS  0x0000
#define  IPOPT_SECUR_CONFID   0xf135
#define  IPOPT_SECUR_EFTO  0x789a
#define  IPOPT_SECUR_MMMM  0xbc4d
#define  IPOPT_SECUR_RESTR 0xaf13
#define  IPOPT_SECUR_SECRET   0xd788
#define  IPOPT_SECUR_TOPSECRET   0x6bc5

/*
 *  * Internet implementation parameters.
 *   */
#define  MAXTTL      255      /* maximum time to live (seconds) */
#define  IPDEFTTL 64    /* default ttl, from RFC 1340 */
#define  IPFRAGTTL   60    /* time to live for frags, slowhz */
#define  IPTTLDEC 1     /* subtracted when forwarding */

#define  IP_MSS      576      /* default maximum segment size */

#define IPv4_PROTO_TO_STR(proto, ptr)  \
   do {  \
      switch (proto) {  \
         case IPPROTO_ICMP:   \
            ptr = "ICMP";  \
            break;   \
         case IPPROTO_UDP: \
            ptr = "UDP";   \
            break;   \
         case IPPROTO_TCP: \
            ptr = "TCP";   \
            break;   \
         default: \
            ptr = "Unknown";  \
            break;   \
      }  \
   } while(0)

#endif
