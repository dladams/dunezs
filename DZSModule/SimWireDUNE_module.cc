// SimWireDUNE_module.cc
//
// David Adams
// December 2015
//
// SimWireDUNE class designed to simulate signal on a wire in the TPC
//
// Developed from  SimWireDUNE35t_module.cc.

#undef  OLDSTUCK

#include <vector>
#include <string>
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

#undef  OLDNOISE2

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussQ.h"

#include "DZSCore/AdcTypes.h"
#include "DZSInterface/ZeroSuppressServiceBase.h"
#include "DZSService/CompressReplaceService.h"
#include "DZSInterface/SimChannelExtractServiceBase.h"
#include "DZSInterface/ChannelNoiseServiceBase.h"
#include "DZSService/StuckBitsService.h"

#include "CalibrationDBI/Interface/IDetPedestalService.h"
#include "CalibrationDBI/Interface/IDetPedestalProvider.h"

///Detector simulation of raw signals on wires 
namespace detsim {

    typedef enum {
      NONACTIVE, UCOMB, VCOMB, ACTIVE, HORIZGAP, VERTGAP
    } GapType_t;

}

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
    unsigned int           fNoiseOn;          ///< noise turned on or off for debugging; default is on
    int                    fFFTSize;          ///< number of ticks for FFT
    unsigned int           fNSamplesReadout;  ///< number of ADC readout samples in 1 readout frame
  
    
    TH1*                fNoiseChanDist;      ///< distribution of noise channels
    TH1*                fPedNoiseDist;       ///< distribution of pedestal noise counts
    

    // Find a collection channel for dhaping extra charge.
    uint32_t               fFirstCollectionChannel;

    // Define max ADC value.
    const float adcmax = 4095;

    //be made a fcl parameter but not likely to ever change
    bool                   fPedestalOn;          ///< switch for simulation of nonzero pedestals

    // input fcl parameters

    bool                   fSimStuckBits;      ///< switch for simulation of stuck bits
#ifdef OLDSTUCK
    std::string            fStuckBitsProbabilitiesFname; ///< file holding ADC stuck code overflow and underflow probabilities 
    std::string            fStuckBitsOverflowProbHistoName; ///< Name of histogram holding ADC stuck code overflow probabilities 
    std::string            fStuckBitsUnderflowProbHistoName; ///< Name of histogram holding ADC stuck code underflow probabilities 
    double               fOverflowProbs[64];       ///< array of probabilities of 6 LSF bits getting stuck at 000000
    double               fUnderflowProbs[64];     ///< array of probabilities of 6 LSF bits getting stuck at 111111
