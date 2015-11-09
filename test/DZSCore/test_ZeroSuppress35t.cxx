// test_ZeroSuppress35t.cxx

// David Adams
// September 2015
//
// Test script for Range.

#include "DZSCore/ZeroSuppress35t.h"

#include <string>
#include <iostream>
#include <sstream>
#include <cassert>
#include <iomanip>

using std::string;
using std::cout;
using std::endl;
using std::istringstream;
using std::setw;

int test_ZeroSuppress35t(int dbg) {
  const string myname = "test_ZeroSuppress35t: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  cout << myname << "Creating suppressor." << endl;
  ZeroSuppress35t zs(1, 10, 3, 4, 5, 5, 6);
  zs.setDebug(dbg);
  zs.print();
  cout << myname << "Suppressing." << endl;
  ZeroSuppress35t::SignalVector indata = { 1,  0, -2, -1,  1,  1,  -1,  3, -1,  2,
                                           6, 12, 21, 17, 12,  7,  5,  2,  1, -1,
                                           2,  1,  1, -1, -1, -2,  3,  1,  1, -1 };
  ZeroSuppress35t::ResultVector keep;
  cout << myname << "Signals:" << endl;
  assert( zs.filter(indata, keep) == 0 );
  for ( unsigned int idat=0; idat<indata.size(); ++idat ) {
    cout << setw(3) << idat << ":" << setw(6) << indata[idat] << setw(6) << keep[idat] << endl;
  }
  cout << myname << "Checking suppressing signal." << endl;
  unsigned int isig = 0;
  for ( isig= 0; isig< 6; ++isig ) { cout << " " << isig; assert( ! keep[isig] ); }
  for ( isig= 6; isig<28; ++isig ) { cout << " " << isig; assert( keep[isig] );  }
  for ( isig=28; isig<30; ++isig ) { cout << " " << isig; assert( ! keep[isig] ); }
  cout << endl;
  cout << myname << "Done." << endl;
  return 0;
}

int main(int argc, char** argv) {
  int dbg = 0;
  if ( argc > 1 ) {
    istringstream ssdbg(argv[1]);
    ssdbg >> dbg;
  }
  return test_ZeroSuppress35t(dbg);
}
