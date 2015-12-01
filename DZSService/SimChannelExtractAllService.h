// SimChannelExtractAllService.h

// David Adams
// December 2015
//
// Interface for a service that extracts charge from
// a SimChannel object and assigns it to ticks.
//
// The charge is distributed over two arrays: chg and xchg.
// The first is for normal collection/induction. The second
// is for charge that is collected on the wire even if it is
// an induction plane.

#ifndef SimChannelExtractAllService_H
#define SimChannelExtractAllService_H

#include <vector>
#include "DZSInterface/SimChannelExtractServiceBase.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

namespace sim {
class SimChannel;
}

class SimChannelExtractAllService : public SimChannelExtractServiceBase {

public:

  SimChannelExtractAllService(fhicl::ParameterSet const& pset, art::ActivityRegistry&);

  int extract(const sim::SimChannel& sc, Charge& chg, Charge& xchg) const;

};

DECLARE_ART_SERVICE_INTERFACE_IMPL(SimChannelExtractAllService, SimChannelExtractServiceBase, LEGACY)

#endif

