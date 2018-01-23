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

#define __STDC_LIMIT_MACROS

#include "ns3/object.h"
#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/nstime.h"
#include "quic-socket.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("QuicSocket");

NS_OBJECT_ENSURE_REGISTERED (QuicSocket);

const char* const
QuicSocket::QuicStateName[QuicSocket::LAST_STATE] = { "CLOSED", "LISTEN", "SYN_SENT",
                                        "SYN_RCVD", "ESTABLISHED", "CLOSE_WAIT",
                                        "LAST_ACK", "FIN_WAIT_1", "FIN_WAIT_2",
                                        "CLOSING", "TIME_WAIT" };

TypeId
QuicSocket::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::QuicSocket")
    .SetParent<UdpSocket> ()
    .SetGroupName ("Internet")
    .AddAttribute ("SndBufSize",
                   "QuicSocket maximum transmit buffer size (bytes)",
                   UintegerValue (131072), // 128k
                   MakeUintegerAccessor (&QuicSocket::GetSndBufSize,
                                         &QuicSocket::SetSndBufSize),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("SegmentSize",
                   "QUIC maximum segment size in bytes (may be adjusted based on MTU discovery)",
                   UintegerValue (536),
                   MakeUintegerAccessor (&QuicSocket::GetSegSize,
                                         &QuicSocket::SetSegSize),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("InitialSlowStartThreshold",
                   "QUIC initial slow start threshold (bytes)",
                   UintegerValue (UINT32_MAX),
                   MakeUintegerAccessor (&QuicSocket::GetInitialSSThresh,
                                         &QuicSocket::SetInitialSSThresh),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("InitialCwnd",
                   "QUIC initial congestion window size (segments)",
                   UintegerValue (1),
                   MakeUintegerAccessor (&QuicSocket::GetInitialCwnd,
                                         &QuicSocket::SetInitialCwnd),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("ConnTimeout",
                   "QUIC retransmission timeout when opening connection (seconds)",
                   TimeValue (Seconds (3)),
                   MakeTimeAccessor (&QuicSocket::GetConnTimeout,
                                     &QuicSocket::SetConnTimeout),
                   MakeTimeChecker ())
    .AddAttribute ("ConnCount",
                   "Number of connection attempts (SYN retransmissions) before "
                   "returning failure",
                   UintegerValue (6),
                   MakeUintegerAccessor (&QuicSocket::GetSynRetries,
                                         &QuicSocket::SetSynRetries),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("DataRetries",
                   "Number of data retransmission attempts",
                   UintegerValue (6),
                   MakeUintegerAccessor (&QuicSocket::GetDataRetries,
                                         &QuicSocket::SetDataRetries),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("DelAckTimeout",
                   "Timeout value for QUIC delayed acks, in seconds",
                   TimeValue (Seconds (0.2)),
                   MakeTimeAccessor (&QuicSocket::GetDelAckTimeout,
                                     &QuicSocket::SetDelAckTimeout),
                   MakeTimeChecker ())
    .AddAttribute ("DelAckCount",
                   "Number of packets to wait before sending a QUIC ack",
                   UintegerValue (2),
                   MakeUintegerAccessor (&QuicSocket::GetDelAckMaxCount,
                                         &QuicSocket::SetDelAckMaxCount),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("TcpNoDelay", "Set to true to disable Nagle's algorithm",
                   BooleanValue (true),
                   MakeBooleanAccessor (&QuicSocket::GetTcpNoDelay,
                                        &QuicSocket::SetTcpNoDelay),
                   MakeBooleanChecker ())
    .AddAttribute ("PersistTimeout",
                   "Persist timeout to probe for rx window",
                   TimeValue (Seconds (6)),
                   MakeTimeAccessor (&QuicSocket::GetPersistTimeout,
                                     &QuicSocket::SetPersistTimeout),
                   MakeTimeChecker ())
  ;
  return tid;
}

QuicSocket::QuicSocket ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

QuicSocket::~QuicSocket ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

} // namespace ns3
