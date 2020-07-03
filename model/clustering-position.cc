/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "clustering-position.h"
#include "ns3/log.h"
#include "ns3/mobility-helper.h"
#include <fstream>
#include <string>
#include "ns3/mobility-module.h"
namespace ns3 {

/* ... */

NS_LOG_COMPONENT_DEFINE ("PositionModel");

PosInfo::PosInfo (void) {
  NS_LOG_FUNCTION (this);
}

PosInfo::PosInfo (const std::string& posfile) {
  NS_LOG_FUNCTION (this);
  std::ifstream infile;
  nodeCount = 0;
  PosList = std::vector<double>(0,0);
  infile.open (posfile);
  if (!infile) {
    NS_LOG_ERROR("Cannot open Position file");
  }
  double x;
  while (infile >> x) {
    PosList.push_back(x);
    nodeCount++;
  }
  infile.close();
}

NodeContainer PosInfo::GetNodeContainer (void) {
  NodeContainer nodes = NodeContainer();
  nodes.Create(nodeCount);
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  for (std::vector<double>::iterator it = PosList.begin(); it != PosList.end(); ++it) {
    if ((it - PosList.begin()) % 2 == 0) {
      positionAlloc->Add (Vector (*it, 0.0, 0.0));
    } else {
      positionAlloc->Add (Vector (*it, 10.0, 0.0));
    }

  }
  mobility.SetPositionAllocator (positionAlloc);
  //mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  //mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);
  // for (uint i = 0; i < nodes.GetN (); i++) {
  //   Ptr<ConstantVelocityMobilityModel> mob = nodes.Get(i)->GetObject<ConstantVelocityMobilityModel>();
  //   mob->SetVelocity (Vector (22.0, 0.0, 0.0));
  // }


  return nodes;
}

NodeContainer PosInfo::GetNodeContainer (uint32_t lanes, double lane_width) {
  NodeContainer nodes = NodeContainer();
  nodes.Create(nodeCount);
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  for (std::vector<double>::iterator it = PosList.begin(); it != PosList.end(); ++it) {
    positionAlloc->Add (Vector (*it, lane_width * ((it - PosList.begin()) % lanes), 0.0));
  }
  mobility.SetPositionAllocator (positionAlloc);
  //mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  //mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);
  // for (uint i = 0; i < nodes.GetN (); i++) {
  //   Ptr<ConstantVelocityMobilityModel> mob = nodes.Get(i)->GetObject<ConstantVelocityMobilityModel>();
  //   mob->SetVelocity (Vector (22.0, 0.0, 0.0));
  // }


  return nodes;
}

NodeContainer PosInfo::GetNodeContainer (uint32_t lanes, double lane_width, uint32_t nodeNum)
{
  NodeContainer nodes = NodeContainer();
  nodes.Create(nodeNum);
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  for (std::vector<double>::iterator it = PosList.begin(); (it != PosList.end()) || (it - PosList.begin() < nodeNum); ++it) {
    positionAlloc->Add (Vector (*it, lane_width * ((it - PosList.begin()) % lanes), 0.0));
  }
  mobility.SetPositionAllocator (positionAlloc);
  //mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  //mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);
  // for (uint i = 0; i < nodes.GetN (); i++) {
  //   Ptr<ConstantVelocityMobilityModel> mob = nodes.Get(i)->GetObject<ConstantVelocityMobilityModel>();
  //   mob->SetVelocity (Vector (22.0, 0.0, 0.0));
  // }
  return nodes;
}

}

