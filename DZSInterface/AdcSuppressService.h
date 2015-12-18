// AdcSuppressService.h
//
// David Adams
// December 2015
//
// Service interface for identifying zero-suppressed ticks in an ADC vector.

#ifndef AdcSuppressService_H
#define AdcSuppressService_H

#include <string>
#include <iostream>
#include <memory>
#include <string>
#include <iostream>
#include "DZSInterface/AdcTypes.h"
#include "SimpleTypesAndConstants/RawTypes.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

namespace fhicl {
class ParameterSet;
}
namespace art {
class ActivityRegistry;
}

class AdcSuppressService {

public:

  typedef unsigned int Channel;

  // Dtor.
  virtual ~AdcSuppressService() =default;

  // Suppress a vector of signals.
  //     sigs: Input ADC vector.
  //      ped: Input pedestal.
  //     keep: I/O boolean vector indicating which signals are retained.
  virtual int filter(const AdcCountVector& sigs,
                     Channel chan,
                     AdcPedestal& ped,
                     AdcFilterVector& keep) const =0;

  // Print the configuration.
  virtual std::ostream& print(std::ostream& out =std::cout, std::string prefix ="  ") const =0;

};

DECLARE_ART_SERVICE_INTERFACE(AdcSuppressService, LEGACY)

#endif
