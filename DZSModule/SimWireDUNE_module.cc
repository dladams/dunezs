// SimWireDUNE_module.cc
//
// David Adams
// December 2015
//
// SimWireDUNE class designed to simulate signal on a wire in the TPC
//
// Developed from  SimWireDUNE35t_module.cc.

#undef OLDPED

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <sstream>
#include <bitset>

extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
}

#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Optional/TFileDirectory.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// art extensions
#include "artextensions/SeedService/SeedService.hh"

#include "Utilities/LArFFT.h"
#include "RawData/raw.h"
#include "Utilities/LArProperties.h"
#include "dune/Utilities/SignalShapingServiceDUNE35t.h"
#include "Geometry/Geometry.h"

#include "Simulation/sim.h"
#include "Simulation/SimChannel.h"
#include "RawData/RawDigit.h"
#include "Utilities/DetectorProperties.h"

#include "TMath.h"
#include "TComplex.h"
#include "TString.h"
#include "TH2.h"
#include "TH1D.h"
#include "TFile.h"
#include "TProfile.h"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussQ.h"

#include "DZSCore/AdcTypes.h"
#include "DZSInterface/ZeroSuppressServiceBase.h"
#include "DZSService/CompressReplaceService.h"
#include "DZSInterface/SimChannelExtractServiceBase.h"
#include "DZSInterface/ChannelNoiseServiceBase.h"
#include "DZSInterface/PedestalAdditionService.h"
#include "DZSService/StuckBitsService.h"

#ifdef OLDPED
#include "CalibrationDBI/Interface/IDetPedestalService.h"
#include "CalibrationDBI/Interface/IDetPedestalProvider.h"
#endif

using std::ostringstream;
using std::endl;

//**********************************************************************

// Base class for creation of raw signals on wires. 
class SimWireDUNE : public art::EDProducer {
    
public:
        
  explicit SimWireDUNE(fhicl::ParameterSet const& pset); 
  virtual ~SimWireDUNE();
    
  // read/write access to event
  void produce (art::Event& evt);
  void beginJob();
  void endJob();
  void reconfigure(fhicl::ParameterSet const& p);

private:

  void         GenNoise(std::vector<float>& array);

  std::string            fDriftEModuleLabel;///< module making the ionization electrons
  int                    fFFTSize;          ///< number of ticks for FFT
  unsigned int           fNSamplesReadout;  ///< number of ADC readout samples in 1 readout frame
  
  TH1*                fNoiseChanDist;      ///< distribution of noise channels
#ifdef OLDPED
  TH1*                fPedNoiseDist;       ///< distribution of pedestal noise counts
#endif
    

  // Find a collection channel for dhaping extra charge.
  uint32_t               fFirstCollectionChannel;

  // Define max ADC value.
  const float adcmax = 4095;

  bool fNoiseOn;           ///< noise turned on or off for debugging; default is on
  bool fPedestalOn;        ///< switch for simulation of nonzero pedestals
  bool fStuckBitsOn;       ///< switch for simulation of stuck bits
  bool fSaveEmptyChannel;  ///< switch for saving channels with all zero entries

  // New services.
  art::ServiceHandle<ZeroSuppressBase> m_pzs;
  art::ServiceHandle<CompressReplaceService> m_pcmp;
  art::ServiceHandle<SimChannelExtractServiceBase> m_pscx;
  art::ServiceHandle<ChannelNoiseServiceBase> m_pcns;
  art::ServiceHandle<StuckBitsService> m_pasb;
#ifndef OLDPED
  art::ServiceHandle<PedestalAdditionService> m_ppad;
#endif

}; // class SimWireDUNE

DEFINE_ART_MODULE(SimWireDUNE)

//**********************************************************************

SimWireDUNE::SimWireDUNE(fhicl::ParameterSet const& pset) {
  reconfigure(pset);
  produces< std::vector<raw::RawDigit> >();
  // create a default random engine; obtain the random seed from SeedService,
  // unless overridden in configuration with key "Seed"
  //seedSvc->createEngine(*this, pset, "SimWireDUNENoise");
  // Create other services with local seeds. SeedService will not create two engines.
  int seedNoise = 2001;
#ifdef OLDPED
  int seedPedestal = 2003;
#endif
  bool useSeedSvc = true;
  if ( useSeedSvc ) {
    art::ServiceHandle<artext::SeedService> seedSvc;
    seedNoise    = seedSvc->getSeed("SimWireDUNENoise");
#ifdef OLDPED
    seedPedestal = seedSvc->getSeed("SimWireDUNEPedestal");
#endif
  }
  createEngine(seedNoise,    "HepJamesRandom", "SimWireDUNENoise");
#ifdef OLDPED
  createEngine(seedPedestal, "HepJamesRandom", "SimWireDUNEPedestal");
#endif
}

//**********************************************************************

