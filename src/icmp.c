/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   imcp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:30:00 by tdameros          #+#    #+#             */
/*   Updated: 2024/08/15 14:30:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <string.h>
#include <sys/time.h>

#include "icmp.h"

uint32_t create_icmp_echo_packet(uint8_t *buffer, uint8_t packet_len,
                                 uint16_t id, uint16_t sequence) {
    if (packet_len < sizeof(struct icmp)) {
        return -1;
    }
    struct icmp header = create_icmp_echo_header(id, sequence);
    memcpy(buffer, &header, sizeof(header));
    header.icmp_cksum = calculate_icmp_checksum(buffer, packet_len);
    memcpy(buffer, &header, sizeof(header));
    return 0;
}

struct icmp create_icmp_echo_header(uint16_t id, uint16_t sequence) {
    struct icmp header;

    header.icmp_type = ICMP_ECHO;
    header.icmp_code = 0;
    header.icmp_id = htons(id);
    header.icmp_seq = htons(sequence);
    header.icmp_cksum = 0;
    return header;
}

uint16_t calculate_icmp_checksum(void *packet, uint32_t len) {
    uint16_t *buf = packet;
    uint32_t sum = 0;
    uint16_t result;
    for (sum = 0; len > 1; len -= 2) {
        sum += *buf++;
    }
    if (len == 1) {
        sum += *(uint8_t *)buf;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}
