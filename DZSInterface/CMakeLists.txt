# dunezs/DZSCore/CMakeLists.txt
#
# David Adams
# December 2015
#
# CMake instructions for service interface classes.

# No need to make a library for these.
#art_make(BASENAME_ONLY
#  LIB_LIBRARIES
#    ${Boost_SYSTEM_LIBRARY}
#    ${ART_FRAMEWORK_SERVICES_REGISTRY}
#)

install_headers()
install_source( README.txt )
