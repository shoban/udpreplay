#ifndef __IP_H__
#define __IP_H__

#include <stdint.h>

struct my_ip {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    unsigned    ip_v : 4;       /* 0     version == 4            */
    unsigned    ip_hl : 4;      /*       header length           */
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    unsigned    ip_hl : 4;      /* 0     header length           */
    unsigned    ip_v : 4;       /*       version == 4            */
#else
#   error "unknown byte order"
#endif
    uint8_t     ip_tos;         /* 1     type of service         */
    uint16_t    ip_len;         /* 2-3   total length            */
    uint16_t    ip_id;          /* 4-5   identification          */
    uint16_t    ip_off;         /* 6-7   fragment offset field       */
    uint8_t     ip_ttl;         /* 8     time to live            */
    uint8_t     ip_p;           /* 9     protocol            */
    uint16_t    ip_sum;         /* 10-11 checksum            */
    uint32_t    ip_src;        /* 12-15 source address          */
    uint32_t    ip_dst;        /* 16-19 destination address         */
};

/* ip_protocol */
#define IP_PROTO_ICMP       1
#define IP_PROTO_IGMP       2
#define IP_PROTO_GGP        3
#define IP_PROTO_IPIP       4
#define IP_PROTO_ST     5
#define IP_PROTO_TCP        6
#define IP_PROTO_UCL        7
#define IP_PROTO_EGP        8
#define IP_PROTO_IGP        9
#define IP_PROTO_BBN        10
#define IP_PROTO_NVPII      11
#define IP_PROTO_PUP        12
#define IP_PROTO_ARGUS      13
#define IP_PROTO_EMCON      14
#define IP_PROTO_XNET       15
#define IP_PROTO_CHAOS      16
#define IP_PROTO_UDP        17
#define IP_PROTO_MUX        18
#define IP_PROTO_DCN        19
#define IP_PROTO_HMP        20
#define IP_PROTO_PRM        21
#define IP_PROTO_XNS        22
#define IP_PROTO_TRUNK1     23
#define IP_PROTO_TRUNK2     24
#define IP_PROTO_LEAF1      25
#define IP_PROTO_LEAF2      26
#define IP_PROTO_RDP        27
#define IP_PROTO_IRTP       28
#define IP_PROTO_ISOTP4     29
#define IP_PROTO_NETBLT     30
#define IP_PROTO_MFENSP     31
#define IP_PROTO_MERIT      32
#define IP_PROTO_DCCP       33
#define IP_PROTO_RSVP       46
#define IP_PROTO_GRE        47
#define IP_PROTO_ICMP6          58
#define IP_PROTO_CFTP       62
#define IP_PROTO_SATNET     64
#define IP_PROTO_MITSUBNET  65
#define IP_PROTO_RVD        66
#define IP_PROTO_IPPC       67
#define IP_PROTO_SATMON     69
#define IP_PROTO_IPCV       71
#define IP_PROTO_BRSATMON   76
#define IP_PROTO_WBMON      78
#define IP_PROTO_WBEXPAK    79
#define IP_PROTO_SCTP       132
#define IP_PROTO_UDPLITE    136

#define IP_PROTO_NONE       257
#define IP_PROTO_TRANSP     258
#define IP_PROTO_TCP_OR_UDP 256
#define IP_PROTO_PAYLOAD    259

#endif /* __IP_H__ */

