#ifndef _NETMON_ETHERNET_H
#define _NETMON_ETHERNET_H

#include <netmon.h>

/*
 * Mostly copied from net/ethernet.h from glibc
 */

/*
 * These are the defined Ethernet Protocol ID's.
 */

#define ETH_P_LOOP   0x0060      /* Ethernet Loopback packet   */
#define ETH_P_PUP 0x0200      /* Xerox PUP packet     */
#define ETH_P_PUPAT  0x0201      /* Xerox PUP Addr Trans packet   */
#define ETH_P_IP  0x0800      /* Internet Protocol packet   */
#define ETH_P_X25 0x0805      /* CCITT X.25        */
#define ETH_P_ARP 0x0806      /* Address Resolution packet  */
#define  ETH_P_BPQ   0x08FF      /* G8BPQ AX.25 Ethernet Packet   [ NOT AN OFFICIALLY REGISTERED ID ] */
#define ETH_P_IEEEPUP   0x0a00      /* Xerox IEEE802.3 PUP packet */
#define ETH_P_IEEEPUPAT 0x0a01      /* Xerox IEEE802.3 PUP Addr Trans packet */
#define ETH_P_DEC       0x6000          /* DEC Assigned proto           */
#define ETH_P_DNA_DL    0x6001          /* DEC DNA Dump/Load            */
#define ETH_P_DNA_RC    0x6002          /* DEC DNA Remote Console       */
#define ETH_P_DNA_RT    0x6003          /* DEC DNA Routing              */
#define ETH_P_LAT       0x6004          /* DEC LAT                      */
#define ETH_P_DIAG      0x6005          /* DEC Diagnostics              */
#define ETH_P_CUST      0x6006          /* DEC Customer use             */
#define ETH_P_SCA       0x6007          /* DEC Systems Comms Arch       */
#define ETH_P_RARP      0x8035      /* Reverse Addr Res packet */
#define ETH_P_ATALK  0x809B      /* Appletalk DDP     */
#define ETH_P_AARP   0x80F3      /* Appletalk AARP    */
#define ETH_P_8021Q  0x8100          /* 802.1Q VLAN Extended Header  */
#define ETH_P_IPX 0x8137      /* IPX over DIX         */
#define ETH_P_IPV6   0x86DD      /* IPv6 over bluebook      */
#define ETH_P_PAUSE  0x8808      /* IEEE Pause frames. See 802.3 31B */
#define ETH_P_SLOW   0x8809      /* Slow Protocol. See 802.3ad 43B */
#define ETH_P_WCCP   0x883E      /* Web-cache coordination protocol defined in draft-wilson-wrec-wccp-v2-00.txt */
#define ETH_P_PPP_DISC  0x8863      /* PPPoE discovery messages     */
#define ETH_P_PPP_SES   0x8864      /* PPPoE session messages  */
#define ETH_P_MPLS_UC   0x8847      /* MPLS Unicast traffic    */
#define ETH_P_MPLS_MC   0x8848      /* MPLS Multicast traffic  */
#define ETH_P_ATMMPOA   0x884c      /* MultiProtocol Over ATM  */
#define ETH_P_ATMFATE   0x8884      /* Frame-based ATM Transport over Ethernet */
#define ETH_P_AOE 0x88A2      /* ATA over Ethernet    */
#define ETH_P_TIPC   0x88CA      /* TIPC        */

/*
 * IEEE 802.3 Ethernet magic constants.  The frame sizes omit the preamble
 * and FCS/CRC (frame check sequence). 
*/

#define ETH_ALEN  6     /* Octets in one ethernet addr    */
#define ETH_HLEN  14    /* Total octets in header.  */
#define ETH_ZLEN  60    /* Min. octets in frame sans FCS */
#define ETH_DATA_LEN 1500     /* Max. octets in payload   */
#define ETH_FRAME_LEN   1514     /* Max. octets in frame sans FCS */
#define ETH_FCS_LEN  4     /* Octets in the FCS     */

#define  ETHER_ADDR_LEN ETH_ALEN                 /* size of ethernet addr */
#define  ETHER_TYPE_LEN 2                        /* bytes in type field */
#define  ETHER_CRC_LEN  4                        /* bytes in CRC field */
#define  ETHER_HDR_LEN  ETH_HLEN                 /* total octets in header */
#define  ETHER_MIN_LEN  (ETH_ZLEN + ETHER_CRC_LEN) /* min packet length */
#define  ETHER_MAX_LEN  (ETH_FRAME_LEN + ETHER_CRC_LEN) /* max packet length */

#define ETHERNET_PROTONUM_TO_STR(protonum, ptr) \
   do {  \
      switch(protonum) {   \
         case ETH_P_IP: \
            ptr = "IPv4"; \
            break;   \
         case ETH_P_ARP:   \
            ptr = "ARP";   \
            break;   \
         case ETH_P_RARP:  \
            ptr = "Reverse ARP"; \
            break;   \
         case ETH_P_IPV6:  \
            ptr = "IPv6";  \
            break;   \
         default: \
            ptr = "Unknown";  \
            break;   \
      } \
   } while(0)

struct ethernet_header {
   unsigned char source[ETH_ALEN];
   unsigned char dest[ETH_ALEN];
   unsigned short proto;   
} __attribute__((packed));

#endif
