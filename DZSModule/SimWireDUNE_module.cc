// SimWireDUNE_module.cc
//
// David Adams
// December 2015
//
// SimWireDUNE class designed to simulate signal on a wire in the TPC
//
// Developed from  SimWireDUNE35t_module.cc.

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

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussQ.h"

#include "DZSInterface/ZeroSuppressServiceBase.h"
#include "DZSService/CompressReplaceService.h"

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
    unsigned int           fNoiseModel;          ///< noise model>
    float                  fNoiseFact;        ///< noise scale factor
    float                  fNoiseWidth;       ///< exponential noise width (kHz)
    float                  fLowCutoff;        ///< low frequency filter cutoff (kHz)
    float                  fNoiseFactZ;        ///< noise scale factor for Z (collection) plane
    float                  fNoiseWidthZ;       ///< exponential noise width (kHz)  for Z (collection) plane
    float                  fLowCutoffZ;        ///< low frequency filter cutoff (kHz) for Z (collection) plane
    float                  fNoiseFactU;        ///< noise scale factor  for U plane
    float                  fNoiseWidthU;       ///< exponential noise width (kHz)   for U plane
    float                  fLowCutoffU;        ///< low frequency filter cutoff (kHz)  for U plane
    float                  fNoiseFactV;        ///< noise scale factor   for V plane
    float                  fNoiseWidthV;       ///< exponential noise width (kHz)   for V plane
    float                  fLowCutoffV;        ///< low frequency filter cutoff (kHz)  for V plane
    int                    fNTicks;           ///< number of ticks of the clock
    double                 fSampleRate;       ///< sampling rate in ns
    unsigned int           fNSamplesReadout;  ///< number of ADC readout samples in 1 readout frame
    unsigned int           fNTimeSamples;     ///< number of ADC readout samples in all readout frames (per event)
    unsigned int           fNoiseArrayPoints; ///< number of  points in randomly generated noise array
  
    std::vector<double>    fChargeWork;
    std::vector< std::vector<float> > fNoiseZ;///< noise on each channel for each time for Z (collection) plane
    std::vector< std::vector<float> > fNoiseU;///< noise on each channel for each time for U plane
    std::vector< std::vector<float> > fNoiseV;///< noise on each channel for each time for V plane
    
    TH1D*                fNoiseDist;          ///< distribution of noise counts
    

    // variables for simulating the charge deposition in gaps and charge drifting over the comb materials.

    uint32_t               fFirstCollectionChannel;

    // variables for finding the first and last channel numbers on each plane

    std::vector< uint32_t > fFirstChannelsInPlane;
    std::vector< uint32_t > fLastChannelsInPlane;

    //define max ADC value - if one wishes this can
    //be made a fcl parameter but not likely to ever change
    const float adcsaturation = 4095;
    float                  fCollectionPed;    ///< ADC value of baseline for collection plane
    float                  fCollectionPedRMS;    ///< ADC value of baseline RMS for collection plane
    float                  fInductionPed;     ///< ADC value of baseline for induction plane
    float                  fInductionPedRMS;     ///< ADC value of baseline RMS for induction plane
    float                  fCollectionCalibPed;  ///< Assumed measured value for coll plane pedestal
    float                  fCollectionCalibPedRMS;  ///< Assumed measured value for coll plane pedestal RMS
    float                  fInductionCalibPed;     ///< Assumed measured value for ind plane pedestal
    float                  fInductionCalibPedRMS;     ///< Assumed measured value for ind plane pedestal RMS
    bool                   fPedestalOn;          ///< switch for simulation of nonzero pedestals

    // input fcl parameters

    bool                   fSimCombs;          ///< switch for simulation of the combs
    bool                   fSimStuckBits;      ///< switch for simulation of stuck bits

    std::string            fStuckBitsProbabilitiesFname; ///< file holding ADC stuck code overflow and underflow probabilities 
    std::string            fStuckBitsOverflowProbHistoName; ///< Name of histogram holding ADC stuck code overflow probabilities 
    std::string            fStuckBitsUnderflowProbHistoName; ///< Name of histogram holding ADC stuck code underflow probabilities 

    bool                   fSaveEmptyChannel;  // switch for saving channels with all zero entries

    double               fOverflowProbs[64];       ///< array of probabilities of 6 LSF bits getting stuck at 000000
    double               fUnderflowProbs[64];     ///< array of probabilities of 6 LSF bits getting stuck at 111111

    // New properties.
    art::ServiceHandle<ZeroSuppressBase> m_pzs;
    art::ServiceHandle<CompressReplaceService> m_pcmp;

  }; // class SimWireDUNE

  DEFINE_ART_MODULE(SimWireDUNE)

  //-------------------------------------------------
  SimWireDUNE::SimWireDUNE(fhicl::ParameterSet const& pset)
  {

    this->reconfigure(pset);

    produces< std::vector<raw::RawDigit>   >();

// create a default random engine; obtain the random seed from SeedService,
// unless overridden in configuration with key "Seed"
    art::ServiceHandle<artext::SeedService>()
      ->createEngine(*this, pset, "Seed");

  }

  //-------------------------------------------------
  SimWireDUNE::~SimWireDUNE()
  {

    fChargeWork.clear();
 
    for(unsigned int i = 0; i < fNoiseZ.size(); ++i) fNoiseZ[i].clear();
    fNoiseZ.clear();
   
    for(unsigned int i = 0; i < fNoiseU.size(); ++i) fNoiseU[i].clear();
    fNoiseU.clear();
   
    for(unsigned int i = 0; i < fNoiseV.size(); ++i) fNoiseV[i].clear();
    fNoiseV.clear();

  }

  //-------------------------------------------------
  void SimWireDUNE::reconfigure(fhicl::ParameterSet const& p) 
  {
    fDriftEModuleLabel= p.get< std::string         >("DriftEModuleLabel");


    fNoiseFactZ        = p.get< double              >("NoiseFactZ");
    fNoiseWidthZ       = p.get< double              >("NoiseWidthZ");
    fLowCutoffZ        = p.get< double              >("LowCutoffZ");
    fNoiseFactU        = p.get< double              >("NoiseFactU");
    fNoiseWidthU       = p.get< double              >("NoiseWidthU");
    fLowCutoffU        = p.get< double              >("LowCutoffU");
    fNoiseFactV        = p.get< double              >("NoiseFactV");
    fNoiseWidthV       = p.get< double              >("NoiseWidthV");
    fLowCutoffV        = p.get< double              >("LowCutoffV");
    fNoiseArrayPoints = p.get< unsigned int         >("NoiseArrayPoints");
    fNoiseOn           = p.get< unsigned int        >("NoiseOn");
    fNoiseModel           = p.get< unsigned int     >("NoiseModel");
    fCollectionPed    = p.get< float                >("CollectionPed");
    fCollectionPedRMS = p.get< float                >("CollectionPedRMS");
    fInductionPed     = p.get< float                >("InductionPed");
    fInductionPedRMS  = p.get< float                >("InductionPedRMS");
    fCollectionCalibPed    = p.get< float                >("CollectionCalibPed");
    fCollectionCalibPedRMS = p.get< float                >("CollectionCalibPedRMS");
    fInductionCalibPed     = p.get< float                >("InductionCalibPed");
    fInductionCalibPedRMS  = p.get< float                >("InductionCalibPedRMS");
    fPedestalOn       = p.get< bool                 >("PedestalOn");  
    art::ServiceHandle<util::DetectorProperties> detprop;
    fSampleRate       = detprop->SamplingRate();
    fNSamplesReadout  = detprop->ReadOutWindowSize();
    fNTimeSamples  = detprop->NumberTimeSamples();
    
    fSimCombs            = p.get< bool >("SimCombs");  
    fSimStuckBits        = p.get< bool >("SimStuckBits"); 

    fStuckBitsProbabilitiesFname = p.get< std::string         >("StuckBitsProbabilitiesFname");
    fStuckBitsOverflowProbHistoName = p.get< std::string         >("StuckBitsOverflowProbHistoName");
    fStuckBitsUnderflowProbHistoName = p.get< std::string         >("StuckBitsUnderflowProbHistoName");
  
    fSaveEmptyChannel    = p.get< bool >("SaveEmptyChannel");  

    m_pzs->print();
    m_pcmp->print();

    return;
  }

  //-------------------------------------------------
  void SimWireDUNE::beginJob() { 

    // get access to the TFile service
    art::ServiceHandle<art::TFileService> tfs;

    fNoiseDist  = tfs->make<TH1D>("Noise", ";Noise  (ADC);", 1000,   -10., 10.);


    art::ServiceHandle<util::LArFFT> fFFT;
    fNTicks = fFFT->FFTSize();

    if ( fNTicks%2 != 0 ) 
      LOG_DEBUG("SimWireDUNE") << "Warning: FFTSize not a power of 2. "
                                  << "May cause issues in (de)convolution.\n";

    if ( (int)fNSamplesReadout > fNTicks ) 
      mf::LogError("SimWireDUNE") << "Cannot have number of readout samples "
                                     << "greater than FFTSize!";

    fChargeWork.resize(fNTicks, 0.);
    art::ServiceHandle<geo::Geometry> geo;

    bool foundfirstcollectionchannel = false;
    fFirstChannelsInPlane.push_back(0);
    unsigned int currentPlaneNumber = geo->ChannelToWire(0).at(0).Plane; // ID of current wire plane
    unsigned int currentTPCNumber = geo->ChannelToWire(0).at(0).TPC; // ID of current wire plane

    for (uint32_t ichan=0;ichan<geo->Nchannels();ichan++) {

        if(!foundfirstcollectionchannel)
          {
            const geo::View_t view = geo->View(ichan);
            if (view == geo::kZ)
              {
                foundfirstcollectionchannel = true;
                fFirstCollectionChannel = ichan;
                //break;
              }
          }

        const unsigned int thisPlaneNumber = geo->ChannelToWire(ichan).at(0).Plane;
        const unsigned int thisTPCNumber = geo->ChannelToWire(ichan).at(0).TPC;
                
        if(thisPlaneNumber != currentPlaneNumber || (thisPlaneNumber == geo::kZ && thisTPCNumber != currentTPCNumber))
          {
            fLastChannelsInPlane.push_back(ichan-1);
            fFirstChannelsInPlane.push_back(ichan); 
            currentPlaneNumber = thisPlaneNumber;
            currentTPCNumber = thisTPCNumber;
          }

      } 
    if (!foundfirstcollectionchannel)
      {
        throw  cet::exception("SimWireDUNE  BeginJob") << " Could not find any collection channels\n";
      }
    
    fLastChannelsInPlane.push_back(geo->Nchannels()-1);

     
    // //Check starting and ending channels for each wire plane
    // for(size_t ip = 0; ip < fFirstChannelsInPlane.size(); ++ip){
    //   std::cout << "First channel in plane " << ip << " is " << fFirstChannelsInPlane.at(ip) << std::endl;
    // }

    // for(size_t ip = 0; ip < fLastChannelsInPlane.size(); ++ip){
    //   std::cout << "Last channel in plane " << ip << " is " << fLastChannelsInPlane.at(ip) << std::endl;
    // }

    //Generate noise if selected to be on
    if(fNoiseOn && fNoiseModel==1){

      //fNoise.resize(geo->Nchannels());
      fNoiseZ.resize(fNoiseArrayPoints);
      fNoiseU.resize(fNoiseArrayPoints);
      fNoiseV.resize(fNoiseArrayPoints);
      
      // GenNoise() will further resize each channel's 
      // fNoise vector to fNoiseArrayPoints long.
      
      for(unsigned int p = 0; p < fNoiseArrayPoints; ++p){
        
        fNoiseFact = fNoiseFactZ;
        fNoiseWidth = fNoiseWidthZ;
        fLowCutoff = fLowCutoffZ;

        GenNoise(fNoiseZ[p]);
        for(int i = 0; i < fNTicks; ++i)
          fNoiseDist->Fill(fNoiseZ[p][i]);
        
        fNoiseFact = fNoiseFactU;
        fNoiseWidth = fNoiseWidthU;
        fLowCutoff = fLowCutoffU;

        GenNoise(fNoiseU[p]);
        for(int i = 0; i < fNTicks; ++i)         
          fNoiseDist->Fill(fNoiseU[p][i]);


        fNoiseFact = fNoiseFactV;
        fNoiseWidth = fNoiseWidthV;
        fLowCutoff = fLowCutoffV;
 
    
        GenNoise(fNoiseV[p]);
        for(int i = 0; i < fNTicks; ++i)
          fNoiseDist->Fill(fNoiseV[p][i]);
        
      }// end loop over wires
    } 

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
    return;

  }

  //-------------------------------------------------
  void SimWireDUNE::endJob() { }

  //-------------------------------------------------
  void SimWireDUNE::produce(art::Event& evt) {
    // get the geometry to be able to figure out signal types and chan -> plane mappings
    art::ServiceHandle<geo::Geometry> geo;
    unsigned int signalSize = fNTicks;

    // vectors for working
    std::vector<short>    adcvec(signalSize, 0);        
    std::vector<const sim::SimChannel*> chanHandle;
    evt.getView(fDriftEModuleLabel,chanHandle);

    //Get fIndShape and fColShape from SignalShapingService, on the fly
    art::ServiceHandle<util::SignalShapingServiceDUNE35t> sss;

    // make a vector of const sim::SimChannel* that has same number
    // of entries as the number of channels in the detector
    // and set the entries for the channels that have signal on them
    // using the chanHandle
    std::vector<const sim::SimChannel*> channels(geo->Nchannels());
    for ( size_t c=0; c<chanHandle.size(); ++c ) {
      channels[chanHandle[c]->Channel()] = chanHandle[c];
    }
    
    // make an unique_ptr of sim::SimDigits that allows ownership of the produced
    // digits to be transferred to the art::Event after the put statement below
    std::unique_ptr< std::vector<raw::RawDigit>   >  digcol(new std::vector<raw::RawDigit>);
          
    unsigned int chan = 0; 
    fChargeWork.clear();
    fChargeWork.resize(fNTicks, 0.);
          
 
    std::vector<double> fChargeWorkCollInd;

    art::ServiceHandle<util::LArFFT> fFFT;

    // Add all channels  
    art::ServiceHandle<art::RandomNumberGenerator> rng;
    CLHEP::HepRandomEngine &engine = rng->getEngine();
    CLHEP::RandFlat flat(engine);

    std::map<int,double>::iterator mapIter;      


    unsigned int plane_number = 0;

    for ( chan = 0; chan<geo->Nchannels(); ++chan ) {    
 
      fChargeWork.clear();    
      fChargeWork.resize(fNTimeSamples, 0.);    
      if (fSimCombs)
        {
          fChargeWorkCollInd.clear();
          fChargeWorkCollInd.resize(fNTimeSamples, 0.);
        }

      // get the sim::SimChannel for this channel
      const sim::SimChannel* sc = channels[chan];
      const geo::View_t view = geo->View(chan);


      if ( sc ) {      
        // loop over the ticks and grab the number of electrons for each
        if ( fSimCombs ) {
          throw cet::exception("SimWireDUNE") << "Add call to 35t SC extractor" <<"\n";
        } else {
          for ( size_t t=0; t<fChargeWork.size(); ++t ) {
            fChargeWork[t] = sc->Charge(t);
          }
        }

        // Convolve charge with appropriate response function 
        fChargeWork.resize(fNTicks,0);
        sss->Convolute(chan,fChargeWork);

        fChargeWorkCollInd.resize(fNTicks,0);
        sss->Convolute(fFirstCollectionChannel,fChargeWorkCollInd); 

      }  // end if sc

      float ped_mean = fCollectionPed;
      float ped_rms = fCollectionPedRMS;
      geo::SigType_t sigtype = geo->SignalType(chan);
      if (sigtype == geo::kInduction){
        ped_mean = fInductionPed;
        ped_rms = fInductionPedRMS;
      }
      else if (sigtype == geo::kCollection){
        ped_mean = fCollectionPed;
        ped_rms = fCollectionPedRMS;
      }

      // noise was already generated for each wire in the event
      // raw digit vec is already in channel order
      // pick a new "noise channel" for every channel  - this makes sure    
      // the noise has the right coherent characteristics to be on one channel

      int noisechan = nearbyint(flat.fire()*(1.*(fNoiseArrayPoints-1)+0.1));
      // optimize for speed -- access vectors as arrays 

      double *fChargeWork_a=0;
      double *fChargeWorkCollInd_a=0;
      short *adcvec_a=0;
      float *noise_a_U=0;
      float *noise_a_V=0;
      float *noise_a_Z=0;

      if (signalSize>0)        {
        fChargeWork_a = fChargeWork.data();
        fChargeWorkCollInd_a = fChargeWorkCollInd.data();
        adcvec_a = adcvec.data();
        if (fNoiseOn && fNoiseModel==1) {
          noise_a_U=(fNoiseU[noisechan]).data();
          noise_a_V=(fNoiseV[noisechan]).data();
          noise_a_Z=(fNoiseZ[noisechan]).data();
        }
      }

      float tmpfv=0;  // this is here so we do our own rounding from floats to short ints (saves CPU time)
      float tnoise=0;

      if (view != geo::kU && view != geo::kV && view != geo::kZ) {
        mf::LogError("SimWireDUNE") << "ERROR: CHANNEL NUMBER " << chan << " OUTSIDE OF PLANE";
      }

      if(fNoiseOn && fNoiseModel==1) {              
        for(unsigned int i = 0; i < signalSize; ++i){
          if(view==geo::kU)       { tnoise = noise_a_U[i]; }
          else if (view==geo::kV) { tnoise = noise_a_V[i]; }
          else                    { tnoise = noise_a_Z[i]; }
          tmpfv = tnoise + fChargeWork_a[i] ;
          //mf::LogInfo("SimWireDUNE") << "Channel-bin " << chan << "-" << i << ": Signal: " << fChargeWork_a[i] << " ,  Noise: " << tnoise;
          if (fSimCombs)  tmpfv += fChargeWorkCollInd_a[i];
          //allow for ADC saturation
          if ( tmpfv > adcsaturation - ped_mean)
            tmpfv = adcsaturation- ped_mean;
          //don't allow for "negative" saturation
          if ( tmpfv < 0 - ped_mean)
            tmpfv = 0- ped_mean;

          adcvec_a[i] = (tmpfv >=0) ? (short) (tmpfv+0.5) : (short) (tmpfv-0.5); 
        }
      }else if (fNoiseOn && fNoiseModel==2){

        float fASICGain      = sss->GetASICGain(chan);  
        
        double fShapingTime   = sss->GetShapingTime(chan);
        std::map< double, int > fShapingTimeOrder;
        fShapingTimeOrder = { {0.5, 0}, {1.0, 1}, {2.0, 2}, {3.0, 3} };
        DoubleVec              fNoiseFactVec;

        //

        auto tempNoiseVec = sss->GetNoiseFactVec();

        if ( fShapingTimeOrder.find( fShapingTime ) != fShapingTimeOrder.end() ){
          size_t i = 0;
          fNoiseFactVec.resize(2);
          for (auto& item : tempNoiseVec) {
            fNoiseFactVec[i]   = item.at( fShapingTimeOrder.find( fShapingTime )->second );
            fNoiseFactVec[i] *= fASICGain/4.7;
            ++i;
          }
        }
        else {//Throw exception...
          throw cet::exception("SimWireDUNE")
            << "\033[93m"
            << "Shaping Time received from signalservices_dune.fcl is not one of allowed values"
            << std::endl
            << "Allowed values: 0.5, 1.0, 2.0, 3.0 usec"
            << "\033[00m"
            << std::endl;
        }
        //std::cout << "Xin " << fASICGain << " " << fShapingTime << " " << fNoiseFactVec[0] << " " << fNoiseFactVec[1] << std::endl;

        art::ServiceHandle<art::RandomNumberGenerator> rng;
        CLHEP::HepRandomEngine &engine = rng->getEngine();
        CLHEP::RandGaussQ rGauss_Ind(engine, 0.0, fNoiseFactVec[0]);
        CLHEP::RandGaussQ rGauss_Col(engine, 0.0, fNoiseFactVec[1]);


        for(unsigned int i = 0; i < signalSize; ++i){
          if(view==geo::kU)       { tnoise = rGauss_Ind.fire(); }
          else if (view==geo::kV) { tnoise = rGauss_Ind.fire(); }
          else                    { tnoise = rGauss_Col.fire(); }
          tmpfv = tnoise + fChargeWork_a[i] ;
          if (fSimCombs)  tmpfv += fChargeWorkCollInd_a[i];
          //allow for ADC saturation
          if ( tmpfv > adcsaturation - ped_mean)
            tmpfv = adcsaturation- ped_mean;
          //don't allow for "negative" saturation
          if ( tmpfv < 0 - ped_mean)
            tmpfv = 0- ped_mean;
          adcvec_a[i] = (tmpfv >=0) ? (short) (tmpfv+0.5) : (short) (tmpfv-0.5); 
        }
      }else {   // no noise, so just round the values to nearest short ints and store them
        for(unsigned int i = 0; i < signalSize; ++i){
          tmpfv = fChargeWork_a[i];
          if (fSimCombs) tmpfv += fChargeWorkCollInd_a[i] ;
          //allow for ADC saturation
          if ( tmpfv > adcsaturation - ped_mean)
            tmpfv = adcsaturation- ped_mean;
          //don't allow for "negative" saturation
          if ( tmpfv < 0 - ped_mean)
            tmpfv = 0- ped_mean;
          adcvec_a[i] = (tmpfv >=0) ? (short) (tmpfv+0.5) : (short) (tmpfv-0.5); 
        }
      }

      // resize the adcvec to be the correct number of time samples, 
      // just drop the extra samples


      adcvec.resize(fNSamplesReadout);

      float calibrated_pedestal_value = 0; // Estimated calibrated value of pedestal to be passed to RawDigits collection
      float calibrated_pedestal_rms_value = 0; // Estimated calibrated value of pedestal RMS to be passed to RawDigits collection
      //dla int calibrated_integer_pedestal_value = 0; // Estimated calibrated value of pedestal to be passed to data compression

      // add pedestal values
      if(fPedestalOn)
        {
          if(ped_rms>0){
            art::ServiceHandle<art::RandomNumberGenerator> rng;
            CLHEP::HepRandomEngine &engine = rng->getEngine();
            CLHEP::RandGaussQ rGauss_Ped(engine, 0.0, ped_rms);
            for(unsigned int i = 0; i < signalSize; ++i){
              float ped_variation = rGauss_Ped.fire();
              tmpfv = adcvec_a[i] + ped_mean + ped_variation;
              
              adcvec_a[i] = (short) tmpfv; 
              
            }
          }
          else{
            for(unsigned int i = 0; i < signalSize; ++i){
              tmpfv = adcvec_a[i] + ped_mean;
              adcvec_a[i] = (short) tmpfv; 
            }

          }
          


          if (sigtype == geo::kInduction){
            calibrated_pedestal_value = fInductionCalibPed;
            calibrated_pedestal_rms_value = fInductionCalibPedRMS;
          }
          else if (sigtype == geo::kCollection){
            calibrated_pedestal_value = fCollectionCalibPed;
            calibrated_pedestal_rms_value = fCollectionCalibPedRMS;
          }
          
        }
      else{
        calibrated_pedestal_value = 0;
        calibrated_pedestal_rms_value = 0;

      }
      
      //dla calibrated_integer_pedestal_value = (int) calibrated_pedestal_value;
      

      if ( fSimStuckBits ) {

          for ( size_t i = 0; i < adcvec.size(); ++i ) {

            art::ServiceHandle<art::RandomNumberGenerator> rng;
            CLHEP::HepRandomEngine &engine = rng->getEngine();
            CLHEP::RandFlat flat(engine);
            
            
            double rnd = flat.fire(0,1);
           

            unsigned int zeromask = 0xffc0;
            unsigned int onemask = 0x003f;

            unsigned int sixlsbs = adcvec_a[i] & onemask;

            int probability_index = (int)sixlsbs;

            if(rnd < fUnderflowProbs[probability_index]){
              adcvec_a[i] = adcvec_a[i] | onemask; // 6 LSBs are stuck at 3F
              adcvec_a[i] -= 64; // correct 1st MSB value by subtracting 64
            }
            else if(rnd > fUnderflowProbs[probability_index] && rnd < fUnderflowProbs[probability_index] + fOverflowProbs[probability_index]){
              adcvec_a[i] = adcvec_a[i] & zeromask; // 6 LSBs are stuck at 0
              adcvec_a[i] += 64; // correct 1st MSB value by adding 64
            }
            //else adcvec value remains unchanged
         }

      }
      
      // Zero suppress and compress.
      ZeroSuppressBase::ResultVector keep;
      m_pzs->filter(adcvec, chan, calibrated_pedestal_value, keep);
      int nkeep = 0;
      for ( bool kept : keep ) if ( kept ) ++nkeep;
      m_pcmp->compress(adcvec, keep);
      raw::Compress_t myCompression = raw::kNone;

      // Create and store raw digit.
      raw::RawDigit rd(chan, fNSamplesReadout, adcvec, myCompression);
      rd.SetPedestal(calibrated_pedestal_value,calibrated_pedestal_rms_value);
      adcvec.resize(signalSize);        // Then, resize adcvec back to full length.  Do not initialize to zero (slow)
      if ( fSaveEmptyChannel || nkeep>0 )
      digcol->push_back(rd);            // add this digit to the collection


      adcvec.resize(signalSize);        // Then, resize adcvec back to full length.  Do not initialize to zero (slow)
 
      if(chan==fLastChannelsInPlane.at(plane_number))
        ++plane_number;

    }// end loop over channels      

    evt.put(std::move(digcol));

    return;
  }

  //-------------------------------------------------
  void SimWireDUNE::GenNoise(std::vector<float>& noise) {
    art::ServiceHandle<art::RandomNumberGenerator> rng;
    CLHEP::HepRandomEngine &engine = rng->getEngine();
    CLHEP::RandFlat flat(engine);

    noise.clear();
    noise.resize(fNTicks,0.0);
    // noise in frequency space
    std::vector<TComplex> noiseFrequency(fNTicks/2+1, 0.);

    double pval = 0.; 
    double lofilter = 0.;
    double phase = 0.;
    double rnd[2] = {0.};

    // width of frequencyBin in kHz
    double binWidth = 1.0/(fNTicks*fSampleRate*1.0e-6);
    for(int i=0; i< fNTicks/2+1; ++i){
      // exponential noise spectrum 
      pval = fNoiseFact*exp(-(double)i*binWidth/fNoiseWidth);
      // low frequency cutoff     
      lofilter = 1.0/(1.0+exp(-(i-fLowCutoff/binWidth)/0.5));
      // randomize 10%
      flat.fireArray(2,rnd,0,1);
      pval *= lofilter*(0.9+0.2*rnd[0]);
      // random pahse angle
      phase = rnd[1]*2.*TMath::Pi();

      TComplex tc(pval*cos(phase),pval*sin(phase));
      noiseFrequency[i] += tc;
    }
  
   
    // inverse FFT MCSignal
    art::ServiceHandle<util::LArFFT> fFFT;
    fFFT->DoInvFFT(noiseFrequency, noise);

    noiseFrequency.clear();

    // multiply each noise value by fNTicks as the InvFFT 
    // divides each bin by fNTicks assuming that a forward FFT
    // has already been done.
    for(unsigned int i = 0; i < noise.size(); ++i) noise[i] *= 1.*fNTicks;

    return;
  }
