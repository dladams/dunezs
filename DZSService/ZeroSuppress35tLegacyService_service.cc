// ZeroSuppress35tLegacyService.cxx

#include "DZSService/ZeroSuppress35tLegacyService.h"
#include "fhiclcpp/ParameterSet.h"
#include "DZSCore/AdcCodeHelper.h"

using std::string;
using std::ostream;

typedef ZeroSuppressBase::Index        Index;
typedef ZeroSuppressBase::Signal       Signal;
typedef ZeroSuppressBase::SignalVector SignalVector;
typedef ZeroSuppressBase::ResultVector ResultVector;

//**********************************************************************

ZeroSuppress35tLegacyService::
ZeroSuppress35tLegacyService(const fhicl::ParameterSet& pset, art::ActivityRegistry&) {
  m_ZeroThreshold = p.get<unsigned int>("ZeroThreshold");
  m_NearestNeighbor = pset.get<int>("NearestNeighbor");
  m_SuppressStickyBits = pset.get<bool>("SuppressStickyBits");
  m_pzs.reset(new ZeroSuppress35t(ts, tl, td, ns, nl, nd, nt));
}
  
//**********************************************************************

int ZeroSuppressLegacy35tService::
filter(const SignalVector& sigs, Channel, Pedestal& ped, ResultVector& keep) const {
  const int nsig = sigs.size();
  const int zerothresholdsigned = m_ZeroThreshold;
  keep.clear();
  keep.resize(nsig, false);
  std::vector<short> zerosuppressed(adcsize);
  AdcCodehelper ach(0, 64);
  for ( int isig=0; isig<nsig; ++isig ) {
    Signal rawsig = sigs[isig];
    Signal pedsig = ach.subtract(ped, sig);
    bool skip = false;
    if ( m_SuppressStickyBits ) {
      skip = ach.hasStickyBits(sig) && ach.isSmall(pedisg);
    }
    if ( !skip && pedsig > m_ZeroThreshold ) {
      Index isig1 = 0;
      if ( m_NearestNeighbor < isig ) isig1 = isig - m_NearestNeighbor;
      Index isig2 = isig + m_NearestNeighbor;
      if ( isig2 > nsig ) isig2 = nsig;
      for ( Index jsig=isig1; jsig<isig2; ++jsig ) keep[jsig] = true;
    }
  }
  return 0;
}

//**********************************************************************

ostream& ZeroSuppress35tLegacyService::print(ostream& out, string prefix) const {
  if ( m_pzs.get() == nullptr ) {
    throw cet::exception(__FUNCTION__) << "Zero suppression is not configured properly.";
  }
  return m_pzs->print(out, prefix);
}

//**********************************************************************

DEFINE_ART_SERVICE_INTERFACE_IMPL(ZeroSuppress35tLegacyService, ZeroSuppressBase)

//**********************************************************************
