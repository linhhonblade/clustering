/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

// Include a header file from your module to test.
#include "ns3/clustering-position.h"

// An essential include is test.h
#include "ns3/test.h"
#include "ns3/simulator.h"


// Do not put your test classes in namespace ns3.  You may find it useful
// to use the using directive to access the ns3 namespace directly
using namespace ns3;

// This is an example TestCase.
class ClusteringPositionTestCase : public TestCase
{
public:
  ClusteringPositionTestCase ();
  virtual ~ClusteringPositionTestCase ();

private:
  virtual void DoRun (void);
};


// Add some help text to this case to describe what it is intended to test
ClusteringPositionTestCase::ClusteringPositionTestCase ()
  : TestCase ("Check reading from txt file")
{
}

ClusteringPositionTestCase::~ClusteringPositionTestCase () {}

void ClusteringPositionTestCase::DoRun(void) {

    Simulator::Run();
    Simulator::Destroy();
}

class ClusteringTestSuite : public TestSuite
{
public:
  ClusteringTestSuite ();
};

ClusteringTestSuite::ClusteringTestSuite ()
  : TestSuite ("clustering", UNIT)
{
  // TestDuration for TestCase can be QUICK, EXTENSIVE or TAKES_FOREVER
  AddTestCase (new ClusteringPositionTestCase, TestCase::QUICK);
}

// Do not forget to allocate an instance of this TestSuite
static ClusteringTestSuite clusteringTestSuite;

