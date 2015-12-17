// CompressReplaceService.h
//
// David Adams
// November 2015
//
// Utility/service to compress an ADC vector by replacing filtered out values
// with a configurable zero value plus a passed offset (pedestal).
//
// FCL parameters
//   Zero [0] - replacement value

#ifndef CompressReplaceService_H
#define CompressReplaceService_H

#include "DZSInterface/AdcCompressService.h"

namespace fhicl {
class ParameterSet;
}
namespace art {
class ActivityRegistry;
}

class CompressReplaceService : public AdcCompressService {

public:

  // Ctor from parameters that characterize the algorithm.
  CompressReplaceService(AdcCount azero =0);

  // Ctor from fcl.
  CompressReplaceService(fhicl::ParameterSet const& pset, art::ActivityRegistry&);

  // Compress a vector of signals.
  // Suppressed signals are replaced with the value of offset + (FCL parameter) Zero.
  int compress(AdcCountVector& sigs,
               const AdcFilterVector& keep,
               AdcCount offset,
               raw::Compress_t& comp) const;

  // Return the value assigned to suppressed channels.
  AdcCount zero() const;

  // Print the configuration.
  std::ostream& print(std::ostream& out =std::cout, std::string prefix ="  ") const;

private:

  AdcCount m_zero;

};

DECLARE_ART_SERVICE_INTERFACE_IMPL(CompressReplaceService, AdcCompressService, LEGACY)

#endif
