# dunezs/test/DZSService/CMakeLists.txt
#
# David Adams
# November 2015

cet_test(test_Dune35tZeroSuppressService SOURCES test_Dune35tZeroSuppressService.cxx
  LIBRARIES dune_DetSim_Service_Dune35tZeroSuppressService_service AXService DetSim
)

cet_test(test_Legacy35tZeroSuppressService SOURCES test_Legacy35tZeroSuppressService.cxx
  LIBRARIES dune_DetSim_Service_Legacy35tZeroSuppressService_service AXService DetSim
)

cet_test(test_ReplaceCompressService SOURCES test_ReplaceCompressService.cxx
  LIBRARIES dune_DetSim_Service_ReplaceCompressService_service AXService
)
