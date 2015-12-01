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
extract(const sim::SimChannel& sc, Charge& fChargeWork, Charge&) const {
  for ( size_t t=0; t<fChargeWork.size(); ++t ) {
    fChargeWork[t] = sc.Charge(t);
  }
  return 0;
}

//**********************************************************************

DEFINE_ART_SERVICE_INTERFACE_IMPL(SimChannelExtractAllService, SimChannelExtractServiceBase)

//**********************************************************************
