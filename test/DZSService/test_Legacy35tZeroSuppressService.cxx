// test_Legacy35tZeroSuppressService.cxx
//
// David Adams
// September 2015
//
// This test demonstrates how to configure and use the LArSoft DetectorProperties
// service outside the art framework.

#include "dune/DetSim/Service/Legacy35tZeroSuppressService.h"

#include <string>
#include <iostream>
#include <iomanip>
#include "AXService/ArtServiceHelper.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

using std::string;
using std::cout;
using std::endl;
using std::setw;

int test_Legacy35tZeroSuppressService() {
  const string myname = "test_Legacy35tZeroSuppressService: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  string line = "-----------------------------";
  string scfg;

  cout << myname << line << endl;
  cout << myname << "Fetch art service helper." << endl;
  ArtServiceHelper& ash = ArtServiceHelper::instance();

  // Add the ZS service.
  // Note the service is added with it base name while the full type is included
  // along with its parameters in the configuration.
  cout << myname << line << endl;
  cout << myname << "Add the ZeroSuppress35tLegacy service." << endl;
  scfg = "service_provider: \"Legacy35tZeroSuppressService\"";
  scfg += " AdcThreshold: 5.0";
  scfg += " TickRange: 3";
  scfg += " SuppressStickyBits: true";
  scfg += " MinTickGap: 2";
  cout << myname << "Configuration: " << scfg << endl;
  assert( ash.addService("AdcSuppressService", scfg) == 0 );

  cout << myname << line << endl;
  cout << myname << "Load the services." << endl;
  assert( ash.loadServices() == 1 );
  ash.print();

  cout << myname << line << endl;
  cout << myname << "Get the service." << endl;
  art::ServiceHandle<Legacy35tZeroSuppressService> pzs;
  pzs->print();

  cout << myname << "Create ADC data." << endl;
  AdcCountVector indata = { 1,  0, -2, -1,  1,  1, -1,  3, -1,  2,
                            6, 12, 21, 17, 12,  7,  5,  2,  1, -1,
                            2,  1,  1, -1, -1, -2,  3,  1,  1, -1 };
  cout << myname << "Add pedestal." << endl;
  float ped = 211.2;
  short iped = short(ped);
  cout << "Pedestal: " << ped << endl;
  for ( AdcCount& data : indata ) data += iped;
  cout << myname << "Suppressing." << endl;
  AdcFilterVector keep;
  assert( pzs->filter(indata, 1234, ped, keep) == 0 );
  for ( unsigned int idat=0; idat<indata.size(); ++idat ) {
    cout << setw(4) << idat << setw(6) << indata[idat] << setw(6) << keep[idat] << endl;
  }
  unsigned int isig = 0;
  for ( ; isig< 7; ++isig ) assert( ! keep[isig] );
  for ( ; isig<19; ++isig ) assert( keep[isig] );
  for ( ; isig<30; ++isig ) assert( ! keep[isig] );
  cout << myname << "Done." << endl;

  return 0;
}

int main() {
  return test_Legacy35tZeroSuppressService();
}

