/* Copyright (C) 2015-2021, Wazuh Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation
 */

#ifndef NETBUFFER_WRAPPERS_H
#define NETBUFFER_WRAPPERS_H

void __wrap_nb_close(__attribute__((unused)) netbuffer_t * buffer, int sock);

void __wrap_nb_open(__attribute__((unused)) netbuffer_t * buffer, int sock, const struct sockaddr_in * peer_info);

int __wrap_nb_queue(__attribute__((unused)) netbuffer_t * buffer, int socket, char * crypt_msg, ssize_t msg_size);

int __wrap_nb_recv(__attribute__((unused)) netbuffer_t * buffer, int sock);

int __wrap_nb_send(__attribute__((unused)) netbuffer_t * buffer, int sock);

#endif
