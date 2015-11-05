// ZeroSuppress35tService.h
//
// David Adams
// November 2015
//
// Service wrapper for 35-ton zero suppression.

#include "DZSCore/ZeroSuppress35t.h"
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

class ZeroSuppress35tService {

public:

  // Ctor.
  ZeroSuppress35tService(fhicl::ParameterSet const& pset, art::ActivityRegistry&);

  // Filter an array of signals. Result is written to keep.
  int filter(const ZeroSuppress35t::SignalVector& sigs, ZeroSuppress35t::ResultVector& keep) const;

  // Filter an array of signals.
  // Suppressed signals are replaced with FCL parameter Zero.
  int filter(ZeroSuppress35t::SignalVector& sigs) const;

  // Print the configuration.
  std::ostream& print(std::ostream& out =std::cout, std::string prefix ="  ") const;


private:

  // Pointer to the class that does the work.
  std::shared_ptr<ZeroSuppress35t> m_pzs;

};

DECLARE_ART_SERVICE(ZeroSuppress35tService, LEGACY)
