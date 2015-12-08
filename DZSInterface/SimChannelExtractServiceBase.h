// SimChannelExtractServiceBase.h

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

#ifndef SimChannelExtractServiceBase_H
#define SimChannelExtractServiceBase_H

#include <vector>
#include "DZSCore/AdcTypes.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

namespace sim {
class SimChannel;
}

class SimChannelExtractServiceBase {

public:

  virtual ~SimChannelExtractServiceBase();

  virtual int extract(const sim::SimChannel& sc, AdcSignalVector& sig, AdcSignalVector& xsig) const =0;

};

DECLARE_ART_SERVICE_INTERFACE(SimChannelExtractServiceBase, LEGACY)

#endif

