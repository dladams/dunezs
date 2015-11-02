# dunezs/DZSCore/CMakeLists.txt
#
# David Adams
# November 2015
#
# CMake instructions for the DZSCore library which holds
# core classes for zero supression.

art_make(BASENAME_ONLY
  LIB_LIBRARIES
    ${CETLIB}
)

install_headers()
install_source( README.txt )
