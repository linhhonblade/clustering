/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/clustering-position.h"
#include "ns3/node-container.h"
#include "ns3/packet.h"
#include "ns3/node.h"
#include "ns3/simulator.h"
// #include "ns3/clustering-header.h"
// #include "ns3/clustering-utils.h"

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

  return 0;
}


