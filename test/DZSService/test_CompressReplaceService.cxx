// test_CompressReplaceService.cxx
//
// David Adams
// September 2015
//
// This test demonstrates how to configure and use the LArSoft DetectorProperties
// service outside the art framework.

#include "DZSService/CompressReplaceService.h"

#include <string>
#include <iostream>
#include <iomanip>
#include "AXService/ArtServiceHelper.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

using std::string;
using std::cout;
using std::endl;
using std::setw;

int test_CompressReplaceService() {
  const string myname = "test_CompressReplaceService: ";
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
  cout << myname << "Add the CompressReplace service." << endl;
  scfg = "Zero: 0";
  cout << myname << "Configuration: " << scfg << endl;
  assert( ash.addService("CompressReplaceService", scfg) == 0 );

  cout << myname << line << endl;
  cout << myname << "Load the services." << endl;
  assert( ash.loadServices() == 1 );
  ash.print();

  cout << myname << line << endl;
  cout << myname << "Get the service." << endl;
  art::ServiceHandle<CompressReplaceService> pcr;
  pcr->print();

  cout << myname << "Compressing." << endl;
  CompressReplace::SignalVector indata = { 1,  0, -2, -1,  1,  1,  -1,  3, -1,  2,
                                           6, 12, 21, 17, 12,  7,  5,  2,  1, -1,
                                           2,  1,  1, -1, -1, -2,  3,  1,  1, -1 };
  CompressReplace::SignalVector outdata(indata);
  CompressReplace::FilterVector keep(30, false);
  unsigned int isig = 0;
  float offset = 20.0;
  for ( isig= 6; isig<28; ++isig ) keep[isig] = true;
  assert( pcr->compress(outdata, keep, offset) == 0 );
  for ( unsigned int idat=0; idat<indata.size(); ++idat ) {
    cout << setw(6) << indata[idat] << setw(6) << outdata[idat] << endl;
  }
  CompressReplace::Signal zero = 0;
  for ( isig= 0; isig< 6; ++isig ) assert( outdata[isig] == zero+offset );
  for ( isig= 6; isig<28; ++isig ) assert( outdata[isig] == indata[isig] );
  for ( isig=28; isig<30; ++isig ) assert( outdata[isig] == zero+offset );
  cout << myname << "Done." << endl;

  return 0;
}

int main() {
  return test_CompressReplaceService();
}

