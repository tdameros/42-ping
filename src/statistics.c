/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   statistics.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/18 18:27:00 by tdameros          #+#    #+#             */
/*   Updated: 2024/08/18 18:27:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

void statistics_init(ping_statistics_t *statistics) {
    statistics->packets_transmitted = 0;
    statistics->packets_received = 0;
    statistics->min_ms = 0;
    statistics->max_ms = 0;
    statistics->avg_ms = 0;
    statistics->is_init = false;
}

void statistics_packet_transmit(ping_statistics_t *statistics) {
    statistics->packets_transmitted++;
}

void statistics_packet_receive(ping_statistics_t *statistics, double ms) {
    statistics->packets_received++;
    if (!statistics->is_init) {
        statistics->min_ms = ms;
        statistics->max_ms = ms;
        statistics->avg_ms = ms;
        statistics->is_init = true;
    } else {
        if (ms < statistics->min_ms) {
            statistics->min_ms = ms;
        }
        if (ms > statistics->max_ms) {
            statistics->max_ms = ms;
        }
        statistics->avg_ms = (statistics->avg_ms + ms) / 2;
    }
}