// ZeroSuppressFixedService.cxx

#include "DZSService/ZeroSuppressFixedService.h"
#include <cmath>
#include "fhiclcpp/ParameterSet.h"
#include "DZSCore/AdcCodeHelper.h"

using std::string;
using std::ostream;
using std::endl;

typedef ZeroSuppressBase::Index        Index;
typedef ZeroSuppressBase::Signal       Signal;
typedef AdcCodeHelper::FloatSignal     FloatSignal;
typedef ZeroSuppressBase::SignalVector SignalVector;
typedef ZeroSuppressBase::ResultVector ResultVector;

//**********************************************************************

ZeroSuppressFixedService::
ZeroSuppressFixedService(const fhicl::ParameterSet&, art::ActivityRegistry&) {
}
  
//**********************************************************************

ZeroSuppressFixedService::ZeroSuppressFixedService() { }

//**********************************************************************

int ZeroSuppressFixedService::
filter(const SignalVector& sigs, Channel, FloatSignal&, ResultVector& keep) const {
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

DEFINE_ART_SERVICE_INTERFACE_IMPL(ZeroSuppressFixedService, ZeroSuppressBase)

//**********************************************************************
