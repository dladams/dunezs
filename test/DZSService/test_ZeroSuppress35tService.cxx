// test_ZeroSuppress35tService.cxx
//
// David Adams
// September 2015
//
// This test demonstrates how to configure and use the LArSoft DetectorProperties
// service outside the art framework.

#include "DZSService/ZeroSuppress35tService.h"

#include <string>
#include <iostream>
#include <iomanip>
#include "AXService/ArtServiceHelper.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

using std::string;
using std::cout;
using std::endl;
using std::setw;

int test_ZeroSuppress35tService() {
  const string myname = "test_ZeroSuppress35tService: ";
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

  cout << myname << line << endl;
  cout << myname << "Add the ZeroSuppress35t service." << endl;
  scfg = "Zero: 0.0 TL: 10.0 TD: 3.0 NL: 5 ND: 5 NT: 6";
  cout << myname << "Configuration: " << scfg << endl;
  assert( ash.addService("ZeroSuppress35tService", scfg) == 0 );

  cout << myname << line << endl;
  cout << myname << "Load the services." << endl;
  assert( ash.loadServices() == 1 );
  ash.print();

  cout << myname << line << endl;
  cout << myname << "Get the service." << endl;
  art::ServiceHandle<ZeroSuppress35tService> pzs;
  pzs->print();

  cout << myname << "Suppressing." << endl;
  ZeroSuppress35t::SignalVector indata = { 1,  0, -2, -1,  1,  1,  -1,  3, -1,  2,
                                           6, 12, 21, 17, 12,  7,  5,  2,  1, -1,
                                           2,  1,  1, -1, -1, -2,  3,  1,  1, -1 };
  ZeroSuppress35t::SignalVector outdata(indata);
  assert( pzs->filter(outdata) == 0 );
  for ( unsigned int idat=0; idat<indata.size(); ++idat ) {
    cout << setw(6) << indata[idat] << setw(6) << outdata[idat] << endl;
  }
  unsigned int isig = 0;
  ZeroSuppress35t::Signal zero = 0;
  for ( isig= 0; isig< 6; ++isig ) assert( outdata[isig] == zero );
  for ( isig= 6; isig<28; ++isig ) assert( outdata[isig] == indata[isig] );
  for ( isig=28; isig<30; ++isig ) assert( outdata[isig] == zero );
  cout << myname << "Done." << endl;

  return 0;
}

int main() {
  return test_ZeroSuppress35tService();
}

