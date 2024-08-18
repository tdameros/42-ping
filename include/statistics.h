/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   statistics.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/18 18:33:00 by tdameros          #+#    #+#             */
/*   Updated: 2024/08/18 18:33:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATISTICS_H
# define STATISTICS_H

#include "ping.h"

void statistics_init(ping_statistics_t *statistics);
void statistics_packet_receive(ping_statistics_t *statistics, double ms);
void statistics_packet_transmit(ping_statistics_t *statistics);
#endif