/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 CCSL IME-USP
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Diego Araújo <diegoamc@ime.usp.br>
 */

#ifndef QUIC_L4_PROTOCOL_H
#define QUIC_L4_PROTOCOL_H

#include <stdint.h>

#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/sequence-number.h"
#include "ns3/udp-l4-protocol.h"


namespace ns3 {

class Node;
class Socket;
class QuicHeader;
class Ipv4EndPointDemux;
class Ipv6EndPointDemux;
class Ipv4Interface;
class QuicSocketBase;
class Ipv4EndPoint;
class Ipv6EndPoint;
class NetDevice;


/**
 * \ingroup internet
 * \defgroup quic QUIC
 *
 */

/**
 * \ingroup quic
 * \brief QUIC socket creation and multiplexing/demultiplexing
 *
 * A single instance of this class is held by one instance of class Node.
 *
 * The creation of QuicSocket are handled in the method CreateSocket, which is
 * called by QuicSocketFactory. Upon creation, this class is responsible to
 * the socket initialization and handle multiplexing/demultiplexing of data
 * between node's QUIC sockets. Demultiplexing is done by receiving
 * packets from IP, and forwards them up to the right socket. Multiplexing
 * is done through the SendPacket function, which sends the packet down the stack.
 *
 * Moreover, this class allocates "endpoint" objects (ns3::Ipv4EndPoint) for QUIC,
 * and SHOULD checksum packets its receives from the socket layer going down
 * the stack, but currently checksumming is disabled.
 *
 * \see CreateSocket
 * \see NotifyNewAggregate
 * \see SendPacket
*/

class QuicL4Protocol : public UdpL4Protocol {
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  static const uint8_t PROT_NUMBER; //!< protocol number (0x6)

  QuicL4Protocol ();
  virtual ~QuicL4Protocol ();

  /**
   * Set node associated with this stack
   * \param node the node
   */
  void SetNode (Ptr<Node> node);

  /**
   * \brief Create a QUIC socket using the TypeId set by SocketType attribute
   *
   * \return A smart Socket pointer to a QuicSocket allocated by this instance
   * of the QUIC protocol
   */
  Ptr<Socket> CreateSocket (void);

  /**
   * \brief Create a QUIC socket using the specified congestion control algorithm TypeId
   *
   * \return A smart Socket pointer to a QuicSocket allocated by this instance
   * of the QUIC protocol
   *
   * \warning using a congestionTypeId other than QUIC is a bad idea.
   *
   * \param congestionTypeId the congestion control algorithm TypeId
   */
  Ptr<Socket> CreateSocket (TypeId congestionTypeId);

  /**
   * \brief Allocate an IPv4 Endpoint
   * \return the Endpoint
   */
  Ipv4EndPoint *Allocate (void);
  /**
   * \brief Allocate an IPv4 Endpoint
   * \param address address to use
   * \return the Endpoint
   */
  Ipv4EndPoint *Allocate (Ipv4Address address);
  /**
   * \brief Allocate an IPv4 Endpoint
   * \param port port to use
   * \return the Endpoint
   */
  Ipv4EndPoint *Allocate (uint16_t port);
  /**
   * \brief Allocate an IPv4 Endpoint
   * \param address address to use
   * \param port port to use
   * \return the Endpoint
   */
  Ipv4EndPoint *Allocate (Ipv4Address address, uint16_t port);
  /**
   * \brief Allocate an IPv4 Endpoint
   * \param localAddress local address to use
   * \param localPort local port to use
   * \param peerAddress remote address to use
   * \param peerPort remote port to use
   * \return the Endpoint
   */
  Ipv4EndPoint *Allocate (Ipv4Address localAddress, uint16_t localPort,
                          Ipv4Address peerAddress, uint16_t peerPort);
  /**
   * \brief Allocate an IPv6 Endpoint
   * \return the Endpoint
   */
  Ipv6EndPoint *Allocate6 (void);
  /**
   * \brief Allocate an IPv6 Endpoint
   * \param address address to use
   * \return the Endpoint
   */
  Ipv6EndPoint *Allocate6 (Ipv6Address address);
  /**
   * \brief Allocate an IPv6 Endpoint
   * \param port port to use
   * \return the Endpoint
   */
  Ipv6EndPoint *Allocate6 (uint16_t port);
  /**
   * \brief Allocate an IPv6 Endpoint
   * \param address address to use
   * \param port port to use
   * \return the Endpoint
   */
  Ipv6EndPoint *Allocate6 (Ipv6Address address, uint16_t port);
  /**
   * \brief Allocate an IPv6 Endpoint
   * \param localAddress local address to use
   * \param localPort local port to use
   * \param peerAddress remote address to use
   * \param peerPort remote port to use
   * \return the Endpoint
   */
  Ipv6EndPoint *Allocate6 (Ipv6Address localAddress, uint16_t localPort,
                           Ipv6Address peerAddress, uint16_t peerPort);
  /**
   * \brief Send a packet via QUIC (IP-agnostic)
   *
   * \param pkt The packet to send
   * \param outgoing The packet header
   * \param saddr The source Ipv4Address
   * \param daddr The destination Ipv4Address
   * \param oif The output interface bound. Defaults to null (unspecified).
   */
  void SendPacket (Ptr<Packet> pkt, const QuicHeader &outgoing,
                   const Address &saddr, const Address &daddr,
                   Ptr<NetDevice> oif = 0);

  /**
   * \brief Make a socket fully operational
   *
   * Called after a socket has been bound, it is inserted in an internal vector.
   *
   * \param socket Socket to be added
   */
  void AddSocket (Ptr<QuicSocketBase> socket);

  /**
   * \brief Remove a socket from the internal list
   *
   * \param socket socket to Remove
   * \return true if the socket has been removed
   */
  bool RemoveSocket (Ptr<QuicSocketBase> socket);
  /**
   * \brief Remove an IPv4 Endpoint.
   * \param endPoint the end point to remove
   */
  void DeAllocate (Ipv4EndPoint *endPoint);
  /**
   * \brief Remove an IPv6 Endpoint.
   * \param endPoint the end point to remove
   */
  void DeAllocate (Ipv6EndPoint *endPoint);

  virtual int GetProtocolNumber (void) const;

protected:
  virtual void DoDispose (void);

  /**
   * \brief Setup socket factory and callbacks when aggregated to a node
   *
   * This function will notify other components connected to the node that a
   * new stack member is now connected. This will be used to notify Layer 3
   * protocol of layer 4 protocol stack to connect them together.
   * The aggregation is completed by setting the node in the QUIC stack, link
   * it to the ipv4 or ipv6 stack and adding QUIC socket factory to the node.
   */
  virtual void NotifyNewAggregate ();

private:
  Ptr<Node> m_node;                //!< the node this stack is associated with
  Ipv4EndPointDemux *m_endPoints;  //!< A list of IPv4 end points.
  Ipv6EndPointDemux *m_endPoints6; //!< A list of IPv6 end points.
  TypeId m_rttTypeId;              //!< The RTT Estimator TypeId
  TypeId m_congestionTypeId;       //!< The socket TypeId
  std::vector<Ptr<QuicSocketBase> > m_sockets;      //!< list of sockets

  /**
   * \brief Copy constructor
   *
   * Defined and not implemented to avoid misuse
   */
  QuicL4Protocol (const QuicL4Protocol &);
  /**
   * \brief Copy constructor
   *
   * Defined and not implemented to avoid misuse
   * \returns
   */
  QuicL4Protocol &operator = (const QuicL4Protocol &);
};

} // namespace ns3

#endif /* QUIC_L4_PROTOCOL_H */
