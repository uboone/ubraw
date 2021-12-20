art_make( BASENAME_ONLY
          MODULE_LIBRARIES
                        lardata_Utilities
                        larevt_Filters
                        lardataobj_RawData
			larcoreobj_SummaryData
			UB_DATA_TYPES
			Boost::serialization
                        nurandom::RandomUtils_NuRandomService_service
                        art::Framework_Core
                        art::Framework_Principal
                        art::Framework_Services_Registry
                        art_root_io::tfile_support ROOT::Core
                        art::Framework_Services_Optional_RandomNumberGenerator_service
                        art_root_io::TFileService_service
                        art::Persistency_Common canvas::canvas
                        art::Persistency_Provenance canvas::canvas
                        art::Utilities canvas::canvas
                        messagefacility::MF_MessageLogger
                        
                        fhiclcpp::fhiclcpp
                        cetlib::cetlib cetlib_except
                        CLHEP::CLHEP

        )

# install_headers()
install_fhicl()
install_source()
