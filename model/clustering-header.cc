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

NS_LOG_COMPONENT_DEFINE ("ClusteringHeader");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(ClusteringBeaconHeader);

ClusteringBeaconHeader::ClusteringBeaconHeader() :
   m_seq(0){
   NS_LOG_FUNCTION (this);
}

ClusteringBeaconHeader::~ClusteringBeaconHeader(){
   NS_LOG_FUNCTION (this);
}

void
ClusteringBeaconHeader::SetSeq(uint64_t seq) {
   NS_LOG_FUNCTION (this << seq);
   m_seq = seq;
}

uint64_t
ClusteringBeaconHeader::GetSeq(void) const {
   NS_LOG_FUNCTION (this);
   return m_seq;
}

void
ClusteringBeaconHeader::SetMobilityInfo(ClusteringUtils::NeighborInfo mobilityInfo){
   NS_LOG_FUNCTION (this << mobilityInfo.nodeId);
   m_mobilityInfo = mobilityInfo;
}

ClusteringUtils::NeighborInfo
ClusteringBeaconHeader::GetMobilityInfo(void) const {
   NS_LOG_FUNCTION (this);
   return m_mobilityInfo;
}

TypeId
ClusteringBeaconHeader::GetTypeId(void) {
   static TypeId tid = TypeId("ns3::ClusteringBeaconHeader").SetParent<Header>().AddConstructor<ClusteringBeaconHeader>();
   return tid;
}

TypeId
ClusteringBeaconHeader::GetInstanceTypeId(void) const {
   return GetTypeId();
}

void
ClusteringBeaconHeader::Print(std::ostream &os) const {
   NS_LOG_FUNCTION (this << &os);
   os << "(seq=" << m_seq
           << "NodeId=" << m_mobilityInfo.nodeId
           << "ClusterId=" << m_mobilityInfo.CID
           << "NodeState=" << m_mobilityInfo.state
           << "Position=" << m_mobilityInfo.position
           << "Velocity=" << m_mobilityInfo.velocity;
}

uint32_t
ClusteringBeaconHeader::GetSerializedSize(void) const {
   NS_LOG_FUNCTION (this);
   return sizeof(uint64_t) + sizeof(ClusteringUtils::NeighborInfo);
}

void
ClusteringBeaconHeader::Serialize(Buffer::Iterator start) const {
   NS_LOG_FUNCTION (this << &start);

   Buffer::Iterator i = start;
   i.WriteHtonU64(m_seq);

   // Write mobility structure
   unsigned char temp[sizeof(ClusteringUtils::NeighborInfo)];
   memcpy( temp, &m_mobilityInfo, sizeof(ClusteringUtils::NeighborInfo) );
   i.Write(temp, sizeof(ClusteringUtils::NeighborInfo));

}

uint32_t
ClusteringBeaconHeader::Deserialize(Buffer::Iterator start) {
   NS_LOG_INFO (this << &start);

   Buffer::Iterator i = start;
   m_seq = i.ReadNtohU64();

   unsigned char temp[sizeof(ClusteringUtils::NeighborInfo)];
   i.Read(temp, sizeof(ClusteringUtils::NeighborInfo));
   memcpy(&m_mobilityInfo, &temp, sizeof(ClusteringUtils::NeighborInfo));

   return GetSerializedSize();
}


/////////////////////////////////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED(ClusteringRsuBeaconHeader);

ClusteringRsuBeaconHeader::ClusteringRsuBeaconHeader() :
   m_seq(0){
   NS_LOG_FUNCTION (this);
}

ClusteringRsuBeaconHeader::~ClusteringRsuBeaconHeader(){
   NS_LOG_FUNCTION (this);
}

void
ClusteringRsuBeaconHeader::SetSeq(uint64_t seq) {
   NS_LOG_FUNCTION (this << seq);
   m_seq = seq;
}

uint64_t
ClusteringRsuBeaconHeader::GetSeq(void) const {
   NS_LOG_FUNCTION (this);
   return m_seq;
}

void
ClusteringRsuBeaconHeader::SetRsuInfo(ClusteringUtils::RsuInfo rsuInfo){
   NS_LOG_FUNCTION (this << rsuInfo.nodeId);
   m_rsuInfo = rsuInfo;
}

