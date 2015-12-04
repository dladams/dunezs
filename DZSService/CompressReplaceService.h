// CompressReplaceService.h
//
// David Adams
// November 2015
//
// Service wrapper for the element-replacing compressor.
//
// FCL parameters
//   Zero [0] - replacement value

#ifndef CompressReplaceService_H
#define CompressReplaceService_J

#include "DZSCore/CompressReplace.h"
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

class CompressReplaceService {

public:

  // Ctor.
  CompressReplaceService(fhicl::ParameterSet const& pset, art::ActivityRegistry&);

  // Compress a vector of signals.
  // Suppressed signals are replaced with the value of offset + (FCL parameter) Zero.
  int compress(CompressReplace::SignalVector& sigs,
               const CompressReplace::FilterVector& keep,
               CompressReplace::Signal offset) const;

  // Print the configuration.
  std::ostream& print(std::ostream& out =std::cout, std::string prefix ="  ") const;

private:

  // Pointer to the class that does the work.
  std::shared_ptr<CompressReplace> m_pact;

};

DECLARE_ART_SERVICE(CompressReplaceService, LEGACY)

#endif
