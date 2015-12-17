// CompressReplaceService.cxx

#include "DZSService/CompressReplaceService.h"
#include "fhiclcpp/ParameterSet.h"

using std::string;
using std::ostream;

typedef CompressReplace::FilterVector FilterVector;
typedef CompressReplace::Index Index;

//**********************************************************************

CompressReplaceService::
CompressReplaceService(const fhicl::ParameterSet& pset, art::ActivityRegistry&) {
  AdcCount zero = 0;
  pset.get_if_present<AdcCount>("Zero", zero);
  m_pact.reset(new CompressReplace(zero));
}
  
//**********************************************************************

int CompressReplaceService::
compress(AdcCountVector& sigs, const FilterVector& keep, AdcCount offset,
         raw::Compress_t& comp) const {
  if ( m_pact.get() == nullptr ) {
    throw cet::exception(__FUNCTION__) << "Compression is not configured properly.";
  }
  return m_pact->compress(sigs, keep, offset, comp);
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
