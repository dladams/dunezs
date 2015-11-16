// ZeroSuppress35tService.cxx

#include "DZSService/ZeroSuppress35tService.h"
#include "fhiclcpp/ParameterSet.h"

using std::string;
using std::ostream;

typedef ZeroSuppressBase::Index        Index;
typedef ZeroSuppressBase::Signal       Signal;
typedef ZeroSuppressBase::SignalVector SignalVector;
typedef ZeroSuppressBase::ResultVector ResultVector;

//**********************************************************************

ZeroSuppress35tService::
ZeroSuppress35tService(const fhicl::ParameterSet& pset, art::ActivityRegistry&) {
  Signal ts = pset.get<Signal>("TS");
  Signal tl = pset.get<Signal>("TL");
  Signal td = pset.get<Signal>("TD");
  Index ns = pset.get<Index>("NS");
  Index nl = pset.get<Index>("NL");
  Index nd = pset.get<Index>("ND");
  Index nt = pset.get<Index>("NT");
  m_pzs.reset(new ZeroSuppress35t(ts, tl, td, ns, nl, nd, nt));
}
  
//**********************************************************************

int ZeroSuppress35tService::
filter(const SignalVector& sigs, Channel chan, Pedestal& ped, ResultVector& keep) const {
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
