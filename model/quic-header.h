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

#ifndef QUIC_HEADER_H
#define QUIC_HEADER_H

#include <stdint.h>
#include <string>
#include "ns3/header.h"

namespace ns3 {
/**
 * \ingroup quic
 * \brief Packet header for QUIC packets
 *
 * This class has fields corresponding to those in a network QUIC header
 * (connection ID, packet number, type, payload). Work is based on the current
 * working Internet Draft 03 available on:
 * https://datatracker.ietf.org/doc/draft-ietf-quic-transport/
 */
class QuicHeader : public Header
{
public:

  /**
   * \brief Constructor
   *
   * Creates a null header
   */
  QuicHeader ();
  QuicHeader (uint64_t connectionId, uint32_t packetNumber, bool headerForm);
  ~QuicHeader ();

  /**
   * \param connectionId the connection ID for this QuicHeader
   */
  void SetConnectionId (uint64_t connectionId);
  /**
   * \param packetNumber the packet number for this QuicHeader
   */
  void SetPacketNumber (uint32_t packetNumber);
  /**
   * \return The connection ID for this QuicHeader
   */
  uint64_t GetConnectionId (void) const;
  /**
   * \return The packet number for this QuicHeader
   */
  uint32_t GetPacketNumber (void) const;
  /**
   * \return The packet number for this QuicHeader
   */
  bool GetHeaderForm (void) const;

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
  uint64_t m_connectionId;    //!< Connection ID
  uint32_t m_packetNumber;    //!< Packet Number
  bool m_headerForm;          //!< Header Form
};

class LongQuicHeader : public QuicHeader
{
public:

  /**
   * \brief Constructor
   *
   * Creates a null header
   */
  LongQuicHeader ();
  ~LongQuicHeader ();

  /**
   * \param longPacketType the packet type for this LongQuicHeader
   */
  void SetLongPacketType (uint8_t longPacketType);
  /**
   * \return The packet type for this LongQuicHeader
   */
  uint8_t GetLongPacketType (void) const;

  /**
   * \brief Long Quic Header packet type values
   */
  typedef enum
  {
    VERN = 1,   //!< Version Negotiation
    CLIN = 2,   //!< Client Initial
    SSRT = 3,   //!< Server Stateless Retry
    SCLT = 4,   //!< Server Cleartext
    CCLT = 5,   //!< Client Cleartext
    ZRPR = 6,   //!< 0-RTT Protected
    ORPZ = 7,   //!< 1-RTT Protected (key phase 0)
    ORPO = 8,   //!< 1-RTT Protected (key phase 1)
    PURS = 9    //!< Public Reset
  } LongPacketType_t;

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
  uint32_t m_version;         //!< Protocol Version
  uint8_t  m_longPacketType;  //!< Long Packet Type (really 7 bits)
};

class ShortQuicHeader : public QuicHeader
{
public:

  /**
   * \brief Constructor
   *
   * Creates a null header
   */
  ShortQuicHeader ();
  ~ShortQuicHeader ();

  /**
   * \param connectionIdFlag the ShortQuicHeader flag that indicates
   * whether or not the packet has a connection ID field
   */
  void SetConnectionIdFlag (bool connectionIdFlag);
  /**
   * \param keyPhaseBit the ShortQuicHeader flag that allows a recepient
   * of a packet to identify the protection keys that are used to protect
   * the packet
   */
  void SetKeyPhaseBit (bool keyPhaseBit);
  /**
   * \param shortPacketType the packet type for this ShortQuicHeader
   */
  void SetShortPacketType (uint8_t shortPacketType);
  /**
   * \return The connection ID flag value for this ShortQuicHeader
   */
  bool GetConnectionIdFlag (void) const;
  /**
   * \return The key phase bit value for this ShortQuicHeader
   */
  bool GetKeyPhaseBit (void) const;
  /**
   * \return The packet type for this ShortQuicHeader
   */
  uint8_t GetShortPacketType (void) const;

  /**
   * \brief Short Quic Header packet type values.
   * Currently, it only determines the size of the packet number field.
   */
  typedef enum
  {
    OOCT = 1,   //!< 1 Octet
    TOCT = 2,   //!< 2 Octets
    FOCT = 3   //!< 4 Octects
  } ShortPacketType_t;

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
  bool m_connectionIdFlag;    //!< Connection ID Flag
  bool m_keyPhaseBit;         //!< Key Phase Bit
  uint8_t m_shortPacketType;  //!< Short Packet Type (really 5 bits)
};

} // namespace ns3
#endif /* QUIC_HEADER */
