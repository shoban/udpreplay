#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// From Beej's nw prog guide

void ts_add(struct timespec *a, uint32_t nanosecs)
{
    a->tv_nsec = a->tv_nsec + nanosecs;
    a->tv_sec += a->tv_nsec/1000000000;
    a->tv_nsec = a->tv_nsec%1000000000;
}

double ts_diff(const struct timespec *a, const struct timespec *b)
{
    double r = a->tv_sec - b->tv_sec;
    r += (a->tv_nsec - b->tv_nsec)*1E-9;
    return r;
}

void udpplay_rate(
        const char *dst_ip, const char *dst_port,
        uint8_t *data, uint32_t size,
        uint32_t pkt_size, uint32_t rate)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv, i;
    int numbytes;
    uint16_t len;

    struct timespec st, ct, slt;

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

    clock_gettime(CLOCK_MONOTONIC, &st);
    slt = st;


    uint32_t no_pkts = size/pkt_size;
    uint32_t sent = 0;
    uint32_t must_send = 0;

    fprintf(stdout, "size=%u, pkt_size=%u, no_pkts=%u.\n", size, pkt_size, no_pkts);

    while (sent < no_pkts) {
        ts_add(&slt, 1000000); /* sleep for 1ms */
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &slt, NULL);
        must_send = ts_diff(&slt, &st)*rate*1024*1024/pkt_size;
        //fprintf(stdout, "must_send=%u, sent=%u, end=%u\n", must_send, sent,
        //        no_pkts);
        while (sent < must_send && sent < no_pkts) {
            if (sendto(sockfd, data, pkt_size, 0,
                        p->ai_addr, p->ai_addrlen) == -1) {
                perror("sendto() error");
                goto end;
            }
            data += pkt_size;
            sent++;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &slt);
end:
    freeaddrinfo(servinfo);
    fprintf(stdout, "sent %u pkts, %u size in time %f secs.",
            no_pkts, no_pkts*pkt_size, ts_diff(&slt, &st));

    close(sockfd);
}

int read_data_file(const char *file, uint8_t *data, uint32_t max,
        uint32_t *read_bytes)
{
#define BLOCK_SIZE 1024
    int fd, rb, tb;

    fd = open(file, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Unable to read file: %s\n", file);
        return fd;
    }

    rb = 0;
    while (rb + BLOCK_SIZE < max) {
        tb = read(fd, &data[rb], BLOCK_SIZE);
        if (tb < 0) {
            perror("read() error");
            break;
        }
        if (tb == 0) break;
        rb += tb;
    }

    fprintf(stdout, "read %u mb in memory.\n", rb/(1024*1024));
    *read_bytes = rb;
    return 0;
}

void usage(const char *p)
{
    fprintf(stderr, "USAGE: %s <data-file> <rate-Mbps> <dst-ip> <dst-port>\n", p);
}

int main(int argc, const char **argv)
{
    int max;
    uint8_t *data;
    uint32_t no_pkts;
    uint32_t rate, size;

    if (argc != 5) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    max = 1024*1024*1024;
    size = 0;
    data = (uint8_t *) malloc(max);
    rate = atoi(argv[2]);

    if (!data) {
        fprintf(stderr, "Unable to alloc.\n");
        exit(EXIT_FAILURE);
    }

    if (read_data_file(argv[1], data, max, &size) < 0) {
        exit(EXIT_FAILURE);
    }

    udpplay_rate(
            argv[3], argv[4],
            data, size, 1400, rate);

    return 0;
}
