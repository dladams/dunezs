// ExponentialChannelNoiseService

// Implementation of TPC channel noise model with an exponential
// shape in frequency.
// Same as the nose model 1 in SimWireDUNE35t, e.g. from dunetpc v04_29_01.

#ifndef ExponentialChannelNoiseService_H
#define ExponentialChannelNoiseService_H

#include "DZSInterface/ChannelNoiseServiceBase.h"
#include <vector>
#include <iostream>

class ExponentialChannelNoiseService : public ChannelNoiseServiceBase {

public:

  typedef std::vector<SignalVector> SignalVectorVector;

  // Ctor.
  ExponentialChannelNoiseService(fhicl::ParameterSet const& pset, art::ActivityRegistry&);

  // Add noise to a signal array.
  int addNoise(Channel chan, SignalVector& sigs) const;

  // Print the configuration.
  std::ostream& print(std::ostream& out =std::cout, std::string prefix ="") const;

  // Fill a noise vector.
  // Input vector contents are lost.
  // The size of the vector is obtained from the FFT service.
  void generateNoise(float aNoiseFact, float aNoiseWidth, float aLowCutoff, SignalVector& noise) const;

private:
 
  // Fill the noise vectors.
  void generateNoise();

  // Parameters.
  float        fNoiseFactZ;        ///< noise scale factor for Z (collection) plane
  float        fNoiseWidthZ;       ///< exponential noise width (kHz)  for Z (collection) plane
  float        fLowCutoffZ;        ///< low frequency filter cutoff (kHz) for Z (collection) plane
  float        fNoiseFactU;        ///< noise scale factor  for U plane
  float        fNoiseWidthU;       ///< exponential noise width (kHz)   for U plane
  float        fLowCutoffU;        ///< low frequency filter cutoff (kHz)  for U plane
  float        fNoiseFactV;        ///< noise scale factor   for V plane
  float        fNoiseWidthV;       ///< exponential noise width (kHz)   for V plane
  float        fLowCutoffV;        ///< low frequency filter cutoff (kHz)  for V plane
  unsigned int fNoiseArrayPoints;  ///< number of points in randomly generated noise array

  // Noise arrays.
  SignalVectorVector fNoiseZ;  ///< noise on each channel for each time for Z (collection) plane
  SignalVectorVector fNoiseU;  ///< noise on each channel for each time for U plane
  SignalVectorVector fNoiseV;  ///< noise on each channel for each time for V plane


};

DECLARE_ART_SERVICE_INTERFACE_IMPL(ExponentialChannelNoiseService, ChannelNoiseServiceBase, LEGACY)

#endif
