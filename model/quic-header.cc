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

#include "quic-header.h"
#include "ns3/address-utils.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (QuicHeader);

QuicHeader::QuicHeader (uint64_t connectionId, uint32_t packetNumber, bool headerForm)
  : m_connectionId (connectionId),
    m_packetNumber (packetNumber),
    m_headerForm   (headerForm)
{
}

QuicHeader::QuicHeader ()
{
}

QuicHeader::~QuicHeader ()
{
}

void
QuicHeader::SetConnectionId (uint64_t connectionId)
{
  m_connectionId = connectionId;
}

void
QuicHeader::SetPacketNumber (uint32_t packetNumber)
{
  m_packetNumber = packetNumber;
}

uint64_t
QuicHeader::GetConnectionId (void) const
{
  return m_connectionId;
}

uint32_t
QuicHeader::GetPacketNumber (void) const
{
  return m_packetNumber;
}

TypeId
QuicHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::QuicHeader")
    .SetParent<Header> ()
    .SetGroupName ("Internet")
    .AddConstructor<QuicHeader> ()
  ;
  return tid;
}

TypeId
QuicHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
QuicHeader::Print (std::ostream &os) const
{
  os << "header form: " << m_headerForm
     << " "
     << "connection ID: " << m_connectionId
     << " "
     << "packet number: " << m_packetNumber
  ;
}

uint32_t
QuicHeader::GetSerializedSize (void) const
{
  /* Arbitrary number for now */
  return 42;
}

bool
QuicHeader::GetHeaderForm (void) const
{
  return m_headerForm;
}

void
QuicHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteHtonU64 (m_connectionId);
  i.WriteHtonU32 (m_packetNumber);
  i.WriteU16 (m_headerForm);
}

uint32_t
QuicHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_connectionId = i.ReadNtohU64 ();
  m_packetNumber = i.ReadNtohU32 ();
  m_headerForm = i.ReadU16 ();

  return GetSerializedSize ();
}

LongQuicHeader::LongQuicHeader ()
  : QuicHeader (true, 0, 0),
    m_version (0x00000001),
    m_longPacketType (0)
{
}

LongQuicHeader::~LongQuicHeader ()
{
}

void
LongQuicHeader::SetLongPacketType (uint8_t longPacketType)
{
  if (longPacketType < 1 || longPacketType > 9)
    {
      m_longPacketType = 0;
    }
  else
    {
      m_longPacketType = longPacketType;
    }
}

uint8_t
LongQuicHeader::GetLongPacketType (void) const
{
  return m_longPacketType;
}

TypeId
LongQuicHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LongQuicHeader")
    .SetParent<QuicHeader> ()
    .SetGroupName ("Internet")
    .AddConstructor<LongQuicHeader> ()
  ;
  return tid;
}

TypeId
LongQuicHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
LongQuicHeader::Print (std::ostream &os) const
{
  os << "header form: " << m_headerForm
     << " "
     << "connection ID: " << m_connectionId
     << " "
     << "packet number: " << m_packetNumber
     << " "
     << "long packet type: " << m_longPacketType
     << " "
     << "version: " << m_version
  ;
}

uint32_t
LongQuicHeader::GetSerializedSize (void) const
{
  return 8+4+2+4+2;
}

void
LongQuicHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteHtonU64 (m_connectionId);
  i.WriteHtonU32 (m_packetNumber);
  i.WriteU16 (m_headerForm);
  i.WriteHtonU32 (m_version);
  i.WriteHtonU16 (m_longPacketType);
}

uint32_t
LongQuicHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_connectionId = i.ReadNtohU64 ();
  m_packetNumber = i.ReadNtohU32 ();
  m_headerForm = i.ReadU16 ();
  m_version = i.ReadNtohU32 ();
  m_longPacketType = i.ReadNtohU16 ();

  return GetSerializedSize ();
}

ShortQuicHeader::ShortQuicHeader ()
  : QuicHeader (false, 0, 0),
    m_connectionIdFlag (false),
    m_keyPhaseBit (false),
    m_shortPacketType (0)
{
}

ShortQuicHeader::~ShortQuicHeader ()
{
}

void
ShortQuicHeader::SetConnectionIdFlag (bool connectionIdFlag)
{
  m_connectionIdFlag = connectionIdFlag;
}

void
ShortQuicHeader::SetKeyPhaseBit (bool keyPhaseBit)
{
  m_keyPhaseBit = keyPhaseBit;
}

void
ShortQuicHeader::SetShortPacketType (uint8_t shortPacketType)
{
  m_shortPacketType = shortPacketType;
}

bool
ShortQuicHeader::GetConnectionIdFlag (void) const
{
  return m_connectionIdFlag;
}

bool
ShortQuicHeader::GetKeyPhaseBit (void) const
{
  return m_keyPhaseBit;
}

uint8_t
ShortQuicHeader::GetShortPacketType (void) const
{
  return m_shortPacketType;
}

TypeId
ShortQuicHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ShortQuicHeader")
    .SetParent<QuicHeader> ()
    .SetGroupName ("Internet")
    .AddConstructor<ShortQuicHeader> ()
  ;
  return tid;
}

TypeId
ShortQuicHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
ShortQuicHeader::Print (std::ostream &os) const
{
  os << "header form: " << m_headerForm
     << " "
     << "connection ID: " << m_connectionId
     << " "
     << "packet number: " << m_packetNumber
     << " "
     << "short packet type: " << m_shortPacketType
     << " "
     << "connection ID flag: " << m_connectionIdFlag
     << " "
     << "key phase bit: " << m_keyPhaseBit
  ;
}

uint32_t
ShortQuicHeader::GetSerializedSize (void) const
{
  return 8+4+2+2+2+2;
}

void
ShortQuicHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteHtonU64 (m_connectionId);
  i.WriteHtonU32 (m_packetNumber);
  i.WriteU16 (m_headerForm);
  i.WriteHtonU16 (m_connectionIdFlag);
  i.WriteHtonU16 (m_keyPhaseBit);
  i.WriteHtonU16 (m_shortPacketType);
}

uint32_t
ShortQuicHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_connectionId = i.ReadNtohU64 ();
  m_packetNumber = i.ReadNtohU32 ();
  m_headerForm = i.ReadU16 ();
  m_connectionIdFlag = i.ReadNtohU16 ();
  m_keyPhaseBit = i.ReadNtohU16 ();
  m_shortPacketType = i.ReadNtohU16 ();

  return GetSerializedSize ();
}

} // namespace ns3
