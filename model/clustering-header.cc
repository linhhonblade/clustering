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

#include "ns3/simulator.h"
#include "clustering-header.h"
#include "clustering-utils.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("ClusteringHeader");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (ClusteringBeaconHeader);

ClusteringBeaconHeader::ClusteringBeaconHeader () : m_seq (0)
{
  NS_LOG_FUNCTION (this);
}

ClusteringBeaconHeader::~ClusteringBeaconHeader ()
{
  NS_LOG_FUNCTION (this);
}

void
ClusteringBeaconHeader::SetSeq (uint64_t seq)
{
  NS_LOG_FUNCTION (this << seq);
  m_seq = seq;
}

uint64_t
ClusteringBeaconHeader::GetSeq (void) const
{
  NS_LOG_FUNCTION (this);
  return m_seq;
}

void
ClusteringBeaconHeader::SetMobilityInfo (ClusteringUtils::NeighborInfo mobilityInfo)
{
  NS_LOG_FUNCTION (this << mobilityInfo.nodeId);
  m_ts = mobilityInfo.ts;
    m_nodeId = mobilityInfo.nodeId;
    m_CID = mobilityInfo.CID;
    m_positionX = mobilityInfo.positionX;
    m_positionY = mobilityInfo.positionY;
    m_positionZ = mobilityInfo.positionZ;
    m_velocityX = mobilityInfo.velocityX;
    m_velocityY = mobilityInfo.velocityY;
    m_velocityZ = mobilityInfo.velocityZ;
    m_state = mobilityInfo.state;
}

ClusteringUtils::NeighborInfo
ClusteringBeaconHeader::GetMobilityInfo (void)
{
  NS_LOG_FUNCTION (this);
  ClusteringUtils::NeighborInfo mobilityInfo
  {
    m_ts,
    m_nodeId,
    m_CID,
    m_positionX,
    m_positionY,
    m_positionZ,
    m_velocityX,
    m_velocityY,
    m_velocityZ,
    m_state
  };
  return mobilityInfo;
}

TypeId
ClusteringBeaconHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ClusteringBeaconHeader")
                          .SetParent<Header> ()
                          .AddConstructor<ClusteringBeaconHeader> ();
  return tid;
}

TypeId
ClusteringBeaconHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
ClusteringBeaconHeader::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION (this << &os);
  os << "(seq=" << m_seq << "Ts=" << m_ts << "NodeId=" << m_nodeId << "ClusterId=" << m_CID
     << "NodeState=" << m_state << "Position={" << m_positionX << "," << m_positionY << ","
     << m_positionZ << "} Velocity={" << m_velocityX << "," << m_velocityY << "," << m_velocityZ
     << "})";
}

uint32_t
ClusteringBeaconHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);
  return sizeof (uint64_t) + sizeof (ClusteringUtils::NeighborInfo);
}

void
ClusteringBeaconHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);

  Buffer::Iterator i = start;
  i.WriteHtonU64 (m_seq);

  // Write mobility structure
  i.WriteHtonU64 (m_ts);
  i.WriteHtonU64 (m_nodeId);
  i.WriteHtonU64 (m_CID);
  i.WriteHtonU64 (m_positionX);
  i.WriteHtonU64 (m_positionY);
  i.WriteHtonU64 (m_positionZ);
  i.WriteHtonU64 (m_velocityX);
  i.WriteHtonU64 (m_velocityY);
  i.WriteHtonU64 (m_velocityZ);
  i.WriteHtonU16 (m_state);
}

uint32_t
ClusteringBeaconHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_INFO (this << &start);

  Buffer::Iterator i = start;
  m_seq = i.ReadNtohU64 ();
  m_ts = i.ReadNtohU64 ();
  m_nodeId = i.ReadNtohU64 ();
  m_CID = i.ReadNtohU64 ();
  m_positionX = i.ReadNtohU64 ();
  m_positionY = i.ReadNtohU64 ();
  m_positionZ = i.ReadNtohU64 ();
  m_velocityX = i.ReadNtohU64 ();
  m_velocityY = i.ReadNtohU64 ();
  m_velocityZ = i.ReadNtohU64 ();
  m_state = static_cast<ClusteringUtils::NodeState> (i.ReadNtohU16 ());

  return GetSerializedSize ();
}

/////////////////////////////////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (ClusteringRsuBeaconHeader);

ClusteringRsuBeaconHeader::ClusteringRsuBeaconHeader () : m_seq (0)
{
  NS_LOG_FUNCTION (this);
}

