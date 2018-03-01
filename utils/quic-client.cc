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
#include "ns3/address.h"
#include "ns3/address-utils.h"
#include "ns3/log.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/packet-socket-address.h"
#include "ns3/node.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/boolean.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/uinteger.h"
#include "ns3/integer.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/quic-header.h"
#include "ns3/quic-stream-frame.h"
#include "quic-client.h"
#include "net/spdy/core/spdy_header_block.h"
#include "net/quic/platform/api/quic_text_utils.h"

#include "net/tools/quic/quic_simple_client.h"

#include "net/tools/quic/quic_client_message_loop_network_helper.h"
using std::string;
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
using std::stringstream;

namespace ns3 {
  Time QuicClient::last_time;

  namespace {

    using net::ProofVerifier;
    class FakeProofVerifier : public ProofVerifier {
      public:
        net::QuicAsyncStatus VerifyProof(
            const string& /*hostname*/,
            const uint16_t /*port*/,
            const string& /*server_config*/,
            net::QuicVersion /*quic_version*/,
            net::QuicStringPiece /*chlo_hash*/,
            const std::vector<string>& /*certs*/,
            const string& /*cert_sct*/,
            const string& /*signature*/,
            const net::ProofVerifyContext* /*context*/,
            string* /*error_details*/,
            std::unique_ptr<net::ProofVerifyDetails>* /*details*/,
            std::unique_ptr<net::ProofVerifierCallback> /*callback*/) override {
          return net::QUIC_SUCCESS;
        }

        net::QuicAsyncStatus VerifyCertChain(
            const std::string& /*hostname*/,
            const std::vector<std::string>& /*certs*/,
            const net::ProofVerifyContext* /*verify_context*/,
            std::string* /*error_details*/,
            std::unique_ptr<net::ProofVerifyDetails>* /*verify_details*/,
            std::unique_ptr<net::ProofVerifierCallback> /*callback*/) override {
          return net::QUIC_SUCCESS;
        }
    };

  } // namespace

  NS_LOG_COMPONENT_DEFINE ("QuicClient");

  NS_OBJECT_ENSURE_REGISTERED (QuicClient);

