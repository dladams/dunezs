// ZeroSuppress35tLegacyService.cxx

#include "DZSService/ZeroSuppress35tLegacyService.h"
#include <cmath>
#include "fhiclcpp/ParameterSet.h"
#include "DZSCore/AdcCodeHelper.h"

using std::string;
using std::ostream;
using std::endl;

typedef ZeroSuppressBase::Index        Index;
typedef ZeroSuppressBase::Signal       Signal;
typedef AdcCodeHelper::FloatSignal     FloatSignal;
typedef ZeroSuppressBase::SignalVector SignalVector;
typedef ZeroSuppressBase::ResultVector ResultVector;

namespace {

string sbool(bool arg) {
  if ( arg ) return "true";
  return "false";
}

}  // end unnamed namespace.

//**********************************************************************

ZeroSuppress35tLegacyService::
ZeroSuppress35tLegacyService(const fhicl::ParameterSet& pset, art::ActivityRegistry&) {
  m_AdcThreshold       = pset.get<float>("AdcThreshold");
  m_TickRange          = pset.get<unsigned int>("TickRange");
  m_SuppressStickyBits = pset.get<bool>("SuppressStickyBits");
}
  
//**********************************************************************

ZeroSuppress35tLegacyService::
ZeroSuppress35tLegacyService(float aAdcThreshold, unsigned int aTickRange, bool aSuppressStickyBits)
: m_AdcThreshold(aAdcThreshold),
  m_TickRange(aTickRange),
  m_SuppressStickyBits(aSuppressStickyBits) { }

//**********************************************************************

int ZeroSuppress35tLegacyService::
filter(const SignalVector& sigs, Channel, FloatSignal& ped, ResultVector& keep) const {
  const unsigned int nsig = sigs.size();
  keep.clear();
  keep.resize(nsig, false);
  AdcCodeHelper ach(64);
  for ( unsigned int isig=0; isig<nsig; ++isig ) {
    Signal rawsig = sigs[isig];
    FloatSignal pedsig = ach.subtract(ped, rawsig);
    if ( m_SuppressStickyBits ) {
      if ( ach.hasStickyBits(rawsig) && ach.isSmall(pedsig) ) {
        pedsig = 0.0;
      }
    }
    if ( fabs(pedsig) > m_AdcThreshold ) {
      Index isig1 = 0;
      if ( m_TickRange < isig ) isig1 = isig - m_TickRange;
      Index isig2 = isig + m_TickRange;
      if ( isig2 > nsig ) isig2 = nsig;
      for ( Index jsig=isig1; jsig<=isig2; ++jsig ) keep[jsig] = true;
    }
  }
  return 0;
}

//**********************************************************************

ostream& ZeroSuppress35tLegacyService::print(ostream& out, string prefix) const {
  out << prefix << "ZeroSuppress35tLegacyService:"               << endl;
  out << prefix << "       AdcThreshold: " << m_AdcThreshold       << endl;
  out << prefix << "          TickRange: " << m_TickRange          << endl;
  out << prefix << " SuppressStickyBits: " << sbool(m_SuppressStickyBits) << endl;
  return out;
}

//**********************************************************************

DEFINE_ART_SERVICE_INTERFACE_IMPL(ZeroSuppress35tLegacyService, ZeroSuppressBase)

//**********************************************************************
