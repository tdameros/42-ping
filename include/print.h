/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/18 14:13:00 by tdameros          #+#    #+#             */
/*   Updated: 2024/08/18 14:13:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef PRINT_H
# define PRINT_H

#include <netinet/ip_icmp.h>

#include "ping.h"

void print_ping_start(icmp_ping_t *ping);
void print_ping_result(ping_result_t *result);
void print_ping_statistics(icmp_ping_t *ping);

#endif