/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/clustering-position.h"
#include "ns3/node-container.h"
#include "ns3/packet.h"
#include "ns3/node.h"
#include "ns3/simulator.h"
#include "ns3/clustering-header.h"
#include "ns3/clustering-utils.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/wave-net-device.h"
#include "ns3/wave-mac-helper.h"
#include "ns3/wave-helper.h"
#include "ns3/net-device-container.h"
#include "ns3/application-container.h"
#include "ns3/clustering-client-helper.h"
#include "ns3/mobility-module.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ClusteringExample");

int
main (int argc, char *argv[])
{

  /* ... */

  // // Test position
  // PosInfo posInfo = PosInfo ("position_3_1_10_600_50_3.txt");
  // NodeContainer nodes = posInfo.GetNodeContainer (3, 7.0);
  // std::cout << nodes.GetN () << std::endl;

  // // Test headers

  // Packet::EnablePrinting ();
  // ClusteringBeaconHeader beaconHeader;
  // ClusteringRsuBeaconHeader rsuBeaconHeader;
  // ClusteringFormClusterHeader formClusterHeader;
  // ClusteringDataHeader dataHeader;

  // ClusteringUtils::NeighborInfo mobilityInfo{
  //     Simulator::Now ().GetTimeStep (), 2, 0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0,
  //     ClusteringUtils::NodeState::CM};

  // ClusteringUtils::RsuInfo rsuInfo{Simulator::Now ().GetTimeStep (), 1, 2.2, 134.2, 1.0};

  // beaconHeader.SetMobilityInfo (mobilityInfo);
  // Ptr<Packet> p = Create<Packet> ();

  // p->AddHeader (beaconHeader);
  // std::cout << "This is Beacon Packet" << std::endl;
  // p->Print (std::cout);
  // std::cout << std::endl;
  // std::cout << "Data retrieved from the header is" << std::endl;
  // ClusteringBeaconHeader rcvBeacon;
  // p->RemoveHeader (rcvBeacon);
  // rcvBeacon.Print (std::cout);

  // rsuBeaconHeader.SetRsuInfo (rsuInfo);
  // p->AddHeader (rsuBeaconHeader);
  // std::cout << "This is Rsu Beacon Packet" << std::endl;
  // p->Print (std::cout);
  // std::cout << std::endl;
  // std::cout << "Data retrieved from the header is" << std::endl;
  // ClusteringRsuBeaconHeader rcvRsuBeacon;
  // p->RemoveHeader (rcvRsuBeacon);
  // rcvRsuBeacon.Print (std::cout);
  // std::cout << std::endl;

  // Test Application
  /*--------------------- Logging System Configuration -------------------*/
  LogLevel logLevel = (LogLevel) (LOG_INFO);
  LogComponentEnable ("ClusteringExample", logLevel);
  LogComponentEnable ("ClusteringClient", logLevel);

  NS_LOG_INFO ("/------------------------------------------------\\");
  NS_LOG_INFO (" - ClusteringExample [Example] -> Cluster vehicles communication");
  NS_LOG_INFO ("\\------------------------------------------------/");
  /*----------------------------------------------------------------------*/

  /*---------------------- Simulation Default Values ---------------------*/
  double simTime = 10.1;
  uint64_t sender = 9;
  uint64_t receiver = 8;
  double lane_distance = 4.0;

  // Case 1
  uint16_t numberOfVehicles = 5;
  uint16_t numberOfRsu = 2;
  // std::string vPosfile = "position_v.txt";
  // std::string rsuPosfile = "position_rsu.txt";

  // Case 2
  // uint16_t numberOfVehicles = 17;
  // uint16_t numberOfRsu = 1;
  std::string vPosfile = "position_v.txt";
  std::string rsuPosfile = "position_rsu_3.txt";

  std::string rsuMobilityModel = "ns3::ConstantPositionMobilityModel";
  std::string vMobilityModel = "ns3::ConstantVelocityMobilityModel";
  // std::string vMobilityModel = "ns3::ConstantPositionMobilityModel";
  uint8_t simCase = 1;
  double minVelocity = 10.3535;
  double maxVelocity = 11.8686;
  // double minVelocity = 0;
  // double maxVelocity = 0;
  uint32_t rngSeed = 1;
  /*----------------------------------------------------------------------*/

  // bool verbose = true;

  /*-------------------- Command Line Argument Values --------------------*/
  CommandLine cmd;
  cmd.AddValue ("simTime", "Simulation Time in Seconds", simTime);
  cmd.AddValue ("numberOfVehicles", "Number of Vehicles in simulation", numberOfVehicles);
  cmd.AddValue ("numberOfRsu", "Number of Rsu in simulation", numberOfRsu);
  cmd.AddValue ("vPosfile", "name of txt file containing positions of vNodes", vPosfile);
  cmd.AddValue ("rsuPosfile", "name of txt file containing position of rsuNodes", rsuPosfile);
  cmd.AddValue ("MinVelocity", "Minimum velocity of nodes in m/s", minVelocity);
  cmd.AddValue ("MaxVelocity", "Maximum velocity of nodes m/s", maxVelocity);
  cmd.AddValue ("RngSeed", "seed", rngSeed);

  cmd.Parse (argc, argv);

  NS_LOG_INFO ("");
  NS_LOG_INFO ("|---"
               << " SimTime -> " << simTime << " ---|\n");

  NS_LOG_INFO ("|---"
               << " NumberOfVehicles -> " << numberOfVehicles << " ---|\n");
  NS_LOG_INFO ("|---"
               << " NumberOfRsu -> " << numberOfRsu << " ---|\n");
  NS_LOG_INFO ("|---"
               << " vPosfile -> " << vPosfile << " ---|\n");
  NS_LOG_INFO ("|---"
               << " rsuPosfile -> " << rsuPosfile << " ---|\n");
  NS_LOG_INFO ("|---"
               << " minVelocity -> " << minVelocity << " ---|\n");
  NS_LOG_INFO ("|---"
               << " maxVelocity -> " << maxVelocity << " ---|\n");
  NS_LOG_INFO ("|---"
               << " rngSeed -> " << rngSeed << " ---|\n");

  /*------------------------------] At 1.16099s node 18 received a Form CLuster packet: Quit Election and join cluster 6
[Handle Read] At 1.16099s node 2 received a Form CLuster packet: Quit Election and join cluster 6
[Handle Read] At 1.16099s node 0 received a Form CLuster packet: Quit Election and join cluster 6
[Handle Read] At 1.16099s node 19 received a Form CLuster packet: Quit Election and join cluster 6
[Handle Read] At 1.16099s node 12 received a Form CLuster packet: Quit Election and join cluster 6
[Handle Read] At 1.16099s node 5 received a Form CLuster packet: Quit Election and join cluster 6
[Handle Read] At 1.16099s node 9 received a Form CLuster packet: Quit Election and join cluster 6----------------------------------------*/

  // Create  vNodes
  PosInfo posInfo = PosInfo (vPosfile);
  NodeContainer vNodes = posInfo.GetNodeContainer (3, lane_distance, numberOfVehicles,
                                                   vMobilityModel, maxVelocity, minVelocity);
  // Create rsuNodes
  posInfo = PosInfo (rsuPosfile);
  NodeContainer rsuNodes = posInfo.GetNodeContainer (1, 0.0, numberOfRsu, rsuMobilityModel, 0, 0);

  // Setup MAC and PHY layer
  YansWifiChannelHelper waveChannel = YansWifiChannelHelper::Default ();
  YansWavePhyHelper wavePhy = YansWavePhyHelper::Default ();
  wavePhy.SetChannel (waveChannel.Create ());
  wavePhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11);
  QosWaveMacHelper waveMac = QosWaveMacHelper::Default ();
  WaveHelper waveHelper = WaveHelper::Default ();
  NetDeviceContainer vDevices = waveHelper.Install (wavePhy, waveMac, vNodes);
  NetDeviceContainer rsuDevices = waveHelper.Install (wavePhy, waveMac, rsuNodes);

  ApplicationContainer vApps;
  ApplicationContainer rsuApps;

  for (uint32_t u = 0; u < vNodes.GetN (); ++u)
    {
      // install application
      ClusteringVClientHelper vClient (simCase);
      if (u == sender)
        {
          vClient.SetAttribute ("IsSender", BooleanValue (true));
          vClient.SetAttribute ("PeerNode", UintegerValue (receiver));
        }
      vApps.Add (vClient.Install (vNodes.Get (u)));
    }
  for (uint32_t t = 0; t < rsuNodes.GetN (); ++t)
    {
      ClusteringRsuClientHelper rsuClient (simCase);
      rsuApps.Add (rsuClient.Install (rsuNodes.Get (t)));
    }

  vApps.Start (Seconds (0.1));
  vApps.Stop (Seconds (simTime - 0.1));
  rsuApps.Start (Seconds (0.1));
  rsuApps.Stop (Seconds (simTime - 0.1));

  /*---------------------- Simulation Stopping Time ----------------------*/
  Simulator::Stop (Seconds (simTime));
  /*----------------------------------------------------------------------*/

  /*--------------------------- Simulation Run ---------------------------*/
  Simulator::Run ();
  Simulator::Destroy ();
  /*----------------------------------------------------------------------*/
  return 0;
}
