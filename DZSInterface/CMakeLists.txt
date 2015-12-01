# dunezs/DZSCore/CMakeLists.txt
#
# David Adams
# November 2015
#
# CMake instructions for the DZSINterface library which holds
# service interface classes.

art_make(BASENAME_ONLY
  LIB_LIBRARIES
    ${Boost_SYSTEM_LIBRARY}
    ${ART_FRAMEWORK_SERVICES_REGISTRY}
)

install_headers()
install_source( README.txt )
