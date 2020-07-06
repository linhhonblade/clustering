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

#ifndef CLUSTERING_CLIENT_H
#define CLUSTERING_CLIENT_H

#include <map>
#include "ns3/ptr.h"
#include "ns3/double.h"
#include "ns3/address.h"
#include "ns3/event-id.h"
#include "ns3/application.h"
#include "ns3/traced-callback.h"
#include "ns3/clustering-utils.h"
#include "ns3/clustering-header.h"
#include "ns3/wave-net-device.h"

namespace ns3 {

class Address;

class ClusteringVClient: public Application {
public:

	enum MyProcess{
        BEACON_EXCHANGE = 0,
        NEIGHBOR_LIST_EXCHANGE,
        CLUSTER_FORMATION,
        DATA_EXCHANGE,
        SET_UP,
        CURRENT_PROCESS
	};

	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
    static TypeId GetTypeId (void);

	ClusteringVClient();
    ClusteringVClient(uint32_t deltaT);
    virtual ~ClusteringVClient ();

protected:
    virtual void DoDispose (void);

private:
	/// inherited from Application base class.
    virtual void StartApplication (void);    // Called at time specified by Start
    virtual void StopApplication (void);     // Called at time specified by Stop

    void UpdateProcess (void);
    void StartBeaconExchange (void);
    void EndBeaconExchange (void);
    void StartNeighborListExchange (void);
    void EndNeighborListExchange (void);
    void StartElection (void);
    void QuitElection (void);
    void EndElection (void);
    void SendData (void);
    bool HandleRead (Ptr<NetDevice> dev, Ptr<const Packet> pkt, uint16_t mode, const Address &sender);
    void UpdateCurrentMobilityInfo (void);
    void StatusReport (void);
    void AppReport (std::ostream &os);
    void ScheduleTransmit (Time dt);
    void ResetCycleTime (void);
    void ScheduleUpdateProcess (void);
    void Send (void);
    void StartListening (void);
    bool CheckOutOfTransmission (ClusteringUtils::NeighborInfo a, ClusteringUtils::NeighborInfo b);
    Vector GetVelocityVector (ClusteringUtils::NeighborInfo mobilityInfo);
    Vector GetPositionVector (ClusteringUtils::RsuInfo rsuInfo);
    Vector GetPositionVector (ClusteringUtils::NeighborInfo mobilityInfo);

    enum MyProcess m_process;

    EventId m_eventElection;
    EventId m_eventBeaconExchange;
    EventId m_eventNeighborExchange;
    EventId m_eventDataExchange;
    EventId m_eventSendData;
    EventId m_sendEvent;

    std::map<uint64_t, ClusteringUtils::NeighborInfo> m_clusterList;
    std::map<uint64_t, ClusteringUtils::NeighborInfo> m_neighborList;
    ClusteringUtils::NeighborInfo m_currentMobilityInfo;
    Address m_peerIncident;
    ClusteringUtils::RsuInfo m_closestRsuInfo;
    ClusteringUtils::DataInfo m_dataInfo;   // received data info
    uint32_t m_sentCounter; 	//!< Counter for sent packets
    uint32_t m_formationCounter; //!< Counter for sent cluster formation
    uint32_t m_cycleCounter;     //!< Countere for cycle number
    uint32_t m_deltaT;

    ClusteringUtils m_utils;
    Ptr<WaveNetDevice> m_device;


};

} // namespace ns3

#endif /* CLUSTERING_CLIENT_H */
