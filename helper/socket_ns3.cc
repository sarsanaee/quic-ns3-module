#include "helper/socket_ns3.h"

//
#include "ns3/ptr.h"
#include "ns3/socket.h"
#include "ns3/udp-l4-protocol.h"
#include "ns3/tcp-l4-protocol.h"
#include "ns3/tcp-congestion-ops.h"
#include "../../internet/model/tcp-socket-factory-impl.h"
#include "../../internet/model/udp-socket-factory-impl.h"

#include <iostream>
#include <arpa/inet.h>
#include <vector>
#include <cassert>
#include <cerrno>
#include "fcntl.h"

using namespace ns3;
using namespace std;

Ptr<Node> kCurNode;

std::vector<Ptr<Socket>> socket_map;
std::vector<sockaddr> addr_map;
std::vector<int> len_map;
std::vector<bool> nonblock_map;

int socket_ns3 (int domain, int type, int protocol) {
  Ptr<Socket> new_socket;
  if(type == SOCK_DGRAM) {
    new_socket = Socket::CreateSocket(kCurNode, UdpSocketFactory::GetTypeId());
  } else {
    new_socket = Socket::CreateSocket(kCurNode, TcpSocketFactory::GetTypeId());
  }
  socket_map.push_back(new_socket);
  addr_map.push_back(sockaddr());
  len_map.push_back(-1);
  nonblock_map.push_back(false);
  return int(socket_map.size()) - 1;
}

int socketpair_ns3 (int domain, int type, int protocol, int fds[2]) {
  fds[0] = socket_ns3(domain, type, protocol);
  fds[1] = socket_ns3(domain, type, protocol);
  return 0;
}

namespace {

auto get(int fd) {
  assert(fd >= 0 && fd < int(socket_map.size()));
  return socket_map[fd];
}

Address convert_addr(__CONST_SOCKADDR_ARG addr, socklen_t len) {
  char *ip = inet_ntoa((((struct sockaddr_in *)addr)->sin_addr));
  int port = ntohs(((struct sockaddr_in *)addr)->sin_port);
  Address ad = InetSocketAddress(ip, port);
  return ad;
}

} // namespace

ns3::Ptr<ns3::Socket> get_socket(int fd) {
  return get(fd);
}

// It seems addr is 0.0.0.0 that does not seem right...
int bind_ns3 (int fd, __CONST_SOCKADDR_ARG addr, socklen_t len) {
  auto sckt = get(fd);
  len_map[fd] = len;
  addr_map[fd] = *addr;
  return sckt->Bind(convert_addr(addr, len));
}

int getsockname_ns3 (int fd, __SOCKADDR_ARG addr, socklen_t *__restrict len) {
  auto sckt = get(fd);
  *len = len_map[fd];
  *addr = addr_map[fd];
  return 0;
}

int connect_ns3 (int fd, __CONST_SOCKADDR_ARG addr, socklen_t len) {
  auto sckt = get(fd);
  len_map[fd] = len;
  addr_map[fd] = *addr;
  return sckt->Connect(convert_addr(addr, len));
}

int getpeername_ns3 (int fd, __SOCKADDR_ARG addr, socklen_t *__restrict len) {
  abort();
}

ssize_t send_ns3 (int fd, const void *buf, size_t n, int flags) {
  auto sckt = get(fd);
  uint8_t *bbuf = new uint8_t[n]; // TODO CLEAN
  for(int i = 0; i < n; i++) bbuf[i] = *(((uint8_t*)buf) + i);
  return sckt->Send(bbuf, n, flags);
}

ssize_t recv_ns3 (int fd, void *buf, size_t n, int flags) {
  auto sckt = get(fd);
  return sckt->Recv(reinterpret_cast<uint8_t*>(buf), n, flags);
}

ssize_t sendto_ns3 (int fd, const void *buf, size_t n, int flags, __CONST_SOCKADDR_ARG addr, socklen_t len) {
  if(addr == NULL) return send_ns3(fd, buf, n, flags);
  auto sckt = get(fd);
  uint8_t *bbuf = new uint8_t[n]; // TODO CLEAN
  for(int i = 0; i < n; i++) bbuf[i] = *(((uint8_t*)buf) + i);
  return sckt->SendTo(bbuf, n, flags, convert_addr(addr, len));
}

ssize_t recvfrom_ns3 (int fd, void *__restrict buf, size_t n, int flags, __SOCKADDR_ARG addr, socklen_t *__restrict len) {
  assert(false);
}

ssize_t sendmsg_ns3 (int fd, const struct msghdr *message, int flags) {
  abort();
}

ssize_t recvmsg_ns3 (int fd, struct msghdr *message, int flags) {
  abort();
}

int getsockopt_ns3 (int fd, int level, int optname,
    void *__restrict optval,
    socklen_t *__restrict optlen) {
  abort();
}

// quic_socket_utils tries to set options: receive buffer size, overflow detection, send buffer size among others
// I think NS3 sockets only have support for setting the receive buffer. And it is a private function.
int setsockopt_ns3 (int fd, int level, int optname, const void *optval, socklen_t optlen)  {
  return 0;
}

int listen_ns3 (int fd, int n)  {
  abort();
}

int accept_ns3 (int fd, __SOCKADDR_ARG addr, socklen_t *__restrict addr_len) {
  abort();
}

int shutdown_ns3 (int fd, int how) {
  abort();
}

int fcntl_ns3(int fd, int cmd) {
  assert(cmd == F_GETFL);
  return nonblock_map[fd];
}

int fcntls_ns3(int fd, int cmd, int fl) {
  assert(cmd == F_SETFL);
  assert(fl & O_NONBLOCK);
  nonblock_map[fd] = true;
  return 0;
}
