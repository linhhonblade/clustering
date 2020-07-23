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
      case NEIGHBOR_LIST_EXCHANGE: {
        std::cout << "At " <<
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
#include "ns3/wave-net-device.h"
#include "ns3/net-device.h"

#include "ns3/address-utils.h"

#include "clustering-client.h"

#include "ns3/random-variable-stream.h"

namespace ns3 {

// static const std::string ClusterProcessName[ClusteringVClient::CURRENT_PROCESS] = {
//     "BEACON_EXCHANGE", "NEIGHBOR_LIST_EXCHANGE", "CLUSTER_FORMATION", "DATA_EXCHANGE"};

static const std::string ClusterProcessName[ClusteringVClient::CURRENT_PROCESS] = {
    "BEACON_EXCHANGE", "CLUSTER_FORMATION", "DATA_EXCHANGE", "SET_UP"};

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

/*------------------------------------------------------------------
-------------------------ClusteringVClient--------------------------
--------------------------------------------------------------------*/

NS_OBJECT_ENSURE_REGISTERED (ClusteringVClient);

TypeId
ClusteringVClient::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::ClusteringVClient")
          .SetParent<Application> ()
          .AddConstructor<ClusteringVClient> ()
          .AddAttribute ("SimulationCase", "The case that the simulation run", UintegerValue (1),
                         MakeUintegerAccessor (&ClusteringVClient::m_simCase),
                         MakeUintegerChecker<uint8_t> (1))
          .AddAttribute ("IsSender", "Is Node a Sender in DATA_EXCHANGE", BooleanValue (false),
                         MakeBooleanAccessor (&ClusteringVClient::m_isSender),
                         MakeBooleanChecker ())
          .AddAttribute ("PeerNode", "The destination node", UintegerValue (0),
                         MakeUintegerAccessor (&ClusteringVClient::m_peerNode),
                         MakeUintegerChecker<uint64_t> (1));
  return tid;
}

// Public Members
ClusteringVClient::ClusteringVClient ()
{
  NS_LOG_FUNCTION (this);
  m_eventElection = EventId ();
  m_sendEvent = EventId ();
  m_sentCounter = 0;
  m_formationCounter = 0;
  m_cycleCounter = 0;
  m_closestRsuInfo = {0, 500, 0.0, 0.0, 0.0};
  m_nDataPacketReceived = 0;
  m_nDataPacketSent = 0;

  m_utils = ClusteringUtils ();

  m_deltaT = 30;
  m_tWaitMax = 1.0;
  m_waitingTime = INFINITY;
  m_neighborList = std::map<uint64_t, ClusteringUtils::NeighborInfo> ();
  m_clusterList = std::map<uint64_t, ClusteringUtils::NeighborInfo> ();
  m_addressList = std::map<uint64_t, Mac48Address> ();
}

ClusteringVClient::~ClusteringVClient ()
{
  NS_LOG_FUNCTION (this);
  m_eventElection = EventId ();
  m_sentCounter = 0;
  m_formationCounter = 0;
  m_cycleCounter = 0;
  m_neighborList = std::map<uint64_t, ClusteringUtils::NeighborInfo> ();
  m_clusterList = std::map<uint64_t, ClusteringUtils::NeighborInfo> ();
  m_addressList = std::map<uint64_t, Mac48Address> ();
  m_nDataPacketReceived = 0;
  m_nDataPacketSent = 0;
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
  Packet::EnablePrinting ();
  NS_LOG_FUNCTION (this);
  m_process = SET_UP;
  m_currentMobilityInfo.state = ClusteringUtils::STANDALONE;
  m_inElection = false;
  const SchInfo schInfo = SchInfo (SCH1, false, EXTENDED_CONTINUOUS);
  std::cout << "Application starts" << std::endl;
  m_device = DynamicCast<WaveNetDevice> (Application::GetNode ()->GetDevice (0));
  StartListening ();
  m_device->StartSch (schInfo);
  UpdateCurrentMobilityInfo ();
  ScheduleUpdateProcess ();

  ScheduleTransmit (Seconds (0.0 + 0.0005 * m_currentMobilityInfo.nodeId));
}

void
ClusteringVClient::StartListening (void)
{
  NS_LOG_FUNCTION (this);
  m_device->SetReceiveCallback (MakeCallback (&ClusteringVClient::HandleRead, this));
}

void
ClusteringVClient::ScheduleUpdateProcess (void)
{
  Time now = Simulator::Now ();
  switch (m_process)
    {
      case BEACON_EXCHANGE: {
        // Simulator::Cancel (m_eventBeaconExchange);
        // Simulator::Cancel (m_sendEvent);
        // m_process = ClusteringVClient::NEIGHBOR_LIST_EXCHANGE;
        // Simulator::Schedule (Seconds (1.0), &ClusteringVClient::ScheduleUpdateProcess, this);
        // ScheduleTransmit (Seconds (0.1));
        // break;
        Simulator::Cancel (m_sendEvent);
        m_process = CLUSTER_FORMATION;
        Simulator::Schedule (Seconds (2.0), &ClusteringVClient::ScheduleUpdateProcess, this);
        m_waitingTime = GetTwait ();
        NS_LOG_INFO ("[FORM_CLUSTER] START ELECTION => At time "
                     << Simulator::Now ().GetSeconds () << " s node "
                     << m_currentMobilityInfo.nodeId << " has waiting time " << m_waitingTime
                     << "s");

        ScheduleTransmit (Seconds (m_waitingTime));
        break;
      }
      // case NEIGHBOR_LIST_EXCHANGE: {
      // Simulator::Cancel (m_sendEvent);
      // m_process = CLUSTER_FORMATION;
      // Simulator::Schedule (Seconds (2.0), &ClusteringVClient::ScheduleUpdateProcess, this);
      // m_waitingTime = GetTwait ();
      // NS_LOG_INFO ("[FORM_CLUSTER] START ELECTION => At time "
      //              << Simulator::Now ().GetSeconds () << " s node "
      //              << m_currentMobilityInfo.nodeId << " has waiting time " << m_waitingTime
      //              << "s");

      // ScheduleTransmit (Seconds (m_waitingTime));
      // break;
      // }
      case CLUSTER_FORMATION: {
        NS_LOG_DEBUG ("At time " << Simulator::Now ().GetSeconds () << " start Data exchange");
        Simulator::Cancel (m_sendEvent);
        m_process = DATA_EXCHANGE;
        Simulator::Schedule (Seconds (30.0), &ClusteringVClient::ScheduleUpdateProcess, this);
        ScheduleTransmit (Seconds (0.0));
        break;
      }
      case DATA_EXCHANGE: {
        // Simulator::Cancel (m_eventSendData);
        Simulator::Cancel (m_sendEvent);
        ResetCycleTime ();
        m_process = BEACON_EXCHANGE;
        Simulator::Schedule (Seconds (1.0), &ClusteringVClient::ScheduleUpdateProcess, this);
        break;
      }
      case SET_UP: {
        m_process = BEACON_EXCHANGE;
        Simulator::Schedule (Seconds (1.0), &ClusteringVClient::ScheduleUpdateProcess, this);
        break;
      }
    default:
      NS_LOG_DEBUG ("[Default case] In Schedule Update Process");
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
  m_waitingTime = INFINITY;
  m_nDataPacketReceived = 0;
  m_nDataPacketSent = 0;
  m_sentCounter = 0;
  m_sendEvent.Cancel ();
  m_addressList.clear ();
}

void
ClusteringVClient::ScheduleTransmit (Time dt)
{
  NS_LOG_FUNCTION (this << dt);
  switch (m_process)
    {
      case DATA_EXCHANGE: {
        // int u;
        // for (u = 0; u < 100; u++)
        //   {
        //     Simulator::Schedule (Seconds (0.1 * u), &ClusteringVClient::Send, this);
        //   }
        m_sendEvent = Simulator::Schedule (dt, &ClusteringVClient::Send, this);
        break;
      }

      default: {
        m_sendEvent = Simulator::Schedule (dt, &ClusteringVClient::Send, this);
        NS_LOG_DEBUG ("[Schedule Transmit] => NodeId:" << m_currentMobilityInfo.nodeId
                                                       << " EventInfo:" << m_sendEvent.GetTs ()
                                                       << " process: " << ToString (m_process));
        break;
      }
    }
}

void
ClusteringVClient::Send (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("[Send] => NodeId:" << m_currentMobilityInfo.nodeId
                                    << " EventInfo:" << m_sendEvent.GetTs ()
                                    << " process: " << ToString (m_process));
  NS_ASSERT (m_sendEvent.IsExpired ());
  Ptr<Packet> packet = Create<Packet> (0);
  // UpdateCurrentMobilityInfo ();
  switch (m_process)
    {
      case BEACON_EXCHANGE: {
        Mac48Address dest = Mac48Address::GetBroadcast ();
        ClusteringBeaconHeader vBeaconHeader;
        vBeaconHeader.SetSeq (m_sentCounter);
        vBeaconHeader.SetMobilityInfo (m_currentMobilityInfo);
        packet->AddHeader (vBeaconHeader);
        const TxProfile txProfile = TxProfile (SCH1);
        m_device->RegisterTxProfile (txProfile);
        if (m_device->Send (packet, dest, 0))
          {
            ++m_sentCounter;
            m_device->DeleteTxProfile (SCH1);
            NS_LOG_DEBUG ("[Send] BEACON_EXCHANGE => At time "
                          << Simulator::Now ().GetSeconds () << "s node "
                          << m_currentMobilityInfo.nodeId << " sent " << packet->GetSize ()
                          << " bytes to " << dest);
          }

        ScheduleTransmit (Seconds (0.1));
        break;
      }
      // case NEIGHBOR_LIST_EXCHANGE: {
      //   // NS_LOG_INFO ("[Send] NEIGHBOR_LIST_EXCHANGE");
      //   // ScheduleTransmit (Seconds (0.1));
      //   break;
      // }
      case CLUSTER_FORMATION: {
        // UpdateCurrentMobilityInfo ();
        if (m_currentMobilityInfo.state == ClusteringUtils::STANDALONE)
          {
            m_currentMobilityInfo.state = ClusteringUtils::CH;
            m_currentMobilityInfo.CID = m_currentMobilityInfo.nodeId;

            // Send form cluster packet
            Mac48Address dest = Mac48Address::GetBroadcast ();
            ClusteringFormClusterHeader formClusterHeader;
            formClusterHeader.SetSeq (m_sentCounter);
            formClusterHeader.SetMobilityInfo (m_currentMobilityInfo);
            packet->AddHeader (formClusterHeader);
            const TxProfile txProfile = TxProfile (SCH1);
            m_device->RegisterTxProfile (txProfile);
            if (m_device->Send (packet, dest, 0))
              {
                ++m_sentCounter;
                m_device->DeleteTxProfile (SCH1);
                NS_LOG_DEBUG ("[Send] FORM_CLUSTER => At time "
                              << Simulator::Now ().GetSeconds () << "s node "
                              << m_currentMobilityInfo.nodeId << " sent " << packet->GetSize ()
                              << " bytes to " << dest);
              }
            else
              {
                NS_LOG_DEBUG ("Failed to sent package");
              }
          }
        else if (m_currentMobilityInfo.state == ClusteringUtils::CM)
          {
            Mac48Address dest = Mac48Address::GetBroadcast ();
            ClusteringBeaconHeader beaconHeader;
            beaconHeader.SetSeq (m_sentCounter);
            beaconHeader.SetMobilityInfo (m_currentMobilityInfo);
            packet->AddHeader (beaconHeader);
            const TxProfile txProfile = TxProfile (SCH1);
            m_device->RegisterTxProfile (txProfile);
            if (m_device->Send (packet, dest, 0))
              {
                ++m_sentCounter;
                m_device->DeleteTxProfile (SCH1);
                NS_LOG_DEBUG ("[Send] BEACON_EXCHANGE in FORM_CLUSTER At "
                              << Simulator::Now ().GetSeconds () << "s node "
                              << m_currentMobilityInfo.nodeId << " sent " << packet->GetSize ()
                              << " bytes to " << dest);
              }
          }
        break;
      }
      case DATA_EXCHANGE: {
        switch (m_simCase)
          {
            case 1: {
              if (m_isSender)
                {
                  NS_LOG_DEBUG ("At time " << Simulator::Now ().GetSeconds () << "s node "
                                           << m_currentMobilityInfo.nodeId << " is sender");
                  ClusteringDataHeader dataHeader;
                  ClusteringUtils::DataInfo dataInfo = {m_currentMobilityInfo.nodeId, m_peerNode,
                                                        m_currentMobilityInfo.CID,
                                                        ClusteringUtils::NOTIFICATION_EVENT};
                  dataHeader.SetDataInfo (dataInfo);

                  packet->AddHeader (dataHeader);

                  std::map<uint64_t, Mac48Address>::iterator itr = m_addressList.find (m_peerNode);
                  // NS_LOG_INFO("hello");
                  if (itr == m_addressList.end ())
                    {
                      std::map<uint64_t, Mac48Address>::iterator it =
                          m_addressList.find (m_currentMobilityInfo.CID);
                      if (it == m_addressList.end ())
                        {
                          NS_LOG_DEBUG ("[Send] Node: "
                                        << m_currentMobilityInfo.nodeId
                                        << " cannot find the address of CH in address list");
                        }
                      else
                        {
                          const TxProfile txProfile = TxProfile (SCH1);
                          m_device->RegisterTxProfile (txProfile);
                          if (m_device->Send (packet, it->second, 0))
                            {
                              NS_LOG_DEBUG ("[Send] At time "
                                            << Simulator::Now ().GetSeconds () << "s node "
                                            << m_currentMobilityInfo.nodeId << " sent "
                                            << packet->GetSize () << " bytes to " << it->second);
                              ++m_nDataPacketSent;
                              m_device->DeleteTxProfile (SCH1);
                            }
                          else
                            {
                              NS_LOG_DEBUG ("[Send] Failed to send data packet");
                            }
                        }
                    }
                  else
                    {
                      // Send directly to destNode
                      const TxProfile txProfile = TxProfile (SCH1);
                      m_device->RegisterTxProfile (txProfile);
                      if (m_device->Send (packet, itr->second, 0))
                        {
                          ++m_nDataPacketSent;
                          m_device->DeleteTxProfile (SCH1);
                          NS_LOG_INFO ("[Send] At time "
                                       << Simulator::Now ().GetSeconds () << "s node "
                                       << m_currentMobilityInfo.nodeId << " sent "
                                       << packet->GetSize () << " bytes to " << itr->second);
                        }
                      else
                        {
                          NS_LOG_DEBUG ("[Send] Node: " << m_currentMobilityInfo.nodeId
                                                        << " failed to sent data packet");
                        }
                    }
                  ScheduleTransmit (Seconds (0.1));
                }
              else
                {
                  m_sendEvent.Cancel ();
                  NS_LOG_DEBUG ("Node: " << m_currentMobilityInfo.nodeId
                                         << " is not a sender. At time "
                                         << Simulator::Now ().GetSeconds ());
                }
              break;
            }
          default:
            break;
          }
      }
      default: {
        NS_LOG_DEBUG ("[Send] Default Case");
        break;
      }
    }
} // namespace ns3

void
ClusteringVClient::StopApplication (void) // Called at time specified by Stop
{
  NS_LOG_FUNCTION (this);
  Simulator::Cancel (m_sendEvent);
  m_device->DeleteTxProfile (SCH1);
  m_device->StopSch (SCH1);
  std::cout << "Application stops" << std::endl;
  StatusReport ();
}

// Private Members
bool
ClusteringVClient::HandleRead (Ptr<NetDevice> dev, Ptr<const Packet> pkt, uint16_t mode,
                               const Address &sender)
{
  NS_LOG_FUNCTION (this << dev);
  Ptr<Packet> p = pkt->Copy ();
  if (pkt->GetSize () == 0)
    {
    }
  else
    {
      PacketMetadata::ItemIterator metaDataIterator = p->BeginItem ();
      PacketMetadata::Item item;
      while (metaDataIterator.HasNext ())
        {
          item = metaDataIterator.Next ();
          if (item.tid.GetName () == "ns3::ClusteringBeaconHeader")
            {
              ClusteringBeaconHeader rcvVBeaconHeader;
              p->RemoveHeader (rcvVBeaconHeader);
              ClusteringUtils::NeighborInfo otherMobilityInfo = rcvVBeaconHeader.GetMobilityInfo ();
              if ((Application::GetNode()->GetId() == 3) && (otherMobilityInfo.nodeId == 6)){
                std::cout << "p3 = " << m_currentMobilityInfo.positionX << std::endl << "p6 = " << otherMobilityInfo.positionX << std::endl << "v6 = " << otherMobilityInfo.velocityX << std::endl << "v3 = " << m_currentMobilityInfo.velocityX << std::endl;
              }
              // UpdateCurrentMobilityInfo ();

              // insert to neighbor list
              std::map<uint64_t, ClusteringUtils::NeighborInfo>::iterator itr =
                  m_neighborList.find (otherMobilityInfo.nodeId);
              if (itr == m_neighborList.end ())
                {
                  if (CheckOutOfTransmission (m_currentMobilityInfo,
                                              rcvVBeaconHeader.GetMobilityInfo ()))
                    {
                      ;
                    }
                  else
                    {
                      NS_LOG_INFO ("[Handle Read] Node: " << m_currentMobilityInfo.nodeId
                                                          << " Insert packet: "
                                                          << otherMobilityInfo.nodeId);
                      m_neighborList.insert ({otherMobilityInfo.nodeId, otherMobilityInfo});
                    }
                }
              else
                {
                  // NS_LOG_INFO ("[Handle Read] At time "
                  //              << Simulator::Now ().GetSeconds () << "s Node "
                  //              << m_currentMobilityInfo.nodeId << " update node "
                  //              << otherMobilityInfo.nodeId << " in neighbor list");
                  itr->second = otherMobilityInfo;
                }

              // insert to address list
              std::map<uint64_t, Mac48Address>::iterator it =
                  m_addressList.find (otherMobilityInfo.nodeId);
              if (it == m_addressList.end ())
                {
                  AddressValue addressVal = AddressValue (sender);
                  std::string macStr = addressVal.SerializeToString (MakeAddressChecker ());
                  Mac48Address macAddress = Mac48Address (macStr.c_str ());
                  m_addressList.insert ({otherMobilityInfo.nodeId, macAddress});
                  NS_LOG_INFO ("[Handle Read] Node: "
                               << m_currentMobilityInfo.nodeId << " added address of node "
                               << otherMobilityInfo.nodeId << " into address list");
                }
            }
          else if (item.tid.GetName () == "ns3::ClusteringRsuBeaconHeader")
            {
              ClusteringRsuBeaconHeader rcvRsuBeaconHeader;
              p->RemoveHeader (rcvRsuBeaconHeader);
              ClusteringUtils::RsuInfo otherInfo = rcvRsuBeaconHeader.GetRsuInfo ();
              if ((m_closestRsuInfo.nodeId == 500) | (m_closestRsuInfo.nodeId == otherInfo.nodeId))
                {
                  m_closestRsuInfo = otherInfo;
                  NS_LOG_DEBUG ("[Update closest RSU] At time " << Simulator::Now ().GetSeconds ()
                                                                << " rsu node " << otherInfo.nodeId
                                                                << " became closest RSU of node "
                                                                << m_currentMobilityInfo.nodeId);
                }
              else
                {
                  Vector pRsuThat = GetPositionVector (otherInfo);
                  Vector pRsuThis = GetPositionVector (m_closestRsuInfo);
                  Vector pVehicleThis = GetPositionVector (m_currentMobilityInfo);
                  if ((pVehicleThis - pRsuThat).GetLength () <
                      (pVehicleThis - pRsuThis).GetLength ())
                    {
                      m_closestRsuInfo = otherInfo;
                      NS_LOG_INFO ("[Update closest RSU] At time "
                                   << Simulator::Now ().GetSeconds () << "rsu node "
                                   << otherInfo.nodeId << "became closest RSU of node "
                                   << m_currentMobilityInfo.nodeId);
                    }
                }
            }
          else if (item.tid.GetName () == "ns3::ClusteringFormClusterHeader")
            {
              switch (m_currentMobilityInfo.state)
                {
                  case ClusteringUtils::STANDALONE: {
                    // m_sendEvent.Cancel ();
                    ClusteringFormClusterHeader formClusterHeader;
                    p->RemoveHeader (formClusterHeader);
                    ClusteringUtils::NeighborInfo otherMobilityInfo =
                        formClusterHeader.GetMobilityInfo ();

                    // m_currentMobilityInfo.state = ClusteringUtils::CM;
                    // m_currentMobilityInfo.CID = otherMobilityInfo.CID;

                    // NS_LOG_INFO (
                    //     "[Handle Read] At "
                    //     << Simulator::Now ().GetSeconds () << "s node "
                    //     << m_currentMobilityInfo.nodeId
                    //     << " received a Form CLuster packet: Quit Election and join cluster "
                    //     << otherMobilityInfo.CID);

                    std::map<uint64_t, ClusteringUtils::NeighborInfo>::iterator itr =
                        m_neighborList.find (otherMobilityInfo.nodeId);
                    if (itr == m_neighborList.end ())
                      {
                        NS_LOG_DEBUG ("[Handle Read] Node: " << m_currentMobilityInfo.nodeId
                                                             << " Insert packet: "
                                                             << otherMobilityInfo.nodeId);
                        // m_neighborList.insert ({otherMobilityInfo.nodeId, otherMobilityInfo});
                      }
                    else
                      {
                        m_sendEvent.Cancel ();

                        m_currentMobilityInfo.state = ClusteringUtils::CM;
                        m_currentMobilityInfo.CID = otherMobilityInfo.CID;

                        NS_LOG_INFO (
                            "[Handle Read] At "
                            << Simulator::Now ().GetSeconds () << "s node "
                            << m_currentMobilityInfo.nodeId
                            << " received a Form CLuster packet: Quit Election and join cluster "
                            << otherMobilityInfo.CID);
                        // NS_LOG_INFO ("[Handle Read] Node "
                        //              << m_currentMobilityInfo.nodeId << "  update node "
                        //              << otherMobilityInfo.nodeId << " in neighbor list");
                        itr->second = otherMobilityInfo;
                      }
                    ScheduleTransmit (Seconds (0.001 * m_currentMobilityInfo.nodeId));
                    break;
                  }
                  case ClusteringUtils::CM: {
                    ClusteringFormClusterHeader formClusterHeader;
                    p->RemoveHeader (formClusterHeader);
                    ClusteringUtils::NeighborInfo otherMobilityInfo =
                        formClusterHeader.GetMobilityInfo ();
                    std::map<uint64_t, ClusteringUtils::NeighborInfo>::iterator itr =
                        m_neighborList.find (otherMobilityInfo.nodeId);
                    if (itr == m_neighborList.end ())
                      {
                        // m_neighborList.insert ({otherMobilityInfo.nodeId, otherMobilityInfo});
                      }
                    else
                      {
                        itr->second = otherMobilityInfo;
                      }
                    break;
                  }
                  case ClusteringUtils::CH: {
                    ClusteringFormClusterHeader formClusterHeader;
                    p->RemoveHeader (formClusterHeader);
                    ClusteringUtils::NeighborInfo otherMobilityInfo =
                        formClusterHeader.GetMobilityInfo ();
                    std::map<uint64_t, ClusteringUtils::NeighborInfo>::iterator itr =
                        m_neighborList.find (otherMobilityInfo.nodeId);
                    if (itr == m_neighborList.end ())
                      {
                        // m_neighborList.insert ({otherMobilityInfo.nodeId, otherMobilityInfo});
                      }
                    else
                      {
                        itr->second = otherMobilityInfo;
                      }
                    break;
                  }
                  default: {
                    NS_LOG_INFO ("[Handle Read] Node " << m_currentMobilityInfo.nodeId
                                                       << " Default case");
                    break;
                  }
                }
            }
          else if (item.tid.GetName () == "ns3::ClusteringDataHeader")
            {
              switch (m_simCase)
                {
                  case 1: {
                    ++m_nDataPacketReceived;
                    ClusteringDataHeader dataHeader;
                    p->RemoveHeader (dataHeader);
                    if (m_currentMobilityInfo.nodeId == dataHeader.GetDestNode ())
                      {
                        NS_LOG_INFO ("[Handle Read] At time " << Simulator::Now ().GetSeconds ()
                                                              << "s node "
                                                              << m_currentMobilityInfo.nodeId
                                                              << " received data packet from node "
                                                              << dataHeader.GetSrcNode ());
                      }
                    else
                      {
                        if (m_currentMobilityInfo.state == ClusteringUtils::CH)
                          {
                            std::map<uint64_t, Mac48Address>::iterator itr =
                                m_addressList.find (dataHeader.GetDestNode ());
                            if (itr == m_addressList.end ())
                              {

                                NS_LOG_INFO ("[Handle Read] CH doesnt have address of destination "
                                             "node, send to RSU");
                              }
                            else
                              {
                                NS_LOG_INFO ("hello");
                                Ptr<Packet> newPacket = Create<Packet> (0);
                                newPacket->AddHeader (dataHeader);
                                const TxProfile txProfile = TxProfile (SCH1);
                                m_device->RegisterTxProfile (txProfile);
                                if (m_device->Send (newPacket, itr->second, 0))
                                  {
                                    NS_LOG_INFO ("[Handle Read] CH "
                                                 << m_currentMobilityInfo.nodeId
                                                 << " received a packet. Forward to destinatino");
                                    ++m_nDataPacketSent;
                                    m_device->DeleteTxProfile (SCH1);
                                  }
                                else
                                  {
                                    NS_LOG_DEBUG ("[Handle Read] Failed to forward data packet in "
                                                  "DATA_EXCHANGE");
                                  }
                              }
                          }
                        else
                          {
                            NS_LOG_DEBUG ("[Handle Read] Received an anomynous packet");
                          }
                      }
                    break;
                  }
                default:
                  break;
                }
            }
        }
    }
  return true;
}

bool
ClusteringVClient::CheckOutOfTransmission (ClusteringUtils::NeighborInfo a,
                                           ClusteringUtils::NeighborInfo b)
{
  double commRange = 50.0;
  // Position of a and b after deltaT secs
  Vector pa = GetPositionVector (a) +
              Vector (a.velocityX * m_deltaT, a.velocityY * m_deltaT, a.velocityZ * m_deltaT);
  Vector pb = GetPositionVector (b) +
              Vector (b.velocityX * m_deltaT, b.velocityY * m_deltaT, b.velocityZ * m_deltaT);
  if ((pb - pa).GetLength () < commRange)
    {
      return false;
    }
  else
    {
      return true;
    }
}

void
ClusteringVClient::UpdateCurrentMobilityInfo (void)
{

  //!< Acquire current mobility stats
  m_currentMobilityInfo.ts = Simulator::Now ().GetTimeStep ();
  m_currentMobilityInfo.nodeId = Application::GetNode ()->GetId ();

  // Position and Velocity
  Vector p = this->GetNode ()->GetObject<ConstantVelocityMobilityModel> ()->GetPosition ();
  m_currentMobilityInfo.positionX = p.x;
  m_currentMobilityInfo.positionY = p.y;
  m_currentMobilityInfo.positionZ = p.z;

  p = this->GetNode ()->GetObject<ConstantVelocityMobilityModel> ()->GetVelocity ();
  m_currentMobilityInfo.velocityX = p.x;
  m_currentMobilityInfo.velocityY = p.y;
  m_currentMobilityInfo.velocityZ = p.z;
}

void
ClusteringVClient::StatusReport (void)
{
  UpdateCurrentMobilityInfo ();
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
                 << "\n Neighbors: " << m_neighborList.size ()
                 << "\n Closest RSU node: " << m_closestRsuInfo.nodeId);
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
  // NS_LOG_UNCOND ("-----------------------------  clusterList  ---------------------------------");
  // for (std::map<uint64_t, ClusteringUtils::NeighborInfo>::iterator it = m_clusterList.begin ();
  //      it != m_clusterList.end (); ++it)
  //   {
  //     uint64_t id = it->first;
  //     ClusteringUtils::NeighborInfo node = it->second;
  //     NS_LOG_UNCOND (" * key: " << id << " clusterId: " << node.CID << " NodeState:"
  //                               << ToString (node.state) << " nodeId:" << node.nodeId
  //                               << " Position:(" << node.positionX << "," << node.positionY << ","
  //                               << node.positionZ << ") Velocity" << node.velocityX << ","
  //                               << node.velocityY << "," << node.velocityZ << ")");
  //   }
  // NS_LOG_UNCOND ("-----------------------------  Address List  ---------------------------------");
  // for (std::map<uint64_t, Mac48Address>::iterator it = m_addressList.begin ();
  //      it != m_addressList.end (); ++it)
  //   {
  //     uint64_t id = it->first;
  //     NS_LOG_UNCOND (" * key: " << id << " address: " << it->second);
  //   }
  NS_LOG_UNCOND ("Number of data packets sent: " << m_nDataPacketSent);
  NS_LOG_UNCOND ("Number of data packet received: " << m_nDataPacketReceived);
}

Vector
ClusteringVClient::GetVelocityVector (ClusteringUtils::NeighborInfo mobilityInfo)
{
  return Vector (mobilityInfo.velocityX, mobilityInfo.velocityY, mobilityInfo.velocityZ);
}

Vector
ClusteringVClient::GetPositionVector (ClusteringUtils::NeighborInfo mobilityInfo)
{
  return Vector (mobilityInfo.positionX, mobilityInfo.positionY, mobilityInfo.positionZ);
}

Vector
ClusteringVClient::GetPositionVector (ClusteringUtils::RsuInfo rsuInfo)
{
  return Vector (rsuInfo.positionX, rsuInfo.positionY, rsuInfo.positionZ);
}

double
ClusteringVClient::GetTwait (void)
{
  double commRange = 50.0;
  double distanceIdx;
  if (m_closestRsuInfo.nodeId == 500)
    {
      distanceIdx = 0;
    }
  else
    {
      Vector p = GetPositionVector (m_currentMobilityInfo);
      Vector pRsu = GetPositionVector (m_closestRsuInfo);
      distanceIdx = (50.0 - (p - pRsu).GetLength ()) / commRange;
    }
  double centrailityIdx = 1 - 1 / (m_neighborList.size () + 1);
  double connectivityIdx = (centrailityIdx + distanceIdx) / 2;
  return (1 - connectivityIdx) * m_tWaitMax;
}

/*------------------------------------------------------------------
-------------------------ClusteringRsuClient------------------------
--------------------------------------------------------------------*/

NS_OBJECT_ENSURE_REGISTERED (ClusteringRsuClient);

TypeId
ClusteringRsuClient::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::ClusteringRsuClient")
          .SetParent<Application> ()
          .AddConstructor<ClusteringRsuClient> ()
          .AddAttribute ("SimulationCase", "The case that the simulation run", UintegerValue (0),
                         MakeUintegerAccessor (&ClusteringRsuClient::m_simCase),
                         MakeUintegerChecker<uint8_t> (1));
  return tid;
}

