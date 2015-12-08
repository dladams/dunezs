// ChannelNoiseServiceBase.h

// David Adams
// December 2015
//
// Interface for a service that adds noise to a TPC signal vector.

#ifndef ChannelNoiseServiceBase_H
#define ChannelNoiseServiceBase_H

#include <vector>
#include "DZSCore/AdcTypes.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

class ChannelNoiseServiceBase {

public:

  typedef unsigned int Channel;

  virtual ~ChannelNoiseServiceBase();

  // Add noise to a signal vector sigs appropriate for channel chan.
  // Noise is added for all entries in the input vector.
  virtual int addNoise(Channel chan, AdcSignalVector& sigs) const =0;

};

DECLARE_ART_SERVICE_INTERFACE(ChannelNoiseServiceBase, LEGACY)

#endif

