// CompressReplace.cxx

#include "CompressReplace.h"
#include <vector>
#include <cassert>

using std::string;
using std::ostream;
using std::endl;
using std::vector;

typedef CompressReplace::Signal Signal;
typedef CompressReplace::SignalVector SignalVector;
typedef CompressReplace::FilterVector FilterVector;

//**********************************************************************

CompressReplace::CompressReplace(Signal azero)
: m_zero(azero) { }

//**********************************************************************

Signal CompressReplace::zero() const {
  return m_zero;
}

//**********************************************************************

int CompressReplace::
compress(SignalVector& sigs, const FilterVector& keep, Signal offset) const {
  for ( unsigned int isig=0; isig<sigs.size(); ++isig ) {
    if ( ! keep[isig] ) sigs[isig] = zero() + offset;
  }
  return 0;
}

//**********************************************************************

ostream& CompressReplace::print(ostream& out, string prefix) const {
  out << prefix << "Zero = " << m_zero << endl;
  return out;
}

//**********************************************************************
