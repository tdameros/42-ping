/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flags.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 22:12:00 by tdameros          #+#    #+#             */
/*   Updated: 2024/08/27 22:12:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#include "flags.h"

#define ERROR_MSG_TRY_HELP "Try 'ft_ping --help' or 'ft_ping --usage' for more information."

static int8_t convert_arg_to_u32(const char *str, uint32_t *result);

int8_t parse_flags(int argc, char *argv[], flags_t *flags) {
    int opt;
    int option_index = 0;

    struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"verbose", no_argument, 0, 'v'},
        {"count", required_argument, 0, 'c'},
        {"flood", no_argument, 0, 'f'},
        {"interval", required_argument, 0, 'i'},
        {"timeout", required_argument, 0, 'w'},
        {"linger", required_argument, 0, 'W'},
        {"debug", no_argument, 0, 'd'},
        {0, 0, 0, 0}
    };
    flags->interval_value = DEFAULT_INTERVAL;
    flags->linger_value = DEFAULT_LINGER;

    while ((opt = getopt_long(argc, argv, "hvc:fi:w:W:d", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                printf("Usage: %s [options]\n", argv[0]);
                printf("  -h, --help            Show this help message\n");
                printf("  -c, --count=NUMBER    Stop after sending NUMBER packets\n");
                printf("  -i, --interval=NUMBER Wait NUMBER seconds between sending each packet\n");
                return 0;
            case 'c':
                if (convert_arg_to_u32(optarg, &flags->count_value) < 0) {
                    printf("ft_ping: invalid value (`%s')\n", optarg);
                    return -1;
                }
                flags->options.count = true;
                break;
            case 'v':
                flags->options.verbose = true;
                break;
            case 'f':
                flags->options.flood = true;
                break;
            case 'i':
                if (convert_arg_to_u32(optarg, &flags->interval_value) < 0) {
                    printf("ft_ping: invalid value (`%s')\n", optarg);
                    return -1;
                }
                flags->options.interval = true;
                break;
            case 'w':
                if (convert_arg_to_u32(optarg, &flags->timeout_value) < 0) {
                    printf("ft_ping: invalid value (`%s')\n", optarg);
                    return -1;
                }
                flags->options.timeout = true;
                break;
            case 'W':
                if (convert_arg_to_u32(optarg, &flags->linger_value) < 0) {
                    printf("ft_ping: invalid value (`%s')\n", optarg);
                    return -1;
                }
                flags->linger_value *= 1000;
                flags->options.linger = true;
                break;
            case 'd':
                flags->options.debug = true;
                break;
            default:
                fprintf(stderr, ERROR_MSG_TRY_HELP"\n");
                return -1;
        }
    }
    if (flags->options.flood && flags->options.interval) {
        fprintf(stderr, "ft_ping: -f and -i incompatible options");
    }

    if (optind < argc) {
        flags->hostname = argv[optind];
        return 0;
    } else {
        fprintf(stderr, "ft_ping: missing host operand\n");
        fprintf(stderr, ERROR_MSG_TRY_HELP"\n");
        return -1;
    }
}


static int8_t convert_arg_to_u32(const char *str, uint32_t *result) {
    char *endptr;
    long val;

    errno = 0;
    val = strtol(str, &endptr, 10);
    if (endptr == str || *endptr != '\0' || val < 0) {
        return -1;
    }
    *result = val;
    return 0;
}
