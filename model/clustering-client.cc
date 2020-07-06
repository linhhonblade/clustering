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

#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/socket.h"
#include "ns3/string.h"
#include "ns3/packet.h"
#include "ns3/address.h"
#include "ns3/pointer.h"
#include "ns3/uinteger.h"
#include "ns3/simulator.h"
#include "ns3/mobility-module.h"

#include "ns3/address-utils.h"

#include "clustering-client.h"

#include "ns3/random-variable-stream.h"

namespace ns3 {

static const std::string ClusterProcessName[ClusteringVClient::CURRENT_PROCESS] = {
    "BEACON_EXCHANGE", "NEIGHBOR_LIST_EXCHANGE", "CLUSTER_FORMATION", "DATA_EXCHANGE"};

static const std::string &
ToString (ClusteringVClient::MyProcess process)
{
  return ClusterProcessName[process];
}

static const std::string DataName[ClusteringUtils::DATA_TYPES] = {"EMERGENCY_EVENT",
                                                                  "NOTIFICATION_EVENT"};

// static const std::string &
// ToString (ClusteringUtils::DataType dataType)
// {
//   return DataName[dataType];
// }

static const std::string StateName[ClusteringUtils::NODE_STATES] = {"STANDALONE", "CH", "CM"};

static const std::string &
ToString (ClusteringUtils::NodeState nodeState)
{
  return StateName[nodeState];
}

NS_LOG_COMPONENT_DEFINE ("ClusteringClient");
NS_OBJECT_ENSURE_REGISTERED (ClusteringVClient);

TypeId
ClusteringVClient::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ClusteringVClient")
                          .SetParent<Application> ()
                          .AddConstructor<ClusteringVClient> ();
  return tid;
}

// Public Members
ClusteringVClient::ClusteringVClient ()
{
  NS_LOG_FUNCTION (this);
  m_eventElection = EventId ();
  m_eventBeaconExchange = EventId ();
  m_eventDataExchange = EventId ();
  m_eventNeighborExchange = EventId ();
  m_eventDataExchange = EventId ();
  m_eventSendData = EventId ();

  m_sentCounter = 0;
  m_formationCounter = 0;
  m_cycleCounter = 0;
}

ClusteringVClient::~ClusteringVClient ()
{
  NS_LOG_FUNCTION (this);
  m_eventElection = EventId ();
  m_eventBeaconExchange = EventId ();
  m_eventDataExchange = EventId ();
  m_eventNeighborExchange = EventId ();
  m_eventDataExchange = EventId ();
  m_eventSendData = EventId ();

  m_sentCounter = 0;
  m_formationCounter = 0;
  m_cycleCounter = 0;
}

void
ClusteringVClient::AppReport (std::ostream &os)
{

  os << "***********************" << std::endl
     << "  - Cluster Metrics -  " << std::endl
     << "Node:" << m_currentMobilityInfo.nodeId << " Sent overal: " << m_sentCounter << " Packets."
     << std::endl
     << " Formation Messages: " << m_formationCounter << std::endl
     << "***********************" << std::endl;
}

// Protected Members
void
ClusteringVClient::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  // chain up
  Application::DoDispose ();
}

void
ClusteringVClient::StartApplication (void)
{
  NS_LOG_FUNCTION (this);
  m_process = SET_UP;
  std::cout << "Application starts" << std::endl;
  UpdateCurrentMobilityInfo ();
  ScheduleUpdateProcess ();
  ScheduleTransmit (Seconds (0.0));
}

