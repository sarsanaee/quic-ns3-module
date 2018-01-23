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

#ifndef QUIC_STREAM_FRAME_H
#define QUIC_STREAM_FRAME_H

#include <stdint.h>
#include <string>
#include "ns3/header.h"
#include "quic-frame.h"

namespace ns3 {
/**
 * \ingroup quic
 * \brief Implementation of QUIC STREAM frames
 *
 * This class has fields corresponding to those in a network QUIC STREAM frame
 * (frame type, stream ID, offset, data lenght and stream data). Work is based on the current
 * working Internet Draft 04 available on:
 * https://datatracker.ietf.org/doc/draft-ietf-quic-transport/
 *
 * Specifically on section 8.1.
 */
class QuicStreamFrame : public QuicFrame
{
public:

  /**
   * \brief Constructor
   *
   * Creates a null header
   */
  QuicStreamFrame ();
  ~QuicStreamFrame ();

  /**
   * \brief Set the frame type
   * \param frameType the frame type of this QuicStreamFrame
   */
  void SetFrameType (uint8_t frameType);
  /**
   * \brief Set the stream ID
   * \param streamId the stream ID for this QuicStreamFrame
   */
  void SetStreamId (uint32_t streamId);
  /**
   * \brief Set the offset
   * \param offset the offset for this QuicStreamFrame
   */
  void SetOffset (uint64_t offset);
  /**
   * \brief Set the data length
   * \param dataLength the data length of this QuicStreamFrame
   */
  void SetDataLength (uint16_t dataLength);
  /**
   * TODO: NOT SURE IF NEEDED
   * \brief Set the stream data
   * \param streamData the number of bytes allocated for this QuicStreamFrame stream data
   */
  void SetStreamData (uint16_t streamData);
  /**
   * \brief Get the stream ID
   * \return The stream ID for this QuicStreamFrame
   */
  uint32_t GetStreamId () const;
  /**
   * \brief Get the offset
   * \return The offset for this QuicStreamFrame
   */
  uint64_t GetOffset () const;
  /**
   * \brief Get the data length
   * \return The data length for this QuicStreamFrame
   */
  uint16_t GetDataLength () const;
  /**
   * \brief Get the stream data
   * \return The stream data for this QuicStreamFrame
   */
  uint16_t GetStreamData () const;

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

private:
  uint32_t m_streamId;   //!< Stream ID
  uint64_t m_offset;     //!< Offset
  uint16_t m_dataLength; //!< Data Length
  uint16_t m_streamData; //!< Stream Data. The data transferred of m_dataLength max length
};

} // namespace ns3
#endif /* QUIC_FRAME */
