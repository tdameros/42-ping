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

typedef struct {
    char *hostname;
    uint32_t count_value;
    struct {
        uint8_t verbose: 1;
        uint8_t help: 1;
        uint8_t count: 1;
        uint8_t flood: 1;
        uint8_t interval: 1;
        uint8_t timeout: 1;
        uint8_t debug: 1;
        uint8_t _reserved: 1;
    } options;
} flags_t;

int8_t parse_flags(int argc, char *argv[], flags_t *flags);

#endif