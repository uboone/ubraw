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
                   ${ART_FRAMEWORK_CORE}
                   ${ART_FRAMEWORK_PRINCIPAL}
                   ${ART_FRAMEWORK_SERVICES_REGISTRY}
                   ${ART_ROOT_IO_TFILE_SUPPORT} ${ROOT_CORE}
                   art_Persistency_Common canvas
                   art_Persistency_Provenance canvas
                   art_Utilities canvas
                   ${MF_MESSAGELOGGER}
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
			${Boost_SERIALIZATION_LIBRARY}
                        nurandom_RandomUtils_NuRandomService_service
                        ${ART_FRAMEWORK_CORE}
                        ${ART_FRAMEWORK_PRINCIPAL}
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        ${ART_ROOT_IO_TFILE_SUPPORT} ${ROOT_CORE}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL_RANDOMNUMBERGENERATOR_SERVICE}
                        ${ART_ROOT_IO_TFILESERVICE_SERVICE}
                        ${ART_FRAMEWORK_SERVICES_SYSTEM_FILECATALOGMETADATA_SERVICE}
                        art_Persistency_Common canvas
                        art_Persistency_Provenance canvas
                        art_Utilities canvas
			ifdh
                        ${MF_MESSAGELOGGER}
                        ${FHICLCPP}
                        cetlib cetlib_except
                        ${CLHEP}
                        ROOT::Tree
		LIB_LIBRARIES
			UB_DATA_TYPES
			${ROOT_SPECTRUM}
        )


# install_headers()
install_fhicl()
install_source()
