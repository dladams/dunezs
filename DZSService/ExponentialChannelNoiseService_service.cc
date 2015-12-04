// ExponentialChannelNoiseService.cxx

#include "DZSService/ExponentialChannelNoiseService.h"
#include "Utilities/DetectorProperties.h"
#include "Utilities/LArFFT.h"
#include "art/Framework/Services/Optional/RandomNumberGenerator.h"
#include "CLHEP/Random/RandFlat.h"


using std::ostream;
using std::endl;
using std::string;

//**********************************************************************

ExponentialChannelNoiseService::
ExponentialChannelNoiseService(fhicl::ParameterSet const& pset, art::ActivityRegistry&) {
  fNoiseFactZ        = pset.get<double>("NoiseFactZ");
  fNoiseWidthZ       = pset.get<double>("NoiseWidthZ");
  fLowCutoffZ        = pset.get<double>("LowCutoffZ");
  fNoiseFactU        = pset.get<double>("NoiseFactU");
  fNoiseWidthU       = pset.get<double>("NoiseWidthU");
  fLowCutoffU        = pset.get<double>("LowCutoffU");
  fNoiseFactV        = pset.get<double>("NoiseFactV");
  fNoiseWidthV       = pset.get<double>("NoiseWidthV");
  fLowCutoffV        = pset.get<double>("LowCutoffV");
  fNoiseArrayPoints  = pset.get<unsigned int>("NoiseArrayPoints");
}

//**********************************************************************

int ExponentialChannelNoiseService::addNoise(Channel chan, SignalVector& sigs) const {
  return 0;
}

//**********************************************************************

ostream& ExponentialChannelNoiseService::print(ostream& out, string prefix) const {
  string myprefix = prefix + "  ";
  out << myprefix << "       NoiseFactZ: " << fNoiseFactZ   << endl;
  out << myprefix << "      NoiseWidthZ: " << fNoiseWidthZ  << endl;
  out << myprefix << "       LowCutoffZ: " << fLowCutoffZ << endl;
  out << myprefix << "       NoiseFactU: " << fNoiseFactU   << endl;
  out << myprefix << "      NoiseWidthU: " << fNoiseWidthU  << endl;
  out << myprefix << "       LowCutoffU: " << fLowCutoffU << endl;
  out << myprefix << "       NoiseFactV: " << fNoiseFactV   << endl;
  out << myprefix << "      NoiseWidthV: " << fNoiseWidthV  << endl;
  out << myprefix << "       LowCutoffV: " << fLowCutoffV << endl;
  out << myprefix << " NoiseArrayPoints: " << fNoiseArrayPoints << endl;
  return out;
}

//**********************************************************************

void ExponentialChannelNoiseService::
generateNoise(float aNoiseFact, float aNoiseWidth, float aLowCutoff, SignalVector& noise) const {
  // Fetch sampling rate.
  art::ServiceHandle<util::DetectorProperties> detprop;
  float sampleRate = detprop->SamplingRate();
  // Fetch FFT service and # ticks.
  art::ServiceHandle<util::LArFFT> pfft;
  unsigned int ntick = pfft->FFTSize();
  // Fetch random number engine.
  art::ServiceHandle<art::RandomNumberGenerator> rng;
  CLHEP::HepRandomEngine &engine = rng->getEngine("SimWireDUNENoise");
  CLHEP::RandFlat flat(engine);
  // Create noise spectrum in frequency.
  unsigned nbin = ntick/2 + 1;
  std::vector<TComplex> noiseFrequency(nbin, 0.);
  double pval = 0.;
  double lofilter = 0.;
  double phase = 0.;
  double rnd[2] = {0.};
  // width of frequencyBin in kHz
  double binWidth = 1.0/(ntick*sampleRate*1.0e-6);
  for ( unsigned int i=0; i<ntick/2+1; ++i ) {
    // exponential noise spectrum 
    pval = aNoiseFact*exp(-(double)i*binWidth/aNoiseWidth);
    // low frequency cutoff     
    lofilter = 1.0/(1.0+exp(-(i-aLowCutoff/binWidth)/0.5));
    // randomize 10%
    flat.fireArray(2, rnd, 0, 1);
    pval *= lofilter*(0.9 + 0.2*rnd[0]);
    // random pahse angle
    phase = rnd[1]*2.*TMath::Pi();
    TComplex tc(pval*cos(phase),pval*sin(phase));
    noiseFrequency[i] += tc;
  }
  // Obtain time spectrum from frequency spectrum.
  noise.clear();
  noise.resize(ntick,0.0);
  pfft->DoInvFFT(noiseFrequency, noise);
  noiseFrequency.clear();
  // Multiply each noise value by ntick as the InvFFT 
  // divides each bin by ntick assuming that a forward FFT
  // has already been done.
  for ( unsigned int i=0; i<noise.size(); ++i ) noise[i] *= 1.*ntick;
}

//**********************************************************************

void ExponentialChannelNoiseService::generateNoise() {
  art::ServiceHandle<art::RandomNumberGenerator> pransrv;
  fNoiseZ.resize(fNoiseArrayPoints);
  fNoiseU.resize(fNoiseArrayPoints);
  fNoiseV.resize(fNoiseArrayPoints);
  for ( unsigned int inch=0; inch<fNoiseArrayPoints; ++inch ) {
    generateNoise(fNoiseFactZ, fNoiseWidthZ, fLowCutoffZ, fNoiseZ[inch]);
    generateNoise(fNoiseFactU, fNoiseWidthU, fLowCutoffU, fNoiseZ[inch]);
    generateNoise(fNoiseFactV, fNoiseWidthV, fLowCutoffV, fNoiseZ[inch]);
  }
}

//**********************************************************************
