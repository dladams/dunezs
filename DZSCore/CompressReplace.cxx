// CompressReplace.cxx

#include "CompressReplace.h"
#include <vector>
#include <cassert>

using std::string;
using std::ostream;
using std::endl;
using std::vector;

typedef CompressReplace::FilterVector FilterVector;

//**********************************************************************

CompressReplace::CompressReplace(AdcCount azero)
: m_zero(azero) { }

//**********************************************************************

AdcCount CompressReplace::zero() const {
  return m_zero;
}

//**********************************************************************

int CompressReplace::
compress(AdcCountVector& sigs, const FilterVector& keep, AdcCount offset) const {
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