// Public Members
ClusteringRsuClient::ClusteringRsuClient ()
{
  NS_LOG_FUNCTION (this);
  m_sendEvent = EventId ();
  m_sentCounter = 0;
  m_neighborList = std::map<uint64_t, ClusteringUtils::NeighborInfo> ();
  m_clusterList = std::map<uint64_t, ClusteringUtils::NeighborInfo> ();
  m_addressList = std::map<uint64_t, Mac48Address> ();
  m_deltaT = 30.0;
  // m_simCase = 0;
}

ClusteringRsuClient::~ClusteringRsuClient ()
{
  NS_LOG_FUNCTION (this);
  m_sendEvent = EventId ();
  m_sentCounter = 0;
  m_neighborList = std::map<uint64_t, ClusteringUtils::NeighborInfo> ();
  m_clusterList = std::map<uint64_t, ClusteringUtils::NeighborInfo> ();
  m_addressList = std::map<uint64_t, Mac48Address> ();
  m_simCase = 0;
}

void
ClusteringRsuClient::AppReport (std::ostream &os)
{
  os << "***********************" << std::endl
     << "  - Cluster Metrics -  " << std::endl
     << "Rsu Node:" << m_rsuInfo.nodeId << " Sent overal: " << m_sentCounter << " Packets."
     << std::endl
     << "***********************" << std::endl;
}