ClusteringUtils::RsuInfo
ClusteringRsuBeaconHeader::GetRsuInfo(void) const {
   NS_LOG_FUNCTION (this);
   return m_rsuInfo;
}

TypeId
ClusteringRsuBeaconHeader::GetTypeId(void) {
   static TypeId tid = TypeId("ns3::ClusteringRsuBeaconHeader").SetParent<Header>().AddConstructor<ClusteringRsuBeaconHeader>();
   return tid;
}

TypeId
ClusteringRsuBeaconHeader::GetInstanceTypeId(void) const {
   return GetTypeId();
}

void
ClusteringRsuBeaconHeader::Print(std::ostream &os) const {
   NS_LOG_FUNCTION (this << &os);
   os << "(seq=" << m_seq
           << "NodeId=" << m_rsuInfo.nodeId
           << "Position=" << m_rsuInfo.position;
}

uint32_t
ClusteringRsuBeaconHeader::GetSerializedSize(void) const {
   NS_LOG_FUNCTION (this);
   return sizeof(uint64_t) + sizeof(ClusteringUtils::RsuInfo);
}

void
ClusteringRsuBeaconHeader::Serialize(Buffer::Iterator start) const {
   NS_LOG_FUNCTION (this << &start);

   Buffer::Iterator i = start;
   i.WriteHtonU64(m_seq);

   // Write mobility structure
   unsigned char temp[sizeof(ClusteringUtils::RsuInfo)];
   memcpy( temp, &m_rsuInfo, sizeof(ClusteringUtils::RsuInfo) );
   i.Write(temp, sizeof(ClusteringUtils::RsuInfo));

}

uint32_t
ClusteringRsuBeaconHeader::Deserialize(Buffer::Iterator start) {
   NS_LOG_INFO (this << &start);

   Buffer::Iterator i = start;
   m_seq = i.ReadNtohU64();

   unsigned char temp[sizeof(ClusteringUtils::RsuInfo)];
   i.Read(temp, sizeof(ClusteringUtils::RsuInfo));
   memcpy(&m_rsuInfo, &temp, sizeof(ClusteringUtils::RsuInfo));

   return GetSerializedSize();
}


/////////////////////////////////////////////////////////////////////
NS_OBJECT_ENSURE_REGISTERED(ClusteringFormClusterHeader);

ClusteringFormClusterHeader::ClusteringFormClusterHeader() :
        m_seq(0){
    NS_LOG_FUNCTION (this);
}

ClusteringFormClusterHeader::~ClusteringFormClusterHeader(){
    NS_LOG_FUNCTION (this);
}

void
ClusteringFormClusterHeader::SetSeq(uint64_t seq) {
    NS_LOG_FUNCTION (this << seq);
    m_seq = seq;
}
uint64_t
ClusteringFormClusterHeader::GetSeq(void) const {
    NS_LOG_FUNCTION (this);
    return m_seq;
}

void
ClusteringFormClusterHeader::SetMobilityInfo(ClusteringUtils::NeighborInfo mobilityInfo){
    NS_LOG_FUNCTION (this << mobilityInfo.nodeId);
    m_mobilityInfo = mobilityInfo;
}

ClusteringUtils::NeighborInfo
ClusteringFormClusterHeader::GetMobilityInfo(void) const {
    NS_LOG_FUNCTION (this);
    return m_mobilityInfo;
}

TypeId
ClusteringFormClusterHeader::GetTypeId(void) {
    static TypeId tid =
            TypeId("ns3::ClusteringFormClusterHeader").SetParent<Header>().AddConstructor<ClusteringFormClusterHeader>();
    return tid;
}

TypeId
ClusteringFormClusterHeader::GetInstanceTypeId(void) const {
    return GetTypeId();
}

void
ClusteringFormClusterHeader::Print(std::ostream &os) const {
    NS_LOG_FUNCTION (this << &os);
    os << "(seq=" << m_seq
            << "NodeId=" << m_mobilityInfo.nodeId
            << "ClusterId=" << m_mobilityInfo.CID
            << "NodeState=" << m_mobilityInfo.state
            << "Position=" << m_mobilityInfo.position
            << "Velocity=" << m_mobilityInfo.velocity;
}

