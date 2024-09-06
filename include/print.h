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
#include "flags.h"

void print_ping_start(const icmp_ping_t *ping, const flags_t *flags);
void print_ping_result(const ping_result_t *result, const flags_t *flags);
void print_ping_statistics(const icmp_ping_t *ping);
void print_help(char *binary_name);
void print_usage(char *binary_name);

#endif