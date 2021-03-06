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

#ifndef CLUSTERING_UTILS_H
#define CLUSTERING_UTILS_H

#include <list>
#include "ns3/nstime.h"
#include "ns3/vector.h"

namespace ns3 {

class ClusteringUtils
{

public:
  virtual ~ClusteringUtils ();

  enum NodeState { STANDALONE = 0, CH, CM, NODE_STATES };

  enum DataType { EMERGENCY_EVENT = 0, NOTIFICATION_EVENT, DATA_TYPES };

  struct NeighborInfo
  {
    int64_t ts;
    uint64_t nodeId;
    uint64_t CID;
    double positionX;
    double positionY;
    double positionZ;
    double velocityX;
    double velocityY;
    double velocityZ;
    NodeState state;
  };

  struct RsuInfo
  {
    int64_t ts;
    uint64_t nodeId;
    double positionX;
    double positionY;
    double positionZ;
  };

  struct DataInfo
  {
    uint64_t srcNode;
    uint64_t destNode;
    uint64_t CID;
    DataType dataType;
  };

private:
};

} // namespace ns3

#endif // CLUSTERING_UTILS_H
