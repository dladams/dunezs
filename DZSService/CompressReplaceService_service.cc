// CompressReplaceService.cxx

#include "DZSService/CompressReplaceService.h"
#include "fhiclcpp/ParameterSet.h"

using std::string;
using std::ostream;

typedef CompressReplace::Signal Signal;
typedef CompressReplace::SignalVector SignalVector;
typedef CompressReplace::FilterVector FilterVector;
typedef CompressReplace::Index Index;

//**********************************************************************

CompressReplaceService::
CompressReplaceService(const fhicl::ParameterSet& pset, art::ActivityRegistry&) {
  Signal zero = 0;
  pset.get_if_present<Signal>("Zero", zero);
  pset.get_if_present<Signal>("Option", zero);
  m_pact.reset(new CompressReplace(zero));
}
  
//**********************************************************************

int CompressReplaceService::compress(SignalVector& sigs, const FilterVector& keep, Signal offset) const {
  if ( m_pact.get() == nullptr ) {
    throw cet::exception(__FUNCTION__) << "Compression is not configured properly.";
  }
  return m_pact->compress(sigs, keep, offset);
}

//**********************************************************************

ostream& CompressReplaceService::print(ostream& out, string prefix) const {
  if ( m_pact.get() == nullptr ) {
    throw cet::exception(__FUNCTION__) << "Compression is not configured properly.";
  }
  return m_pact->print(out, prefix);
}

//**********************************************************************

DEFINE_ART_SERVICE(CompressReplaceService)

//**********************************************************************
