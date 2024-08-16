/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/11 19:00:00 by tdameros          #+#    #+#             */
/*   Updated: 2024/08/11 19:00:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_H
# define SOCKET_H

int32_t create_icmp_socket(void);
int32_t set_icmp_socket_timeout(int32_t socketfd, uint32_t seconds, uint32_t microseconds);
int32_t resolve_host(char *hostname, struct sockaddr_in *address);
#endif