uint32_t
ClusteringFormClusterHeader::GetSerializedSize(void) const {
    NS_LOG_FUNCTION (this);
    return sizeof(uint64_t) + sizeof(ClusteringUtils::NeighborInfo);
}

void
ClusteringFormClusterHeader::Serialize(Buffer::Iterator start) const {
    NS_LOG_FUNCTION (this << &start);

    Buffer::Iterator i = start;
    i.WriteHtonU64(m_seq);

    // Write mobility structure
    unsigned char temp[sizeof(ClusteringUtils::NeighborInfo)];
    memcpy( temp, &m_mobilityInfo, sizeof(ClusteringUtils::NeighborInfo));
    i.Write(temp, sizeof(ClusteringUtils::NeighborInfo));

}

uint32_t
ClusteringFormClusterHeader::Deserialize(Buffer::Iterator start) {
    NS_LOG_INFO (this << &start);

    Buffer::Iterator i = start;
    m_seq = i.ReadNtohU64();

    unsigned char temp[sizeof(ClusteringUtils::NeighborInfo)];
    i.Read(temp, sizeof(ClusteringUtils::NeighborInfo));
    memcpy(&m_mobilityInfo, &temp, sizeof(ClusteringUtils::NeighborInfo) );

    return GetSerializedSize();
}


/////////////////////////////////////////////////////////////////////
NS_OBJECT_ENSURE_REGISTERED(ClusteringDataHeader);

ClusteringDataHeader::ClusteringDataHeader():
    m_ts(Simulator::Now().GetTimeStep()){
    NS_LOG_FUNCTION (this);
}

ClusteringDataHeader::~ClusteringDataHeader(){
    NS_LOG_FUNCTION (this);
}

Time
ClusteringDataHeader::GetTs(void) const {
    NS_LOG_FUNCTION (this);
    return TimeStep(m_ts);
}

void
ClusteringDataHeader::SetDataInfo(ClusteringUtils::DataInfo dataInfo) {
    NS_LOG_FUNCTION (this << dataInfo.CID);
    m_dataInfo = dataInfo;
}

ClusteringUtils::DataInfo
ClusteringDataHeader::GetDataInfo(void) const {
    NS_LOG_FUNCTION (this);
    return m_dataInfo;
}

TypeId
ClusteringDataHeader::GetTypeId(void) {
    static TypeId tid =
            TypeId("ns3::ClusteringDataHeader").SetParent<Header>().AddConstructor<ClusteringDataHeader>();
    return tid;
}

TypeId
ClusteringDataHeader::GetInstanceTypeId(void) const {
    return GetTypeId();
}

void
ClusteringDataHeader::Print(std::ostream &os) const {
    NS_LOG_FUNCTION (this << &os);
    os << "(time=" << TimeStep(m_ts).GetSeconds() << " ClusterId=" << m_dataInfo.CID
       << " IncidentType =" << m_dataInfo.dataType << ")";
}

uint32_t
ClusteringDataHeader::GetSerializedSize(void) const {
    NS_LOG_FUNCTION (this);
    return sizeof(uint64_t) + sizeof(ClusteringUtils::DataInfo);
}

void
ClusteringDataHeader::Serialize(Buffer::Iterator start) const {
    NS_LOG_FUNCTION (this << &start);

    Buffer::Iterator i = start;
    i.WriteHtonU64(m_ts);

    // Write DataInfo structure
    unsigned char temp[sizeof(ClusteringUtils::DataInfo)];
    memcpy( temp, &m_dataInfo, sizeof(ClusteringUtils::DataInfo) );
    i.Write(temp, sizeof(ClusteringUtils::DataInfo));
}

uint32_t
ClusteringDataHeader::Deserialize(Buffer::Iterator start) {
    NS_LOG_INFO (this << &start);

    Buffer::Iterator i = start;
    m_ts = i.ReadNtohU64 ();

    unsigned char temp[sizeof(ClusteringUtils::DataInfo)];
    i.Read(temp, sizeof(ClusteringUtils::DataInfo));
    memcpy(&m_dataInfo, &temp, sizeof(ClusteringUtils::DataInfo) );

    return GetSerializedSize();
}

} // namespace ns3
