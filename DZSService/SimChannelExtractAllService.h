// SimChannelExtractAllService.h

// David Adams
// December 2015
//
// Interface for a service that extracts charge from
// a SimChannel object and assigns it to ticks.
//
// The charge is distributed over two arrays: sig and xsig.
// The first is for normal collection/induction. The second
// is for charge that is collected on the wire even if it is
// an induction plane.

#ifndef SimChannelExtractAllService_H
#define SimChannelExtractAllService_H

#include <vector>
#include "DZSInterface/SimChannelExtractServiceBase.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "Utilities/LArFFT.h"
#include "dune/Utilities/SignalShapingServiceDUNE35t.h"

namespace sim {
class SimChannel;
}

class SimChannelExtractAllService : public SimChannelExtractServiceBase {

public:

  SimChannelExtractAllService(fhicl::ParameterSet const& pset, art::ActivityRegistry&);

  int extract(const sim::SimChannel* psc, AdcSignalVector& sig) const;

private:

  art::ServiceHandle<util::LArFFT> m_pfft;
  art::ServiceHandle<util::SignalShapingServiceDUNE35t> m_psss;
  unsigned int m_ntick;

};

DECLARE_ART_SERVICE_INTERFACE_IMPL(SimChannelExtractAllService, SimChannelExtractServiceBase, LEGACY)

#endif

