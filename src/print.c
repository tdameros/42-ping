/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/18 14:12:00 by tdameros          #+#    #+#             */
/*   Updated: 2024/08/18 14:12:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>

#include "statistics.h"
#include "print.h"
#include "ping.h"

static void print_ping_echo_reply(ping_result_t *result);
static void print_ping_timeout(ping_result_t *result);
static void print_ping_description(ping_result_t *result);
static char *get_icmp_description(ping_result_t *result);

typedef struct
{
    int type;
    int code;
    char *diag;
} icmp_code_description_t;

icmp_code_description_t icmp_code_description[] =
    {
        {ICMP_DEST_UNREACH, ICMP_NET_UNREACH, "Destination Net Unreachable"},
        {ICMP_DEST_UNREACH, ICMP_HOST_UNREACH, "Destination Host Unreachable"},
        {ICMP_DEST_UNREACH, ICMP_PROT_UNREACH, "Destination Protocol Unreachable"},
        {ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, "Destination Port Unreachable"},
        {ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED, "Fragmentation needed and DF set"},
        {ICMP_DEST_UNREACH, ICMP_SR_FAILED, "Source Route Failed"},
        {ICMP_DEST_UNREACH, ICMP_NET_UNKNOWN, "Network Unknown"},
        {ICMP_DEST_UNREACH, ICMP_HOST_UNKNOWN, "Host Unknown"},
        {ICMP_DEST_UNREACH, ICMP_HOST_ISOLATED, "Host Isolated"},
        {ICMP_DEST_UNREACH, ICMP_NET_UNR_TOS, "Destination Network Unreachable At This TOS"},
        {ICMP_DEST_UNREACH, ICMP_HOST_UNR_TOS, "Destination Host Unreachable At This TOS"},
        {ICMP_REDIRECT, ICMP_REDIR_NET, "Redirect Network"},
        {ICMP_REDIRECT, ICMP_REDIR_HOST, "Redirect Host"},
        {ICMP_REDIRECT, ICMP_REDIR_NETTOS, "Redirect Type of Service and Network"},
        {ICMP_REDIRECT, ICMP_REDIR_HOSTTOS, "Redirect Type of Service and Host"},
        {ICMP_TIME_EXCEEDED, ICMP_EXC_TTL, "Time to live exceeded"},
        {ICMP_TIME_EXCEEDED, ICMP_EXC_FRAGTIME, "Frag reassembly time exceeded"}
    };

void print_ping_start(icmp_ping_t *ping) {
    printf("PING %s (%s): %ld data bytes\n",
           ping->original_host,
           inet_ntoa(ping->destination.sin_addr),
           DEFAULT_PACKET_SIZE - sizeof(struct icmphdr));
}

void print_ping_result(ping_result_t *result) {
    if (result->status == PING_SUCCESS && result->code == ICMP_ECHOREPLY) {
        print_ping_echo_reply(result);
    } else if (result->status == PING_TIMEOUT) {
        print_ping_timeout(result);
    } else {
        print_ping_description(result);
    }
}

void print_ping_statistics(icmp_ping_t *ping) {
    uint32_t transmitted = ping->statistics.packets_transmitted;
    uint32_t received = ping->statistics.packets_received;
    uint32_t packet_loss = (transmitted - received) / transmitted * 100;
    printf("--- %s ping statistics ---\n", ping->original_host);
    printf("%d packets transmitted, %d packets received, %d%% packet loss\n", transmitted, received, packet_loss);
    printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", ping->statistics.min_ms,
           statistics_get_average(&ping->statistics),
           ping->statistics.max_ms,
           statistics_get_stddev(&ping->statistics));
}

static void print_ping_echo_reply(ping_result_t *result) {
    printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
           result->size,
           inet_ntoa(result->reply_address.sin_addr),
           result->seq,
           result->ttl,
           result->time);
}

static void print_ping_timeout(ping_result_t *result) {
    printf("Request timeout for icmp_seq %d\n", result->seq);
}

static void print_ping_description(ping_result_t *result) {
    char *description = get_icmp_description(result);
    char host[NI_MAXHOST];
    int32_t result_code = getnameinfo((struct sockaddr *)&result->reply_address, sizeof(result->reply_address), host, NI_MAXHOST, NULL, 0, 0);
    if (result_code != 0) {
            printf("%d bytes from %s: %s\n",
                   result->size,
                   inet_ntoa(result->reply_address.sin_addr),
                   description);
    } else {
        printf("%d bytes from %s (%s): %s\n",
               result->size,
                host,
               inet_ntoa(result->reply_address.sin_addr),
               description);
    }
}

static char *get_icmp_description(ping_result_t *result) {
    for (uint32_t i = 0; i < sizeof(icmp_code_description) / sizeof(icmp_code_description[0]); i++) {
        if (result->status == PING_SUCCESS && result->type == icmp_code_description[i].type && result->code == icmp_code_description[i].code) {
            return icmp_code_description[i].diag;
        }
    }
    return NULL;
}