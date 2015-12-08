// ZeroSuppress35tService.cxx

#include "DZSService/ZeroSuppress35tService.h"
#include "fhiclcpp/ParameterSet.h"

using std::string;
using std::ostream;

typedef ZeroSuppressBase::Index        Index;
typedef ZeroSuppressBase::ResultVector ResultVector;

//**********************************************************************

ZeroSuppress35tService::
ZeroSuppress35tService(const fhicl::ParameterSet& pset, art::ActivityRegistry&) {
  AdcCount ts = pset.get<AdcCount>("TS");
  AdcCount tl = pset.get<AdcCount>("TL");
  AdcCount td = pset.get<AdcCount>("TD");
  Index ns = pset.get<Index>("NS");
  Index nl = pset.get<Index>("NL");
  Index nd = pset.get<Index>("ND");
  Index nt = pset.get<Index>("NT");
  m_pzs.reset(new ZeroSuppress35t(ts, tl, td, ns, nl, nd, nt));
}
  
//**********************************************************************

int ZeroSuppress35tService::
filter(const AdcCountVector& sigs, Channel chan, AdcPedestal& ped, ResultVector& keep) const {
  if ( m_pzs.get() == nullptr ) {
    throw cet::exception(__FUNCTION__) << "Zero suppression is not configured properly.";
  }
  return m_pzs->filter(sigs, chan, ped, keep);
}

//**********************************************************************

ostream& ZeroSuppress35tService::print(ostream& out, string prefix) const {
  if ( m_pzs.get() == nullptr ) {
    throw cet::exception(__FUNCTION__) << "Zero suppression is not configured properly.";
  }
  return m_pzs->print(out, prefix);
}

//**********************************************************************

DEFINE_ART_SERVICE_INTERFACE_IMPL(ZeroSuppress35tService, ZeroSuppressBase)

//**********************************************************************
