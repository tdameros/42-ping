/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:53:00 by tdameros          #+#    #+#             */
/*   Updated: 2024/08/15 14:53:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef ICMP_H
# define ICMP_H

#include <stdint.h>

struct icmp create_icmp_echo_header(uint16_t id, uint16_t sequence);
uint16_t calculate_icmp_checksum(void *packet, uint32_t len);

uint32_t create_icmp_echo_packet(uint8_t *buffer, uint8_t packet_len,
                                 uint16_t id, uint16_t sequence);
#endif