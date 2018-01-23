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

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (QuicFrame);

QuicFrame::QuicFrame ()
{
}

QuicFrame::~QuicFrame ()
{
}

uint8_t
QuicFrame::GetFrameType (void) const
{
  return m_frameType;
}

TypeId
QuicFrame::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::QuicFrame")
    .SetParent<Header> ()
    .SetGroupName ("Internet")
  ;
  return tid;
}

TypeId
QuicFrame::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
QuicFrame::Print (std::ostream &os) const
{
  os << "frame type: " << m_frameType;
}

uint32_t
QuicFrame::GetSerializedSize (void) const
{
  /* Arbitrary number for now */
  return 42;
}

void
QuicFrame::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU8 (m_frameType);
}

uint32_t
QuicFrame::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_frameType = i.ReadU8 ();

  return GetSerializedSize ();
}

} // namespace ns3
