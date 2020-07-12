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

#include <ns3/names.h>
#include "ns3/application-container.h"
#include "clustering-client-helper.h"

namespace ns3 {


/*--------------------------------------------------------------
----------------- ClusteringVClientHelper ----------------------
----------------------------------------------------------------*/


ClusteringVClientHelper::ClusteringVClientHelper(uint8_t simCase)
{
  m_factory.SetTypeId ("ns3::ClusteringVClient");
  m_factory.Set ("SimulationCase", UintegerValue (simCase));
}

void
ClusteringVClientHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
ClusteringVClientHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
ClusteringVClientHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
ClusteringVClientHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
ClusteringVClientHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<ClusteringVClient> ();
  node->AddApplication (app);

  return app;
}



/*--------------------------------------------------------------------
------------------ ClusteringRsuClientHelper -------------------------
----------------------------------------------------------------------*/

ClusteringRsuClientHelper::ClusteringRsuClientHelper(uint8_t simCase)
{
  m_factory.SetTypeId ("ns3::ClusteringRsuClient");
  m_factory.Set ("SimulationCase", UintegerValue (simCase));
}

void
ClusteringRsuClientHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
ClusteringRsuClientHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
ClusteringRsuClientHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
ClusteringRsuClientHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
ClusteringRsuClientHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<ClusteringRsuClient> ();
  node->AddApplication (app);

  return app;
}

} // namespace ns3
