// CompressReplaceService.cxx

#include "DZSService/CompressReplaceService.h"
#include "fhiclcpp/ParameterSet.h"

using std::string;
using std::ostream;
using std::endl;

//**********************************************************************

CompressReplaceService::CompressReplaceService(AdcCount azero)
: m_zero(azero) { }

//**********************************************************************

CompressReplaceService::
CompressReplaceService(const fhicl::ParameterSet& pset, art::ActivityRegistry&)
: m_zero(0) {
  pset.get_if_present<AdcCount>("Zero", m_zero);
}
  
//**********************************************************************

int CompressReplaceService::
compress(AdcCountVector& sigs, const AdcFilterVector& keep, AdcCount offset,
         raw::Compress_t& comp) const {
  for ( unsigned int isig=0; isig<sigs.size(); ++isig ) {
    if ( ! keep[isig] ) sigs[isig] = zero() + offset;
  }
  comp = raw::kNone;
  return 0;
}

//**********************************************************************

AdcCount CompressReplaceService::zero() const {
  return m_zero;
}

//**********************************************************************

ostream& CompressReplaceService::print(ostream& out, string prefix) const {
  out << prefix << "CompressReplaceService:" << endl;
  prefix += "  ";
  out << prefix << "Zero = " << m_zero << endl;
  return out;
}

//**********************************************************************

DEFINE_ART_SERVICE_INTERFACE_IMPL(CompressReplaceService, AdcCompressService)

//**********************************************************************
