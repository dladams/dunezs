# dunezs/test/DZSCore/CMakeLists.txt
#
# David Adams
# November 2015

cet_test(test_SignalTypeConverter SOURCES test_SignalTypeConverter.cxx
  LIBRARIES DZSCore
)
cet_test(test_AdcCodeHelper SOURCES test_AdcCodeHelper.cxx
  LIBRARIES DZSCore
)
