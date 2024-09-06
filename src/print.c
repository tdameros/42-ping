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
#include "flags.h"

static void print_ping_echo_reply(const ping_result_t *result, const flags_t *flags);
static void print_ping_timeout(const ping_result_t *result);
static void print_ping_description(const ping_result_t *result, const flags_t *flags);
static char *get_icmp_description(const ping_result_t *result);

typedef struct
{
    int type;
    int code;
    char *diag;
} icmp_code_description_t;


#ifdef __linux__
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
#endif
#ifdef __APPLE__
icmp_code_description_t icmp_code_description[] =
    {
        {ICMP_UNREACH, ICMP_UNREACH_NET, "Destination Net Unreachable"},
        {ICMP_UNREACH, ICMP_UNREACH_HOST, "Destination Host Unreachable"},
        {ICMP_UNREACH, ICMP_UNREACH_PROTOCOL, "Destination Protocol Unreachable"},
        {ICMP_UNREACH, ICMP_UNREACH_PORT, "Destination Port Unreachable"},
        {ICMP_UNREACH, ICMP_UNREACH_NEEDFRAG, "Fragmentation needed and DF set"},
        {ICMP_UNREACH, ICMP_UNREACH_SRCFAIL, "Source Route Failed"},
        {ICMP_UNREACH, ICMP_UNREACH_NET_UNKNOWN, "Network Unknown"},
        {ICMP_UNREACH, ICMP_UNREACH_HOST_UNKNOWN, "Host Unknown"},
        {ICMP_UNREACH, ICMP_UNREACH_ISOLATED, "Host Isolated"},
        {ICMP_UNREACH, ICMP_UNREACH_TOSNET, "Destination Network Unreachable At This TOS"},
        {ICMP_UNREACH, ICMP_UNREACH_TOSHOST, "Destination Host Unreachable At This TOS"},
        {ICMP_REDIRECT, ICMP_REDIRECT_NET, "Redirect Network"},
        {ICMP_REDIRECT, ICMP_REDIRECT_HOST, "Redirect Host"},
        {ICMP_REDIRECT, ICMP_REDIRECT_TOSNET, "Redirect Type of Service and Network"},
        {ICMP_REDIRECT, ICMP_REDIRECT_TOSHOST, "Redirect Type of Service and Host"},
        {ICMP_TIMXCEED, ICMP_TIMXCEED_INTRANS, "Time to live exceeded"},
        {ICMP_TIMXCEED, ICMP_TIMXCEED_REASS, "Frag reassembly time exceeded"}
    };
#endif

void print_ping_start(const icmp_ping_t *ping, const flags_t *flags) {
    if (flags->options.verbose) {
        printf("PING %s (%s): %d data bytes, id 0x%04x = %u\n",
               ping->original_host,
               inet_ntoa(ping->destination.sin_addr),
               DEFAULT_PACKET_SIZE - ICMP_MINLEN,
                   ping->id, ping->id);
    } else {
        printf("PING %s (%s): %d data bytes\n",
               ping->original_host,
               inet_ntoa(ping->destination.sin_addr),
               DEFAULT_PACKET_SIZE - ICMP_MINLEN);
    }
}

void print_ping_result(const ping_result_t *result, const flags_t *flags) {
    if (result->status == PING_SUCCESS && result->code == ICMP_ECHOREPLY) {
        print_ping_echo_reply(result, flags);
    } else if (result->status == PING_TIMEOUT) {
        print_ping_timeout(result);
    } else {
        print_ping_description(result, flags);
    }
}

void print_ping_statistics(const icmp_ping_t *ping) {
    uint32_t transmitted = ping->statistics.packets_transmitted;
    uint32_t received = ping->statistics.packets_received;
    uint8_t packet_loss = (double) (transmitted - received) / transmitted * 100;

    printf("--- %s ping statistics ---\n", ping->original_host);
    printf("%d packets transmitted, %d packets received, %u%% packet loss\n", transmitted, received, packet_loss);
    printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", ping->statistics.min_ms,
           statistics_get_average(&ping->statistics),
           ping->statistics.max_ms,
           statistics_get_stddev(&ping->statistics));
}

