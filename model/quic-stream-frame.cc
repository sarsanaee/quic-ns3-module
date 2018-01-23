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

#include "quic-frame.h"
#include "quic-stream-frame.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (QuicStreamFrame);

QuicStreamFrame::QuicStreamFrame ()
{
}

QuicStreamFrame::~QuicStreamFrame ()
{
}

void
QuicStreamFrame::SetStreamId (uint32_t streamId)
{
  m_streamId = streamId;
}

void
QuicStreamFrame::SetOffset (uint64_t offset)
{
  m_offset = offset;
}

void
QuicStreamFrame::SetDataLength (uint16_t dataLength)
{
  m_dataLength = dataLength;
}

void
QuicStreamFrame::SetStreamData (uint16_t streamData)
{
  m_streamData = streamData;
}

uint32_t
QuicStreamFrame::GetStreamId () const
{
  return m_streamId;
}

uint64_t
QuicStreamFrame::GetOffset () const
{
  return m_offset;
}

uint16_t
QuicStreamFrame::GetDataLength () const
{
  return m_dataLength;
}

uint16_t
QuicStreamFrame::GetStreamData () const
{
  return m_streamData;
}

void
QuicStreamFrame::SetFrameType (uint8_t frameType)
{
  m_frameType = frameType;
}

TypeId
QuicStreamFrame::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::QuicStreamFrame")
    .SetParent<QuicFrame> ()
    .SetGroupName ("Internet")
    .AddConstructor<QuicStreamFrame> ()
  ;
  return tid;
}

TypeId
QuicStreamFrame::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
QuicStreamFrame::Print (std::ostream &os) const
{
  os << "frame type: " << m_frameType
     << " "
     << "stream id: " << m_streamId
     << " "
     << "offset: " << m_offset
     << " "
     << "data length: " << m_dataLength
     << " "
     << "stream data: " << m_streamData
  ;
}

uint32_t
QuicStreamFrame::GetSerializedSize (void) const
{
  return 1+4+8+2+2;
}

void
QuicStreamFrame::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU8  (m_frameType);
  i.WriteHtonU32 (m_streamId);
  i.WriteHtonU64 (m_offset);
  i.WriteHtonU16 (m_dataLength);
  i.WriteHtonU16 (m_streamData);
}

uint32_t
QuicStreamFrame::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_frameType  = i.ReadU8 ();
  m_streamId   = i.ReadNtohU32 ();
  m_offset     = i.ReadNtohU64 ();
  m_dataLength = i.ReadNtohU16 ();
  m_streamData = i.ReadNtohU16 ();

  return GetSerializedSize ();
}

} // namespace ns3
