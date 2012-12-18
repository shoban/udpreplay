#include "pcap.h"
#include "ip.h"
#include "udp.h"
#include "tcp.h"

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
 * PCAP format details:
 * http://wiki.wireshark.org/Development/LibpcapFileFormat
 */

int read_pkt(pcaprec_hdr_t *rh, int fd, uint8_t *data, uint32_t cur, uint32_t max)
{
    uint8_t tmp[9000];
    struct my_ip *iph;
    struct my_udp *uh;
    struct my_tcp *th;
    uint16_t len;
    int rbytes;

    rbytes = read(fd, tmp, rh->incl_len);
    assert(rbytes == rh->incl_len);

    uint16_t start = 14; // skip ethernet frame
    iph = (struct my_ip *) (tmp+start);
    start += iph->ip_hl << 2;
    if (iph->ip_p == IP_PROTO_TCP) {
        th = (struct my_tcp *) (tmp + start);
        start += th->th_off << 2;
    } else if (iph->ip_p == IP_PROTO_UDP) {
        uh = (struct my_udp *) (tmp + start);
        start += sizeof(struct my_udp);
    } else {
        return 0;
    }

    if (start >= rh->incl_len) goto err;

    len = rh->incl_len - start;

    if (cur + 2 + len >= max) goto err;

    memcpy(data+cur, &len, sizeof(len));
    memcpy(data+cur+2, tmp+start, len);
    //fprintf(stdout, "read bytes: %u\n", len);

    return 2 + len;

err:
    return -1;
}

int read_file(const char *file, uint32_t *no_pkts, uint8_t *data, uint32_t max)
{
    int fd, rbytes;
    pcap_hdr_t h;
    pcaprec_hdr_t rh;
    uint32_t cur = 0;
    uint32_t pkts = 0;

    fd = open(file, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Unable to read file: %s\n", file);
        return fd;
    }

    assert(read(fd, &h, sizeof(h)) == sizeof(h));
    while ((rbytes = read(fd, &rh, sizeof(rh)))) {
        assert(rbytes == sizeof(rh));
        //fprintf(stdout, "packet (inc-len=%u, orig-len=%u)\n",
        //        rh.incl_len, rh.orig_len);
        //lseek(fd, rh.incl_len, SEEK_CUR);
        rbytes = read_pkt(&rh, fd, data, cur, max);
        if (rbytes > 0) cur += rbytes, pkts++;
    }

    fprintf(stdout, "data in mem: %u pkts, %u bytes.\n", pkts, cur);
    if (no_pkts) *no_pkts = pkts;

    return 0;
}
