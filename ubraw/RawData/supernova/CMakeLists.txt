include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
	
find_ups_product( uboonedaq_datatypes )
  

art_make( NO_PLUGINS 
          LIBRARY_NAME ubraw_RawData_supernova
          LIB_LIBRARIES lardata_Utilities_DatabaseUtil_service
                        larcorealg_Geometry
                        larcore_Geometry_Geometry_service
                        lardata_Utilities
                        ubcore_Geometry
                        ubcore::UBOpReadoutMap_service
                        lardataobj_OpticalDetectorData
                        lardataobj_RawData
                        larcoreobj_SummaryData
                        lardata_ArtDataHelper
                        # ubraw_RawData
                        # LArRawInputSourceUBooNE_source
                        UB_DATA_TYPES
                        PostgreSQL::PostgreSQL
                        Boost::serialization
                        art::Framework_Core
                        art::Framework_IO_Sources
                        art::Framework_Principal
                        art::Framework_Services_Registry
                        art_root_io::tfile_support ROOT::Core
                        art_root_io::TFileService_service
                        art::Persistency_Common canvas::canvas
                        art::Persistency_Provenance canvas::canvas
                        art::Utilities canvas::canvas
                        messagefacility::MF_MessageLogger
                        
                        fhiclcpp::fhiclcpp
                        cetlib::cetlib cetlib_except
                        ROOT::Geom
                        ROOT::XMLIO
                        ROOT::Gdml



          MODULE_LIBRARIES lardata_Utilities_DatabaseUtil_service
                        larcorealg_Geometry
                        larcore_Geometry_Geometry_service
                        lardata_Utilities
                        ubcore_Geometry
                        ubcore::UBOpReadoutMap_service
                        lardataobj_OpticalDetectorData
                        lardataobj_RawData
                        lardataobj_RecoBase
                        lardata_RecoBaseArt
                        larcoreobj_SummaryData
                        # ubraw_RawData
                        # ubraw_RawData_utils
                        UB_DATA_TYPES
                        PostgreSQL::PostgreSQL
                        Boost::serialization
                        art::Framework_Core
                        art::Framework_IO_Sources
                        art::Framework_Principal
                        art::Framework_Services_Registry
                        art_root_io::tfile_support ROOT::Core
                        art_root_io::TFileService_service
                        art::Persistency_Common canvas::canvas
                        art::Persistency_Provenance canvas::canvas
                        art::Utilities canvas::canvas
                        messagefacility::MF_MessageLogger
                        
                        fhiclcpp::fhiclcpp
                        cetlib::cetlib cetlib_except
                        ROOT::Geom
                        ROOT::XMLIO
                        ROOT::Gdml


        )		


simple_plugin( SnFileSource
               "source"
               ubraw_RawData_supernova
               ubcore::UBOpReadoutMap_service
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
