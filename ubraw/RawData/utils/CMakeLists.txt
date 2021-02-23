cet_find_library( LIBWDA NAMES wda PATHS ENV LIBWDA_LIB NO_DEFAULT_PATH )
cet_find_library( SQLITE NAMES sqlite3_ups PATHS ENV SQLITE_LIB NO_DEFAULT_PATH )
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
               ${ROOT_BASIC_LIB_LIST}
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
                   ${ROOT_BASIC_LIB_LIST}
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
			${UBDut}
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
			${LIBWDA}
			${SQLITE}
                        ${MF_MESSAGELOGGER}
                        
                        ${FHICLCPP}
                        cetlib cetlib_except
                        ${ROOT_GEOM}
                        ${ROOT_XMLIO}
                        ${ROOT_GDML}
                        ${Boost_SYSTEM_LIBRARY}
                        ${ROOT_BASIC_LIB_LIST}

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
			${UBDut}
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
                        ${Boost_SYSTEM_LIBRARY}
                        ${ROOT_BASIC_LIB_LIST}
        )                                                           

install_headers()
install_source()

add_subdirectory(job)
