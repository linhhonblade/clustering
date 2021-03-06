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

class ClusteringClientTestCase : public TestCase
{
  public:
    ClusteringClientTestCase ();
    virtual ~ClusteringClientTestCase ();
  private:
    virtual void DoRun (void);
};
class ClusteringPositionTestCase : public TestCase
{
public:
  ClusteringPositionTestCase ();
  virtual ~ClusteringPositionTestCase ();

private:
  virtual void DoRun (void);
};

class ClusteringBeaconHeaderTestCase : public TestCase
{
  public:
    ClusteringBeaconHeaderTestCase ();
    virtual ~ClusteringBeaconHeaderTestCase ();
  private:
    virtual void DoRun (void);
};

class ClusteringRsuBeaconHeaderTestCase : public TestCase
{
  public:
    ClusteringRsuBeaconHeaderTestCase ();
    virtual ~ClusteringRsuBeaconHeaderTestCase ();
  private:
    virtual void DoRun (void);
};

class ClusteringFormClusterHeaderTestCase : public TestCase
{
  public:
    ClusteringFormClusterHeaderTestCase ();
    virtual ~ClusteringFormClusterHeaderTestCase ();
  private:
    virtual void DoRun (void);
};

class ClusteringDataHeaderTestCase : public TestCase
{
  public:
    ClusteringDataHeaderTestCase ();
    virtual ~ClusteringDataHeaderTestCase ();
  private:
    virtual void DoRun (void);
};


// Add some help text to this case to describe what it is intended to test

ClusteringClientTestCase::ClusteringClientTestCase ()
  : TestCase ("Dunno what to check yet")
{
}

ClusteringPositionTestCase::ClusteringPositionTestCase ()
  : TestCase ("Check reading from txt file")
{
}

ClusteringBeaconHeaderTestCase::ClusteringBeaconHeaderTestCase ()
  : TestCase("Check V2vClusterInfoHeader class serialization-deserialization")
{}

ClusteringRsuBeaconHeaderTestCase::ClusteringRsuBeaconHeaderTestCase ()
  : TestCase("Check V2vClusterInfoHeader class serialization-deserialization")
{}

ClusteringFormClusterHeaderTestCase::ClusteringFormClusterHeaderTestCase ()
  : TestCase("Check V2vClusterInfoHeader class serialization-deserialization")
{}

ClusteringDataHeaderTestCase::ClusteringDataHeaderTestCase ()
  : TestCase("Check V2vClusterInfoHeader class serialization-deserialization")
{}

ClusteringClientTestCase::~ClusteringClientTestCase () {}
ClusteringPositionTestCase::~ClusteringPositionTestCase () {}
ClusteringBeaconHeaderTestCase::~ClusteringBeaconHeaderTestCase () {}
ClusteringRsuBeaconHeaderTestCase::~ClusteringRsuBeaconHeaderTestCase () {}
ClusteringFormClusterHeaderTestCase::~ClusteringFormClusterHeaderTestCase () {}
ClusteringDataHeaderTestCase::~ClusteringDataHeaderTestCase () {}

void ClusteringClientTestCase::DoRun(void) {
  Simulator::Run();
  Simulator::Destroy();
}

void ClusteringPositionTestCase::DoRun(void) {

    Simulator::Run();
    Simulator::Destroy();
}
void
ClusteringBeaconHeaderTestCase::DoRun (void)
{
  Simulator::Run();
  Simulator::Destroy();
}
void
ClusteringRsuBeaconHeaderTestCase::DoRun (void)
{
  Simulator::Run();
  Simulator::Destroy();
}
void
ClusteringDataHeaderTestCase::DoRun (void)
{
  Simulator::Run();
  Simulator::Destroy();
}
void
ClusteringFormClusterHeaderTestCase::DoRun (void)
{
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
  AddTestCase (new ClusteringClientTestCase, TestCase::QUICK);
  AddTestCase (new ClusteringPositionTestCase, TestCase::QUICK);
  AddTestCase (new ClusteringBeaconHeaderTestCase, TestCase::QUICK);
  AddTestCase (new ClusteringRsuBeaconHeaderTestCase, TestCase::QUICK);
  AddTestCase (new ClusteringFormClusterHeaderTestCase, TestCase::QUICK);
  AddTestCase (new ClusteringDataHeaderTestCase, TestCase::QUICK);
}

// Do not forget to allocate an instance of this TestSuite
static ClusteringTestSuite clusteringTestSuite;

