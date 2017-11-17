link_libraries( -L$ENV{BOOST_LIB} -lboost_system -lboost_filesystem)

add_subdirectory(job)

art_make( BASENAME_ONLY

	  EXCLUDE
			plotMV4.C
			plotsupimp.C

          LIB_LIBRARIES
			uboone_RawData
                        larcore_Geometry
                        larcore_Geometry_Geometry_service
			uboonecode_uboone_BasicTool_GeoAlgo
                        lardata_Utilities
                        larevt_Filters
                        lardataobj_RawData
			lardataobj_RecoBase
			larcoreobj_SummaryData
			larsim_MCCheater_BackTracker_service
			larpandora_LArPandoraInterface
			nusimdata_SimulationBase
			${UB_DATA_TYPES}
			${Boost_SERIALIZATION_LIBRARY}
                        nutools_RandomUtils_NuRandomService_service
                        ${ART_FRAMEWORK_CORE}
                        ${ART_FRAMEWORK_PRINCIPAL}
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL_RANDOMNUMBERGENERATOR_SERVICE}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
                        art_Persistency_Common canvas_Persistency_Common
                        art_Persistency_Provenance canvas_Persistency_Provenance
                        art_Utilities canvas_Utilities
                        ${MF_MESSAGELOGGER}
                        ${MF_UTILITIES}
                        ${FHICLCPP}
			${CETLIB}
                        ${CETLIB_EXCEPT}
                        ${CLHEP}
                        ${ROOT_BASIC_LIB_LIST}

          MODULE_LIBRARIES
			uboone_RawData
                        larcore_Geometry
                        larcore_Geometry_Geometry_service
			uboonecode_uboone_BasicTool_GeoAlgo
                        lardata_Utilities
                        larevt_Filters
                        lardataobj_RawData
			lardataobj_RecoBase
			larcoreobj_SummaryData	
			larsim_MCCheater_BackTracker_service
			larpandora_LArPandoraInterface
			nusimdata_SimulationBase
			uboone_LEEPhotonAnalysis
			${UB_DATA_TYPES}
			${Boost_SERIALIZATION_LIBRARY}
                        nutools_RandomUtils_NuRandomService_service
                        ${ART_FRAMEWORK_CORE}
                        ${ART_FRAMEWORK_PRINCIPAL}
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL_RANDOMNUMBERGENERATOR_SERVICE}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
                        art_Persistency_Common canvas_Persistency_Common
                        art_Persistency_Provenance canvas_Persistency_Provenance
                        art_Utilities canvas_Utilities
                        ${MF_MESSAGELOGGER}
                        ${MF_UTILITIES}
                        ${FHICLCPP}
			${CETLIB}
                        ${CETLIB_EXCEPT}
                        ${CLHEP}
                        ${ROOT_BASIC_LIB_LIST}
        )

# install_headers()
install_fhicl()
install_source()
