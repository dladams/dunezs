// CompressReplace.h
//
// David Adams
// November 2015
//
// Class to compress a signal array by replacing filtered out values
// with a configurable zero value plus a passed offset.

#ifndef CompressReplace_H
#define CompressReplace_H

#include <string>
#include <iostream>
#include "AdcTypes.h"
#include "SimpleTypesAndConstants/RawTypes.h"

class CompressReplace {

public:

  typedef unsigned int Index;
  typedef std::vector<bool> FilterVector;

  // Ctor from the five parameters that characterize the algorithm.
  CompressReplace(AdcCount azero =0);

  // Return the value assigned to suppressed channels.
  AdcCount zero() const;

  // Apply a filter to a signal vector.
  // Filtered-out signals are replaced with the value offset + zero().
  // Returns nonzero for error.
  int compress(AdcCountVector& sigs, const FilterVector& keep, AdcCount offset,
               raw::Compress_t& comp) const;

  // Display the signal parameters.
  std::ostream& print(std::ostream& out =std::cout, std::string prefix ="  ") const;

private:

  AdcCount m_zero;

};

#endif
