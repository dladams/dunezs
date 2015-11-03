// ZeroSuppress35tService.cxx

#include "DZSService/ZeroSuppress35tService.h"
#include "fhiclcpp/ParameterSet.h"

typedef ZeroSuppress35t::Signal Signal;
typedef ZeroSuppress35t::SignalArray SignalArray;
typedef ZeroSuppress35t::Index Index;

//**********************************************************************

ZeroSuppress35tService::
ZeroSuppress35tService(fhicl::ParameterSet const& pset, art::ActivityRegistry&) {
  Signal tl = 0;
  Signal td = 0;
  Index nl = 0;
  Index nd = 0;
  Index nt = 0;
  Signal zero = 0;
  pset.get_if_present<Signal>("Zero", zero);
  pset.get<Signal>("TL", tl);
  pset.get<Signal>("TD", td);
  pset.get<Index>("NL", nl);
  pset.get<Index>("ND", nd);
  pset.get<Index>("NT", nt);
  pset.get<Signal>("Zero", zero);
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

DEFINE_ART_SERVICE(ZeroSuppress35tService)

//**********************************************************************
