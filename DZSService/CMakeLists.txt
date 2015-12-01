# dunezs/DZSService/CMakeLists.txt
#
# David Adams
# November 2015
#
# CMake instructions for dunezs services.

# Build the library excluding services.
#art_make(NO_PLUGINS
#         LIBRARY_NAME DZSService
#         LIB_LIBRARIES
##          ZeroSuppression35t_service
#           DZSCore
#        )

#art_make( BASENAME_ONLY
#          SERVICE_LIBRARIES
#            DZSCore
#            Geometry_service
#            ${FHICLCPP}
#)

# Add plugin for each service.
simple_plugin(ZeroSuppress35tService        "service" DZSCore BASENAME_ONLY)
simple_plugin(ZeroSuppress35tLegacyService  "service" DZSCore BASENAME_ONLY)
simple_plugin(ZeroSuppressFixedService      "service" DZSCore BASENAME_ONLY)
simple_plugin(CompressReplaceService        "service" DZSCore BASENAME_ONLY)
simple_plugin(SimChannelExtract35tService "service"
                DZSCore DZSInterface
                Geometry Simulation Utilities
                ${ART_UTILITIES}
                ${ROOT_BASIC_LIB_LIST} BASENAME_ONLY
             )
