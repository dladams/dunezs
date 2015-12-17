// ZeroSuppressFixedService.cxx

#include "DZSService/ZeroSuppressFixedService.h"
#include <cmath>
#include "fhiclcpp/ParameterSet.h"
#include "DZSCore/AdcCodeHelper.h"

using std::string;
using std::ostream;
using std::endl;

//**********************************************************************

ZeroSuppressFixedService::
ZeroSuppressFixedService(const fhicl::ParameterSet&, art::ActivityRegistry&) {
}
  
//**********************************************************************

ZeroSuppressFixedService::ZeroSuppressFixedService() { }

//**********************************************************************

int ZeroSuppressFixedService::
filter(const AdcCountVector& sigs, Channel, AdcPedestal&, AdcFilterVector& keep) const {
  keep.clear();
  keep.resize(sigs.size(), true);
  return 0;
}

//**********************************************************************

ostream& ZeroSuppressFixedService::print(ostream& out, string prefix) const {
  out << prefix << "ZeroSuppressFixedService" << endl;
  return out;
}

//**********************************************************************

DEFINE_ART_SERVICE_INTERFACE_IMPL(ZeroSuppressFixedService, AdcSuppressService)

//**********************************************************************
