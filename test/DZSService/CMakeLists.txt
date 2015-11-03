# dunezs/test/DZSService/CMakeLists.txt
#
# David Adams
# November 2015

cet_test(test_ZeroSuppress35tService SOURCES test_ZeroSuppress35tService.cxx
  LIBRARIES ZeroSuppress35tService_service AXService DZSCore
)
