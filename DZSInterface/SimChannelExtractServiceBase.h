// SimChannelExtractServiceBase.h

// David Adams
// December 2015
//
// Interface for a service that extracts charge for one channel from
// a SimChannel object and assigns it to ticks.

#ifndef SimChannelExtractServiceBase_H
#define SimChannelExtractServiceBase_H

#include <vector>
#include "DZSInterface/AdcTypes.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

namespace sim {
class SimChannel;
}

class SimChannelExtractServiceBase {

public:

  virtual ~SimChannelExtractServiceBase() =default;

  virtual int extract(const sim::SimChannel* psc, AdcSignalVector& sig) const =0;

};

DECLARE_ART_SERVICE_INTERFACE(SimChannelExtractServiceBase, LEGACY)

#endif

