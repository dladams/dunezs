// AdcCodeHelper.h
//
// David Adams
// November 2015
//
// Utility class to help interprete ADC signals.
// Parameters:
//   SuppressedValue - value to return when a signal is suppressed
//   SupressedSignalMax - Signals with |sig-ped| >= this are not suppressed.
//                        If 0, all are suppressed
//
// Legacy behavior, i.e. that for ADCStickyCodeCheck in
// dunetpc v04.29.02 is recovered with parameter values
//   SupresssedValue = 0
//   SuppressedSignalMax = 64

#ifndef AdcCodeHelper_H
#define AdcCodeHelper_H

class AdcCodeHelper {

public:

  typedef short Signal;
  typedef float FloatSignal;

  // Ctor.
  AdcCodeHelper(Signal aSupresssedValue =0, Signal aSignalMax =64);

  // Return if the sticky bits are set.
  bool hasStickyBits(Signal sig);

  // Return if |pedsig| < SuppressedSignalMax.
  bool isSmall(FloatSignal pedsig);

  // Convert a float signal to an integer signal.
  Signal intSignal(FloatSignal fsig) const;

  // Return a pedestal-subtracted signal.
  FloatSignal subtract(Signal sig, FloatSignal ped) const;

  // Return a pedestal-subtracted signal as an integer.
  Signal intSubtract(Signal sig, FloatSignal ped) const;

private:

  // Properties.
  Signal m_SupresssedValue;
  Float m_SupressedSignalMax;

};
  
#endif