void
ClusteringVClient::ScheduleUpdateProcess (void)
{
  Time now = Simulator::Now ();
  switch (m_process)
    {
      case BEACON_EXCHANGE: {
        std::cout << "At " << now.GetSeconds ()
                  << ": switch from BEACON_EXCHANGE to NEIGHBOR_LIST_EXCHANGE" << std::endl;
        // Simulator::Cancel (m_eventBeaconExchange);
        Simulator::Cancel (m_sendEvent);
        m_process = ClusteringVClient::NEIGHBOR_LIST_EXCHANGE;
        Simulator::Schedule (Seconds (1.0), &ClusteringVClient::ScheduleUpdateProcess, this);
        ScheduleTransmit (Seconds (0.1));
        break;
      }
      case NEIGHBOR_LIST_EXCHANGE: {
        std::cout << "At " << now.GetSeconds ()
                  << ": switch from NEIGHBOR_LIST_EXCHANGE to CLUSTER_FORMATION" << std::endl;
        // Simulator::Cancel (m_eventNeighborExchange);
        Simulator::Cancel (m_sendEvent);
        m_process = CLUSTER_FORMATION;
        Simulator::Schedule (Seconds (2.0), &ClusteringVClient::ScheduleUpdateProcess, this);
        break;
      }
      case CLUSTER_FORMATION: {
        std::cout << "At " << now.GetSeconds ()
                  << ": switch from CLUSTER_FORMATION to DATA_EXCHANGE" << std::endl;
        // Simulator::Cancel (m_eventElection);
        Simulator::Cancel (m_sendEvent);
        m_process = DATA_EXCHANGE;
        Simulator::Schedule (Seconds (30.0), &ClusteringVClient::ScheduleUpdateProcess, this);
        break;
      }
      case DATA_EXCHANGE: {
        std::cout << "At " << now.GetSeconds () << ": switch from DATA_EXCHANGE to BEACON_EXCHANGE"
                  << std::endl;
        // Simulator::Cancel (m_eventSendData);
        Simulator::Cancel (m_sendEvent);
        ResetCycleTime ();
        m_process = BEACON_EXCHANGE;
        Simulator::Schedule (Seconds (1.0), &ClusteringVClient::ScheduleUpdateProcess, this);
        break;
      }
      case SET_UP: {
        std::cout << "At " << now.GetSeconds () << ": switch from SET_UP to BEACON_EXCHANGE"
                  << std::endl;
        m_process = BEACON_EXCHANGE;
        Simulator::Schedule (Seconds (1.0), &ClusteringVClient::ScheduleUpdateProcess, this);
        break;
      }
    default:
      std::cout << "This line shouldn't be shown" << std::endl;
      break;
    }
}

void
ClusteringVClient::ResetCycleTime ()
{
  std::cout << "This is ResetCycleTime function" << std::endl;
  m_clusterList.clear ();
  m_neighborList.clear ();
  m_currentMobilityInfo.state = ClusteringUtils::STANDALONE;
}

void
ClusteringVClient::ScheduleTransmit (Time dt)
{
  NS_LOG_FUNCTION (this << dt);
  // std::cout << "This is Schedule Transmit" << std::endl;
  m_sendEvent = Simulator::Schedule (dt, &ClusteringVClient::Send, this);
  NS_LOG_DEBUG ("[ScheduleTransmit] => NodeId:" << m_currentMobilityInfo.nodeId
                                                << " EventInfo:" << m_sendEvent.GetTs ()
                                                << " process: " << ToString (m_process));
}

void
ClusteringVClient::Send (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("[Send] => NodeId:" << m_currentMobilityInfo.nodeId
                                    << " EventInfo:" << m_sendEvent.GetTs ()
                                    << " process: " << ToString (m_process));
  NS_ASSERT (m_sendEvent.IsExpired ());
  switch (m_process)
    {
      case BEACON_EXCHANGE:
      {
        Mac48Address dest = Mac48Address::GetBroadcast ();
        ClusteringBeaconHeader vBeaconHeader;
        vBeaconHeader.SetSeq (m_sentCounter);
        vBeaconHeader.SetMobilityInfo (m_currentMobilityInfo);
        Ptr<Packet> packet = Create<Packet> (0);
        packet->AddHeader (vBeaconHeader);
        Ptr<Node> node = Application::GetNode ();
        node->GetDevice (0)->Send (packet, dest, 0);
        ++m_sentCounter;
        NS_LOG_INFO ("[Send] BEACON_EXCHANGE => At time "
                     << Simulator::Now ().GetSeconds () << "s node" << m_currentMobilityInfo.nodeId
                     << "sent " << packet->GetSize () << " bytes to " << dest);
        ScheduleTransmit (Seconds (0.1));
        break;
      }
      case NEIGHBOR_LIST_EXCHANGE:
      {
        NS_LOG_INFO ("[Send] NEIGHBOR_LIST_EXCHANGe");
        break;
      }
      case CLUSTER_FORMATION:
      {
        NS_LOG_INFO ("[Send] CLUSTER_FORMATION");
        break;
      }
      case DATA_EXCHANGE:
      {
        NS_LOG_INFO ("[Send] DATA_EXCHANGE");
        break;
      }
      default:
        NS_LOG_DEBUG ("[Send] Default Case");
        break;
    }
}

