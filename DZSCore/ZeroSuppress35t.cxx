// ZeroSuppress35t.cxx

#include "ZeroSuppress35t.h"
#include <vector>
#include <cassert>

using std::string;
using std::ostream;
using std::endl;
using std::vector;

typedef ZeroSuppress35t::Signal Signal;
typedef ZeroSuppress35t::SignalArray SignalArray;

enum SigState { OUT, HIGH, LOW };

//**********************************************************************

Signal ZeroSuppress35t::zero() {
  return 0.0;
}

//**********************************************************************

ZeroSuppress35t::ZeroSuppress35t(Signal tl, Signal td, Index nl, Index nd, Index nt)
: m_tl(tl), m_td(td), m_nl(nl), m_nd(nd), m_nt(nt) { }

//**********************************************************************

int ZeroSuppress35t::filter(SignalArray& sigs) const {
  vector<bool> inTail(sigs.size(), false);
  // Loop over signals.
  SigState state = OUT;
  unsigned int nout = 0;
  unsigned int nlow = 0;
  for ( unsigned int isig=0; isig<sigs.size(); ++isig ) {
    Signal sig = sigs[isig];
    if ( state == OUT ) {
      if ( sig > m_tl ) {
        state = HIGH;
        nout = 0;
      } else {
        // If we are outside the signal for more than m_nl ticks, and we are
        // not in the tail of an earlier signal, zero tick isig-m_nl.
        if ( ++nout > m_nl && isig >= m_nl ) {
          unsigned int isigZero = isig - m_nl;
          if ( ! inTail[isigZero] ) sigs[isigZero] = zero();
        }
      }
    } else if ( state == HIGH ) {
      if ( sig <= m_td ) {
        state = LOW;
        nlow = 1;
      }
    } else if ( state == LOW ) {
      if ( sig > m_td ) {
        state = HIGH;
        nlow = 0;
      } else {
        // Reach the end of the low (dead) region.
        if ( ++nlow >= m_nl ) {
          state = OUT;
          nlow = 0;
          // Protect the tail.
          for ( unsigned int itail=1; itail<=m_nt; ++itail ) {
            inTail[isig+itail] = true;
          }
        }
      }
    } else {
      assert(false);
    }
  }
  // If we end up outside a signal, then the last m_nl ticks are zeroed.
  if ( state == OUT ) {
    unsigned int isig1 = 0;
    if ( sigs.size() > m_nl ) isig1 = sigs.size() - m_nl;
    for ( unsigned int isig=isig1; isig<sigs.size(); ++isig ) {
      if ( ! inTail[isig] ) sigs[isig] = zero();
    }
  }
  return 0;
}

//**********************************************************************

ostream& ZeroSuppress35t::print(ostream& out, string prefix) const {
  out << prefix << "TL = " << m_tl << endl;
  out << prefix << "TD = " << m_td << endl;
  out << prefix << "NL = " << m_nl << endl;
  out << prefix << "ND = " << m_nd << endl;
  out << prefix << "NT = " << m_nt << endl;
  return out;
}

//**********************************************************************
