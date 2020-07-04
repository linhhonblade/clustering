/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/clustering-position.h"
#include "ns3/node-container.h"
#include "ns3/packet.h"
#include "ns3/node.h"
#include "ns3/simulator.h"
#include "ns3/clustering-header.h"
#include "ns3/clustering-utils.h"

using namespace ns3;


int
main (int argc, char *argv[])
{
  bool verbose = true;

  CommandLine cmd;
  cmd.AddValue ("verbose", "Tell application to log if true", verbose);

  cmd.Parse (argc,argv);

  /* ... */

  // Test position
  PosInfo posInfo = PosInfo ("position_3_1_10_600_50_3.txt");
  NodeContainer nodes = posInfo.GetNodeContainer (3, 7.0);
  std::cout << nodes.GetN() << std::endl;

  // Test headers

  Packet::EnablePrinting();
  ClusteringBeaconHeader beaconHeader;
  ClusteringRsuBeaconHeader rsuBeaconHeader;
  ClusteringFormClusterHeader formClusterHeader;
  ClusteringDataHeader dataHeader;

  ClusteringUtils::NeighborInfo mobilityInfo {
    Simulator::Now ().GetTimeStep (),
    2,
    3,
    1.0,
    0.0,
    1.0,
    0.0,
    0.0,
    0.0,
    ClusteringUtils::NodeState::CM
  };

  ClusteringUtils::RsuInfo rsuInfo {
    Simulator::Now ().GetTimeStep (),
    1,
    2.2,
    134.2,
    1.0
  };

  beaconHeader.SetMobilityInfo (mobilityInfo);
  Ptr<Packet> p = Create<Packet> ();

  p->AddHeader (beaconHeader);
  std::cout << "This is Beacon Packet" << std::endl;
  p->Print (std::cout);
  std::cout << std::endl;
  std::cout << "Data retrieved from the header is" << std::endl;
  ClusteringBeaconHeader rcvBeacon;
  p->RemoveHeader (rcvBeacon);
  rcvBeacon.Print (std::cout);

  rsuBeaconHeader.SetRsuInfo (rsuInfo);
  p->AddHeader(rsuBeaconHeader);
  std::cout << "This is Rsu Beacon Packet" << std::endl;
  p->Print (std::cout);
  std::cout << std::endl;
  std::cout << "Data retrieved from the header is" << std::endl;
  ClusteringRsuBeaconHeader rcvRsuBeacon;
  p->RemoveHeader (rcvRsuBeacon);
  rcvRsuBeacon.Print (std::cout);
  std::cout << std::endl;


  return 0;
}


