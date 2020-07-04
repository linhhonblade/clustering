/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015 University of Athens (UOA)
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
 * Author:  - Lampros Katsikas <lkatsikas@di.uoa.gr>
 *          - Konstantinos Chatzikokolakis <kchatzi@di.uoa.gr>
 */

#ifndef CLUSTERING_HEADER_H
#define CLUSTERING_HEADER_H

#include "ns3/log.h"
#include "ns3/header.h"
#include "clustering-utils.h"

namespace ns3 {

/**
 * \ingroup clustering
 * \class ClusteringBeaconHeader
 * \brief Packet header for ClusteringApplication application.
 *
 * The header is made of a 64bits sequence number followed by
 * a mobility information structure.
 */
class ClusteringBeaconHeader : public Header
{
public:
  ClusteringBeaconHeader ();
  virtual ~ClusteringBeaconHeader ();

  /**
     * \param seq the sequence number
     */
  void SetSeq (uint64_t seq);

  /**
     * \return the sequence number
     */
  uint64_t GetSeq (void) const;

  void SetMobilityInfo (ClusteringUtils::NeighborInfo mobilityInfo);

  ClusteringUtils::NeighborInfo GetMobilityInfo (void);

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
  uint64_t m_seq; //!< Sequence number
  //ClusteringUtils::NeighborInfo m_mobilityInfo;     //!< Mobile Info
  int64_t m_ts;
  uint64_t m_nodeId;
  uint64_t m_CID;
  double m_positionX;
  double m_positionY;
  double m_positionZ;
  double m_velocityX;
  double m_velocityY;
  double m_velocityZ;
  ClusteringUtils::NodeState m_state;
};

/**
 * \ingroup clustering
 * \class ClusteringRsuBeaconHeader
 * \brief Packet header for ClusteringApplication application.
 *
 * The header is made of a 64bits seq number followed by rsu info
 */
class ClusteringRsuBeaconHeader : public Header
{
public:
  ClusteringRsuBeaconHeader ();
  virtual ~ClusteringRsuBeaconHeader ();

  /**
     * \return the time stamp
     */
  Time GetTs (void) const;

  /**
     * \param seq the sequence number
     */
  void SetSeq (uint64_t seq);

  /**
     * \return the sequence number
     */
  uint64_t GetSeq (void);

  /**
     * \param degree the degree of the node
     */
  void SetRsuInfo (ClusteringUtils::RsuInfo rsuInfo);

  /**
     * \return the rsu Info
     **/
  ClusteringUtils::RsuInfo GetRsuInfo (void);
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
  uint64_t m_seq; //!< Sequence number
  ClusteringUtils::RsuInfo m_rsuInfo;
};

/**
 * \ingroup clustering
 * \class ClusteringFormClusterHeader
 * \brief Packet header for ClusteringApplication application.
 *
 * The header is made of a 64bits sequence number followed by
 * a mobility information structure.
 */
class ClusteringFormClusterHeader : public Header
{
public:
  ClusteringFormClusterHeader ();
  virtual ~ClusteringFormClusterHeader ();

  /**
     * \param seq the sequence number
     */
  void SetSeq (uint64_t seq);

  /**
     * \return the sequence number
     */
  uint64_t GetSeq (void) const;

  /**
     * \param nodeId the id of the node
     */
  void SetMobilityInfo (ClusteringUtils::NeighborInfo mobilityInfo);
  /**
     * \return the id of the node
     */
  ClusteringUtils::NeighborInfo GetMobilityInfo (void);

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
  uint64_t m_seq; //!< Sequence number
  ClusteringUtils::NeighborInfo m_mobilityInfo; //!< Mobile Info
};

/**
 * \ingroup clustering
 * \class ClusteringDataHeader
 * \brief Packet header for incident event
 *
 * The header is made of a 64bits timestamp followed by
 * an IncidentInfo structure
 */
class ClusteringDataHeader : public Header
{
public:
  ClusteringDataHeader ();
  virtual ~ClusteringDataHeader ();

  /**
     * \return the time stamp
     */
  Time GetTs (void) const;

  /**
     * \param dataType the incident type
     */
  void SetDataInfo (ClusteringUtils::DataInfo dataInfo);

  /**
     * \return the incident type of the event
     */
  ClusteringUtils::DataInfo GetDataInfo (void);

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
  uint64_t m_ts; //!< Timestamp
  ClusteringUtils::DataInfo m_dataInfo; //!< Incident Information
};

} // namespace ns3

#endif // CLUSTERING_HEADER_H