ClusteringRsuBeaconHeader::~ClusteringRsuBeaconHeader ()
{
  NS_LOG_FUNCTION (this);
}

void
ClusteringRsuBeaconHeader::SetSeq (uint64_t seq)
{
  NS_LOG_FUNCTION (this << seq);
  m_seq = seq;
}

uint64_t
ClusteringRsuBeaconHeader::GetSeq (void)
{
  NS_LOG_FUNCTION (this);
  return m_seq;
}

void
ClusteringRsuBeaconHeader::SetRsuInfo (ClusteringUtils::RsuInfo rsuInfo)
{
  NS_LOG_FUNCTION (this << rsuInfo.nodeId);
  m_rsuInfo = rsuInfo;
}

ClusteringUtils::RsuInfo
ClusteringRsuBeaconHeader::GetRsuInfo (void)
{
  NS_LOG_FUNCTION (this);
  return m_rsuInfo;
}

TypeId
ClusteringRsuBeaconHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ClusteringRsuBeaconHeader")
                          .SetParent<Header> ()
                          .AddConstructor<ClusteringRsuBeaconHeader> ();
  return tid;
}

TypeId
ClusteringRsuBeaconHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
ClusteringRsuBeaconHeader::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION (this << &os);
  os << "(seq=" << m_seq << "NodeId=" << m_rsuInfo.nodeId << "Position={" << m_rsuInfo.positionX
     << "," << m_rsuInfo.positionY << "," << m_rsuInfo.positionZ << "})";
}

uint32_t
ClusteringRsuBeaconHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);
  return sizeof (uint64_t) + sizeof (ClusteringUtils::RsuInfo);
}

void
ClusteringRsuBeaconHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);

  Buffer::Iterator i = start;
  i.WriteHtonU64 (m_seq);
  i.WriteHtonU64 (m_rsuInfo.ts);
  i.WriteHtonU64 (m_rsuInfo.nodeId);
  i.WriteHtonU64 (m_rsuInfo.positionX);
  i.WriteHtonU64 (m_rsuInfo.positionY);
  i.WriteHtonU64 (m_rsuInfo.positionZ);
}

uint32_t
ClusteringRsuBeaconHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_INFO (this << &start);

  Buffer::Iterator i = start;
  m_seq = i.ReadNtohU64 ();
  m_rsuInfo.ts = i.ReadNtohU64 ();
  m_rsuInfo.nodeId = i.ReadNtohU64 ();
  m_rsuInfo.positionX = i.ReadNtohU64 ();
  m_rsuInfo.positionY = i.ReadNtohU64 ();
  m_rsuInfo.positionZ = i.ReadNtohU64 ();
  return GetSerializedSize ();
}

/////////////////////////////////////////////////////////////////////
NS_OBJECT_ENSURE_REGISTERED (ClusteringFormClusterHeader);

ClusteringFormClusterHeader::ClusteringFormClusterHeader () : m_seq (0)
{
  NS_LOG_FUNCTION (this);
}

ClusteringFormClusterHeader::~ClusteringFormClusterHeader ()
{
  NS_LOG_FUNCTION (this);
}

void
ClusteringFormClusterHeader::SetSeq (uint64_t seq)
{
  NS_LOG_FUNCTION (this << seq);
  m_seq = seq;
}
uint64_t
ClusteringFormClusterHeader::GetSeq (void) const
{
  NS_LOG_FUNCTION (this);
  return m_seq;
}

void
ClusteringFormClusterHeader::SetMobilityInfo (ClusteringUtils::NeighborInfo mobilityInfo)
{
  NS_LOG_FUNCTION (this << mobilityInfo.nodeId);
  m_mobilityInfo = mobilityInfo;
}

ClusteringUtils::NeighborInfo
ClusteringFormClusterHeader::GetMobilityInfo (void)
{
  NS_LOG_FUNCTION (this);
  return m_mobilityInfo;
}

TypeId
ClusteringFormClusterHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ClusteringFormClusterHeader")
                          .SetParent<Header> ()
                          .AddConstructor<ClusteringFormClusterHeader> ();
  return tid;
}

TypeId
ClusteringFormClusterHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
ClusteringFormClusterHeader::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION (this << &os);
  os << "(seq=" << m_seq << "NodeId=" << m_mobilityInfo.nodeId << "ClusterId=" << m_mobilityInfo.CID
     << "NodeState=" << m_mobilityInfo.state << "Position={" << m_mobilityInfo.positionX << ","
     << m_mobilityInfo.positionY << "," << m_mobilityInfo.positionZ << "} Velocity={"
     << m_mobilityInfo.velocityX << "," << m_mobilityInfo.velocityY << ","
     << m_mobilityInfo.velocityZ << "})";
}

