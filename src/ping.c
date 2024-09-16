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

#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "flags.h"
#include "icmp.h"
#include "ping.h"
#include "socket.h"
#include "statistics.h"
#include "time.h"

static int32_t send_icmp_ping(icmp_ping_t *ping);
static ping_result_t receive_icmp_ping(icmp_ping_t *ping, flags_t flags, double start_time);
static ping_result_t parse_packet(icmp_ping_t *ping,
                                  int bytes_received,
                                  struct sockaddr_in reply_addr,
                                  double start_time);

int32_t init_icmp_ping(icmp_ping_t *ping, flags_t flags) {
  int32_t socket = create_icmp_socket();
  if (socket < 0) {
    perror("ft_ping: socket");
    return -1;
  }
  if (set_icmp_socket_timeout(socket, 1, 0) < 0) {
    perror("ft_ping: socket_timeout");
    return -1;
  }
  set_icmp_socket_debug(socket, flags.options.debug);
  struct sockaddr_in address = {0};
  if (resolve_host(flags.hostname, &address) < 0) {
    fprintf(stderr, "ft_ping: unknow host\n");
    return -1;
  }
  memset(ping, 0, sizeof(icmp_ping_t));
  ping->original_host = flags.hostname;
  ping->socket = socket;
  ping->destination = address;
  ping->id = getpid();
  ping->seq = 0;
  statistics_init(&ping->statistics);
  return 0;
}

ping_result_t icmp_ping(icmp_ping_t *ping, flags_t flags) {
  double start_time = get_current_time_in_ms();
  if (send_icmp_ping(ping) < 0) {
    return (ping_result_t){.status = PING_FATAL_ERROR};
  }
  ping_result_t result = receive_icmp_ping(ping, flags, start_time);
  ping->seq++;
  return result;
}

static int32_t send_icmp_ping(icmp_ping_t *ping) {
  create_icmp_echo_packet(ping->packet, DEFAULT_PACKET_SIZE, ping->id, ping->seq);
  if (sendto(ping->socket,
             ping->packet,
             DEFAULT_PACKET_SIZE,
             0,
             (struct sockaddr *)&ping->destination,
             sizeof(ping->destination)) < 0) {
    perror("ft_ping: sending packet");
    return -1;
  }
  statistics_packet_transmit(&ping->statistics);
  return 0;
}

static ping_result_t receive_icmp_ping(icmp_ping_t *ping, flags_t flags, double start_time) {
  ping_result_t result = {0};
  struct sockaddr_in reply_addr;
  socklen_t reply_addr_len = sizeof(reply_addr);
  while (get_current_time_in_ms() - start_time <= flags.linger_value) {
    int bytes_received = recvfrom(ping->socket,
                                  ping->packet,
                                  MAX_PACKET_SIZE,
                                  0,
                                  (struct sockaddr *)&reply_addr,
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
  result.seq = ping->seq;
  return result;
}

static ping_result_t parse_packet(icmp_ping_t *ping,
                                  int bytes_received,
                                  struct sockaddr_in reply_addr,
                                  double start_time) {
  ping_result_t result = {0};
  const struct ip *receive_ip_header = (struct ip *)ping->packet;
  struct icmp *receive_icmp_header = (struct icmp *)(ping->packet + sizeof(struct ip));
  struct ip *ip_icmp = &receive_icmp_header->icmp_ip;
  uint16_t original_id;

  result.ip_icmp = ip_icmp;
  result.type = receive_icmp_header->icmp_type;
  if (result.type != ICMP_ECHOREPLY && result.type != ICMP_ECHO) {
    struct icmp *original_icmp_hdr =
        (struct icmp *)(ping->packet + sizeof(struct ip) + ICMP_MINLEN + sizeof(struct ip));
    original_id = ntohs(original_icmp_hdr->icmp_hun.ih_idseq.icd_id);
    result.seq = ntohs(original_icmp_hdr->icmp_hun.ih_idseq.icd_seq);
  } else {
    original_id = ntohs(receive_icmp_header->icmp_hun.ih_idseq.icd_id);
    result.seq = ntohs(receive_icmp_header->icmp_hun.ih_idseq.icd_seq);
  }
  if (original_id != ping->id || result.type == ICMP_ECHO) {
    return (ping_result_t){.status = PING_ERROR};
  }
  result.ttl = receive_ip_header->ip_ttl;
  result.time = get_current_time_in_ms() - start_time;
  result.reply_address = reply_addr;
  result.size = bytes_received - sizeof(struct ip);
  result.code = receive_icmp_header->icmp_code;
  result.status = PING_SUCCESS;

  uint16_t checksum = receive_icmp_header->icmp_cksum;
  receive_icmp_header->icmp_cksum = 0;
  if (checksum !=
      calculate_icmp_checksum(receive_icmp_header, bytes_received - sizeof(struct ip))) {
    fprintf(stderr, "checksum mismatch from %s\n", ping->original_host);
    return result;
  }
  if (result.type == ICMP_ECHOREPLY && result.code == ICMP_ECHOREPLY) {
    statistics_packet_receive(&ping->statistics, result.time);
  }
  return result;
}
