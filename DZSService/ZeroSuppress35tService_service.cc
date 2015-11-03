// ZeroSuppress35tService.cxx

#include "DZSService/ZeroSuppress35tService.h"
#include "fhiclcpp/ParameterSet.h"

using std::string;
using std::ostream;

typedef ZeroSuppress35t::Signal Signal;
typedef ZeroSuppress35t::SignalArray SignalArray;
typedef ZeroSuppress35t::Index Index;

//**********************************************************************

ZeroSuppress35tService::
ZeroSuppress35tService(const fhicl::ParameterSet& pset, art::ActivityRegistry&) {
  Signal tl = 1;
  Signal td = 1;
  Index nl = 1;
  Index nd = 1;
  Index nt = 1;
  Signal zero = 1;
  pset.get_if_present<Signal>("Zero", zero);
  tl = pset.get<Signal>("TL");
  td = pset.get<Signal>("TD");
  nl = pset.get<Index>("NL");
  nd = pset.get<Index>("ND");
  nt = pset.get<Index>("NT");
  m_pzs.reset(new ZeroSuppress35t(tl, td, nl, nd, nt, zero));
}
  
//**********************************************************************

int ZeroSuppress35tService::filter(SignalArray& sigs) const {
  if ( m_pzs.get() == nullptr ) {
    throw cet::exception(__FUNCTION__) << "Zero suppression is not configured properly.";
  }
  return m_pzs->filter(sigs);
}

//**********************************************************************

ostream& ZeroSuppress35tService::print(ostream& out, string prefix) const {
  if ( m_pzs.get() == nullptr ) {
    throw cet::exception(__FUNCTION__) << "Zero suppression is not configured properly.";
  }
  return m_pzs->print(out, prefix);
}

//**********************************************************************

DEFINE_ART_SERVICE(ZeroSuppress35tService)

//**********************************************************************
