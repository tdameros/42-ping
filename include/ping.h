/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/16 17:11:00 by tdameros          #+#    #+#             */
/*   Updated: 2024/08/16 17:11:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef PING_H
# define PING_H

#include <stdint.h>
#include <arpa/inet.h>

typedef struct {
    uint8_t packet[1024];
    struct sockaddr_in destination;
    char *original_host;
    int32_t socket;
    uint32_t packet_size;
    uint16_t id;
    uint16_t seq;
} icmp_ping_t;

typedef enum {
    PING_SUCCESS,
    PING_TIMEOUT,
    PING_ERROR,
    PING_UNKNOWN_HOST,
} ping_status_t;

typedef struct {
    ping_status_t status;
    double time;
    struct sockaddr_in reply_address;
    uint8_t type;
    uint16_t ttl;
    uint16_t seq;
    uint32_t size;
} ping_result_t;

int32_t init_icmp_ping(icmp_ping_t *ping, char *hostname);
ping_result_t icmp_ping(icmp_ping_t *ping);
#endif