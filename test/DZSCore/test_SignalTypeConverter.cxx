// test_SignalTypeConverter.cxx

// David Adams
// September 2015
//
// Test script for Range.

#include "DZSCore/SignalTypeConverter.h"

#include <string>
#include <iostream>
#include <cassert>
#include <iomanip>
#include <cmath>

using std::string;
using std::cout;
using std::endl;
using std::setw;

// Check arbitrary conversion.
template<typename T1, typename T2>
int check(T1 v1, T2 v2) {
  cout << ".";
  SignalTypeConverter con;
  T2 conval = con.convert<T1,T2>(v1);
  if ( conval != v2 ) {
    cout << endl;
    cout << "  " << conval << " != " << v2 << " ! " << endl;
    assert(false);
    return 1;
  }
  return 0;
}

// Check float to int conversion.
template<typename T1, typename T2>
int checkfi(T1 v1, T2 v2) {
  cout << ".";
  SignalTypeConverter con;
  T2 conval = con.floatToInt(v1);
  if ( conval != v2 ) {
    cout << endl;
    cout << "  " << conval << " != " << v2 << " ! " << endl;
    assert(false);
    return 1;
  }
  return 0;
}

template<typename F, typename I>
int test_float_to_int(string lab) {
  const unsigned int nval = 9;
  F val1[nval] = {-1.0, 0.0, 1.0, 0.49, 0.50, 0.51, -0.49, -0.50, -0.51};
  I val2[nval] = {  -1,   0,   1,    0,    1,    1,     0,    -1,    -1};
  cout << "Checking " << lab << " ";
  for ( unsigned int ival=0; ival<nval; ++ival ) check(val1[ival], val2[ival]);
  for ( unsigned int ival=0; ival<nval; ++ival ) checkfi(val1[ival], val2[ival]);
  cout << endl;
  return 0;
}

template<typename I, typename F>
int test_int_to_float(string lab) {
  const unsigned int nval = 9;
  I val2[nval] = {-2,   -1,   0,   1,   2};
  F val1[nval] = {-2.0, -1.0, 0.0, 1.0, 2.0};
  cout << "Checking " << lab << " ";
  for ( unsigned int ival=0; ival<nval; ++ival ) check(val1[ival], val2[ival]);
  cout << endl;
  return 0;
}

template<typename I1, typename I2>
int test_int_to_int(string lab) {
  const unsigned int nval = 5;
  I1 val1[nval] = {-2, -1, 0, 1, 2};
  I1 val2[nval] = {-2, -1, 0, 1, 2};
  cout << "Checking " << lab << " ";
  for ( unsigned int ival=0; ival<nval; ++ival ) check(val1[ival], val2[ival]);
  cout << endl;
  return 0;
}

template<typename F1, typename F2>
int test_float_to_float(string lab) {
  const unsigned int nval = 9;
  F1 val1[nval] = {-1.0, 0.0, 1.0, 0.49, 0.50, 0.51, -0.49, -0.50, -0.51};
  F1 val2[nval] = {-1.0, 0.0, 1.0, 0.49, 0.50, 0.51, -0.49, -0.50, -0.51};
  cout << "Checking " << lab << " ";
  for ( unsigned int ival=0; ival<nval; ++ival ) check(val1[ival], val2[ival]);
  cout << endl;
  return 0;
}

int test_SignalTypeConverter() {
  const string myname = "test_SignalTypeConverter: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  test_float_to_int<double, int>("double to int");
  test_float_to_int<float, short>("float to short");
  test_int_to_float<int, float>("int to float");
  test_int_to_int<int, int>("int to int");
  test_float_to_float<float, float>("float to float");
  test_float_to_float<double, double>("double to double");
  return 0;
}

int main() {
  return test_SignalTypeConverter();
}
