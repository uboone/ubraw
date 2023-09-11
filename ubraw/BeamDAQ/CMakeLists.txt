cet_make_library(
  SOURCE
  MWRData.cpp
  beamDAQConfig.cpp
  beamIFDBInterface.cpp
  beamRun.cpp
  beamRunHeader.cpp
  httpResponse.cpp
  LIBRARIES
  PUBLIC
  uboonedaq_datatypes::datatypes
  fhiclcpp::fhiclcpp
  messagefacility::MF_MessageLogger
  Boost::serialization
  Boost::program_options
  curl
)

cet_make_exec(
  NAME bdaq_get
  LIBRARIES
  PRIVATE
  ubraw::BeamDAQ
)

cet_make_exec(
  NAME bdaq_readraw
  LIBRARIES
  PRIVATE
  ubraw::BeamDAQ
)

# ----------------------------------------------------------------------
# Install this project's headers, sources, library, and executables:

install_headers()
install_fhicl()
install_source()
install_scripts()

