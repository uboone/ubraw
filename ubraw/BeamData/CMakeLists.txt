if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
cet_add_compiler_flags(CXX -Wno-dynamic-exception-spec -Wno-return-type-c-linkage )
endif()

include_directories ( $ENV{IFDHC_FQ_DIR}/inc )

cet_make(LIBRARY_NAME BeamUtil
  LIBRARIES UB_DATA_TYPES
            ROOT::MathCore ROOT::Hist ROOT::Spectrum
  EXCLUDE BeamData_module.cc BeamDataQualityFilter_module.cc)

simple_plugin ( BeamDataQualityFilter "module"
                   lardataobj_RawData
                   ubraw_RawData
		   BeamUtil
                   art::Framework_Core
                   art::Framework_Principal
                   art::Framework_Services_Registry
                   art_root_io::tfile_support ROOT::Core
                   art::Persistency_Common canvas::canvas
                   art::Persistency_Provenance canvas::canvas
                   art::Utilities canvas::canvas
                   messagefacility::MF_MessageLogger
		   BASENAME_ONLY
              )

art_make( EXCLUDE BeamDataQualityFilter_module.cc
          BASENAME_ONLY
          MODULE_LIBRARIES
                        lardata_Utilities
                        larevt_Filters
                        lardataobj_RawData
			larcoreobj_SummaryData
			BeamUtil
			BeamDAQ
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
			ifdh::ifdh
                        messagefacility::MF_MessageLogger
                        fhiclcpp::fhiclcpp
                        cetlib::cetlib
                        cetlib_except::cetlib_except
                        CLHEP::CLHEP
                        ROOT::Tree
		LIB_LIBRARIES
			UB_DATA_TYPES
                        ROOT::Spectrum
                        ROOT::Hist
        )


# install_headers()
install_fhicl()
install_source()
