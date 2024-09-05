/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/11 19:00:00 by tdameros          #+#    #+#             */
/*   Updated: 2024/08/11 19:00:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include <sys/time.h>
#include <stddef.h>
#include <netdb.h>
#include <stdbool.h>

#include "socket.h"

int32_t create_icmp_socket(void) {
    return (socket(AF_INET, SOCK_RAW, IPPROTO_ICMP));
}

int32_t set_icmp_socket_timeout(int32_t socketfd, uint32_t seconds, uint32_t microseconds) {
    struct timeval timeout = {seconds, microseconds};
    int32_t return_code;

    return_code = setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    if (return_code < 0) {
        return return_code;
    }
    return setsockopt(socketfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
}

int32_t set_icmp_socket_debug(int32_t socketfd, bool is_debug) {
    return setsockopt(socketfd, SOL_SOCKET, SO_DEBUG, &is_debug, sizeof(is_debug));
}

int32_t resolve_host(char *hostname, struct sockaddr_in *address) {
    struct hostent *host = gethostbyname(hostname);
    if (host == NULL) {
        return -1;
    }
    address->sin_addr = *(struct in_addr *)host->h_addr_list[0];
    return 0;
}