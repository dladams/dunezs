# dunezs/DZSService/CMakeLists.txt
#
# David Adams
# November 2015
#
# CMake instructions for dunze services.

#message("XXXXXXXXXX: " ${DUNEZS})
# Build the library.
#art_make(NO_PLUGINS LIBRARY_NAME DZSService
#         LIB_LIBRARIES
#           ZeroSuppression35t_service
#           DZSCore
#        )

# Add plugin for each service.
simple_plugin(ZeroSuppress35tService "service" DZSCore BASENAME_ONLY)
simple_plugin(CompressReplaceService "service" DZSCore BASENAME_ONLY)