SimWireDUNE::~SimWireDUNE() { }

//**********************************************************************

void SimWireDUNE::reconfigure(fhicl::ParameterSet const& p) {
  fDriftEModuleLabel = p.get<std::string>("DriftEModuleLabel");
  fNoiseOn           = p.get<bool>("NoiseOn");
  fPedestalOn        = p.get<bool>("PedestalOn");  
  fStuckBitsOn        = p.get<bool>("StuckBitsOn");  
  fSaveEmptyChannel    = p.get< bool >("SaveEmptyChannel");  

  art::ServiceHandle<util::DetectorProperties> detprop;
  fNSamplesReadout  = detprop->ReadOutWindowSize();

  ostringstream out;
  out << "  Compression service:";
  m_pcmp->print(out, "    ");
  out << "  Zero suppression service:" << endl;;
  out << endl;
  m_pzs->print(out, "    ");
  if ( fNoiseOn ) {
    out << "  Channel noise service:" << endl;;
    m_pcns->print(out, "    ");
  } else {
    out << "  Channel noise is off.";
  }
  out << endl;
  if ( fPedestalOn ) {
    out << "  Pedestal addition service:" << endl;;
    m_ppad->print(out, "    ");
  } else {
    out << "  Pedestal addition is off.";
  }
  out << endl;
  if ( fStuckBitsOn ) {
    out << "  Stuck bits service:" << endl;;
    m_pasb->print(out, "    ");
  } else {
    out << "  Stuck bits is off.";
  }
  mf::LogInfo("SimWireDUNE::reconfigure") << out.str();

  return;
}

//**********************************************************************

