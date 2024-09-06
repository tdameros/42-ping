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

#include <math.h>

#include "ping.h"

void statistics_init(ping_statistics_t *statistics) {
  statistics->packets_transmitted = 0;
  statistics->packets_received = 0;
  statistics->min_ms = 0;
  statistics->max_ms = 0;
  statistics->sum_ms = 0;
}

void statistics_packet_transmit(ping_statistics_t *statistics) {
  statistics->packets_transmitted++;
}

void statistics_packet_receive(ping_statistics_t *statistics, double ms) {
  if (statistics->packets_received == 0) {
    statistics->min_ms = ms;
    statistics->max_ms = ms;
  } else {
    if (ms < statistics->min_ms) {
      statistics->min_ms = ms;
    }
    if (ms > statistics->max_ms) {
      statistics->max_ms = ms;
    }
  }
  statistics->sum_ms += ms;
  statistics->sum_squared_ms += ms * ms;
  statistics->packets_received++;
}

double statistics_get_average(const ping_statistics_t *statistics) {
  if (statistics->packets_received == 0) {
    return 0;
  }
  return statistics->sum_ms / statistics->packets_received;
}

double statistics_get_stddev(const ping_statistics_t *statistics) {
  if (statistics->packets_received == 0) {
    return 0;
  }
  double mean = statistics_get_average(statistics);
  double variance = statistics->sum_squared_ms / statistics->packets_received - mean * mean;
  return sqrt(variance);
}
