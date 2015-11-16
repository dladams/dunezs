# dunezs/test/DZSService/CMakeLists.txt
#
# David Adams
# November 2015

cet_test(test_ZeroSuppress35tService SOURCES test_ZeroSuppress35tService.cxx
  LIBRARIES ZeroSuppress35tService_service AXService DZSCore
)

cet_test(test_ZeroSuppress35tLegacyService SOURCES test_ZeroSuppress35tLegacyService.cxx
  LIBRARIES ZeroSuppress35tLegacyService_service AXService DZSCore
)

cet_test(test_CompressReplaceService SOURCES test_CompressReplaceService.cxx
  LIBRARIES CompressReplaceService_service AXService DZSCore
)
