#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "pcap.h"

void usage(const char *p)
{
    fprintf(stderr, "USAGE: %s <pcap-dump> <dst-ip> <dst-port>\n", p);
}

// From Beej's nw prog guide
void udpreplay_dump(
        const char *dst_ip, const char *dst_port,
        uint8_t *data, uint32_t no_pkts)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv, i;
    int numbytes;
    uint16_t len;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(dst_ip, dst_port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        goto end;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "failed to bind socket\n");
        goto end;
    }

    for (i = 0; i < no_pkts; i++) {
        memcpy(&len, data, sizeof(len));
        //fprintf(stdout, "pkt %u.\n", len);
        data += 2;
        if (sendto(sockfd, data, len, 0,
                    p->ai_addr, p->ai_addrlen) == -1) {
            perror("sendto() error");
            goto end;
        }
        data += len;
    }

end:
    freeaddrinfo(servinfo);

    fprintf(stdout, "%d pkts sent to %s:%d.\n", i, dst_ip, atoi(dst_port));
    close(sockfd);
}

int main(int argc, const char **argv)
{
    int max;
    uint8_t *data;
    uint32_t no_pkts;

    if (argc != 4) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    max = 1024*1024*1024;
    data = (uint8_t *) malloc(max);

    if (!data) {
        fprintf(stderr, "Unable to alloc.\n");
        exit(EXIT_FAILURE);
    }

    read_file(argv[1], &no_pkts, data, max);

    udpreplay_dump(
            argv[2], argv[3],
            data, no_pkts);

    return 0;
}
