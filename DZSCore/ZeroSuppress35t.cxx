// ZeroSuppress35t.cxx

#include "ZeroSuppress35t.h"

typedef ZeroSuppress35t::Signal Signal;
typedef ZeroSuppress35t::SignalArray SignalArray;

//**********************************************************************

Signal ZeroSuppress35t::zero() {
  return 0.0;
}

//**********************************************************************

ZeroSuppress35t::ZeroSuppress35t(Signal tl, Signal td, Index nl, Index nd, Index nt)
: m_tl(tl), m_td(td), m_nl(nl), m_nd(nd), m_nt(nt) { }

//**********************************************************************

int ZeroSuppress35t::filter(SignalArray& sigs) const {
  return 0;
}

//**********************************************************************

std::ostream& ZeroSuppress35t::print(std::ostream& out, std::string prefix) const {
  out << prefix << "TL = " << m_tl << endl;
  out << prefix << "TD = " << m_td << endl;
  out << prefix << "NL = " << m_nl << endl;
  out << prefix << "ND = " << m_nd << endl;
  out << prefix << "NT = " << m_nt << endl;
  return out;
}

//**********************************************************************
