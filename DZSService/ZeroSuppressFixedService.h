// ZeroSuppressFixedService.h
//
// David Adams
// November 2015
//
// Service wrapper for fixed zero suppression.
// For now all bins are accepted.
// Later we may add an array for each channel.
// No parameters (yet).

#ifndef ZeroSuppressFixedService_H
#define ZeroSuppressFixedService_H

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

class ZeroSuppressFixedService : public ZeroSuppressBase {

public:

  // Ctor from fcl.
  ZeroSuppressFixedService(fhicl::ParameterSet const& pset, art::ActivityRegistry&);

  // Ctor from direct params.
  ZeroSuppressFixedService();

  // Filter an array of signals. Result is written to keep.
  int filter(const SignalVector& sigs, Channel chan, Pedestal& ped, ResultVector& keep) const;

  // Print the configuration.
  std::ostream& print(std::ostream& out =std::cout, std::string prefix ="") const;

private:

  // Parameters.

};

DECLARE_ART_SERVICE_INTERFACE_IMPL(ZeroSuppressFixedService, ZeroSuppressBase, LEGACY)

#endif