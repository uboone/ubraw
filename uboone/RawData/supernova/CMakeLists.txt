include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
	
find_ups_product( uboonedaq_datatypes )
  

art_make( NO_PLUGINS 
          LIBRARY_NAME uboone_RawData_supernova
          LIB_LIBRARIES lardata_Utilities_DatabaseUtil_service
                        larcorealg_Geometry
                        larcore_Geometry_Geometry_service
                        lardata_Utilities
                        uboone_Geometry
                        UBOpReadoutMap_service
                        lardataobj_OpticalDetectorData
                        lardataobj_RawData
                        larcoreobj_SummaryData
                        lardata_ArtDataHelper
                        # uboone_RawData
                        # LArRawInputSourceUBooNE_source
                        ${UBDut}
                        ${PQ}
                        ${Boost_SERIALIZATION_LIBRARY}
                        ${ART_FRAMEWORK_CORE}
                        ${ART_FRAMEWORK_IO_SOURCES}
                        ${ART_FRAMEWORK_PRINCIPAL}
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
                        art_Persistency_Common 
                        art_Persistency_Provenance 
                        art_Utilities 
                        ${MF_MESSAGELOGGER}
                        
                        ${FHICLCPP}
                        ${CETLIB}
                        ${ROOT_GEOM}
                        ${ROOT_XMLIO}
                        ${ROOT_GDML}
                        ${Boost_SYSTEM_LIBRARY}
                        ${ROOT_BASIC_LIB_LIST}

          MODULE_LIBRARIES lardata_Utilities_DatabaseUtil_service
                        larcorealg_Geometry
                        larcore_Geometry_Geometry_service
                        lardata_Utilities
                        uboone_Geometry
                        UBOpReadoutMap_service
                        lardataobj_OpticalDetectorData
                        lardataobj_RawData
                        lardataobj_RecoBase
                        lardata_RecoBaseArt
                        larcoreobj_SummaryData
                        # uboone_RawData
                        # uboone_RawData_utils
                        ${UBDut}
                        ${PQ}
                        ${Boost_SERIALIZATION_LIBRARY}
                        ${ART_FRAMEWORK_CORE}
                        ${ART_FRAMEWORK_IO_SOURCES}
                        ${ART_FRAMEWORK_PRINCIPAL}
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
                        art_Persistency_Common 
                        art_Persistency_Provenance
                        art_Utilities 
                        ${MF_MESSAGELOGGER}
                        
                        ${FHICLCPP}
                        ${CETLIB}
                        ${ROOT_GEOM}
                        ${ROOT_XMLIO}
                        ${ROOT_GDML}
                        ${Boost_SYSTEM_LIBRARY}
                        ${ROOT_BASIC_LIB_LIST}
        )		


simple_plugin( SnFileSource
               "source"
               uboone_RawData_supernova
               UBOpReadoutMap_service
               ${ART_FRAMEWORK_PERSISTENCY_COMMON}
               BASENAME_ONLY
             )


# temporary workaround until art v1_12_01 is available
# set_target_properties(supernova_dict supernova_map
#                       PROPERTIES COMPILE_FLAGS -fno-var-tracking)

install_headers()
install_fhicl()
# install_source()
# install_scripts()