void SimWireDUNE::beginJob() { 

  // get access to the TFile service
  art::ServiceHandle<art::TFileService> tfs;

#ifdef OLDPED
  fPedNoiseDist  = tfs->make<TH1F>("PedNoise", ";Pedestal noise  (ADC);", 1000,   -1., 1.);
#endif

  art::ServiceHandle<util::LArFFT> fFFT;
  fFFTSize = fFFT->FFTSize();

  if ( fFFTSize%2 != 0 ) 
    LOG_DEBUG("SimWireDUNE") << "Warning: FFTSize not a power of 2. "
                             << "May cause issues in (de)convolution.\n";

  if ( (int)fNSamplesReadout > fFFTSize ) 
    mf::LogError("SimWireDUNE") << "Cannot have number of readout samples "
                                  << "greater than FFTSize!";

  art::ServiceHandle<geo::Geometry> geo;

  // Find the first collection channel.
  for ( uint32_t ichan=0; ichan<geo->Nchannels(); ++ichan ) {
    if ( geo->View(ichan) == geo::kZ ) {
      fFirstCollectionChannel = ichan;
      break;
    }
  }

  return;

  }

  //-------------------------------------------------
  void SimWireDUNE::endJob() { }

  //-------------------------------------------------
  void SimWireDUNE::produce(art::Event& evt) {

    // Get the geometry.
    art::ServiceHandle<geo::Geometry> geo;
    unsigned int signalSize = fFFTSize;

    // Get the SignalShapingService.
    art::ServiceHandle<util::SignalShapingServiceDUNE35t> sss;

#ifdef OLDPED
    // Get the pedestal retriever.
    const lariov::IDetPedestalProvider& pedestalProvider
      = art::ServiceHandle<lariov::IDetPedestalService>()->GetPedestalProvider();
    mf::LogInfo("SimWireDUNE") << "Pedestal for channel 0: " << pedestalProvider.PedMean(0);
#endif

    // make a vector of const sim::SimChannel* that has same number
    // of entries as the number of channels in the detector
    // and set the entries for the channels that have signal on them
    // using the chanHandle
    std::vector<const sim::SimChannel*> chanHandle;
    std::vector<const sim::SimChannel*> channels(geo->Nchannels());
    evt.getView(fDriftEModuleLabel, chanHandle);
    for ( size_t c=0; c<chanHandle.size(); ++c ) {
      channels[chanHandle[c]->Channel()] = chanHandle[c];
    }
    
    // make an unique_ptr of sim::SimDigits that allows ownership of the produced
    // digits to be transferred to the art::Event after the put statement below
    std::unique_ptr< std::vector<raw::RawDigit>   >  digcol(new std::vector<raw::RawDigit>);
          
    art::ServiceHandle<util::LArFFT> fFFT;
    art::ServiceHandle<art::RandomNumberGenerator> rng;

    // Add all channels  
    std::map<int,double>::iterator mapIter;      
    for ( unsigned int chan = 0; chan<geo->Nchannels(); ++chan ) {    
 

      // get the sim::SimChannel for this channel
      const sim::SimChannel* psc = channels[chan];
      const geo::View_t view = geo->View(chan);
      if (view != geo::kU && view != geo::kV && view != geo::kZ) {
        mf::LogError("SimWireDUNE") << "ERROR: CHANNEL NUMBER " << chan << " OUTSIDE OF PLANE";
      }

      // Create vector that holds the floating ADC count for each tick.
      std::vector<AdcSignal> fChargeWork(signalSize, 0.0);

      // Use the SimChannel to assign signal charge to each tick in the channel.
      if ( psc ) {      
        // Use the SimChannel extraction service to find the charge seen by each channel.
        // If extra charge is collected, it is stored in fChargeWorkCollInd and should
        // be shaped for collection even if this is an induction channel.
        std::vector<AdcSignal> fChargeWorkCollInd(signalSize, 0.0);
        fChargeWorkCollInd.resize(0);
        if ( m_pscx->extract(*psc, fChargeWork, fChargeWorkCollInd) ) {
          throw cet::exception("SimWireDUNE") << "Error calling SC extractor" <<"\n";
        }

        // Convolve charge with appropriate response function 
        fChargeWork.resize(signalSize, 0);
        sss->Convolute(chan,fChargeWork);

        // If there is extra charge, convolve it as collected charge and add it to the working charge.
        if ( fChargeWorkCollInd.size() > 0 ) {
          fChargeWorkCollInd.resize(signalSize, 0);
          sss->Convolute(fFirstCollectionChannel, fChargeWorkCollInd); 
          for ( unsigned int itck=0; itck<signalSize; ++itck ) {
            fChargeWork[itck] += fChargeWorkCollInd[itck];
          }
        }

      }  // end if psc

      // Add noise to each tick in the channel.
      if ( fNoiseOn ) {              
        m_pcns->addNoise(chan, fChargeWork);
      }

      float calibrated_pedestal_value     = 0.0; // Pedestal to be recorded in RawDigits collection
      float calibrated_pedestal_rms_value = 0.0; // Pedestal RMS to be recorded in RawDigits collection

#ifdef OLDPED
      // Add pedestal including variation to each tick in the channel.
      if ( fPedestalOn ) {
        // Fetch the pedestal for this channel.
        float ped_mean = pedestalProvider.PedMean(chan);
        float ped_rms =  pedestalProvider.PedRms(chan);
        for ( unsigned int itck=0; itck<signalSize; ++itck ) {
          fChargeWork[itck] += ped_mean;
        }
        if ( ped_rms > 0 ) {
          art::ServiceHandle<art::RandomNumberGenerator> rng;
          CLHEP::HepRandomEngine &ped_engine = rng->getEngine("SimWireDUNEPedestal");
          CLHEP::RandGaussQ rGauss_Ped(ped_engine, 0.0, ped_rms);
          for ( unsigned int itck=0; itck<signalSize; ++itck ) {
            double ped_variation = rGauss_Ped.fire();
            fPedNoiseDist->Fill(ped_variation);
            fChargeWork[itck] += ped_variation;
          }
        }
        calibrated_pedestal_value = ped_mean;
        calibrated_pedestal_rms_value = ped_rms;
      }
#else
      m_ppad->addPedestal(chan, fChargeWork, calibrated_pedestal_value, calibrated_pedestal_rms_value);
#endif

      // Convert floating ADC to count in ADC range.
      std::vector<short> adcvec(signalSize, 0);        
      for ( unsigned int itck=0; itck<signalSize; ++itck ) {
        double chg = fChargeWork[itck];
        short adc = (chg >=0) ? (short) (chg+0.5) : (short) (chg-0.5);
        if ( adc > adcmax ) adc = adcmax;
        if ( adc < 0 ) adc = 0;
        adcvec[itck] = adc;
      }
      // Resize to the correct number of time samples, dropping extra samples.
      adcvec.resize(fNSamplesReadout);
      
      // Add stuck bits.
      if ( fStuckBitsOn ) {
        m_pasb->modify(chan, adcvec);
      }
      
      // Zero suppress and compress.
      ZeroSuppressBase::ResultVector keep;
      m_pzs->filter(adcvec, chan, calibrated_pedestal_value, keep);
      int nkeep = 0;
      for ( bool kept : keep ) if ( kept ) ++nkeep;
      m_pcmp->compress(adcvec, keep, calibrated_pedestal_value);
      raw::Compress_t myCompression = raw::kNone;

      // Create and store raw digit.
      raw::RawDigit rd(chan, fNSamplesReadout, adcvec, myCompression);
      rd.SetPedestal(calibrated_pedestal_value,calibrated_pedestal_rms_value);
      adcvec.resize(signalSize);        // Then, resize adcvec back to full length.  Do not initialize to zero (slow)
      if ( fSaveEmptyChannel || nkeep>0 )
      digcol->push_back(rd);            // add this digit to the collection


    }// end loop over channels      

    evt.put(std::move(digcol));

    return;
  }