// Protected Members
void
ClusteringRsuClient::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  // chain up
  Application::DoDispose ();
}

void
ClusteringRsuClient::StartApplication (void)
{
  NS_LOG_FUNCTION (this);
  Packet::EnablePrinting ();
  m_process = SET_UP;
  const SchInfo schInfo = SchInfo (SCH1, false, EXTENDED_CONTINUOUS);
  std::cout << "Rsu Application starts" << std::endl;
  m_device = DynamicCast<WaveNetDevice> (Application::GetNode ()->GetDevice (0));
  StartListening ();
  m_device->StartSch (schInfo);
  UpdateRsuInfo ();
  ScheduleUpdateProcess ();
  ScheduleTransmit (Seconds (0.1 + 0.001 * m_rsuInfo.nodeId));
}

void
ClusteringRsuClient::StartListening (void)
{
  NS_LOG_FUNCTION (this);
  m_device->SetReceiveCallback (MakeCallback (&ClusteringRsuClient::HandleRead, this));
}

void
ClusteringRsuClient::ScheduleTransmit (Time dt)
{
  NS_LOG_FUNCTION (this << dt);
  m_sendEvent = Simulator::Schedule (dt, &ClusteringRsuClient::Send, this);
  NS_LOG_DEBUG ("[ScheduleTransmit] => NodeId:" << m_rsuInfo.nodeId
                                                << " EventInfo:" << m_sendEvent.GetTs ());
}

