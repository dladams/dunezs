// test_AdcCodeHelper.cxx

// David Adams
// September 2015
//
// Test script for Range.

#include "DZSCore/AdcCodeHelper.h"

#include <string>
#include <iostream>
#include <cassert>
#include <iomanip>
#include <cmath>

using std::string;
using std::cout;
using std::endl;
using std::setw;

bool checkdiff(float x1, float x2) {
  if ( x1 == 0.0 && x2 == 0.0 ) return true;
  float drat = 2.0*fabs(x1-x2)/(fabs(x1) + fabs(x2));
  if ( drat < 2.e-6 ) return true;
  cout << "drat = " << drat << endl;
  return false;
}

int main() {
  const string myname = "test_AdcCodeHelper: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  cout << myname << "Creating helper." << endl;
  AdcCodeHelper ach(64);
  cout << myname << "Test sticky bits." << endl;
  assert( ach.hasStickyBits(127) );
  assert( ach.hasStickyBits(128) );
  assert( !ach.hasStickyBits(129) );
  cout << myname << "Test is small." << endl;
  assert(  ach.isSmall(163, 100.0));
  assert(  ach.isSmall(164, 100.1));
  assert( !ach.isSmall(164, 100.0));
  assert( !ach.isSmall(164,  99.9));
  assert( !ach.isSmall(165, 100.0));
  cout << myname << "Test subtract." << endl;
  assert( checkdiff(ach.subtract(140, 40.0), 100.0) );
  assert( checkdiff(ach.subtract( 80, 55.3),  24.7) );
  cout << myname << "Done." << endl;
  assert( ach.intSubtract(140, 40.0) == 100 );
  assert( ach.intSubtract(80, 55.3) == 25 );
  return 0;
}
