/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flags.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 22:13:00 by tdameros          #+#    #+#             */
/*   Updated: 2024/08/27 22:13:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef FLAGS_H
# define FLAGS_H

#include <stdbool.h>
#include <stdint.h>

#define DEFAULT_INTERVAL 1
#define DEFAULT_LINGER 1000

typedef struct {
    char *hostname;
    uint32_t count_value;
    uint32_t interval_value;
    uint32_t timeout_value;
    uint32_t linger_value;
    struct {
        uint8_t verbose: 1;
        uint8_t help: 1;
        uint8_t count: 1;
        uint8_t flood: 1;
        uint8_t interval: 1;
        uint8_t timeout: 1;
        uint8_t linger: 1;
        uint8_t debug: 1;
    } options;
} flags_t;

int8_t parse_flags(int argc, char *argv[], flags_t *flags);

#endif