uint32_t
ClusteringFormClusterHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);
  return sizeof (uint64_t) + sizeof (ClusteringUtils::NeighborInfo);
}

void
ClusteringFormClusterHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);

  Buffer::Iterator i = start;
  i.WriteHtonU64 (m_seq);
  // Write mobility structure
  i.WriteHtonU64 (m_mobilityInfo.ts);
  i.WriteHtonU64 (m_mobilityInfo.nodeId);
  i.WriteHtonU64 (m_mobilityInfo.CID);
  i.WriteHtonU64 (m_mobilityInfo.positionX);
  i.WriteHtonU64 (m_mobilityInfo.positionY);
  i.WriteHtonU64 (m_mobilityInfo.positionZ);
  i.WriteHtonU64 (m_mobilityInfo.velocityX);
  i.WriteHtonU64 (m_mobilityInfo.velocityY);
  i.WriteHtonU64 (m_mobilityInfo.velocityZ);
  i.WriteHtonU16 (m_mobilityInfo.state);
}

uint32_t
ClusteringFormClusterHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_INFO (this << &start);

  Buffer::Iterator i = start;
  m_seq = i.ReadNtohU64 ();
  m_mobilityInfo.ts = i.ReadNtohU64 ();
  m_mobilityInfo.nodeId = i.ReadNtohU64 ();
  m_mobilityInfo.CID = i.ReadNtohU64 ();
  m_mobilityInfo.positionX = i.ReadNtohU64 ();
  m_mobilityInfo.positionY = i.ReadNtohU64 ();
  m_mobilityInfo.positionZ = i.ReadNtohU64 ();
  m_mobilityInfo.velocityX = i.ReadNtohU64 ();
  m_mobilityInfo.velocityY = i.ReadNtohU64 ();
  m_mobilityInfo.velocityZ = i.ReadNtohU64 ();
  m_mobilityInfo.state = static_cast<ClusteringUtils::NodeState> (i.ReadLsbtohU16 ());

  return GetSerializedSize ();
}

/////////////////////////////////////////////////////////////////////
NS_OBJECT_ENSURE_REGISTERED (ClusteringDataHeader);

ClusteringDataHeader::ClusteringDataHeader () : m_ts (Simulator::Now ().GetTimeStep ())
{
  NS_LOG_FUNCTION (this);
}

ClusteringDataHeader::~ClusteringDataHeader ()
{
  NS_LOG_FUNCTION (this);
}

Time
ClusteringDataHeader::GetTs (void) const
{
  NS_LOG_FUNCTION (this);
  return TimeStep (m_ts);
}

void
ClusteringDataHeader::SetDataInfo (ClusteringUtils::DataInfo dataInfo)
{
  NS_LOG_FUNCTION (this << dataInfo.CID);
  m_dataInfo = dataInfo;
}

ClusteringUtils::DataInfo
ClusteringDataHeader::GetDataInfo (void)
{
  NS_LOG_FUNCTION (this);
  return m_dataInfo;
}

TypeId
ClusteringDataHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ClusteringDataHeader")
                          .SetParent<Header> ()
                          .AddConstructor<ClusteringDataHeader> ();
  return tid;
}

TypeId
ClusteringDataHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
ClusteringDataHeader::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION (this << &os);
  os << "(time=" << TimeStep (m_ts).GetSeconds () << " ClusterId=" << m_dataInfo.CID
     << " IncidentType =" << m_dataInfo.dataType << ")";
}

uint32_t
ClusteringDataHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);
  return sizeof (uint64_t) + sizeof (ClusteringUtils::DataInfo);
}

void
ClusteringDataHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);

  Buffer::Iterator i = start;
  i.WriteHtonU64 (m_ts);

  // Write DataInfo structure
  i.WriteHtonU64 (m_dataInfo.CID);
  i.WriteHtonU16 (m_dataInfo.dataType);
}

uint32_t
ClusteringDataHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_INFO (this << &start);

  Buffer::Iterator i = start;
  m_ts = i.ReadNtohU64 ();

  m_dataInfo.CID = i.ReadNtohU64 ();
  m_dataInfo.dataType = static_cast<ClusteringUtils::DataType> (i.ReadNtohU16 ());

  return GetSerializedSize ();
}

} // namespace ns3
