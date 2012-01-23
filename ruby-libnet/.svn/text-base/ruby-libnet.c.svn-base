/* Ruby Binding for Libnet libary
 * -------------------------------
 *
 *  libnet homepage: http://packetfactory.net/libnet/
 *
 * --- 
 * * (28/12/2006) *
 *                * added build_icmpv4_* methods
 * * (27/12/2006) *
 *                * added build_tcp
 *                * added build_tcp_options
 *                * added build_icmpv4_echo
 *                * added test for icmp ping
 * * (26/12/2006) * 
 *                * added auto_build_ethernet
 *                * added auto_build_arp
 *                * added build_ipv4
 *                * added auto_build_ipv4
 *                * initial testing
 * * (20/10/2006) * added arp packet building support
 * * (11/10/2006) * Started development
 *
 */
#include <ruby.h>
#include <libnet.h>
#include "ruby-libnet-defs.h"

/*
 * debugging macros
 */
#define DEBUG_PRINT(x, ...)                                                   \
     ((RTEST(ruby_debug) && RTEST(ruby_verbose))?                            \
     (fprintf(stderr, "libnet: "x"\n", ##__VA_ARGS__),fflush(stderr)) : 0)
/*
 * Main libnet class handle
 */
VALUE cLibnet;

/*
 * initialize method for Libnet class
 * returns a new libnet handle
 *
 * NOTE:
 *    Most libnet proto-header definitions which are defined
 *    in libet-headers.h in libnet source is available
 *    as constants inside the Libnet class.
 *
 * * @param1 (optional) Injection type (default: Libnet::LIBNET_LINK)
 * * @param2 (options) device (default: NULL, libnet autodetects)
 *
 *   For example to access LIBNET_BGP4_HEADER_H
 *   just do Libnet::LIBNET_BGP4_HEADER_H
 * 
 */
static VALUE rb_libnet_init(int argc, VALUE *argv, VALUE self)
{
   libnet_t *lctx;
   char errbuf[LIBNET_ERRBUF_SIZE];
   VALUE inj_type = INT2FIX(LIBNET_LINK);
   VALUE dev = Qnil;
   VALUE wrap;
   
   if(argc > 0)
      rb_scan_args(argc, argv, "02", &inj_type, &dev);

   DEBUG_PRINT("initialize(type = %d, dev = %s)", 
         NUM2INT(inj_type), 
         dev == Qnil ? "NULL" : StringValuePtr(dev));

   lctx = libnet_init(NUM2INT(inj_type), dev == Qnil ? NULL : StringValuePtr(dev), errbuf);
   
   if(lctx == NULL) {
      rb_raise(rb_eRuntimeError, errbuf);
      return Qnil;
   }

   wrap = Data_Wrap_Struct(rb_cObject, NULL, NULL, lctx);
   rb_iv_set(self, "@handle", wrap);

   return self;
}

/*
 * destroy the libnet handle and close it
 *
 * SECURITY NOTE:
 *    If libnet_handle#deinit called twice then double free condition will occur.
 *    I cant find a way to handle this here since I am treating libnet handle as a
 *    data object for wrapping it inside the class. libnet authors should have avoided
 *    dangling pointers after a call to libnet_destroy()
 */

static VALUE rb_libnet_free(VALUE self)
{
   libnet_t *lctx = NULL;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   DEBUG_PRINT("deinit()");

   if(lctx)
      libnet_destroy(lctx);

   return Qnil;
}

/*
 * clears the libnet pblocks from the libnet context
 */

static VALUE rb_libnet_clear_packet(VALUE self)
{
   libnet_t *lctx = NULL;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   DEBUG_PRINT("libnet_clear_packet");

   if(lctx)
      libnet_clear_packet(lctx);
   
   return Qnil;
}

/*
 * returns the canonical name of the device used for
 * packet injection
 */

static VALUE rb_libnet_getdevice(VALUE self)
{
   libnet_t *lctx = NULL;
   char *dev;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   DEBUG_PRINT("libnet_getdevice()");

   dev = libnet_getdevice(lctx);

   return (dev == NULL ? Qnil : rb_str_new2(dev));
}

/*
 * returns the IP address of the interfaced used for
 * packet injection
 */
static VALUE rb_libnet_getip(VALUE self)
{
   libnet_t *lctx = NULL;
   unsigned long addr;
   char *iptxt;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   addr = libnet_get_ipaddr4(lctx);
   iptxt = libnet_addr2name4(addr, LIBNET_DONT_RESOLVE);

   return (iptxt ? rb_str_new2(iptxt) : Qnil);
}

/*
 * Returns the MAC address for the device libnet was initialized with. If
 * libnet was initialized without a device the function will attempt to find
 * one. If the function fails and returns NULL a call to libnet_geterror() will
 * tell you why.
 *
 * * @param l pointer to a libnet context
 * * @return a pointer to the MAC address or NULL
*/
static VALUE rb_libnet_gethwaddr(VALUE self)
{
   libnet_t *lctx = NULL;
   struct libnet_ether_addr *eth = NULL;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   if(lctx)
      eth = libnet_get_hwaddr(lctx);

   if(eth)
      return rb_str_new(eth->ether_addr_octet, 6);

   return Qnil;
}

/*
 * returns the last error set inside libnet
 */

static VALUE rb_libnet_geterror(VALUE self)
{
   libnet_t *lctx = NULL;
   char *err;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   DEBUG_PRINT("libnet_geterror()");

   err = libnet_geterror(lctx);

   return (err == NULL ? Qnil : rb_str_new2(err));
}

/*
 * builds a libnet header and updates the internal libnet handle
 *
 * returns the ptag for the libnet pblock used
 *
 * NOTE:
 * 
 *     *  Libnet ptags are how we identify specific protocol blocks inside the
 *     *  list.
 *  
 *     *  typedef int32_t libnet_ptag_t;
 *     *  #define LIBNET_PTAG_INITIALIZER         0
 *
 */

static VALUE rb_libnet_build_ethernet(VALUE self,
                              VALUE dst,
                              VALUE src,
                              VALUE type,
                              VALUE payload,
                              VALUE payload_size,
                              VALUE ptag)
{
   libnet_ptag_t tag;
   libnet_t *lctx = NULL;

   Data_Get_Struct(
            rb_iv_get(self, "@handle"),
            libnet_t,
            lctx);

   DEBUG_PRINT("libnet_build_ethernet()");

   tag = libnet_build_ethernet(
            StringValuePtr(dst),
            StringValuePtr(src),
            (u_int16_t)NUM2INT(type),
            (payload == Qnil ? NULL : StringValuePtr(payload)),
            (payload_size == Qnil ? 0 : NUM2INT(payload_size)),
            lctx,
            (libnet_ptag_t)(ptag == Qnil ? 0 : NUM2INT(ptag))
         );


   return INT2FIX(tag);   /* -1 would cause a bug? */
}

/*
 * builds a libnet header automatically and updates the 
 * internal libnet handle
 *
 * returns the libnet ptag for pblock used
 *
 * NOTE:
 * 
 *     *  Libnet ptags are how we identify specific protocol blocks inside the
 *     *  list.
 *  
 *     *  typedef int32_t libnet_ptag_t;
 *     *  #define LIBNET_PTAG_INITIALIZER         0
 *
 */
static VALUE rb_libnet_autobuild_ethernet(VALUE self,
                                 VALUE dst,
                                 VALUE type)
{
   libnet_ptag_t tag = -1;
   libnet_t *lctx = NULL;

   Data_Get_Struct(
            rb_iv_get(self, "@handle"),
            libnet_t,
            lctx);

   DEBUG_PRINT("libnet_autobuild_ethernet()");

   tag = libnet_autobuild_ethernet(
            StringValuePtr(dst),
            (u_int16_t)NUM2INT(type),
            lctx);

   return INT2FIX(tag);
}

/*
 * Takes a colon separated hexidecimal address (from the command line) and
 * returns a bytestring suitable for use in a libnet_build function.
 *
 * * @param s the string to be parsed
 * * @return a byte string or NULL on failure
 *
 */
static VALUE rb_libnet_hex_aton(VALUE self, VALUE s)
{
   char *ptr;
   int len;
   VALUE rs;

   ptr = libnet_hex_aton(RSTRING(s)->ptr, &len);

   if(ptr == NULL)
      return Qnil;

   rs = rb_str_new(ptr, len);
   free(ptr);
   
   return rs;
}

/*
 * Takes a binary string representing a MAC address and returns a human readable
 * colon seperated MAC address string.
 *
 * * @params s the binary string
 *
 * * @return a colon seperated MAC string
 */
static VALUE rb_libnet_hex_ntoa(VALUE self, VALUE s)
{
   unsigned char *p;
   char buf[32];

   if(RSTRING(s)->ptr == NULL || RSTRING(s)->len != 6)
      return Qnil;


   p = StringValuePtr(s);
   sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x", p[0],p[1],p[2],p[3],p[4],p[5]);

   return rb_str_new2(buf);
}

/*
 * * @param hrd hardware address format   (int)
 * * @param pro protocol address format   (int)
 * * @param hln hardware address length   (int)
 * * @param pln protocol address length   (int)
 * * @param op ARP operation type         (int)
 * * @param sha sender's hardware address (binary string)
 * * @param spa sender's protocol address (binary string) (network order)
 * * @param tha target hardware address   (binary string)
 * * @param tpa targer protocol address   (binary string) (network order)
 * * @param payload optional payload or NULL (string)
 * * @param payload_s payload length or 0 (int)
 * * @param ptag protocol tag to modify an existing header, 0 to build a new one
 * * @return protocol tag value on success, -1 on error
 *
 */ 

static VALUE rb_libnet_build_arp(VALUE self,
                              VALUE hw_t,
                              VALUE proto_t,
                              VALUE hw_size,
                              VALUE proto_size,
                              VALUE arp_op,
                              VALUE src_haddr,
                              VALUE src_paddr,
                              VALUE dst_haddr,
                              VALUE dst_paddr,
                              VALUE payload,
                              VALUE payload_s,
                              VALUE tag)
{
   libnet_ptag_t p = -1;
   libnet_t *lctx = NULL;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   DEBUG_PRINT("libnet_build_arp()");

   if(lctx)
      p = libnet_build_arp(
                  (u_int16_t)NUM2INT(hw_t),
                  (u_int16_t)NUM2INT(proto_t),
                  (u_int8_t)NUM2INT(hw_size),
                  (u_int8_t)NUM2INT(proto_size),
                  (u_int16_t)NUM2INT(arp_op),
                  StringValuePtr(src_haddr),
                  StringValuePtr(src_paddr),
                  StringValuePtr(dst_haddr),
                  StringValuePtr(dst_paddr),
                  (payload == Qnil ? NULL : StringValuePtr(payload)),
                  (payload_s == Qnil ? 0 : NUM2INT(payload_s)),
                  lctx,
                  (libnet_ptag_t)(tag == Qnil ? 0 : NUM2INT(tag))
         );

   return INT2FIX(p);
}

/*
 * Autouilds an Address Resolution Protocol (ARP) header.  Depending on the op
 * value, the function builds one of several different types of RFC 826 or
 * RFC 903 RARP packets.
 * * @param op ARP operation type
 * * @param sha sender's hardware address (binary string)
 * * @param spa sender's protocol address (binary string) (network order)
 * * @param tha target hardware address   (binary string)
 * * @param tpa targer protocol address   (binary string) (network order)
 * * @return protocol tag value on success, -1 on error
 */
static VALUE rb_libnet_autobuild_arp(VALUE self, 
                              VALUE op, 
                              VALUE sha, 
                              VALUE spa,
                              VALUE tha,
                              VALUE tpa)
{
   libnet_ptag_t p = -1;
   libnet_t *lctx = NULL;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   DEBUG_PRINT("libnet_autobuild_arp()");

   if(lctx)
      p = libnet_autobuild_arp(NUM2INT(op),
                           StringValuePtr(sha),
                           StringValuePtr(spa),
                           StringValuePtr(tha),
                           StringValuePtr(tpa),
                           lctx);

   return INT2FIX(p);                          
}

/**
 * Builds a version 4 RFC 791 Internet Protocol (IP) header.
 * 
 * * @param len total length of the IP packet including all subsequent data
 * * @param tos type of service bits
 * * @param id IP identification number
 * * @param frag fragmentation bits and offset
 * * @param ttl time to live in the network
 * * @param prot upper layer protocol
 * * @param sum checksum (0 for libnet to autofill)
 * * @param src source IPv4 address (string, eg. "192.168.0.1")
 * * @param dst destination IPv4 address (string, eg. "192.168.0.2")
 * * @param payload optional payload or NULL
 * * @param payload_s payload length or 0
 * * @param ptag protocol tag to modify an existing header, 0 to build a new one
 *
 * * @return protocol tag value on success, -1 on error
*/

static VALUE rb_libnet_build_ipv4(VALUE self,
                                  VALUE len,
                                  VALUE tos,
                                  VALUE id,
                                  VALUE frag,
                                  VALUE ttl,
                                  VALUE proto,
                                  VALUE csum,
                                  VALUE src,
                                  VALUE dst,
                                  VALUE payload,
                                  VALUE payload_s,
                                  VALUE tag)
{
   libnet_ptag_t p = -1;
   libnet_t *lctx = NULL;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   DEBUG_PRINT("libnet_build_ipv4()");

   if(lctx) 
      p = libnet_build_ipv4(
                           (u_int16_t)NUM2INT(len),
                           (u_int8_t)NUM2INT(tos),
                           (u_int16_t)NUM2INT(id),
                           (u_int16_t)NUM2INT(frag),
                           (u_int8_t)NUM2INT(ttl),
                           (u_int8_t)NUM2INT(proto),
                           (u_int16_t)NUM2INT(csum),
                           inet_addr(StringValuePtr(src)),
                           inet_addr(StringValuePtr(dst)),
                           (payload == Qnil ? NULL : StringValuePtr(payload)),
                           (payload_s == Qnil ? 0 : NUM2INT(payload_s)),
                           lctx,
                           (libnet_ptag_t)(tag == Qnil ? 0 : NUM2INT(tag))
                           );
                              
   return INT2FIX(p);
}
/*
 * Builds an version 4 Internet Protocol (IP) options header. The function
 * expects options to be a valid IP options string of size options_s, no larger
 * than 40 bytes (the maximum size of an options string). The function checks
 * to make sure that the preceding header is an IPv4 header and that the
 * options string would not result in a packet larger than 65,535 bytes
 * (IPMAXPACKET). The function counts up the number of 32-bit words in the
 * options string and adjusts the IP header length value as necessary.
 *
 * * @param options byte string of IP options
 * * @param options_s length of options string
 * * @param ptag protocol tag to modify an existing header, 0 to build a new one
 * * @return protocol tag value on success, -1 on error
 *
 */
static VALUE rb_libnet_build_ipv4_options(VALUE self,
                                          VALUE options,
                                          VALUE options_s,
                                          VALUE tag)
{
   libnet_ptag_t p = -1;
   libnet_t *lctx = NULL;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   DEBUG_PRINT("libnet_build_ipv4_options()");

   if(lctx)
      p = libnet_build_ipv4_options(
                                   (options == Qnil ? NULL : StringValuePtr(options)),
                                   (u_int32_t)NUM2INT(options),
                                   lctx,
                                   (libnet_ptag_t)(tag == Qnil ? 0 : NUM2INT(tag))
                                   );
   
   return INT2FIX(p);
}

/*
 * Autobuilds a version 4 Internet Protocol (IP) header. The function is useful  * to build an IP header quickly when you do not need a granular level of
 * control. The function takes the same len, prot, and dst arguments as
 * libnet_build_ipv4(). The function does not accept a ptag argument, but it
 * does return a ptag. In other words, you can use it to build a new IP header
 * but not to modify an existing one.
 *
 * * @param len total length of the IP packet including all subsequent data
 * * @param prot upper layer protocol
 * * @param dst destination IPv4 address (string dotted decimal)
 * * @return protocol tag value on success, -1 on error
 *
 */
static VALUE rb_libnet_autobuild_ipv4(VALUE self, VALUE len, VALUE prot, VALUE dst)
{
   libnet_ptag_t p = -1;
   libnet_t *lctx = NULL;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   DEBUG_PRINT("libnet_autobuild_ipv4()");
   
   if(lctx)
      p = libnet_autobuild_ipv4(NUM2INT(len),
                                 NUM2INT(prot),
                                 inet_addr(StringValuePtr(dst)),
                                 lctx);

   return INT2FIX(p);
}

/*
 * * @param sp source port
 * * @param dp destination port
 * * @param seq sequence number
 * * @param ack acknowledgement number
 * * @param control control flags
 * * @param win window size
 * * @param sum checksum (0 for libnet to autofill)
 * * @param urg urgent pointer
 * * @parama len total length of the TCP packet (for checksum calculation)
 * * @param payload_s payload length or 0
 * * @param ptag protocol tag to modify an existing header, 0 to build a new one
 * * @return protocol tag value on success, -1 on error
 *
 */
static VALUE rb_libnet_build_tcp(VALUE self,
                                 VALUE sp,
                                 VALUE dp,
                                 VALUE seq,
                                 VALUE ack,
                                 VALUE control,
                                 VALUE win,
                                 VALUE csum,
                                 VALUE urg,
                                 VALUE len,
                                 VALUE payload,
                                 VALUE payload_s,
                                 VALUE tag)
{
   libnet_ptag_t p = -1;
   libnet_t *lctx = NULL;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   DEBUG_PRINT("libnet_build_tcp()");
   
   if(lctx)
      p = libnet_build_tcp(
                          (u_int16_t)NUM2INT(sp),
                          (u_int16_t)NUM2INT(dp),
                          (u_int32_t)NUM2INT(seq),
                          (u_int32_t)NUM2INT(ack),
                          (u_int8_t)NUM2INT(control),
                          (u_int16_t)NUM2INT(win),
                          (u_int16_t)NUM2INT(csum),
                          (u_int16_t)NUM2INT(urg),
                          (u_int16_t)NUM2INT(len),
                          (payload == Qnil ? NULL : StringValuePtr(payload)),
                          (u_int32_t)NUM2INT(payload_s),
                          lctx,
                          (libnet_ptag_t)(tag == Qnil ? 0 : NUM2INT(tag))
                          );

   return INT2FIX(p);
}

/*
 * Builds an RFC 793 Transmission Control Protocol (TCP) options header.
  * The function expects options to be a valid TCP options string of size
  * options_s, which is no larger than 40 bytes (the maximum size of an
  * options string). The function checks to ensure that the packet consists of
  * a TCP header preceded by an IPv4 header, and that the addition of the
  * options string would not result in a packet larger than 65,535 bytes
  * (IPMAXPACKET). The function counts up the number of 32-bit words in the
  * options string and adjusts the TCP header length value as necessary.
  * 
  * * @param options byte string of TCP options
  * * @param options_s length of options string
  * * @param ptag protocol tag to modify an existing header, 0 to build a new one
  * * @return protocol tag value on success, -1 on error
  *
 */
static VALUE rb_libnet_build_tcp_options(VALUE self,
                                          VALUE options,
                                          VALUE options_s,
                                          VALUE tag
                                          )
{
   libnet_ptag_t p = -1;
   libnet_t *lctx = NULL;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   DEBUG_PRINT("libnet_build_tcp_options()");
   
   if(lctx)
      p = libnet_build_tcp_options(StringValuePtr(options),
                                    NUM2INT(options_s),
                                    lctx,
                                    (libnet_ptag_t)(tag == Qnil ? 0 : NUM2INT(tag))
                                   );

   return INT2FIX(p);
}
/*
  * Builds an RFC 768 User Datagram Protocol (UDP) header.
  * 
  * * @param sp source port
  * * @param dp destination port
  * * @param len total length of the UDP packet
  * * @param sum checksum (0 for libnet to autofill)
  * * @param payload optional payload or NULL
  * * @param payload_s payload length or 0
  * * @param ptag protocol tag to modify an existing header, 0 to build a new one
  * * @return protocol tag value on success, -1 on error
  *
 */
static VALUE rb_libnet_build_udp(VALUE self,
                                 VALUE sp,
                                 VALUE dp,
                                 VALUE len,
                                 VALUE csum,
                                 VALUE payload,
                                 VALUE payload_s,
                                 VALUE tag)
{
   libnet_ptag_t p = -1;
   libnet_t *lctx = NULL;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   DEBUG_PRINT("libnet_build_udp()");
   
   if(lctx)
      p = libnet_build_udp(
                           (u_int16_t)NUM2INT(sp),
                           (u_int16_t)NUM2INT(dp),
                           (u_int16_t)NUM2INT(len),
                           (u_int16_t)NUM2INT(csum),
                           (payload == Qnil ? NULL : StringValuePtr(payload)),
                           (payload_s == Qnil ? 0 : NUM2INT(payload_s)),
                           lctx,
                           (libnet_ptag_t)(tag == Qnil ? 0 : NUM2INT(tag))
                          );

   return INT2FIX(p);                          
}

/*
 * Builds an IP version 4 RFC 792 Internet Control Message Protocol (ICMP)
 * echo request/reply header
 * 
 * * @param type type of ICMP packet (should be ICMP_ECHOREPLY or ICMP_ECHO)
 * * @param code code of ICMP packet (should be 0)
 * * @param sum checksum (0 for libnet to autofill)
 * * @param id identification number
 * * @param seq packet sequence number
 * * @param payload optional payload or NULL
 * * @param payload_s payload length or 0
 * * @param ptag protocol tag to modify an existing header, 0 to build a new one
 * * @return protocol tag value on success, -1 on error
 *
 */
static VALUE rb_libnet_build_icmpv4_echo(VALUE self,
                                          VALUE type,
                                          VALUE code,
                                          VALUE csum,
                                          VALUE id,
                                          VALUE seq,
                                          VALUE payload,
                                          VALUE payload_s,
                                          VALUE tag)
{
   libnet_ptag_t p = -1;
   libnet_t *lctx = NULL;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   DEBUG_PRINT("libnet_build_icmpv4_echo()");

   if(lctx)
      p = libnet_build_icmpv4_echo(
                                    (u_int8_t)NUM2INT(type),
                                    (u_int8_t)NUM2INT(code),
                                    (u_int16_t)NUM2INT(csum),
                                    (u_int16_t)NUM2INT(id),
                                    (u_int16_t)NUM2INT(seq),
                                    (payload == Qnil ? NULL : StringValuePtr(payload)),
                                    (payload_s == Qnil ? 0 : NUM2INT(payload_s)),
                                    lctx,
                                    (libnet_ptag_t)(tag == Qnil ? 0 : NUM2INT(tag))
                                  );

   return INT2FIX(p);                                  
}

/*
 ** Builds an IP version 4 RFC 792 Internet Control Message Protocol (ICMP)
  * IP netmask request/reply header.
  * 
  * * @param type type of ICMP packet (should be ICMP_MASKREQ or ICMP_MASKREPLY)
  * * @param code code of ICMP packet (should be 0)
  * * @param sum checksum (0 for libnet to autofill)
  * * @param id identification number
  * * @param seq packet sequence number
  * * @param mask subnet mask (dotted decimal string)
  * * @param payload optional payload or NULL
  * * @param payload_s payload length or 0
  * * @param ptag protocol tag to modify an existing header, 0 to build a new one
  * * @return protocol tag value on success, -1 on error
  *
 */
static VALUE rb_libnet_build_icmpv4_mask(VALUE self,
                                          VALUE type,
                                          VALUE code,
                                          VALUE csum,
                                          VALUE id,
                                          VALUE seq,
                                          VALUE mask,
                                          VALUE payload,
                                          VALUE payload_s,
                                          VALUE tag)
{
   libnet_ptag_t p = -1;
   libnet_t *lctx = NULL;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   DEBUG_PRINT("libnet_build_icmpv4_mask()");

   if(lctx)
      p = libnet_build_icmpv4_mask(
                                    (u_int8_t)NUM2INT(type),
                                    (u_int8_t)NUM2INT(code),
                                    (u_int16_t)NUM2INT(csum),
                                    (u_int16_t)NUM2INT(id),
                                    (u_int16_t)NUM2INT(seq),
                                    (u_int32_t)(mask == Qnil ? 0 : inet_addr(StringValuePtr(mask))),
                                    (payload == Qnil ? NULL : StringValuePtr(payload)),
                                    (payload_s == Qnil ? 0 : NUM2INT(payload_s)),
                                    lctx,
                                    (libnet_ptag_t)(tag == Qnil ? 0 : NUM2INT(tag))
                                  );

   return INT2FIX(p);                                  
}
/*
  * Builds an IP version 4 RFC 792 Internet Control Message Protocol (ICMP)
  * unreachable header. The IP header that caused the error message should be
  * built by a previous call to libnet_build_ipv4().
  *
  * * @param type type of ICMP packet (should be ICMP_UNREACH)
  * * @param code code of ICMP packet (should be one of the 16 unreachable codes)
  * * @param sum checksum (0 for libnet to autofill)
  * * @param payload optional payload or NULL
  * * @param payload_s payload length or 0
  * * @param ptag protocol tag to modify an existing header, 0 to build a new one
 
  * * @return protocol tag value on success, -1 on error
  *
 */
static VALUE rb_libnet_build_icmpv4_unreach(VALUE self,
                                          VALUE type,
                                          VALUE code,
                                          VALUE csum,
                                          VALUE payload,
                                          VALUE payload_s,
                                          VALUE tag)
{
   libnet_ptag_t p = -1;
   libnet_t *lctx = NULL;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   DEBUG_PRINT("libnet_build_icmpv4_unreach()");

   if(lctx)
      p = libnet_build_icmpv4_unreach(
                                    (u_int8_t)NUM2INT(type),
                                    (u_int8_t)NUM2INT(code),
                                    (u_int16_t)NUM2INT(csum),
                                    (payload == Qnil ? NULL : StringValuePtr(payload)),
                                    (payload_s == Qnil ? 0 : NUM2INT(payload_s)),
                                    lctx,
                                    (libnet_ptag_t)(tag == Qnil ? 0 : NUM2INT(tag))
                                  );

   return INT2FIX(p);                                  
}
/*
 * Builds an IP version 4 RFC 792 Internet Message Control Protocol (ICMP)
 * redirect header.  The IP header that caused the error message should be
 * built by a previous call to libnet_build_ipv4().
 * 
 * * @param type type of ICMP packet (should be ICMP_REDIRECT)
 * * @param code code of ICMP packet (should be one of the four redirect codes)
 * * @param sum checksum (0 for libnet to autofill)
 * * @param gateway (dotted decimal IPv4 address string)
 * * @param payload optional payload or NULL
 * * @param payload_s payload length or 0
 * * @param ptag protocol tag to modify an existing header, 0 to build a new one
 * 
 * * @return protocol tag value on success, -1 on error
 *
 */
static VALUE rb_libnet_build_icmpv4_redirect(VALUE self,
                                          VALUE type,
                                          VALUE code,
                                          VALUE csum,
                                          VALUE gateway,
                                          VALUE payload,
                                          VALUE payload_s,
                                          VALUE tag)
{
   libnet_ptag_t p = -1;
   libnet_t *lctx = NULL;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   DEBUG_PRINT("libnet_build_icmpv4_redirect()");

   if(lctx)
      p = libnet_build_icmpv4_redirect(
                                    (u_int8_t)NUM2INT(type),
                                    (u_int8_t)NUM2INT(code),
                                    (u_int16_t)NUM2INT(csum),
                                    (gateway == Qnil ? 0 : inet_addr(RSTRING(gateway)->ptr)),
                                    (payload == Qnil ? NULL : StringValuePtr(payload)),
                                    (payload_s == Qnil ? 0 : NUM2INT(payload_s)),
                                    lctx,
                                    (libnet_ptag_t)(tag == Qnil ? 0 : NUM2INT(tag))
                                  );

   return INT2FIX(p);                                  
}

/*
  * Builds an IP version 4 RFC 792 Internet Control Message Protocol (ICMP) time
  * exceeded header.  The IP header that caused the error message should be
  * built by a previous call to libnet_build_ipv4().
  * 
  * * @param type type of ICMP packet (should be ICMP_TIMXCEED)
  * * @param code code of ICMP packet (ICMP_TIMXCEED_INTRANS / ICMP_TIMXCEED_REASS)
  * * @param sum checksum (0 for libnet to autofill)
  * * @param payload optional payload or NULL
  * * @param payload optional payload or NULL
  * * @param payload_s payload length or 0
  * * @param ptag protocol tag to modify an existing header, 0 to build a new one
  * 
  * * @return protocol tag value on success, -1 on error
  *
 */
static VALUE rb_libnet_build_icmpv4_timeexceed(VALUE self,
                                          VALUE type,
                                          VALUE code,
                                          VALUE csum,
                                          VALUE payload,
                                          VALUE payload_s,
                                          VALUE tag)
{
   libnet_ptag_t p = -1;
   libnet_t *lctx = NULL;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   DEBUG_PRINT("libnet_build_icmpv4_timeexceed()");

   if(lctx)
      p = libnet_build_icmpv4_timeexceed(
                                    (u_int8_t)NUM2INT(type),
                                    (u_int8_t)NUM2INT(code),
                                    (u_int16_t)NUM2INT(csum),
                                    (payload == Qnil ? NULL : StringValuePtr(payload)),
                                    (payload_s == Qnil ? 0 : NUM2INT(payload_s)),
                                    lctx,
                                    (libnet_ptag_t)(tag == Qnil ? 0 : NUM2INT(tag))
                                  );

   return INT2FIX(p);                                  
}
/*
  * Builds an IP version 4 RFC 792 Internet Control Message Protocol (ICMP)
  * timestamp request/reply header.
  *
  * * @param type type of ICMP packet (should be ICMP_TSTAMP or ICMP_TSTAMPREPLY)
  * * @param code code of ICMP packet (should be 0)
  * * @param sum checksum (0 for libnet to autofill)
  * * @param id identification number
  * * @param seq sequence number
  * * @param otime originate timestamp (int32)
  * * @param rtime receive timestamp (int32)
  * * @param ttime transmit timestamp (int32)
  * * @param payload optional payload or NULL
  * * @param payload_s payload length or 0
  * * @param l pointer to a libnet context
  * * @param ptag protocol tag to modify an existing header, 0 to build a new one
  * * @return protocol tag value on success, -1 on error
  *
 */
static VALUE rb_libnet_build_icmpv4_timestamp(VALUE self,
                                             VALUE type,
                                             VALUE code,
                                             VALUE csum,
                                             VALUE id,
                                             VALUE seq,
                                             VALUE otime,
                                             VALUE rtime,
                                             VALUE ttime,
                                             VALUE payload,
                                             VALUE payload_s,
                                             VALUE tag)
{
   libnet_ptag_t p = -1;
   libnet_t *lctx = NULL;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   DEBUG_PRINT("libnet_build_icmpv4_timestamp()");

   if(lctx)
      p = libnet_build_icmpv4_timestamp(
                                    (u_int8_t)NUM2INT(type),
                                    (u_int8_t)NUM2INT(code),
                                    (u_int16_t)NUM2INT(csum),
                                    (u_int16_t)NUM2INT(id),
                                    (u_int16_t)NUM2INT(seq),
                                    (u_int32_t)NUM2INT(otime),
                                    (u_int32_t)NUM2INT(rtime),
                                    (u_int32_t)NUM2INT(ttime),
                                    (payload == Qnil ? NULL : StringValuePtr(payload)),
                                    (payload_s == Qnil ? 0 : NUM2INT(payload_s)),
                                    lctx,
                                    (libnet_ptag_t)(tag == Qnil ? 0 : NUM2INT(tag))
                                  );

   return INT2FIX(p);                                  
}

/*
*
 * Writes a prebuilt packet to the network. The function assumes that l was
 * previously initialized (via a call to libnet_init()) and that a
 * previously constructed packet has been built inside this context (via one or
 * more calls to the libnet_build* family of functions) and is ready to go.
 * Depending on how libnet was initialized, the function will write the packet
 * to the wire either via the raw or link layer interface. The function will
 * also bump up the internal libnet stat counters which are retrievable via
 * libnet_stats().
 *
 * * @return the number of bytes written, -1 on error
 *
 */
static VALUE rb_libnet_write(VALUE self)
{
   libnet_t *lctx = NULL;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);

   DEBUG_PRINT("libnet_write()");

   return INT2FIX(libnet_write(lctx));
}
/*
 * call-seq:
 *    l.stats           -> [packets_sent, packet_errors, bytes_written]
 */
static VALUE rb_libnet_stats(VALUE self)
{
   VALUE arr_obj;
   struct libnet_stats ls;
   libnet_t *lctx = NULL;

   Data_Get_Struct(
         rb_iv_get(self, "@handle"),
         libnet_t,
         lctx);
   
   if(lctx == NULL) 
      return Qnil;

   libnet_stats(lctx, &ls);

   arr_obj = rb_ary_new();
   
   rb_ary_push(arr_obj, INT2NUM(ls.packets_sent));
   rb_ary_push(arr_obj, INT2NUM(ls.packet_errors));
   rb_ary_push(arr_obj, INT2NUM(ls.bytes_written));

   return arr_obj;
}
                              
/*
 * module main
 *
 * SVN: http://rubyforge.org/viewvc?root=ruby-libnet
 */
void Init_libnet()
{
   cLibnet = rb_define_class("Libnet", rb_cObject);
   /*
    * constants for libnet injection type
    */
   rb_define_const(cLibnet, "LIBNET_LINK", INT2FIX(LIBNET_LINK));
   rb_define_const(cLibnet, "LIBNET_LINK_ADV", INT2FIX(LIBNET_LINK_ADV));
   rb_define_const(cLibnet, "LIBNET_RAW4", INT2FIX(LIBNET_RAW4));
   rb_define_const(cLibnet, "LIBNET_RAW4_ADV", INT2FIX(LIBNET_RAW4_ADV));
   rb_define_const(cLibnet, "LIBNET_RAW6", INT2FIX(LIBNET_RAW6));
   rb_define_const(cLibnet, "LIBNET_RAW6_ADV", INT2FIX(LIBNET_RAW6_ADV));

   rb_define_singleton_method(cLibnet, "hex_aton", rb_libnet_hex_aton, 1);
   rb_define_singleton_method(cLibnet, "hex_ntoa", rb_libnet_hex_ntoa, 1);

   /*
    * methods for class Libnet
    */
   rb_define_method(cLibnet, "initialize", rb_libnet_init, -1);
   rb_define_method(cLibnet, "deinit", rb_libnet_free, 0);
   rb_define_method(cLibnet, "clear_packet", rb_libnet_clear_packet, 0);
   rb_define_method(cLibnet, "getdevice", rb_libnet_getdevice, 0);
   rb_define_method(cLibnet, "gethwaddr", rb_libnet_gethwaddr, 0);
   rb_define_method(cLibnet, "getip", rb_libnet_getip, 0);
   rb_define_method(cLibnet, "geterror", rb_libnet_geterror, 0);
   rb_define_method(cLibnet, "hex_aton", rb_libnet_hex_aton, 1);
   rb_define_method(cLibnet, "hex_ntoa", rb_libnet_hex_ntoa, 1);
   rb_define_method(cLibnet, "build_ethernet", rb_libnet_build_ethernet, 6);
   rb_define_method(cLibnet, "auto_build_ethernet", rb_libnet_autobuild_ethernet, 2);
   rb_define_method(cLibnet, "build_arp", rb_libnet_build_arp, 12);
   rb_define_method(cLibnet, "auto_build_arp", rb_libnet_autobuild_arp, 5);
   rb_define_method(cLibnet, "build_ipv4", rb_libnet_build_ipv4, 12);
   rb_define_method(cLibnet, "auto_build_ipv4", rb_libnet_autobuild_ipv4, 3);
   rb_define_method(cLibnet, "build_ipv4_options", rb_libnet_build_ipv4_options, 3);
   rb_define_method(cLibnet, "build_tcp", rb_libnet_build_tcp, 12);
   rb_define_method(cLibnet, "build_tcp_options", rb_libnet_build_tcp_options, 3);
   rb_define_method(cLibnet, "build_udp", rb_libnet_build_udp, 7);
   rb_define_method(cLibnet, "build_icmpv4_echo", rb_libnet_build_icmpv4_echo, 8);
   rb_define_method(cLibnet, "build_icmpv4_mask", rb_libnet_build_icmpv4_mask, 9);
   rb_define_method(cLibnet, "build_icmpv4_unreach", rb_libnet_build_icmpv4_unreach, 6);
   rb_define_method(cLibnet, "build_icmpv4_redirect", rb_libnet_build_icmpv4_redirect, 7);
   rb_define_method(cLibnet, "build_icmpv4_timeexceed", rb_libnet_build_icmpv4_timeexceed, 6);
   rb_define_method(cLibnet, "build_icmpv4_timestamp", rb_libnet_build_icmpv4_timestamp, 11);
   rb_define_method(cLibnet, "write", rb_libnet_write, 0);
   rb_define_method(cLibnet, "stats", rb_libnet_stats, 0);

   Init_libnet_defs(cLibnet);
}
