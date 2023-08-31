cet_make_library(
  SOURCE
  DaqFile.cpp
  SnFileSourceDriver.cc
  SnRecordHolder.cc
  LIBRARIES
  PUBLIC
  ubcore::Geometry_UBOpReadoutMap_service
  uboonedaq::data_types
  lardata::DetectorClocksService
  lardata::ArtDataHelper
  lardata::Utilities_DatabaseUtil_service
  lardataobj::RecoBase
  art::Framework_Services_Registry
  art::Framework_IO_Sources
  fhiclcpp::fhiclcpp
  Boost::serialization
)

cet_build_plugin(
  SnFileSource art::source
  LIBRARIES
  PRIVATE
  ubraw::RawData_supernova
)


install_headers()
install_fhicl()
# install_source()
# install_scripts()
