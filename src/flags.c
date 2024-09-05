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
#include "flags.h"

#define ERROR_MSG_TRY_HELP "Try 'ft_ping --help' or 'ft_ping --usage' for more information."


int8_t parse_flags(int argc, char *argv[], flags_t *flags) {
    int opt;
    int option_index = 0;

    struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"count", required_argument, 0, 'c'},
        {"verbose", no_argument, 0, 'v'},
//        {"interval", required_argument, 0, 'i'},
//        {"timeout", required_argument, 0, 't'},
//        {"debug", no_argument, 0, 'd'
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "hvc:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                printf("Usage: %s [options]\n", argv[0]);
                printf("  -h, --help          Show this help message\n");
                printf("  -c, --count <num>   Set the count to <num>\n");
                printf("  -i, --interval <sec> Set the interval to <sec>\n");
                printf("  -t, --timeout <sec> Set the timeout to <sec>\n");
                return 0;
            case 'c':
                flags->options.count = true;
                flags->count_value = atoi(optarg);
                break;
            case 'v':
                flags->options.verbose = true;
                break;
            default:
                fprintf(stderr, ERROR_MSG_TRY_HELP"\n");
                return -1;
        }
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

