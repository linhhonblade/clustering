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
  double simTime = 10.0;
  uint16_t numberOfVehicles = 3;
  uint16_t numberOfRsu = 1;
  std::string vPosfile = "position_v.txt";
  std::string rsuPosfile = "position_rsu.txt";
  std::string vMobilityModel = "ns3::ConstantPositionMobilityModel";
  /*----------------------------------------------------------------------*/

  // bool verbose = true;

  /*-------------------- Command Line Argument Values --------------------*/
  CommandLine cmd;
  cmd.AddValue ("simTime", "Simulation Time in Seconds", simTime);
  cmd.AddValue ("numberOfVehicles", "Number of Vehicles in simulation", numberOfVehicles);
  cmd.AddValue ("numberOfRsu", "Number of Rsu in simulation", numberOfRsu);
  cmd.AddValue ("vPosfile", "name of txt file containing positions of vNodes", vPosfile);
  cmd.AddValue ("rsuPosfile", "name of txt file containing position of rsuNodes", rsuPosfile);

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
  /*----------------------------------------------------------------------*/

  // Create  vNodes
  PosInfo posInfo = PosInfo (vPosfile);
  NodeContainer vNodes = posInfo.GetNodeContainer (3, 4.0, numberOfVehicles, vMobilityModel);
  // Create rsuNodes
  posInfo = PosInfo (rsuPosfile);
  NodeContainer rsuNodes = posInfo.GetNodeContainer (1, 0.0, numberOfRsu, vMobilityModel);

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
      ClusteringVClientHelper vClient;
      vApps.Add (vClient.Install (vNodes.Get (u)));
    }
  for (uint32_t t = 0; t < rsuNodes.GetN (); ++t)
    {
      ClusteringRsuClientHelper rsuClient;
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
