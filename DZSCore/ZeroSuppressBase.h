// ZeroSuppressBase.h
//
// David Adams
// November 2015
//
// Interface for DUNE single-channel zero suppresssion algorithms.
// It is expected that both utilities and services that provide ZS will
// inherit this interface.

#ifndef ZeroSuppressBase_H
#define ZeroSuppressBase_H

#include <string>
#include <iostream>
#include <vector>

class ZeroSuppressBase {

public:

  typedef unsigned int Index;
  typedef unsigned int Channel;
  typedef short Signal;
  typedef std::vector<Signal> SignalVector;
  typedef std::vector<bool> ResultVector;
  typedef float Pedestal;

  // Dtor.
  virtual ~ZeroSuppressBase();

  // Apply the ZS filter to a signal vector.
  // Entries in the result vector indicate whether the correponding
  // ADC entry should be retained.
  //   sigs - ADC signals
  //   chan - Channel number
  //    ped - Pedestal for the channel
  //   keep - Reult vector
  // Returns nonzero for error.
  virtual int filter(const SignalVector& sigs, Channel chan, Signal ped, ResultVector& keep) const =0;

  // Print a description of the algorithm, e.g. name and parameter names and values.
  //     out - the stream where the descrption is written
  //  prefix - string prefixing each line
  virtual std::ostream& print(std::ostream& out =std::cout, std::string prefix ="") const =0;

};

#endif
