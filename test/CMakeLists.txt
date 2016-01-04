# dunezs/test/CMakeLists.txt

include(CetTest)
cet_enable_asserts()

# 04jan2016: Temorarily disable these tests because utility and
# service headers are not installed for dunetpc v04_31_00_01.
#add_subdirectory( DZSCore )
#add_subdirectory( DZSService )