  TypeId
    QuicClient::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::QuicClient")
        .SetParent<Application> ()
        .SetGroupName("Applications")
        .AddConstructor<QuicClient> ()
        .AddAttribute ("Protocol",
            "The type id of the protocol to use for the rx socket.",
            TypeIdValue (UdpSocketFactory::GetTypeId ()),
            MakeTypeIdAccessor (&QuicClient::m_tid),
            MakeTypeIdChecker ())
        .AddAttribute ("ServerAddress",
            "The Address on which to connect the rx socket.",
            AddressValue (),
            MakeAddressAccessor (&QuicClient::m_serverAddress),
            MakeAddressChecker ())
        .AddAttribute ("ZeroRttHandshake",
            "Whether or not to initiate a 0-RTT connection.",
            BooleanValue (true),
            MakeBooleanAccessor (&QuicClient::m_zeroRtt),
            MakeBooleanChecker ())
        .AddTraceSource ("Rx",
            "A packet has been received",
            MakeTraceSourceAccessor (&QuicClient::m_rxTrace),
            "ns3::Packet::AddressTracedCallback")
        .AddAttribute  ("MaxBytes",
            "Number of bytes to send.",
            UintegerValue (10),
            ns3::MakeUintegerAccessor (&QuicClient::m_maxBytes),
            ns3::MakeUintegerChecker<unsigned> (uint64_t(1)))
         .AddAttribute ("maxPacketSize",
           "MaximumPacketSize in QUIC Protocol",
            UintegerValue (10),
            ns3::MakeUintegerAccessor (&QuicClient::m_maxPacketSize),
            ns3::MakeUintegerChecker<unsigned> ())
        ;
      return tid;
    }

  QuicClient::QuicClient ()
  {
    NS_LOG_FUNCTION (this);
    m_socket = 0;
    m_totalRx = 0;
  }

  QuicClient::~QuicClient ()
  {
    NS_LOG_FUNCTION (this);
  }

  uint64_t QuicClient::GetTotalRx () const
  {
    NS_LOG_FUNCTION (this);
    return m_totalRx;
  }

  Ptr<Socket>
    QuicClient::GetListeningSocket (void) const
    {
      NS_LOG_FUNCTION (this);
      return m_socket;
    }

  void QuicClient::DoDispose (void)
  {
    NS_LOG_FUNCTION (this);
    m_socket = 0;

    // chain up
    Application::DoDispose ();
  }

  base::AtExitManager *QuicClient::exit_manager = nullptr;
  base::MessageLoopForIO *QuicClient::message_loop = nullptr;

  // Application Methods
  void QuicClient::StartApplication ()    // Called at time specified by Start
  {
    NS_LOG_FUNCTION (this);
    //cerr << "Client Start (MaxBytes " << m_maxBytes << ")" << endl;
    cur_state = CONNECT_LOOP;

    kCurNode = GetNode();

    if(!exit_manager) exit_manager = new base::AtExitManager;
    if(!message_loop) message_loop = new base::MessageLoopForIO;



    net::QuicIpAddress ip_addr;
    uint8_t buffer_tmp[Address::MAX_SIZE];
    int len = m_serverAddress.CopyTo(buffer_tmp);
    stringstream str;
    for(int i = 0; i < 4; i++) {
      str << int(buffer_tmp[i]);
      if(i < 3) str << '.';
    }
    string buffer = str.str();

    if(!ip_addr.FromString(buffer)) {
      exit(13);
    }

    InetSocketAddress addr = InetSocketAddress::ConvertFrom(m_serverAddress);

    net::QuicServerId server_id(buffer, addr.GetPort(), net::PRIVACY_MODE_DISABLED);

    net::QuicVersionVector versions = net::AllSupportedVersions();
    std::unique_ptr<ProofVerifier> proof_verifier;
    proof_verifier.reset(new FakeProofVerifier());

    client = new net::QuicSimpleClient(net::QuicSocketAddress(ip_addr, addr.GetPort()), server_id, versions, std::move(proof_verifier));
    client->set_initial_max_packet_length(net::kDefaultMaxPacketSize); // aghax
    std::cout<<net::kDefaultMaxPacketSize<<std::endl;
    //client->set_initial_max_packet_length(m_maxPacketSize); // aghax

    if(!client->Initialize()) {
      cerr << "FAIL" << endl;
      exit(1);
    }
    dynamic_cast<net::QuicClientMessageLooplNetworkHelper*>(client->network_helper())->packet_reader_->client_ = this;


    client->Connect();
  }

  void QuicClient::StopApplication ()     // Called at time specified by Stop
  {
    NS_LOG_FUNCTION (this);
    if (m_socket)
    {
      m_socket->Close ();
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }
  }

  void QuicClient::SendRequest() {
    using net::SpdyHeaderBlock;
    // Construct the string body from flags, if provided.
    string body = "";

    // Construct a GET or POST request for supplied URL.
    SpdyHeaderBlock header_block;
    header_block["max_bytes"] = net::QuicTextUtils::Uint64ToString(m_maxBytes);


    // Make sure to store the response, for later output.
    client->set_store_response(true);

    // Send the request.
    if(client->SendRequestAndWaitForResponse(header_block, body, /*fin=*/true))
      cur_state = SEND_REQUEST;
    else
      cur_state = AFTER;
  }

  void QuicClient::HandleRead (Ptr<Socket> socket)
  {
    std::default_random_engine generator;
    std::poisson_distribution<int> distribution(m_maxPacketSize);
    socket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
    NS_LOG_FUNCTION (this << socket);
    //cerr << "QuicClient::HandleRead()" << endl;
    last_time = Simulator::Now();

    net::QuicChromiumPacketReader *pktrd = dynamic_cast<net::QuicClientMessageLooplNetworkHelper*>(client->network_helper())->packet_reader_.get();

    int ret = socket->Recv(reinterpret_cast<uint8_t*>(pktrd->read_buffer_->data()), pktrd->read_buffer_->size(), 0);
    //cerr << "Read " << ret << " bytes (return val " << ret << ")" << endl;


    pktrd->OnReadComplete(ret);
    if(cur_state == CONNECT_LOOP) {
      if (client->EncryptionBeingEstablished())
        client->WaitForEvents();
      else {
        if (client->ConnectLogic())
          client->Connect();
        else { 
          client->FinishConnect();
          std::cout << m_maxPacketSize << std::endl;
          //Simulator::Schedule(MicroSeconds(m_maxPacketSize), &QuicClient::SendRequest,  this);
          Simulator::Schedule(MicroSeconds(distribution(generator)), &QuicClient::SendRequest,  this);
          //SendRequest();
        }
      }
    } else if(cur_state == SEND_REQUEST) {
      if(!client->WaitForEvents())
        cur_state = AFTER;
    }
  }

  void
    QuicClient::HandleSucessfulConnection (Ptr<Socket> socket)
    {
      NS_LOG_FUNCTION (this << socket);
      NS_LOG_INFO ("Connection suceeded. Time to start the QUIC handshake.");

      Ptr<Packet> packet = Create<Packet> ();

      QuicStreamFrame streamFrame;
      streamFrame.SetFrameType (193);
      streamFrame.SetStreamId (0);
      streamFrame.SetOffset (0);
      streamFrame.SetDataLength (1);
      streamFrame.SetStreamData (1);
      packet->AddHeader (streamFrame);

      LongQuicHeader longHeader;
      if (m_zeroRtt)
      {
        NS_LOG_INFO ("0-RTT Handshake.");
        longHeader.SetLongPacketType (LongQuicHeader::CCLT);
      }
      else
      {
        NS_LOG_INFO ("1-RTT Handshake.");
        longHeader.SetLongPacketType (LongQuicHeader::CLIN);
      }
      packet->AddHeader (longHeader);

      m_socket->Send (packet);
    }

  void
    QuicClient::HandleFailedConnection (Ptr<Socket> socket)
    {
      NS_LOG_FUNCTION (this << socket);
      NS_LOG_INFO ("Connection failed. There is nothing left to do.");
    }

} // Namespace ns3

