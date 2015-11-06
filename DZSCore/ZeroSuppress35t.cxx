// ZeroSuppress35t.cxx

#include "ZeroSuppress35t.h"
#include <vector>
#include <cassert>

using std::string;
using std::ostream;
using std::endl;
using std::vector;

typedef ZeroSuppressBase::Signal       Signal;
typedef ZeroSuppressBase::SignalVector SignalVector;
typedef ZeroSuppressBase::ResultVector ResultVector;

// Signal state.
//    OUT - outside the signal (may be in the tail of an earlier signal)
//   HIGH - Live region of the signal (above TL or after)
//   DEAD - Dead region of the signal (below TD)
//    END - End of dead region (followed by tail)
enum SigState { OUT, HIGH, LOW, END };

//**********************************************************************

ZeroSuppress35t::ZeroSuppress35t(Signal tl, Signal td, Index nl, Index nd, Index nt)
: m_tl(tl), m_td(td), m_nl(nl), m_nd(nd), m_nt(nt) { }

//**********************************************************************

int ZeroSuppress35t::filter(const SignalVector& sigs, ResultVector& keep) const {
  unsigned int nsig = sigs.size();
  keep.clear();
  keep.resize(nsig, false);
  // Loop over signals.
  SigState state = OUT;
  unsigned int nlow = 0;
  for ( unsigned int isig=0; isig<nsig; ++isig ) {
    Signal sig = sigs[isig];
    // Last tick is outside a signal.
    if ( state == OUT || state == END ) {
      // If this tick is above TL, we are in the live region of a signal.
      // Keep the NL preceding signals.
      if ( sig > m_tl ) {
        state = HIGH;
        unsigned int jsig1 = 0;
        if ( isig > m_nl ) jsig1 = isig - m_nl;
        unsigned int jsig2 = isig;
        for ( unsigned int jsig=jsig1; jsig<jsig2; ++jsig) {
          keep[jsig] = true;
        }
      } else {
        state = OUT;
      }
    // Last tick is is in the live region of a signal.
    } else if ( state == HIGH ) {
      // If this tick is below TD, we are in the dead region of a signal.
      if ( sig <= m_td ) {
        state = LOW;
        nlow = 1;
      }
    // Last tick is is in the dead region of a signal.
    } else if ( state == LOW ) {
      // If signal is above TD, we are back in the live region.
      if ( sig > m_td ) {
        state = HIGH;
        nlow = 0;
      // If this is the ND'th consecutive signal in the dead region, we
      // have reached the end of the signal.
      // Keep this signal and a tail.
      } else if ( ++nlow >= m_nd ) {
        state = END;
        nlow = 0;
        // Protect the tail.
        unsigned int jsig1 = isig + 1;
        unsigned int jsig2 = jsig1 + m_nt;
        if ( jsig2 > nsig ) jsig2 = nsig;
        for ( unsigned int jsig=jsig1; jsig<jsig2; ++jsig) {
          keep[jsig] = true;
        }
      }
    } else {
      assert(false);
    }
    if ( state != OUT ) keep[isig] = true;
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
