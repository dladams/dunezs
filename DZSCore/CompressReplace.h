// CompressReplace.h
//
// David Adams
// November 2015
//
// Class to compress a signal array bey replacing filtered out values
// with a configurable zero value.

#include <string>
#include <iostream>
#include <vector>

class CompressReplace {

public:

  typedef unsigned int Index;
  typedef short Signal;
  typedef std::vector<Signal> SignalVector;
  typedef std::vector<bool> FilterVector;

  // Ctor from the five parameters that characterize the algorithm.
  CompressReplace(Signal azero =0);

  // Return the value assigned to suppressed channels.
  Signal zero() const;

  // Apply a filter to a signal vector.
  // Filtered-out signals are replaced with the value zero().
  // Returns nonzero for error.
  int compress(SignalVector& sigs, const FilterVector& keep) const;

  // Display the signal parameters.
  std::ostream& print(std::ostream& out =std::cout, std::string prefix ="  ") const;

private:

  Signal m_zero;

};
