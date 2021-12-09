# project name
project(beamdaq)

# ----------------------------------------------------------------------
# Build this project's executables:

include_directories ( $ENV{UBOONEDAQ_DATATYPES_INC} )
#set(CURLLIB "-lcurl")
find_library( CURLLIB NAMES curl )
message(STATUS "found CURLLIB ${CURLLIB}")

cet_make(LIBRARY_NAME BeamDAQ LIBRARIES UB_DATA_TYPES
                                        messagefacility::MF_MessageLogger  ${CURLLIB} 
					${Boost_PROGRAM_OPTIONS_LIBRARY} Boost::filesystem Boost::date_time Boost::serialization

					fhiclcpp::fhiclcpp cetlib::cetlib cetlib_except::cetlib_except
					EXCLUDE bdaq_get.cpp bdaq_get.h bdaq_readraw.cpp)

cet_make_exec( bdaq_get SOURCE bdaq_get.cpp LIBRARIES UB_DATA_TYPES
                                messagefacility::MF_MessageLogger  ${CURLLIB} 
				${Boost_PROGRAM_OPTIONS_LIBRARY} Boost::filesystem Boost::date_time Boost::serialization

				fhiclcpp::fhiclcpp cetlib::cetlib cetlib_except::cetlib_except BeamDAQ)
cet_make_exec( bdaq_readraw SOURCE bdaq_readraw.cpp LIBRARIES
               ${Boost_PROGRAM_OPTIONS_LIBRARY} Boost::filesystem Boost::date_time Boost::serialization

                                    UB_DATA_TYPES BeamDAQ )


# ----------------------------------------------------------------------
# Install this project's headers, sources, library, and executables:

install_fhicl()

install_scripts()

install( TARGETS bdaq_get
         DESTINATION ${flavorqual_dir}/bin )

install( TARGETS bdaq_readraw
         DESTINATION ${flavorqual_dir}/bin )

