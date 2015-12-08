// SimChannelExtractAllService.cxx

#include "SimChannelExtractAllService.h"
#include <string>
#include "Geometry/Geometry.h"
#include "Simulation/SimChannel.h"

using std::string;

//**********************************************************************

SimChannelExtractAllService::
SimChannelExtractAllService(const fhicl::ParameterSet&, art::ActivityRegistry&) {
}

//**********************************************************************

int SimChannelExtractAllService::
extract(const sim::SimChannel& sc, AdcSignalVector& sig, AdcSignalVector&) const {
  for ( size_t itck=0; itck<sig.size(); ++itck ) {
    sig[itck] = sc.Charge(itck);
  }
  return 0;
}

//**********************************************************************

DEFINE_ART_SERVICE_INTERFACE_IMPL(SimChannelExtractAllService, SimChannelExtractServiceBase)

//**********************************************************************
