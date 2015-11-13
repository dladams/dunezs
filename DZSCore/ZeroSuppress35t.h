// ZeroSuppress35t.h
//
// David Adams
// November 2015
//
// Class to apply the DUNE 35-ton ZS (zero suppression) algorithm to
// an array of ADC signal values.
//
// The algorithm is that described by James Russell at the September 2015
// DUNE collaboration meeting:
//   https://indico.fnal.gov/conferenceOtherViews.py?view=standard&confId=10100
// and is described with later modifications at
//   https://cdcvs.fnal.gov/redmine/projects/35ton/wiki/Data_compression_and_zero_suppression

#ifndef ZeroSuppress35t_H
#define ZeroSuppress35t_H

#include "DZSCore/ZeroSuppressBase.h"

class ZeroSuppress35t : public ZeroSuppressBase {

public:

  // Ctor from the five parameters that characterize the algorithm.
  ZeroSuppress35t(Signal ts, Signal tl, Signal td, Index ns, Index nl, Index nd, Index nt);

  // Apply the ZS filter to a signal vector.
  // Entries in the result vector indicate whether the corresponding
  // ADC entry should be retained.
  // Returns nonzero for error.
  int filter(const SignalVector& sigs, Channel chan, Signal ped, ResultVector& keep) const;

  // Display the signal parameters.
  std::ostream& print(std::ostream& out =std::cout, std::string prefix ="") const;

  void setDebug(int dbg);

private:

  Signal m_ts;
  Signal m_tl;
  Signal m_td;
  Index m_ns;
  Index m_nl;
  Index m_nd;
  Index m_nt;
  bool m_dbg;

};

#endif
