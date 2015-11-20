// ZeroSuppress35t.cxx

#include "DZSCore/ZeroSuppress35t.h"
#include <vector>
#include <cassert>
#include <iostream>
#include <iomanip>
#include "DZSCore/SignalTypeConverter.h"

using std::string;
using std::ostream;
using std::cout;
using std::endl;
using std::vector;
using std::setw;

typedef ZeroSuppressBase::Index        Index;
typedef ZeroSuppressBase::Signal       Signal;
typedef ZeroSuppressBase::Pedestal     Pedestal;
typedef ZeroSuppressBase::SignalVector SignalVector;
typedef ZeroSuppressBase::ResultVector ResultVector;

// Signal state.
//    OUT - outside the signal (may be in the tail of an earlier signal)
//   LIVE - Live region of the signal (above TL or after)
//   DEAD - Dead region of the signal (below TD)
//    END - End of dead region (followed by tail)
enum SigState { OUT, LIVE, DEAD, END };

namespace {

// Calculate sum for nsig bins before isig.
// If skipStuck, the stuck bins do not contribute to sum or count.
// Bins with sig <= thresh, do not contribute to sum.
struct RunningSum {
  RunningSum(const SignalVector& sigs, Pedestal ped, Index isig, Index ns, Signal thresh, bool skipStuck);
  Signal sigsum;
  Index count;
  SignalTypeConverter sigcon;
};

RunningSum::
RunningSum(const SignalVector& sigs, Pedestal ped, Index isig, Index nsig, Signal thresh, bool skipStuck) {
  sigsum = 0;
  count = 0;
  Index jsig1 = 0;
  if ( isig > nsig ) jsig1 = isig - nsig;
  Index jsig2 = isig;
  if ( jsig2 > sigs.size() ) jsig2 = sigs.size();
  for ( Index jsig=jsig1; jsig<jsig2; ++jsig ) {
    Signal rawsig = sigs[jsig];
    Signal sig = sigcon.convert<Signal>(rawsig - ped);
    if ( skipStuck ) {
      Index lsb = rawsig & 0x3f;
      if ( lsb == 0 ) continue;
      if ( lsb == 0x3f ) continue;
    }
    ++count;
    if ( abs(sig) > thresh ) {
      sigsum +=  sig;
    }
  }
}

// Convert sate to string.
string sstate(SigState state) {
  if ( state == OUT ) return "OUT";
  if ( state == LIVE ) return "LIVE";
  if ( state == DEAD ) return "DEAD";
  if ( state == END ) return "END";
  return "NONE";
}

}  // End unnamed namespace.

//**********************************************************************

ZeroSuppress35t::ZeroSuppress35t(Signal ts, Signal tl, Signal td,
                                 Index ns, Index nl, Index nd, Index nt)
: m_ts(ts), m_tl(tl), m_td(td), m_ns(ns), m_nl(nl), m_nd(nd), m_nt(nt), m_dbg(false) { }

//**********************************************************************

int ZeroSuppress35t::
filter(const SignalVector& sigs, Channel chan, Pedestal& ped, ResultVector& keep) const {
  const string myname = "ZeroSuppress35t::filter: ";
  if ( m_dbg ) cout << "Filtering signal array of size " << sigs.size() << endl;
  bool m_skipStuck = false;
  Signal m_ts = 0;
  unsigned int nsig = sigs.size();
  keep.clear();
  keep.resize(nsig, false);
  // Loop over signals.
  SigState state = OUT;
  unsigned int nlow = 0;
  for ( unsigned int isig=0; isig<nsig; ++isig ) {
    Signal sig = sigs[isig];
    // Evaluate a running signal sum of the preceding m_nl signals.
    RunningSum rs(sigs, ped, isig, m_ns, m_ts, m_skipStuck);
    if ( m_dbg ) cout << setw(3) << isig << setw(6) << sig << setw(5) << sstate(state);
    // Last tick is outside a signal.
    if ( state == OUT || state == END ) {
      // If this tick is above TL, we are in the live region of a signal.
      // Keep the NL preceding signals.
      Signal sumthresh = m_tl*rs.count;
      if ( m_dbg ) cout << " RS sum/thresh=" << setw(3) << rs.sigsum << "/" << setw(3) << sumthresh;
      if ( rs.sigsum > sumthresh ) {
        state = LIVE;
        unsigned int jsig1 = 0;
        if ( isig > m_nl ) jsig1 = isig - m_nl;
        unsigned int jsig2 = isig;
        for ( unsigned int jsig=jsig1; jsig<jsig2; ++jsig) {
          keep[jsig] = true;
        }
      } else {
        state = OUT;
      }
    } else {
      // Last tick is is in the live region of a signal.
      Signal sumthresh = m_td*rs.count;
      if ( m_dbg ) cout << " RS sum/thresh=" << setw(3) << rs.sigsum << "/" << setw(3) << sumthresh;
        if ( state == LIVE ) {
        // If this tick is below TD, we are in the dead region of a signal.
        if ( rs.sigsum <= sumthresh ) {
          state = DEAD;
          nlow = 1;
        }
      // Last tick is is in the dead region of a signal.
      } else if ( state == DEAD ) {
        // If signal is above TD, we are back in the live region.
        if ( rs.sigsum > sumthresh ) {
          state = LIVE;
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
    }
    cout << " --> " << sstate(state) << endl;
    if ( state != OUT ) keep[isig] = true;
  }
  return 0;
}

//**********************************************************************

ostream& ZeroSuppress35t::print(ostream& out, string prefix) const {
  out << prefix << "TS = " << m_ts << endl;
  out << prefix << "TL = " << m_tl << endl;
  out << prefix << "TD = " << m_td << endl;
  out << prefix << "NS = " << m_ns << endl;
  out << prefix << "NL = " << m_nl << endl;
  out << prefix << "ND = " << m_nd << endl;
  out << prefix << "NT = " << m_nt << endl;
  out << prefix << "Debug level: " << m_dbg << endl;
  return out;
}

//**********************************************************************

void ZeroSuppress35t::setDebug(int dbg) {
  m_dbg = dbg;
}

//**********************************************************************