#endif

    bool                   fSaveEmptyChannel;  // switch for saving channels with all zero entries


    // New services.
    art::ServiceHandle<ZeroSuppressBase> m_pzs;
    art::ServiceHandle<CompressReplaceService> m_pcmp;
    art::ServiceHandle<SimChannelExtractServiceBase> m_pscx;
    art::ServiceHandle<ChannelNoiseServiceBase> m_pcns;
    art::ServiceHandle<StuckBitsService> m_pasb;

  }; // class SimWireDUNE

  DEFINE_ART_MODULE(SimWireDUNE)

  //-------------------------------------------------

  SimWireDUNE::SimWireDUNE(fhicl::ParameterSet const& pset) {
    this->reconfigure(pset);
    produces< std::vector<raw::RawDigit> >();
    // create a default random engine; obtain the random seed from SeedService,
    // unless overridden in configuration with key "Seed"
    //seedSvc->createEngine(*this, pset, "SimWireDUNENoise");
    // Create other services with local seeds. SeedService will not create two engines.
    int seedNoise = 2001;
    int seedPedestal = 2003;
#ifdef OLDSTUCK
    int seedStuck = 2005;
#endif
    bool useSeedSvc = true;
    if ( useSeedSvc ) {
      art::ServiceHandle<artext::SeedService> seedSvc;
      seedNoise    = seedSvc->getSeed("SimWireDUNENoise");
      seedPedestal = seedSvc->getSeed("SimWireDUNEPedestal");
#ifdef OLDSTUCK
      seedStuck    = seedSvc->getSeed("SimWireDUNEStuck");
#endif
    }
    createEngine(seedNoise,    "HepJamesRandom", "SimWireDUNENoise");
    createEngine(seedPedestal, "HepJamesRandom", "SimWireDUNEPedestal");
#ifdef OLDSTUCK
    createEngine(seedStuck,    "HepJamesRandom", "SimWireDUNEStuck");
#endif
  }

  //-------------------------------------------------
  SimWireDUNE::~SimWireDUNE() {

  }

  //-------------------------------------------------
  void SimWireDUNE::reconfigure(fhicl::ParameterSet const& p) {
    fDriftEModuleLabel= p.get<std::string>("DriftEModuleLabel");
    fNoiseOn          = p.get<bool>("NoiseOn");
    fPedestalOn       = p.get< bool                 >("PedestalOn");  
    art::ServiceHandle<util::DetectorProperties> detprop;
    fNSamplesReadout  = detprop->ReadOutWindowSize();
    
#ifdef OLDSTUCK
    fSimStuckBits        = p.get< bool >("SimStuckBits"); 

    fStuckBitsProbabilitiesFname = p.get< std::string         >("StuckBitsProbabilitiesFname");
    fStuckBitsOverflowProbHistoName = p.get< std::string         >("StuckBitsOverflowProbHistoName");
    fStuckBitsUnderflowProbHistoName = p.get< std::string         >("StuckBitsUnderflowProbHistoName");
#endif
  
    fSaveEmptyChannel    = p.get< bool >("SaveEmptyChannel");  

    m_pzs->print();
    m_pcmp->print();
    m_pcns->print();
    m_pasb->print();

    return;
  }

  //-------------------------------------------------
  void SimWireDUNE::beginJob() { 

    // get access to the TFile service
    art::ServiceHandle<art::TFileService> tfs;

    fPedNoiseDist  = tfs->make<TH1F>("PedNoise", ";Pedestal noise  (ADC);", 1000,   -1., 1.);

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

     
#ifdef OLDSTUCK
    if(fSimStuckBits){
  
      mf::LogInfo("SimWireDUNE") << " using ADC stuck code probabilities from .root file " ;

      // constructor decides if initialized value is a path or an environment variable
      std::string fname;
      cet::search_path sp("FW_SEARCH_PATH");
      sp.find_file( fStuckBitsProbabilitiesFname, fname );
        
      std::unique_ptr<TFile> fin(new TFile(fname.c_str(), "READ"));
      if ( !fin->IsOpen() ) throw art::Exception( art::errors::NotFound ) << "Could not find the ADC stuck code probabilities file " << fname << "!" << std::endl;
 
      TString iOverflowHistoName = Form( "%s", fStuckBitsOverflowProbHistoName.c_str());
      TProfile *overflowtemp = (TProfile*) fin->Get( iOverflowHistoName );  
      if ( !overflowtemp ) throw art::Exception( art::errors::NotFound ) << "Could not find the ADC code overflow probabilities histogram " << fStuckBitsOverflowProbHistoName << "!" << std::endl;
      
      if ( overflowtemp->GetNbinsX() != 64 ) throw art::Exception( art::errors::InvalidNumber ) << "Overflow ADC stuck code probability histograms should always have 64 bins corresponding to each of 64 LSB cells!" << std::endl;
 
      TString iUnderflowHistoName = Form( "%s", fStuckBitsUnderflowProbHistoName.c_str());     
      TProfile *underflowtemp = (TProfile*) fin->Get( iUnderflowHistoName );  
      if ( !underflowtemp ) throw art::Exception( art::errors::NotFound ) << "Could not find the ADC code underflow probabilities histogram " << fStuckBitsUnderflowProbHistoName << "!" << std::endl;
      
      if ( underflowtemp->GetNbinsX() != 64 ) throw art::Exception( art::errors::InvalidNumber ) << "Underflow ADC stuck code probability histograms should always have 64 bins corresponding to each of 64 LSB cells!" << std::endl;


      for(unsigned int cellnumber=0; cellnumber < 64; ++cellnumber){
        fOverflowProbs[cellnumber] = overflowtemp->GetBinContent(cellnumber+1);
        fUnderflowProbs[cellnumber] = underflowtemp->GetBinContent(cellnumber+1);
      }
    
      fin->Close();
    }
#endif
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

    // Get the pedestal retriever.
    const lariov::IDetPedestalProvider& pedestalProvider
      = art::ServiceHandle<lariov::IDetPedestalService>()->GetPedestalProvider();
    mf::LogInfo("SimWireDUNE") << "Pedestal for channel 0: " << pedestalProvider.PedMean(0);

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

      float calibrated_pedestal_value     = 0.0; // Estimated calibrated value of pedestal to be passed to RawDigits collection
      float calibrated_pedestal_rms_value = 0.0; // Estimated calibrated value of pedestal RMS to be passed to RawDigits collection

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
      if ( fSimStuckBits ) {
#ifdef OLDSTUCK
        for ( size_t i = 0; i < adcvec.size(); ++i ) {
          art::ServiceHandle<art::RandomNumberGenerator> rng;
          CLHEP::HepRandomEngine& stuck_engine = rng->getEngine("SimWireDUNEStuck");
          CLHEP::RandFlat stuck_flat(stuck_engine);
          double rnd = stuck_flat.fire(0,1);
          unsigned int zeromask = 0xffc0;
          unsigned int onemask = 0x003f;
          unsigned int sixlsbs = adcvec[i] & onemask;
          int probability_index = (int)sixlsbs;
          if ( rnd < fUnderflowProbs[probability_index] ) {
            adcvec[i] = adcvec[i] | onemask; // 6 LSBs are stuck at 3F
            adcvec[i] -= 64; // correct 1st MSB value by subtracting 64
          } else if ( rnd > fUnderflowProbs[probability_index] &&
                    rnd < fUnderflowProbs[probability_index] + fOverflowProbs[probability_index] ) {
            adcvec[i] = adcvec[i] & zeromask; // 6 LSBs are stuck at 0
            adcvec[i] += 64; // correct 1st MSB value by adding 64
          }
        }
#else
        m_pasb->modify(chan, adcvec);
#endif
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
