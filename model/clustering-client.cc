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

// static const std::string &
// ToString (ClusteringVClient::MyProcess process)
// {
//   return ClusterProcessName[process];
// }

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
  static TypeId tid = TypeId ("ns3::ClusteringVClient").SetParent<Application> ()
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
  m_process = ClusteringVClient::BEACON_EXCHANGE;
  std::cout << "Application starts" << std::endl;
  UpdateCurrentMobilityInfo ();
  UpdateProcess ();
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
  Vector p = this->GetNode ()->GetObject<ConstantPositionMobilityModel>()
  ->GetPosition();
  m_currentMobilityInfo.positionX = p.x;
  m_currentMobilityInfo.positionY = p.y;
  m_currentMobilityInfo.positionZ = p.z;

  p = this->GetNode ()
  ->GetObject<ConstantPositionMobilityModel> ()
  ->GetVelocity ();
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

void
ClusteringVClient::UpdateProcess (void)
{
    std::cout << "This is update process" << std::endl;
}
} // Namespace ns3
