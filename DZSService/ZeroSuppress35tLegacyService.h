// ZeroSuppress35tLegacyService.h
//
// David Adams
// November 2015
//
// Service wrapper for legacy 35-ton zero suppression.
// Code taken from run.cxx in dunetpc v04.29.02.

#ifndef ZeroSuppress35tLegacyService_H
#define ZeroSuppress35tLegacyService_H

#include "DZSCore/ZeroSuppress35t.h"
#include "DZSService/ZeroSuppressServiceBase.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

#include <memory>
#include <string>
#include <iostream>

namespace fhicl {
class ParameterSet;
}
namespace art {
class ActivityRegistry;
}

class ZeroSuppress35tLegacyService : public ZeroSuppressBase {

public:

  // Ctor.
  ZeroSuppress35tService(fhicl::ParameterSet const& pset, art::ActivityRegistry&);

  // Filter an array of signals. Result is written to keep.
  int filter(const SignalVector& sigs, Channel chan, Pedestal& ped, ResultVector& keep) const;

  // Print the configuration.
  std::ostream& print(std::ostream& out =std::cout, std::string prefix ="") const;


private:

  // Parameters.
  unsigned int  fZeroThreshold;    // Zero suppression threshold
  int           fNearestNeighbor;  // Maximum distance between hits above threshold before they are separated into different blocks


};

DECLARE_ART_SERVICE_INTERFACE_IMPL(ZeroSuppress35tLegacyService, ZeroSuppressBase, LEGACY)

#endif
