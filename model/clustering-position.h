/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef POSITION_H
#define POSITION_H

#include <string>
#include <vector>
#include "ns3/node-container.h"


namespace ns3 {

/* ... */

class PosInfo;

class PosInfo {
  private:
    int nodeCount;
    std::vector<double> PosList;

  public:
    PosInfo (void);
    PosInfo (const std::string& posfile);
    NodeContainer GetNodeContainer (void);
    NodeContainer GetNodeContainer (uint32_t lanes, double lane_width);
    NodeContainer GetNodeContainer (uint32_t lanes, double lane_width, uint32_t nodeNum);
};

}

#endif /* POSITION_H */

