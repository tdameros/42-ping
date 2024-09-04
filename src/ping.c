/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/16 17:11:00 by tdameros          #+#    #+#             */
/*   Updated: 2024/08/16 17:11:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include "icmp.h"
#include "socket.h"
#include "ping.h"
#include <netinet/ip_icmp.h>
#include <string.h>
#include "statistics.h"
#include <sys/time.h>

static int32_t send_icmp_ping(icmp_ping_t *ping);
static ping_result_t receive_icmp_ping(icmp_ping_t *ping, double start_time);
static ping_result_t parse_packet(icmp_ping_t *ping, int bytes_received, struct sockaddr_in reply_addr, double start_time);
static double get_current_time_in_ms();

int32_t init_icmp_ping(icmp_ping_t *ping, char *hostname) {
    int32_t socket = create_icmp_socket();
    if (socket < 0) {
        perror("socket");
        return -1;
    }
    set_icmp_socket_timeout(socket, 1, 0);
    struct sockaddr_in address = {0};
    if (resolve_host(hostname, &address) < 0) {
        fprintf(stderr, "ft_ping: unknow host\n");
        return -1;
    }
    memset(ping, 0, sizeof(icmp_ping_t));
    ping->original_host = hostname;
    ping->socket = socket;
    ping->destination = address;
    ping->id = getpid();
    ping->seq = 0;
    statistics_init(&ping->statistics);
    return 0;
}

ping_result_t icmp_ping(icmp_ping_t *ping) {
    double start_time = get_current_time_in_ms();
    send_icmp_ping(ping);
    ping_result_t result = receive_icmp_ping(ping, start_time);
    ping->seq++;
    return result;
}

static int32_t send_icmp_ping(icmp_ping_t *ping) {
    create_icmp_echo_packet(ping->packet, DEFAULT_PACKET_SIZE, ping->id, ping->seq);
    if (sendto(ping->socket,
               ping->packet,
               DEFAULT_PACKET_SIZE,
               0,
               (struct sockaddr *) &ping->destination,
               sizeof(ping->destination)) < 0) {
        perror("sendto");
        return -1;
    }
    statistics_packet_transmit(&ping->statistics);
    return 0;
}


static ping_result_t receive_icmp_ping(icmp_ping_t *ping, double start_time) {
    ping_result_t result = {0};
    struct sockaddr_in reply_addr;
    socklen_t reply_addr_len = sizeof(reply_addr);
    while (get_current_time_in_ms() - start_time < 2000) {
        int bytes_received = recvfrom(ping->socket,
                                      ping->packet,
                                      MAX_PACKET_SIZE,
                                      0,
                                      (struct sockaddr *) &reply_addr,
                                      &reply_addr_len);
        result.time = get_current_time_in_ms() - start_time;
        if (bytes_received > 0) {
            ping_result_t parsed_result = parse_packet(ping, bytes_received, reply_addr, start_time);
            if (parsed_result.status == PING_SUCCESS) {
                return parsed_result;
            }
        }
    }
    result.status = PING_TIMEOUT;
    return result;
}

static ping_result_t parse_packet(icmp_ping_t *ping, int bytes_received, struct sockaddr_in reply_addr, double start_time) {
    ping_result_t result = {0};
    struct iphdr *ip_hdr = (struct iphdr *) ping->packet;
    struct icmphdr *recv_icmp_hdr = (struct icmphdr *) (ping->packet + sizeof(struct iphdr));
//    struct ip *ip_header = &recv_icmp_hdr->icmp_ip;
    uint16_t original_id;
    struct icmp *ipcmp = (struct icmp *)(ping->packet + sizeof(struct ip));
    struct ip *ip = &ipcmp->icmp_ip;


    result.ip_icmp = ip;
    result.type = recv_icmp_hdr->type;
    if (result.type != ICMP_ECHOREPLY && result.type != ICMP_ECHO) {
        struct icmphdr *original_icmp_hdr = (struct icmphdr *) (ping->packet + sizeof(struct iphdr) + sizeof(struct icmphdr) + sizeof(struct iphdr));
        original_id = ntohs(original_icmp_hdr->un.echo.id);
        result.seq = ntohs(original_icmp_hdr->un.echo.sequence);
    } else {
        original_id = ntohs(recv_icmp_hdr->un.echo.id);
        result.seq = ntohs(recv_icmp_hdr->un.echo.sequence);
    }
    if (original_id != ping->id || result.type == ICMP_ECHO) {
        return (ping_result_t) {.status = PING_ERROR};
    }
    result.ttl = ip_hdr->ttl;
    result.time = get_current_time_in_ms() - start_time;
    result.reply_address = reply_addr;
    result.size = bytes_received - sizeof(struct iphdr);
    result.code = recv_icmp_hdr->code;
    result.status = PING_SUCCESS;

    uint16_t checksum = recv_icmp_hdr->checksum;
    recv_icmp_hdr->checksum = 0;
    if (checksum != calculate_icmp_checksum(recv_icmp_hdr, bytes_received - sizeof(struct iphdr))) {
        fprintf(stderr, "checksum mismatch from %s\n", ping->original_host);
        return result;
    }
    if (result.type == ICMP_ECHOREPLY) {
        statistics_packet_receive(&ping->statistics, result.time);
    }
    return result;
}

static double get_current_time_in_ms() {
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    return ((double)current_time.tv_sec) * 1000.0 + ((double)current_time.tv_usec) / 1000.0;
}