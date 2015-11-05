// test_CompressReplace.cxx

// David Adams
// September 2015
//
// Test script for Range.

#include "DZSCore/CompressReplace.h"

#include <string>
#include <iostream>
#include <cassert>
#include <iomanip>

using std::string;
using std::cout;
using std::endl;
using std::setw;

int main() {
  const string myname = "test_CompressReplace: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  cout << myname << "Creating suppressor." << endl;
  CompressReplace::Signal zero = 999;
  CompressReplace zs(zero);
  zs.print();
  cout << myname << "Checking zero." << endl;
  assert( zs.zero() == zero );
  cout << myname << "Compressing." << endl;
  CompressReplace::SignalVector indata = { 1,  0, -2, -1,  1,  1,  -1,  3, -1,  2,
                                           6, 12, 21, 17, 12,  7,  5,  2,  1, -1,
                                           2,  1,  1, -1, -1, -2,  3,  1,  1, -1 };
  CompressReplace::SignalVector outdata(indata);
  CompressReplace::FilterVector keep(30, false);
  unsigned int isig = 0;
  for ( isig= 6; isig<28; ++isig ) keep[isig] = true;
  cout << myname << "Compressing." << endl;
  assert( zs.compress(outdata, keep) == 0 );
  for ( isig=0; isig<indata.size(); ++isig ) {
    cout << setw(3) << isig << ":" << setw(6) << indata[isig] << setw(6) << outdata[isig] << endl;
  }
  cout << myname << "Checking suppressing signal." << endl;
  for ( isig= 0; isig< 6; ++isig ) { cout << " " << isig; assert( outdata[isig] == zero ); }
  for ( isig= 6; isig<28; ++isig ) { cout << " " << isig; assert( outdata[isig] == indata[isig] );  }
  for ( isig=28; isig<30; ++isig ) { cout << " " << isig; assert( outdata[isig] == zero ); }
  cout << endl;
  cout << myname << "Done." << endl;
  return 0;
}
