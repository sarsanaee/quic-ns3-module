#ifndef SOCKET_NS3_H
#define SOCKET_NS3_H

/* Declarations of socket constants, types, and functions.
   Copyright (C) 1991-2016 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <sys/socket.h>
#ifdef __cplusplus
#include "ns3/node.h"
#include "ns3/ptr.h"
#include "ns3/socket.h"
extern "C" {
extern ns3::Ptr<ns3::Node> kCurNode;
#endif


int socket_ns3 (int __domain, int __type, int __protocol);
int socketpair_ns3 (int __domain, int __type, int __protocol, int __fds[2]);
int bind_ns3 (int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len);
int getsockname_ns3 (int __fd, __SOCKADDR_ARG __addr, socklen_t *__restrict __len);
int connect_ns3 (int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len);
int getpeername_ns3 (int __fd, __SOCKADDR_ARG __addr, socklen_t *__restrict __len);
ssize_t send_ns3 (int __fd, const void *__buf, size_t __n, int __flags);
ssize_t recv_ns3 (int __fd, void *__buf, size_t __n, int __flags);
ssize_t sendto_ns3 (int __fd, const void *__buf, size_t __n,
int __flags, __CONST_SOCKADDR_ARG __addr, socklen_t __addr_len);
ssize_t recvfrom_ns3 (int __fd, void *__restrict __buf, size_t __n,
int __flags, __SOCKADDR_ARG __addr, socklen_t *__restrict __addr_len);
ssize_t sendmsg_ns3 (int __fd, const struct msghdr *__message, int __flags);
ssize_t recvmsg_ns3 (int __fd, struct msghdr *__message, int __flags);
int getsockopt_ns3 (int __fd, int __level, int __optname,
void *__restrict __optval, socklen_t *__restrict __optlen);
int setsockopt_ns3 (int __fd, int __level, int __optname, const void *__optval, socklen_t __optlen);
int listen_ns3 (int __fd, int __n);
int accept_ns3 (int __fd, __SOCKADDR_ARG __addr, socklen_t *__restrict __addr_len);
int shutdown_ns3 (int __fd, int __how);
int fcntl_ns3(int fd, int cmd);
int fcntls_ns3(int fd, int cmd, int fl);

#ifdef __cplusplus
ns3::Ptr<ns3::Socket> get_socket(int fd);
} // closing brace for extern "C"
#endif
#endif // SOCKET_NS3_H
