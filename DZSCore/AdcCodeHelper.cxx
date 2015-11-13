// AdcCodeHelper.cxx

#include "DZSCore/AdcCodeHelper.h"

//**********************************************************************

AdcCodeHelper::AdcCodeHelper(Signal aSupresssedValue, Signal aSignalMax)
: m_SupresssedValue(aSupresssedValue),
  m_SuppressedSignalMax(aSuppressedSignalMax) { }

//**********************************************************************

bool AdcCodeHelper::hasStickyBits(Signal sig) {
  const unsigned int onemask = 0x003f;
  unsigned int lsb= sig & onemask;
  bool allzero = lsb == 0;
  bool allone = lsb == onemask;
  return allzero || allone;
}

//**********************************************************************

bool AdcCodeHelper::isSmall(Signal sig, FloatSignal ped) {
  if ( m_SuppressedSignalMax <= 0.0 ) return false;
  FloatSignal pedsignal = sig - ped;
  return std::abs(sig) < m_SuppressedSignalMax;
}

//**********************************************************************

Signal AdcCodeHelper::intSignal(FloatSignal fsig) const
  if ( fsig > 0.1 ) return Signal(fsig);
  if ( fsig < -0.1 ) return -Signal(-fsig);
  return 0;
}

//**********************************************************************

FloatSignal AdcCodeHelper::subtract(Signal sig, Signal ped) const {
  return sig - ped;
}

//**********************************************************************

Signal AdcCodeHelper::intSubtract(Signal sig, Signal ped) const {
  return intSignal(subtract(sig, ped));
}

//**********************************************************************