void
ClusteringVClient::StopApplication (void) // Called at time specified by Stop
{
  NS_LOG_FUNCTION (this);
  Simulator::Cancel (m_eventSendData);
  Simulator::Cancel (m_eventBeaconExchange);
  Simulator::Cancel (m_eventNeighborExchange);
  Simulator::Cancel (m_eventElection);
  Simulator::Cancel (m_eventDataExchange);
  std::cout << "Application stops" << std::endl;
  StatusReport ();
}

// Private Members
void
ClusteringVClient::HandleRead (void)
{
  NS_LOG_FUNCTION (this);
}

void
ClusteringVClient::UpdateCurrentMobilityInfo (void)
{

  //!< Acquire current mobility stats
  m_currentMobilityInfo.ts = Simulator::Now ().GetTimeStep ();
  m_currentMobilityInfo.nodeId = this->GetNode ()->GetId ();

  // Position and Velocity
  Vector p = this->GetNode ()->GetObject<ConstantPositionMobilityModel> ()->GetPosition ();
  m_currentMobilityInfo.positionX = p.x;
  m_currentMobilityInfo.positionY = p.y;
  m_currentMobilityInfo.positionZ = p.z;

  p = this->GetNode ()->GetObject<ConstantPositionMobilityModel> ()->GetVelocity ();
  m_currentMobilityInfo.velocityX = p.x;
  m_currentMobilityInfo.velocityY = p.y;
  m_currentMobilityInfo.velocityZ = p.z;
}

void
ClusteringVClient::StatusReport (void)
{

  NS_LOG_UNCOND (
      "\n\n-----------------------------------------------------------------------------");
  NS_LOG_UNCOND ("[StatusReport] => At time "
                 << Simulator::Now ().GetSeconds () << "s node [" << m_currentMobilityInfo.nodeId
                 << "] is: " << ToString (m_currentMobilityInfo.state)
                 << " in Cluster: " << m_currentMobilityInfo.CID << " having  ===> \n position: ("
                 << m_currentMobilityInfo.positionX << "," << m_currentMobilityInfo.positionY << ","
                 << m_currentMobilityInfo.positionZ << ") - Velocity: ("
                 << m_currentMobilityInfo.velocityX << "," << m_currentMobilityInfo.velocityY << ","
                 << m_currentMobilityInfo.velocityZ << ")"
                 << "\n last packet sent:" << m_currentMobilityInfo.ts << "s"
                 << "\n Neighbors: " << m_neighborList.size ());
  NS_LOG_UNCOND ("---------------------------- Neighbor List  ---------------------------------");
  for (std::map<uint64_t, ClusteringUtils::NeighborInfo>::iterator it = m_neighborList.begin ();
       it != m_neighborList.end (); ++it)
    {
      uint64_t id = it->first;
      ClusteringUtils::NeighborInfo node = it->second;
      NS_LOG_UNCOND (" * key: " << id << " clusterId: " << node.CID
                                << " State:" << ToString (node.state) << " nodeId:" << node.nodeId
                                << " Position:(" << node.positionX << "," << node.positionY << ","
                                << node.positionZ << ") Velocity:(" << node.velocityX << ","
                                << node.velocityY << "," << node.velocityZ
                                << ") last packet sent:" << node.ts << "s");
    }
  NS_LOG_UNCOND ("-----------------------------  clusterList  ---------------------------------");
  for (std::map<uint64_t, ClusteringUtils::NeighborInfo>::iterator it = m_clusterList.begin ();
       it != m_clusterList.end (); ++it)
    {
      uint64_t id = it->first;
      ClusteringUtils::NeighborInfo node = it->second;
      NS_LOG_UNCOND (" * key: " << id << " clusterId: " << node.CID << " NodeState:"
                                << ToString (node.state) << " nodeId:" << node.nodeId
                                << " Position:(" << node.positionX << "," << node.positionY << ","
                                << node.positionZ << ") Velocity" << node.velocityX << ","
                                << node.velocityY << "," << node.velocityZ << ")");
    }
}

} // Namespace ns3
