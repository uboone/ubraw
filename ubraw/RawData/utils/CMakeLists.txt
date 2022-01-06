cet_find_library( LIBWDA NAMES wda PATHS ENV LIBWDA_LIB NO_DEFAULT_PATH )
cet_find_library( SQLITE NAMES sqlite3_ups PATHS ENV SQLITE_LIB NO_DEFAULT_PATH )
#

link_directories( $ENV{SQLITE_LIB} )

simple_plugin( LArRawInputSourceUBooNE
               "source"
               ubraw_RawData
               ubcore::UBOpReadoutMap_service
               BASENAME_ONLY
             )

simple_plugin( RawDigitWriter "module"
               ubcore::UBOpReadoutMap_service
               lardata_Utilities_DatabaseUtil_service
               lardataobj_RawData
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
               ROOT::Tree
               BASENAME_ONLY
             )

# need a special link list because of the explicit link to a specific service inplementation
simple_plugin( TriggerEnforcement "module"
                   lardata_DetectorInfoServices_DetectorClocksServiceStandard_service # BUG note that we link to a specific service implementation
                   lardataobj_RawData
                   ubraw_RawData
                   art::Framework_Core
                   art::Framework_Principal
                   art::Framework_Services_Registry
                   art_root_io::tfile_support ROOT::Core
                   art::Persistency_Common canvas::canvas
                   art::Persistency_Provenance canvas::canvas
                   art::Utilities canvas::canvas
                   messagefacility::MF_MessageLogger
                   fhiclcpp::fhiclcpp
                   cetlib::cetlib
                   cetlib_except::cetlib_except
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
                        ubcore::UBOpReadoutMap_service
                        lardataobj_OpticalDetectorData
                        lardataobj_RawData
                        larcoreobj_SummaryData
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
                        wda
                        SQLite::SQLite3
                        messagefacility::MF_MessageLogger
                        ROOT::Tree
                        fhiclcpp::fhiclcpp
                        cetlib::cetlib
                        cetlib_except::cetlib_except
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
                        larcoreobj_SummaryData
                        ubraw_RawData
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
                        cetlib::cetlib
                        cetlib_except::cetlib_except
                        ROOT::Geom
                        ROOT::XMLIO
                        ROOT::Gdml
        )                                                           

install_headers()
install_source()

add_subdirectory(job)
