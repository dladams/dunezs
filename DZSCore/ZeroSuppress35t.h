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

#include <iostream>

class ZeroSuppress35t {

  typedef unsigned int Index;
  typedef double Signal;
  typedef std::vector<Signal> SignalArray;

  // Return the value assigned to suppressed channels.
  static Signal zero();

  // Ctor from the five parameters that characterize the algorithm.
  ZeroSuppress35t(Signal tl, Signal td, Index nl, Index nd, Index nt);

  // Apply the ZS filter to an array of signals.
  // Filtered-out signals are replaced with the value zero().
  // Returns nonzero for error.
  int filter(SignalArray& sigs) const;

  // Display the signal parameters.
  std::ostream& print(std::ostream& out =std::cout, std::string prefix ="  ") const;

private:

  Signal m_tl;
  Signal m_td;
  Index m_nl;
  Index m_nd;
  Index m_nt;

};
