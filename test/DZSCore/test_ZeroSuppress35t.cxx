// test_ZeroSuppress35t.cxx

// David Adams
// September 2015
//
// Test script for Range.

#include "DZSCore/ZeroSuppress35t.h"

#include <string>
#include <iostream>
#include <cassert>
#include <iomanip>

using std::string;
using std::cout;
using std::endl;
using std::setw;

int main() {
  const string myname = "test_ZeroSuppress35t: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  cout << myname << "Checking zero." << endl;
  ZeroSuppress35t::Signal zero = 0;
  assert( ZeroSuppress35t::zero() == zero );
  cout << myname << "Creating suppressor." << endl;
  ZeroSuppress35t zs(10, 3, 5, 5, 6);
  zs.print();
  cout << myname << "Suppressing." << endl;
  ZeroSuppress35t::SignalArray indata = { 1,  0, -2, -1,  1,  1,  -1,  3, -1,  2,
                                          6, 12, 21, 17, 12,  7,  5,  2,  1, -1,
                                          2,  1,  1, -1, -1, -2,  3,  1,  1, -1 };
  ZeroSuppress35t::SignalArray outdata(indata);
  assert( zs.filter(outdata) == 0 );
  for ( unsigned int idat=0; idat<indata.size(); ++idat ) {
    cout << setw(6) << indata[idat] << setw(6) << outdata[idat] << endl;
  }
  unsigned int isig = 0;
  for ( isig= 0; isig< 6; ++isig ) assert( outdata[isig] == zero );
  for ( isig= 6; isig<28; ++isig ) assert( outdata[isig] == indata[isig] );
  for ( isig=28; isig<30; ++isig ) assert( outdata[isig] == zero );
  cout << myname << "Done." << endl;
  return 0;
}
