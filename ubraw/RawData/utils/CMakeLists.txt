#
simple_plugin( LArRawInputSourceUBooNE
               "source"
               ubraw_RawData
               UBOpReadoutMap_service
               ${ART_FRAMEWORK_PERSISTENCY_COMMON}
               BASENAME_ONLY
             )

simple_plugin( RawDigitWriter "module"
               UBOpReadoutMap_service
               lardata_Utilities_DatabaseUtil_service
               lardataobj_RawData
               ${ART_FRAMEWORK_CORE}
               ${ART_FRAMEWORK_IO_SOURCES}
               ${ART_FRAMEWORK_PRINCIPAL}
               ${ART_FRAMEWORK_SERVICES_REGISTRY}
               ${ART_ROOT_IO_TFILE_SUPPORT} ${ROOT_CORE}
               ${ART_ROOT_IO_TFILESERVICE_SERVICE}
               art_Persistency_Common canvas
               art_Persistency_Provenance canvas
               art_Utilities canvas
               ${MF_MESSAGELOGGER}
               ROOT::Tree
               BASENAME_ONLY
             )

# need a special link list because of the explicit link to a specific service inplementation
simple_plugin( TriggerEnforcement "module"
                   lardata_DetectorInfoServices_DetectorClocksServiceStandard_service # BUG note that we link to a specific service implementation
                   lardataobj_RawData
                   ubraw_RawData
                   ${ART_FRAMEWORK_CORE}
                   ${ART_FRAMEWORK_PRINCIPAL}
                   ${ART_FRAMEWORK_SERVICES_REGISTRY}
                   ${ART_ROOT_IO_TFILE_SUPPORT} ${ROOT_CORE}
                   art_Persistency_Common canvas
                   art_Persistency_Provenance canvas
                   art_Utilities canvas
                   ${MF_MESSAGELOGGER}
                   ${FHICLCPP}
                   cetlib cetlib_except
                   BASENAME_ONLY
             )


art_make( EXCLUDE LArRawInputSourceUBooNE_source.cc RawDigitWriter_module.cc TriggerEnforcement_module.cc
          BASENAME_ONLY
          LIBRARY_NAME ubraw_RawData
          LIB_LIBRARIES ubobj_RawData
	  		ubobj_Trigger
	  		lardata_Utilities_DatabaseUtil_service
                        larcorealg_Geometry
                        larcore_Geometry_Geometry_service
                        lardata_Utilities
                        ubcore_Geometry
                        UBOpReadoutMap_service
                        lardataobj_OpticalDetectorData
                        lardataobj_RawData
                        larcoreobj_SummaryData
                        UB_DATA_TYPES
                        PQ
                        ${Boost_SERIALIZATION_LIBRARY}
                        ${ART_FRAMEWORK_CORE}
                        ${ART_FRAMEWORK_IO_SOURCES}
                        ${ART_FRAMEWORK_PRINCIPAL}
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        ${ART_ROOT_IO_TFILE_SUPPORT} ${ROOT_CORE}
                        ${ART_ROOT_IO_TFILESERVICE_SERVICE}
                        art_Persistency_Common canvas
                        art_Persistency_Provenance canvas
                        art_Utilities canvas
                        ${MF_MESSAGELOGGER}
                        ROOT::Tree
                        ${FHICLCPP}
                        cetlib cetlib_except
                        ${ROOT_GEOM}
                        ${ROOT_XMLIO}
                        ${ROOT_GDML}
          MODULE_LIBRARIES lardata_Utilities_DatabaseUtil_service
                        larcorealg_Geometry
                        larcore_Geometry_Geometry_service
                        lardata_Utilities
                        ubcore_Geometry
                        UBOpReadoutMap_service
                        lardataobj_OpticalDetectorData
                        lardataobj_RawData
                        larcoreobj_SummaryData
                        ubraw_RawData
                        UB_DATA_TYPES
                        PQ
                        ${Boost_SERIALIZATION_LIBRARY}
                        ${ART_FRAMEWORK_CORE}
                        ${ART_FRAMEWORK_IO_SOURCES}
                        ${ART_FRAMEWORK_PRINCIPAL}
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        ${ART_ROOT_IO_TFILE_SUPPORT} ${ROOT_CORE}
                        ${ART_ROOT_IO_TFILESERVICE_SERVICE}
                        art_Persistency_Common canvas
                        art_Persistency_Provenance canvas
                        art_Utilities canvas
                        ${MF_MESSAGELOGGER}
                        ${FHICLCPP}
                        cetlib cetlib_except
                        ${ROOT_GEOM}
                        ${ROOT_XMLIO}
                        ${ROOT_GDML}
        )                                                           

install_headers()
install_source()

add_subdirectory(job)
