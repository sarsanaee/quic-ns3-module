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

#ifndef QUIC_FRAME_H
#define QUIC_FRAME_H

#include <stdint.h>
#include <string>
#include "ns3/header.h"

namespace ns3 {
/**
 * \ingroup quic
 * \brief Implementation of QUIC frames as NS-3 headers
 *
 * This class has fields corresponding to those in a network QUIC frames
 * (frame type and type dependent fields). Work is based on the current
 * working Internet Draft 04 available on:
 * https://datatracker.ietf.org/doc/draft-ietf-quic-transport/
 *
 * This class is supposed to be extended by other classes that implement specific
 * frame types (STREAM, for example).
 */
class QuicFrame : public Header
{
public:

  /**
   * \brief Constructor
   *
   * Creates a null header
   */
  QuicFrame ();
  ~QuicFrame ();

  /**
   * \return The frame type for this QuicFrame
   */
  uint8_t GetFrameType (void) const;
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

protected:
  uint8_t m_frameType;    //!< Frame Type
};

} // namespace ns3
#endif /* QUIC_FRAME */