void
ClusteringRsuClient::Send (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("[Send] => NodeId:" << m_rsuInfo.nodeId << " EventInfo:" << m_sendEvent.GetTs ());
  NS_ASSERT (m_sendEvent.IsExpired ());
  Ptr<Packet> packet = Create<Packet> (0);
  switch (m_process)
    {
      case BEACON_EXCHANGE: {
        Mac48Address dest = Mac48Address::GetBroadcast ();
        ClusteringRsuBeaconHeader rsuBeaconHeader;
        rsuBeaconHeader.SetSeq (m_sentCounter);
        rsuBeaconHeader.SetRsuInfo (m_rsuInfo);
        packet->AddHeader (rsuBeaconHeader);
        const TxProfile txProfile = TxProfile (SCH1);
        m_device->RegisterTxProfile (txProfile);
        if (m_device->Send (packet, dest, 0))
          {
            ++m_sentCounter;
            m_device->DeleteTxProfile (SCH1);
            NS_LOG_INFO ("[Send] RSU_BEACON_EXCHANGE => At time "
                         << Simulator::Now ().GetSeconds () << "s rsu node " << m_rsuInfo.nodeId
                         << " sent " << packet->GetSize () << " bytes to " << dest);
          }

        ScheduleTransmit (Seconds (0.1));
        break;
      }
      // case NEIGHBOR_LIST_EXCHANGE: {
      //   break;
      // }
      case CLUSTER_FORMATION: {
        break;
      }
      case DATA_EXCHANGE: {
        break;
      }
    default:
      break;
    }
}