static void print_ping_echo_reply(const ping_result_t *result, const flags_t *flags) {
    if (!flags->options.flood) {
        printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
               result->size,
               inet_ntoa(result->reply_address.sin_addr),
               result->seq,
               result->ttl,
               result->time);
    }
}

static void print_ping_timeout(const ping_result_t *result) {
    printf("Request timeout for icmp_seq %d\n", result->seq);
}

static void print_ping_description(const ping_result_t *result, const flags_t *flags) {
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
    if (flags->options.verbose) {

        printf ("IP Hdr Dump:\n ");
        for (uint32_t j = 0; j < sizeof (*result->ip_icmp); ++j)
            printf ("%02x%s", *((unsigned char *) result->ip_icmp + j),
                    (j % 2) ? " " : "");	/* Group bytes two by two.  */
        printf ("\n");
        printf
            ("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src\tDst\tData\n");
        printf (" %1x  %1x  %02x", result->ip_icmp->ip_v, result->ip_icmp->ip_hl, result->ip_icmp->ip_tos);
        printf(" %04x %04x",
                (result->ip_icmp->ip_len > 0x2000) ? ntohs (result->ip_icmp->ip_len) : result->ip_icmp->ip_len,
                ntohs (result->ip_icmp->ip_id));
        printf("   %1x %04x", (ntohs (result->ip_icmp->ip_off) & 0xe000) >> 13,
                ntohs (result->ip_icmp->ip_off) & 0x1fff);
        printf("  %02x  %02x %04x", result->ip_icmp->ip_ttl, result->ip_icmp->ip_p, ntohs (result->ip_icmp->ip_sum));
        printf(" %s ", inet_ntoa (*((struct in_addr *) &result->ip_icmp->ip_src)));
        printf(" %s ", inet_ntoa (*((struct in_addr *) &result->ip_icmp->ip_dst)));
        uint32_t hlen = result->ip_icmp->ip_hl << 2;
        uint8_t *cp = (unsigned char *) result->ip_icmp + sizeof (*result->ip_icmp);	/* point to options */
        while (hlen-- > sizeof (*result->ip_icmp))
            printf ("%02x", *cp++);
    printf("\n");
        int type = *cp;
        int code = *(cp + 1);

        printf ("ICMP: type %u, code %u, size %u", type, code,
                ntohs (result->ip_icmp->ip_len) - hlen);
        if (type == ICMP_ECHOREPLY || type == ICMP_ECHO)
            printf (", id 0x%04x, seq 0x%04x", *(cp + 4) * 256 + *(cp + 5),
                    *(cp + 6) * 256 + *(cp + 7));
        printf ("\n");
    }
}

static char *get_icmp_description(const ping_result_t *result) {
    for (uint32_t i = 0; i < sizeof(icmp_code_description) / sizeof(icmp_code_description[0]); i++) {
        if (result->status == PING_SUCCESS && result->type == icmp_code_description[i].type && result->code == icmp_code_description[i].code) {
            return icmp_code_description[i].diag;
        }
    }
    return NULL;
}

void print_help(char *binary_name) {
    printf("Usage: %s [options]\n", binary_name);
    printf("  -?, --help            Show this help message\n");
    printf("  -c, --count=NUMBER    Stop after sending NUMBER packets\n");
    printf("  -i, --interval=NUMBER Wait NUMBER seconds between sending each packet\n");
    printf("  -w, --timeout=NUMBER  Stop after NUMBER seconds\n");
    printf("  -W, --linger=NUMBER   Time to wait for a response in seconds\n");
    printf("  -v, --verbose         Print detailed output\n");
    printf("  -d, --debug           Set the SO_DEBUG option\n");
    printf("  -u, --usage           Print usage\n");
}

void print_usage(char *binary_name) {
    printf("Usage: %s [-?vdu] [-c NUMBER] [-i NUMBER] [-w NUMBER] [-W NUMBER]\n"
           "                 [--help] [--count=NUMBER] [--interval=NUMBER] \n"
           "                 [--timeout=NUMBER] [--linger=NUMBER] [--verbose]\n"
           "                 [--debug] [--usage] HOST ...\n", binary_name);
}