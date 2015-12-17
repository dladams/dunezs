// StuckBitsService.h
//
// David Adams
// December 2015
//
// Service that adds stuck bits to ADCs.
//
// FCL parameters

#ifndef StuckBitsService_H
#define StuckBitsService_H

#include "art/Framework/Services/Registry/ServiceMacros.h"
#include <string>
#include <iostream>
#include "DZSInterface/AdcTypes.h"

namespace fhicl {
class ParameterSet;
}
namespace art {
class ActivityRegistry;
}
namespace CLHEP {
class HepRandomEngine;
}

class StuckBitsService {

public:

  typedef unsigned int Channel;

  // Ctor.
  StuckBitsService(fhicl::ParameterSet const& pset, art::ActivityRegistry&);

  // Modify an input ADC vector.
  int modify(Channel chan, AdcCountVector& adcvec) const;

  // Print the configuration.
  std::ostream& print(std::ostream& out =std::cout, std::string prefix ="  ") const;

private:

  std::string fStuckBitsProbabilitiesFname;     ///< file holding ADC stuck code probabilities 
  std::string fStuckBitsOverflowProbHistoName;  ///< Name of hist with ADC stuck code overflow probs 
  std::string fStuckBitsUnderflowProbHistoName; ///< Name of hist with ADC stuck code underflow probs 
  double      fOverflowProbs[64];               ///< array of probs for LSF bits getting stuck at 000000
  double      fUnderflowProbs[64];              ///< array of probs for LSF bits getting stuck at 111111

  CLHEP::HepRandomEngine* m_pran;

};

DECLARE_ART_SERVICE(StuckBitsService, LEGACY)

#endif
