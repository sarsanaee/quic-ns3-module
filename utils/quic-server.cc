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

#include "ns3/log.h"
#include "ns3/address.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/quic-header.h"
#include "ns3/quic-stream-frame.h"
#include "quic-server.h"
#include "quic-client.h"

#include <iostream>
using namespace std;

#include "model/base/at_exit.h"
#include "model/base/command_line.h"
#include "model/base/logging.h"
#include "model/base/message_loop/message_loop.h"
#include "model/base/run_loop.h"
#include "model/base/strings/string_number_conversions.h"
#include "model/net/base/ip_address.h"
#include "model/net/base/ip_endpoint.h"
#include "model/net/quic/chromium/crypto/proof_source_chromium.h"
#include "model/net/quic/core/quic_packets.h"
#include "model/net/tools/quic/quic_http_response_cache.h"
#include "model/net/tools/quic/quic_simple_server.h"
#include "model/net/base/ip_address.h"
#include "model/net/base/ip_endpoint.h"
using net::IPAddress;
using net::IPEndPoint;

// The port the quic server will listen on.
int32_t FLAGS_port = 6121;

std::unique_ptr<net::ProofSource> CreateProofSource() {
  std::unique_ptr<net::ProofSourceChromium> proof_source(
      new net::ProofSourceChromium());
  CHECK(proof_source->Initialize(base::FilePath("src/quic/model/net/tools/quic/certs/out/leaf_cert.pem"), base::FilePath("src/quic/model/net/tools/quic/certs/out/leaf_cert.pkcs8"), base::FilePath()));
  return std::move(proof_source);
}


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("QuicServer");

NS_OBJECT_ENSURE_REGISTERED (QuicServer);

TypeId
QuicServer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::QuicServer")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<QuicServer> ()
    .AddAttribute ("SendSize", "The amount of data to send each time.",
                   UintegerValue (512),
                   MakeUintegerAccessor (&QuicServer::m_sendSize),
                   MakeUintegerChecker<uint32_t> (1))
    .AddAttribute ("Local",
                   "The Address on which to Bind the rx socket.",
                   AddressValue (),
                   MakeAddressAccessor (&QuicServer::m_local),
                   MakeAddressChecker ())
    .AddAttribute ("MaxBytes",
                   "The total number of bytes to send. "
                   "Once these bytes are sent, "
                   "no data  is sent again. The value zero means "
                   "that there is no limit.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&QuicServer::m_maxBytes),
                   MakeUintegerChecker<uint64_t> ())
    .AddAttribute ("Protocol", "The type of protocol to use.",
                   TypeIdValue (UdpSocketFactory::GetTypeId ()),
                   MakeTypeIdAccessor (&QuicServer::m_tid),
                   MakeTypeIdChecker ())
    .AddTraceSource ("Tx", "A new packet is created and is sent",
                     MakeTraceSourceAccessor (&QuicServer::m_txTrace),
                     "ns3::Packet::TracedCallback")
  ;
  return tid;
}


QuicServer::QuicServer ()
  : m_socket (0),
    m_connected (false),
    m_totBytes (0)
{
  NS_LOG_FUNCTION (this);
}

QuicServer::~QuicServer ()
{
  NS_LOG_FUNCTION (this);
}

void
QuicServer::SetMaxBytes (uint64_t maxBytes)
{
  NS_LOG_FUNCTION (this << maxBytes);
  m_maxBytes = maxBytes;
}

Ptr<Socket>
QuicServer::GetSocket (void) const
{
  NS_LOG_FUNCTION (this);
  return m_socket;
}

void
QuicServer::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_socket = 0;
  // chain up
  Application::DoDispose ();
}

// Application Methods
void QuicServer::StartApplication (void) // Called at time specified by Start
{
  NS_LOG_FUNCTION (this);
  //cerr << "Server start" << endl;

  kCurNode = GetNode();

  if(!QuicClient::exit_manager) QuicClient::exit_manager = new base::AtExitManager;
  if(!QuicClient::message_loop) QuicClient::message_loop = new base::MessageLoopForIO;

  net::QuicHttpResponseCache response_cache;

  net::IPAddress ip = net::IPAddress::IPv6AllZeros();

  net::QuicConfig config;
  server = new net::QuicSimpleServer(
      CreateProofSource(),
      config, net::QuicCryptoServerConfig::ConfigOptions(),
      net::AllSupportedVersions(), &response_cache);

  server->server_ = this;

  int rc = server->Listen(net::IPEndPoint(ip, FLAGS_port));
  if (rc < 0) {
    return;
  }

  //cerr << "Server End" << endl;
}

void QuicServer::StopApplication (void) // Called at time specified by Stop
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0)
    {
      m_socket->Close ();
      m_connected = false;
    }
  else
    {
      NS_LOG_WARN ("QuicServer found null socket to close in StopApplication");
    }
}


// Private helpers

void QuicServer::SendData (void)
{
  NS_LOG_FUNCTION (this);

  while (m_maxBytes == 0 || m_totBytes < m_maxBytes)
    { // Time to send more

      // uint64_t to allow the comparison later.
      // the result is in a uint32_t range anyway, because
      // m_sendSize is uint32_t.
      uint64_t toSend = m_sendSize;
      // Make sure we don't send too many
      if (m_maxBytes > 0)
        {
          toSend = std::min (toSend, m_maxBytes - m_totBytes);
        }

      NS_LOG_LOGIC ("sending packet at " << Simulator::Now ());

      Ptr<Packet> packet = Create<Packet> (toSend);
      QuicStreamFrame streamFrame;
      streamFrame.SetFrameType (193);
      streamFrame.SetStreamId (2);
      streamFrame.SetOffset (0);
      streamFrame.SetDataLength (1);
      streamFrame.SetStreamData (1);
      packet->AddHeader (streamFrame);

      ShortQuicHeader shortHeader;
      packet->AddHeader (shortHeader);

      int actual = m_socket->SendTo (packet, 0, m_from);
      if (actual > 0)
        {
          m_totBytes += actual;
          m_txTrace (packet);
        }
      // We exit this loop when actual < toSend as the send side
      // buffer is full. The "DataSent" callback will pop when
      // some buffer space has freed ip.
      if ((unsigned)actual != toSend)
        {
          break;
        }
    }
  // Check if time to close (all sent)
  if (m_totBytes == m_maxBytes && m_connected)
    {
      m_socket->Close ();
      m_connected = false;
    }
}

void QuicServer::HandleRead (Ptr<Socket> s)
{
  s->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
  NS_LOG_FUNCTION (this << s);
  NS_LOG_INFO ("Received packet.");
  //cerr << "\nServer::HandleRead()" << endl;
  QuicClient::last_time = Simulator::Now();


  Address ad;
  //Ptr<Packet> pk = s->RecvFrom(ad);
  //if(!pk) return server->OnReadComplete(0);
  int ret = s->RecvFrom(reinterpret_cast<uint8_t*>(server->read_buffer_->data()), server->read_buffer_->size(), 0, ad);
  //cerr << "Read " << ret << " bytes" << endl;

  InetSocketAddress ip = InetSocketAddress::ConvertFrom(ad);
  uint8_t buf[Address::MAX_SIZE];
  int len = ad.CopyTo(buf);
  stringstream ss;
  ss << ip.GetIpv4();
  IPAddress ipp;
  assert(ipp.AssignFromIPLiteral(ss.str()));
  server->client_address_ = IPEndPoint(ipp, InetSocketAddress::ConvertFrom(ad).GetPort());

  server->OnReadComplete(ret);
  //cerr << "QuicServer::HandleRead() FINISHED\n" << endl;
}

} // Namespace ns3
