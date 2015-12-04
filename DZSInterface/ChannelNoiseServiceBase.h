// ChannelNoiseServiceBase.h

// David Adams
// December 2015
//
// Interface for a service that adds noise to a TPC signal vector.

#ifndef ChannelNoiseServiceBase_H
#define ChannelNoiseServiceBase_H

#include <vector>
#include "art/Framework/Services/Registry/ServiceMacros.h"

class ChannelNoiseServiceBase {

public:

  typedef unsigned int Channel;
  typedef std::vector<float> SignalVector;

  virtual ~ChannelNoiseServiceBase();

  virtual int addNoise(Channel chan, SignalVector& sigs) const =0;

};

DECLARE_ART_SERVICE_INTERFACE(ChannelNoiseServiceBase, LEGACY)

#endif