bool
ClusteringRsuClient::HandleRead (Ptr<NetDevice> dev, Ptr<const Packet> pkt, uint16_t mode,
                                 const Address &sender)
{
  Ptr<Packet> p = pkt->Copy ();
  if (pkt->GetSize () == 0)
    {
    }
  else
    {
      PacketMetadata::ItemIterator metaDataIterator = p->BeginItem ();
      PacketMetadata::Item item;
      while (metaDataIterator.HasNext ())
        {
          item = metaDataIterator.Next ();
          if (item.tid.GetName () == "ns3::ClusteringBeaconHeader")
            {
              ClusteringBeaconHeader rcvVBeaconHeader;
              p->RemoveHeader (rcvVBeaconHeader);
              ClusteringUtils::NeighborInfo otherMobilityInfo = rcvVBeaconHeader.GetMobilityInfo ();
              std::map<uint64_t, ClusteringUtils::NeighborInfo>::iterator itr =
                  m_neighborList.find (otherMobilityInfo.nodeId);

              if (itr == m_neighborList.end ())
                {
                  if (CheckOutOfTransmission (m_rsuInfo, rcvVBeaconHeader.GetMobilityInfo ()))
                    {
                    }
                  else
                    {
                      NS_LOG_INFO ("[Handle Read] RSU Node: " << m_rsuInfo.nodeId
                                                              << " Insert packet: "
                                                              << otherMobilityInfo.nodeId);
                      m_neighborList.insert ({otherMobilityInfo.nodeId, otherMobilityInfo});
                    }
                }
              else
                {
                  NS_LOG_DEBUG ("[Handle Read] At time "
                                << Simulator::Now ().GetSeconds () << "s RSU Node "
                                << m_rsuInfo.nodeId << " update node " << otherMobilityInfo.nodeId
                                << " in neighbor list");
                  itr->second = otherMobilityInfo;
                }

              // insert to address list
              std::map<uint64_t, Mac48Address>::iterator it =
                  m_addressList.find (otherMobilityInfo.nodeId);
              if (it == m_addressList.end ())
                {
                  AddressValue addressVal = AddressValue (sender);
                  std::string macStr = addressVal.SerializeToString (MakeAddressChecker ());
                  Mac48Address macAddress = Mac48Address (macStr.c_str ());
                  m_addressList.insert ({otherMobilityInfo.nodeId, macAddress});
                  NS_LOG_DEBUG ("[Handle Read] Node: "
                                << m_rsuInfo.nodeId << " added address of node "
                                << otherMobilityInfo.nodeId << " into address list");
                }
            }
          else if (item.tid.GetName () == "ns3::ClusteringRsuBeaconHeader")
            {
              NS_LOG_DEBUG ("[Handle Read] RSU node receive RSUBeaconHeader");
            }
          else if (item.tid.GetName () == "ns3::ClusteringFormClusterHeader")
            {
              ClusteringFormClusterHeader formClusterHeader;
              p->RemoveHeader (formClusterHeader);
              ClusteringUtils::NeighborInfo otherMobilityInfo =
                  formClusterHeader.GetMobilityInfo ();
              std::map<uint64_t, ClusteringUtils::NeighborInfo>::iterator itr =
                  m_neighborList.find (otherMobilityInfo.nodeId);
              if (itr == m_neighborList.end ())
                {
                  if (CheckOutOfTransmission (m_rsuInfo, formClusterHeader.GetMobilityInfo ()))
                    {
                    }
                  else
                    {
                      NS_LOG_INFO ("[Handle Read] RSU Node: " << m_rsuInfo.nodeId
                                                              << " Insert packet: "
                                                              << otherMobilityInfo.nodeId);
                      m_neighborList.insert ({otherMobilityInfo.nodeId, otherMobilityInfo});
                    }
                }
              else
                {
                  NS_LOG_DEBUG ("[Handle Read] At time "
                                << Simulator::Now ().GetSeconds () << "s RSU Node "
                                << m_rsuInfo.nodeId << " update node " << otherMobilityInfo.nodeId
                                << " in neighbor list");
                  itr->second = otherMobilityInfo;
                }
            }
          else if (item.tid.GetName () == "ns3::ClusteringDataHeader")
            {
              ;
            }
        }
    }
  return true;
}

