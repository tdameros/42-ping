/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 23:24:00 by tdameros          #+#    #+#             */
/*   Updated: 2024/09/05 23:24:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>

uint32_t get_current_time_in_seconds(void) {
  struct timeval current_time;
  gettimeofday(&current_time, NULL);
  return current_time.tv_sec;
}

double get_current_time_in_ms() {
  struct timeval current_time;
  gettimeofday(&current_time, NULL);
  return ((double)current_time.tv_sec) * 1000.0 + ((double)current_time.tv_usec) / 1000.0;
}
