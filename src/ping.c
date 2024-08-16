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
#include <sys/time.h>


static int32_t send_icmp_ping(int32_t socket, uint8_t *packet, uint32_t packet_size, struct sockaddr_in destination, uint16_t id, uint16_t seq);
static ping_result_t receive_icmp_ping(int32_t socket, uint8_t *packet, uint32_t packet_size);

int32_t init_icmp_ping(icmp_ping_t *ping, char *hostname) {
    int32_t socket = create_icmp_socket();
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
//    ping->packet_size = sizeof(ping->packet);
    ping->packet_size = 64;
    return 0;
}

ping_result_t icmp_ping(icmp_ping_t *ping) {
    send_icmp_ping(ping->socket, ping->packet, ping->packet_size, ping->destination, ping->id, ping->seq);
    struct timeval start;
    gettimeofday(&start, NULL);
    ping_result_t result = receive_icmp_ping(ping->socket, ping->packet, ping->packet_size);
    struct timeval end;
    gettimeofday(&end, NULL);
    result.time = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
//    result.time = (end.tv_sec - start.tv_sec) * 1000.0;
//    result.time += (end.tv_usec - start.tv_usec) / 1000.0;
    ping->seq++;
    return result;
}

static int32_t send_icmp_ping(int32_t socket, uint8_t *packet, uint32_t packet_size, struct sockaddr_in destination, uint16_t id, uint16_t seq) {
    create_icmp_echo_packet(packet, packet_size, id, seq);
    if (sendto(socket,
               packet,
               packet_size,
               0,
               (struct sockaddr *) &destination,
               sizeof(destination)) < 0) {
        perror("sendto");
        return -1;
    }
    return 0;
}

static ping_result_t receive_icmp_ping(int32_t socket, uint8_t *packet, uint32_t packet_size) {
    ping_result_t result = {0};
    (void) packet_size;
    // Attendre et lire la réponse
    struct sockaddr_in reply_addr;
    socklen_t reply_addr_len = sizeof(reply_addr);
    int bytes_received = recvfrom(socket,
                                  packet,
                                  1024,
                                  0,
                                  (struct sockaddr *) &reply_addr,
                                  &reply_addr_len);
    if (bytes_received < 0) {
        result.status = PING_ERROR;
        perror("recvfrom");
    } else {
        struct iphdr *ip_hdr;
        struct icmphdr *recv_icmp_hdr;

        // Extraire l'en-tête IP
        ip_hdr = (struct iphdr *) packet;

        // Extraire l'en-tête ICMP de la réponse
        recv_icmp_hdr = (struct icmphdr *) (packet + (sizeof(struct iphdr)));

        result.status = PING_SUCCESS;
        result.reply_address = reply_addr;
        result.ttl = ip_hdr->ttl;
        result.seq = recv_icmp_hdr->un.echo.sequence;
        result.size = bytes_received - sizeof(struct iphdr);
        result.type = recv_icmp_hdr->type;
        if (result.type != ICMP_ECHOREPLY) {
            return receive_icmp_ping(socket, packet, packet_size);
        }
    }
    return result;
}