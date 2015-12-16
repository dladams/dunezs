// SimChannelExtractAllService.cxx

#include "SimChannelExtractAllService.h"
#include <string>
#include "Geometry/Geometry.h"
#include "Simulation/SimChannel.h"

using std::string;

//**********************************************************************

SimChannelExtractAllService::
SimChannelExtractAllService(const fhicl::ParameterSet&, art::ActivityRegistry&)
: m_ntick(m_pfft->FFTSize()) {
  if ( m_ntick%2 != 0 )
    throw cet::exception("SimChannelExtractAllService")
          << "FFT size is not a power of 2.";
}

//**********************************************************************

int SimChannelExtractAllService::
extract(const sim::SimChannel* psc, AdcSignalVector& sigs) const {
  sigs.clear();
  sigs.resize(m_ntick, 0.0);
  if ( psc == nullptr ) return 0;
  for ( size_t itck=0; itck<sigs.size(); ++itck ) {
    sigs[itck] = psc->Charge(itck);
  }
  unsigned int chan = psc->Channel();
  m_psss->Convolute(chan, sigs);
  return 0;
}

//**********************************************************************

DEFINE_ART_SERVICE_INTERFACE_IMPL(SimChannelExtractAllService, SimChannelExtractServiceBase)

//**********************************************************************
