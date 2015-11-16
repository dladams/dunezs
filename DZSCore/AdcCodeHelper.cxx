// AdcCodeHelper.cxx

#include "DZSCore/AdcCodeHelper.h"
#include <cmath>

typedef AdcCodeHelper::Signal Signal;
typedef AdcCodeHelper::FloatSignal FloatSignal;

//**********************************************************************

AdcCodeHelper::AdcCodeHelper(Signal aSuppressedSignalMax)
: m_SuppressedSignalMax(aSuppressedSignalMax) { }

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
  return std::abs(pedsignal) < m_SuppressedSignalMax;
}

//**********************************************************************

Signal AdcCodeHelper::intSignal(FloatSignal fsig) const {
  if ( fsig > 0.1 ) return Signal(fsig + 0.5);
  if ( fsig < -0.1 ) return -Signal(-fsig + 0.5);
  return 0;
}

//**********************************************************************

FloatSignal AdcCodeHelper::subtract(Signal sig, FloatSignal ped) const {
  return sig - ped;
}

//**********************************************************************

Signal AdcCodeHelper::intSubtract(Signal sig, FloatSignal ped) const {
  return intSignal(subtract(sig, ped));
}

//**********************************************************************
