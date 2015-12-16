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
#include <sstream>

#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "RawData/raw.h"
#include "Geometry/Geometry.h"
#include "Simulation/sim.h"
#include "Simulation/SimChannel.h"
#include "RawData/RawDigit.h"
#include "Utilities/DetectorProperties.h"

#include "DZSCore/AdcTypes.h"
#include "DZSInterface/ZeroSuppressServiceBase.h"
#include "DZSService/CompressReplaceService.h"
#include "DZSInterface/SimChannelExtractServiceBase.h"
#include "DZSInterface/ChannelNoiseServiceBase.h"
#include "DZSInterface/PedestalAdditionService.h"
#include "DZSService/StuckBitsService.h"

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

  std::string            fDriftEModuleLabel; ///< module making the ionization electrons
  
  // Flags.
  bool fNoiseOn;           ///< noise turned on or off for debugging; default is on
  bool fPedestalOn;        ///< switch for simulation of nonzero pedestals
  bool fStuckBitsOn;       ///< switch for simulation of stuck bits
  bool fSaveEmptyChannel;  ///< switch for saving channels with all zero entries

  // Services.
  art::ServiceHandle<ZeroSuppressBase> m_pzs;
  art::ServiceHandle<CompressReplaceService> m_pcmp;
  art::ServiceHandle<SimChannelExtractServiceBase> m_pscx;
  art::ServiceHandle<ChannelNoiseServiceBase> m_pcns;
  art::ServiceHandle<StuckBitsService> m_pasb;
  art::ServiceHandle<PedestalAdditionService> m_ppad;

}; // class SimWireDUNE

DEFINE_ART_MODULE(SimWireDUNE)

//**********************************************************************

SimWireDUNE::SimWireDUNE(fhicl::ParameterSet const& pset) {
  reconfigure(pset);
  produces< std::vector<raw::RawDigit> >();
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

void SimWireDUNE::beginJob() { }

//**********************************************************************

void SimWireDUNE::endJob() { }

//**********************************************************************

void SimWireDUNE::produce(art::Event& evt) {

  // Get the geometry.
  art::ServiceHandle<geo::Geometry> geo;

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
  std::unique_ptr<std::vector<raw::RawDigit>>  digcol(new std::vector<raw::RawDigit>);
          
  // Fetch the number of ticks to write out for each channel.
  art::ServiceHandle<util::DetectorProperties> detprop;
  unsigned int nTickReadout  = detprop->ReadOutWindowSize();

  // Loop over channels.
  std::map<int,double>::iterator mapIter;      
  for ( unsigned int chan = 0; chan<geo->Nchannels(); ++chan ) {    

    // get the sim::SimChannel for this channel
    const sim::SimChannel* psc = channels[chan];
    const geo::View_t view = geo->View(chan);
    if (view != geo::kU && view != geo::kV && view != geo::kZ) {
      mf::LogError("SimWireDUNE") << "ERROR: CHANNEL NUMBER " << chan << " OUTSIDE OF PLANE";
    }

    // Create vector that holds the floating ADC count for each tick.
    std::vector<AdcSignal> fChargeWork;

    // Use the SimChannel to assign signal charge to each tick in the channel.
    m_pscx->extract(psc, fChargeWork);

    // Add noise to each tick in the channel.
    if ( fNoiseOn ) {              
      m_pcns->addNoise(chan, fChargeWork);
    }

    // Add pedestal.
    float calibrated_pedestal_value     = 0.0; // Pedestal to be recorded in RawDigits collection
    float calibrated_pedestal_rms_value = 0.0; // Pedestal RMS to be recorded in RawDigits collection
    if ( fPedestalOn ) {
      m_ppad->addPedestal(chan, fChargeWork, calibrated_pedestal_value, calibrated_pedestal_rms_value);
    }

    // Convert floating ADC to count in ADC range.
    std::vector<short> adcvec(fChargeWork.size(), 0);        
    const float adcmax = 4095;
    for ( unsigned int itck=0; itck<fChargeWork.size(); ++itck ) {
      double chg = fChargeWork[itck];
      short adc = (chg >=0) ? (short) (chg+0.5) : (short) (chg-0.5);
      if ( adc > adcmax ) adc = adcmax;
      if ( adc < 0 ) adc = 0;
      adcvec[itck] = adc;
    }
    // Resize to the correct number of time samples, dropping extra samples.
    adcvec.resize(nTickReadout);
    
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
    raw::RawDigit rd(chan, nTickReadout, adcvec, myCompression);
    rd.SetPedestal(calibrated_pedestal_value,calibrated_pedestal_rms_value);
    digcol->push_back(rd);            // add this digit to the collection

  }  // end loop over channels      

  evt.put(std::move(digcol));
 
}