bool
ClusteringRsuClient::CheckOutOfTransmission (ClusteringUtils::RsuInfo rsuInfo,
                                             ClusteringUtils::NeighborInfo mobilityInfo)
{
  Vector pRsu = GetPositionVector (rsuInfo);
  Vector pV = GetPositionVector (mobilityInfo) + Vector (mobilityInfo.velocityX * m_deltaT,
                                                         mobilityInfo.velocityY * m_deltaT,
                                                         mobilityInfo.velocityZ * m_deltaT);
  return ((pRsu - pV).GetLength () > 250.0);
}

void
ClusteringRsuClient::StopApplication (void)
{
  NS_LOG_FUNCTION (this);
  Simulator::Cancel (m_sendEvent);
  m_device->DeleteTxProfile (SCH1);
  m_device->StopSch (SCH1);
  std::cout << "Rsu Application stops" << std::endl;
  StatusReport ();
}

void
ClusteringRsuClient::StatusReport (void)
{
  NS_LOG_UNCOND (
      "\n\n-----------------------------------------------------------------------------");
  NS_LOG_UNCOND ("[StatusReport] => At time " << Simulator::Now ().GetSeconds () << "s rsu node ["
                                              << m_rsuInfo.nodeId << "] having  ===> \n position: ("
                                              << m_rsuInfo.positionX << "," << m_rsuInfo.positionY
                                              << "," << m_rsuInfo.positionZ
                                              << ")\n last packet sent:" << m_rsuInfo.ts << "s"
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
  NS_LOG_UNCOND ("-----------------------------  Address List  ---------------------------------");
  for (std::map<uint64_t, Mac48Address>::iterator it = m_addressList.begin ();
       it != m_addressList.end (); ++it)
    {
      uint64_t id = it->first;
      NS_LOG_UNCOND (" * key: " << id << " address: " << it->second);
    }
  NS_LOG_UNCOND ("-----------------------------  Address List  ---------------------------------");
}

void
ClusteringRsuClient::UpdateRsuInfo (void)
{
  m_rsuInfo.ts = Simulator::Now ().GetTimeStep ();
  m_rsuInfo.nodeId = Application::GetNode ()->GetId ();

  // Position and Velocity
  Vector p = this->GetNode ()->GetObject<ConstantPositionMobilityModel> ()->GetPosition ();
  m_rsuInfo.positionX = p.x;
  m_rsuInfo.positionY = p.y;
  m_rsuInfo.positionZ = p.z;
}

void
ClusteringRsuClient::ScheduleUpdateProcess (void)
{
  Time now = Simulator::Now ();
  switch (m_process)
    {
      case BEACON_EXCHANGE: {
        // Simulator::Cancel (m_eventBeaconExchange);
        // Simulator::Cancel (m_sendEvent);
        // m_process = ClusteringRsuClient::NEIGHBOR_LIST_EXCHANGE;
        // Simulator::Schedule (Seconds (1.0), &ClusteringRsuClient::ScheduleUpdateProcess, this);
        // ScheduleTransmit (Seconds (0.1));
        // break;

        NS_LOG_INFO ("update cluster formation process");
        // Simulator::Cancel (m_eventNeighborExchange);
        Simulator::Cancel (m_sendEvent);
        m_process = CLUSTER_FORMATION;
        Simulator::Schedule (Seconds (2.0), &ClusteringRsuClient::ScheduleUpdateProcess, this);
        break;
      }
      // case NEIGHBOR_LIST_EXCHANGE: {
      // NS_LOG_INFO ("update cluster formation process");
      // // Simulator::Cancel (m_eventNeighborExchange);
      // Simulator::Cancel (m_sendEvent);
      // m_process = CLUSTER_FORMATION;
      // Simulator::Schedule (Seconds (2.0), &ClusteringRsuClient::ScheduleUpdateProcess, this);
      // break;
      // }
      case CLUSTER_FORMATION: {

        // Simulator::Cancel (m_eventElection);
        Simulator::Cancel (m_sendEvent);
        m_process = DATA_EXCHANGE;
        Simulator::Schedule (Seconds (30.0), &ClusteringRsuClient::ScheduleUpdateProcess, this);
        break;
      }
      case DATA_EXCHANGE: {
        // Simulator::Cancel (m_eventSendData);
        Simulator::Cancel (m_sendEvent);
        ResetCycleTime ();
        m_process = BEACON_EXCHANGE;
        Simulator::Schedule (Seconds (1.0), &ClusteringRsuClient::ScheduleUpdateProcess, this);
        break;
      }
      case SET_UP: {
        m_process = BEACON_EXCHANGE;
        Simulator::Schedule (Seconds (1.0), &ClusteringRsuClient::ScheduleUpdateProcess, this);
        break;
      }
    default:
      NS_LOG_DEBUG ("[Default case] In Schedule Update Process");
      break;
    }
}

void
ClusteringRsuClient::ResetCycleTime (void)
{
  std::cout << "This is ResetCycleTime function" << std::endl;
  m_clusterList.clear ();
  m_neighborList.clear ();
}

Vector
ClusteringRsuClient::GetVelocityVector (ClusteringUtils::NeighborInfo mobilityInfo)
{
  return Vector (mobilityInfo.velocityX, mobilityInfo.velocityY, mobilityInfo.velocityZ);
}

Vector
ClusteringRsuClient::GetPositionVector (ClusteringUtils::NeighborInfo mobilityInfo)
{
  return Vector (mobilityInfo.positionX, mobilityInfo.positionY, mobilityInfo.positionZ);
}

Vector
ClusteringRsuClient::GetPositionVector (ClusteringUtils::RsuInfo rsuInfo)
{
  return Vector (rsuInfo.positionX, rsuInfo.positionY, rsuInfo.positionZ);
}

} // namespace ns3
