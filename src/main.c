#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <errno.h>
#include <sys/time.h>

#include <stdint.h>

#include "socket.h"
#include "icmp.h"
#include "ping.h"
#include <netinet/ip_icmp.h>

static void print_ping_start(icmp_ping_t *ping);
static void print_ping_result(ping_result_t *result);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <destination>\n", argv[0]);
        return 1;
    }
    icmp_ping_t ping = {0};
    if (init_icmp_ping(&ping, argv[1]) == -1) {
        return 1;
    }

    ping_result_t result;
    print_ping_start(&ping);
    result = icmp_ping(&ping);
    print_ping_result(&result);
    while (result.status == PING_SUCCESS) {
        sleep(1);
        result = icmp_ping(&ping);
        print_ping_result(&result);
    }

//	close(socket);
    return 0;
}

static void print_ping_start(icmp_ping_t *ping) {
    printf("PING %s (%s): %ld data bytes\n",
           ping->original_host,
           inet_ntoa(ping->destination.sin_addr),
           ping->packet_size - sizeof(struct icmphdr));
}

static void print_ping_result(ping_result_t *result) {
    if (result->status == PING_SUCCESS) {
        printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
                result->size,
               inet_ntoa(result->reply_address.sin_addr),
               result->seq,
               result->ttl,
               result->time);
    } else {
        printf("Request error\n");
    }
}