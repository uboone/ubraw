include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
	
find_ups_product( uboonedaq_datatypes )
  
# find_ups_product( boost v1_61_0 )
# cet_find_library( BOOSER NAMES boost_serialization PATHS ENV BOOST_LIB NO_DEFAULT_PATH )
#
# find_ups_product( uboonedaq_datatypes v6_20_00_sn )
# cet_find_library( DATATYPES NAMES libubdata_types PATHS ENV UBOONEDAQ_DATATYPES_LIB NO_DEFAULT_PATH )
# include_directories( ${UBOONEDAQ_DATATYPES_DIR}/include )



art_make( NO_PLUGINS 
          # BASENAME_ONLY
          LIBRARY_NAME uboone_RawData_supernova
          LIB_LIBRARIES lardata_Utilities_DatabaseUtil_service
                        larcore_Geometry
                        larcore_Geometry_Geometry_service
                        lardata_Utilities
                        uboone_Geometry
                        UBOpReadoutMap_service
                        lardataobj_OpticalDetectorData
                        lardataobj_RawData
                        larcoreobj_SummaryData
                        lardata_ArtDataHelper
                        uboone_RawData
                        LArRawInputSourceUBooNE_source
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
                        ${MF_UTILITIES}
                        ${FHICLCPP}
                        ${CETLIB}
                        ${ROOT_GEOM}
                        ${ROOT_XMLIO}
                        ${ROOT_GDML}
                        ${Boost_SYSTEM_LIBRARY}
                        ${ROOT_BASIC_LIB_LIST}

          MODULE_LIBRARIES lardata_Utilities_DatabaseUtil_service
                        larcore_Geometry
                        larcore_Geometry_Geometry_service
                        lardata_Utilities
                        uboone_Geometry
                        UBOpReadoutMap_service
                        lardataobj_OpticalDetectorData
                        lardataobj_RawData
                        lardataobj_RecoBase
                        lardata_RecoBaseArt
                        larcoreobj_SummaryData
                        uboone_RawData
                        uboone_RawData_utils                        
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
                        ${MF_UTILITIES}
                        ${FHICLCPP}
                        ${CETLIB}
                        ${ROOT_GEOM}
                        ${ROOT_XMLIO}
                        ${ROOT_GDML}
                        ${Boost_SYSTEM_LIBRARY}
                        ${ROOT_BASIC_LIB_LIST}
        )		

# Gonna kill this anyway
# simple_plugin( SnAssemblerSource
#                "source"
#                uboone_RawData_supernova
#                BASENAME_ONLY
#              )

simple_plugin( SnFileSource
               "source"
               uboone_RawData_supernova
               BASENAME_ONLY
             )


simple_plugin( SnHitFinder
               "module"
               uboone_RawData_supernova
               BASENAME_ONLY
             )

simple_plugin( SnHitCompare
              "module"
               uboone_RawData_supernova
               BASENAME_ONLY
             )


# temporary workaround until art v1_12_01 is available
# set_target_properties(supernova_dict supernova_map
#                       PROPERTIES COMPILE_FLAGS -fno-var-tracking)

install_headers()
install_fhicl()
# install_source()
# install_scripts()
