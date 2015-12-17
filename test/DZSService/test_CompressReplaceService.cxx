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

//**********************************************************************

string line() {
  return "----------------------------------";
}
//**********************************************************************

int test_CompressReplaceService(const CompressReplaceService& cr) {
  const string myname = "test_CompressReplaceService: ";
  cout << myname << line() << endl;
  cout << myname << "Compressing." << endl;
  AdcCountVector indata = { 1,  0, -2, -1,  1,  1,  -1,  3, -1,  2,
                            6, 12, 21, 17, 12,  7,  5,  2,  1, -1,
                            2,  1,  1, -1, -1, -2,  3,  1,  1, -1 };
  AdcCountVector outdata(indata);
  AdcFilterVector keep(30, false);
  unsigned int isig = 0;
  float offset = 20.0;
  for ( isig= 6; isig<28; ++isig ) keep[isig] = true;
  raw::Compress_t comp = raw::kHuffman;
  assert( cr.compress(outdata, keep, offset, comp) == 0 );
  assert( comp == raw::kNone );
  for ( unsigned int idat=0; idat<indata.size(); ++idat ) {
    cout << setw(6) << indata[idat] << setw(6) << outdata[idat] << endl;
  }
  AdcCount zero = 0;
  for ( isig= 0; isig< 6; ++isig ) assert( outdata[isig] == zero+offset );
  for ( isig= 6; isig<28; ++isig ) assert( outdata[isig] == indata[isig] );
  for ( isig=28; isig<30; ++isig ) assert( outdata[isig] == zero+offset );
  cout << myname << line() << endl;
  cout << myname << "Done." << endl;
  return 0;
}

//**********************************************************************

int test_CompressReplaceService_as_service() {
  const string myname = "test_CompressReplaceService_as_service: ";
  cout << myname << "Starting test" << endl;
  string scfg;
  cout << myname << line() << endl;
  cout << myname << "Fetch art service helper." << endl;
  ArtServiceHelper& ash = ArtServiceHelper::instance();

  cout << myname << line() << endl;
  cout << myname << "Add the CompressReplace service." << endl;
  scfg += "service_provider: \"CompressReplaceService\"";
  scfg += "\n";
  scfg += "Zero: 0";
  cout << myname << "Configuration: " << scfg << endl;
  assert( ash.addService("AdcCompressService", scfg) == 0 );

  cout << myname << line() << endl;
  cout << myname << "Load the services." << endl;
  assert( ash.loadServices() == 1 );
  ash.print();

  cout << myname << line() << endl;
  cout << myname << "Get the service." << endl;
  art::ServiceHandle<CompressReplaceService> pcr;
  pcr->print();
  return test_CompressReplaceService(*pcr);
}

//**********************************************************************

int test_CompressReplaceService_as_utility() {
  const string myname = "test_CompressReplaceService_as_utility: ";
  cout << myname << "Starting test" << endl;
  CompressReplaceService cr(0);
  return test_CompressReplaceService(cr);
}

//**********************************************************************

int test_CompressReplaceService() {
  int stat = 0;
  cout << "\nTesting as service." << endl;
  stat += test_CompressReplaceService_as_service();
  cout << "\nTesting as utility." << endl;
  stat += test_CompressReplaceService_as_utility();
  return stat;
}

//**********************************************************************

int main() {
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  return test_CompressReplaceService();
}

//**********************************************************************
