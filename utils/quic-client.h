/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2017 CCSL IME-USP
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
 * Author:  Diego Araújo <diegoamc@ime.usp.br>
 */

#ifndef QUIC_CLIENT_H
#define QUIC_CLIENT_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"
#include "ns3/address.h"
#include <random>

#include "base/at_exit.h"
#include "base/message_loop/message_loop.h"

namespace net {
  class QuicSimpleClient;
}
using net::QuicSimpleClient;

namespace ns3 {

class Address;
class Socket;
class Packet;

enum state {
	CONNECT_LOOP,
	SEND_REQUEST,
	AFTER
};

/**
 * \ingroup applications
 * \defgroup quicclient QuicClient
 *
 * A client that requests bytes from a server using the QUIC protocol as
 * transport.
 *
 */

/**
 * \ingroup quicclient
 *
 * \brief A client that requests bytes from a server using the QUIC protocol
 * as transport.
 *
 * TODO: Rewrite this once things are working
 *
 */
class QuicClient : public Application
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  QuicClient ();

  virtual ~QuicClient ();

  /**
   * \return the total bytes received by the client
   */
  uint64_t GetTotalRx () const;

  /**
   * \return pointer to listening socket
   */
  Ptr<Socket> GetListeningSocket (void) const;

  net::QuicSimpleClient *client;
  /**
   * \brief Handle a packet received by the application
   * \param socket the receiving socket
   */
  void HandleRead (Ptr<Socket> socket);

protected:
  virtual void DoDispose (void);
private:
  // inherited from Application base class.
  virtual void StartApplication (void);    // Called at time specified by Start
  virtual void StopApplication (void);     // Called at time specified by Stop

  /**
   * \brief Handle a successful connection
   * \param socket the connected socket
   */
  void HandleSucessfulConnection (Ptr<Socket> socket);
  /**
   * \brief Handle a failed connection
   * \param socket the connected socket
   */
  void HandleFailedConnection (Ptr<Socket> socket);

  Ptr<Socket> m_socket;         //!< Listening socket

  Address     m_serverAddress;  //!< Server address
  uint64_t    m_totalRx;        //!< Total bytes received
  TypeId      m_tid;            //!< Protocol TypeId
  bool        m_zeroRtt;        //!< 0-RTT flag
  unsigned    m_maxBytes;
  
  // aghax
  uint64_t    m_maxPacketSize;



  /// Traced Callback: received packets, source address.
  TracedCallback<Ptr<const Packet>, const Address &> m_rxTrace;

  state cur_state;
  void SendRequest();
public:
  static base::AtExitManager *exit_manager;
  static base::MessageLoopForIO *message_loop;
  static Time last_time;


};

} // namespace ns3

#endif /* QUIC_CLIENT_H */


