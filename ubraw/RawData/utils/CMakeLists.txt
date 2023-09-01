cet_make_library(
  SOURCE
  LArRawInputDriverUBooNE.cxx
  LIBRARIES
  PUBLIC
  ubcore::Geometry_UBOpReadoutMap_service
  ubobj::RawData
  ubobj::Trigger
  uboonedaq::data_types
  lardata::Utilities
  art::Framework_Services_Registry
  art::Framework_IO_Sources
  art_root_io::TFileService_service
  fhiclcpp::fhiclcpp
  wda::wda
  ROOT::Tree
  Boost::serialization
  SQLite::SQLite3
)

cet_build_plugin(
  LArRawInputSourceUBooNE art::source
  LIBRARIES
  PRIVATE
  ubraw::RawData_utils
)

cet_build_plugin(
  TriggerEnforcement art::EDFilter
  LIBRARIES
  PRIVATE
  ubobj::Trigger
  lardataobj::RawData
)

cet_build_plugin(
  TriggerDumper art::EDAnalyzer
  LIBRARIES
  PRIVATE
  ubobj::Trigger
  lardataobj::RawData
)

cet_build_plugin(
  RawDigitWriter art::EDAnalyzer
  LIBRARIES
  PRIVATE
  ubcore::Geometry_UBOpReadoutMap_service
  larevt::Filters
  lardata::Utilities_DatabaseUtil_service
  lardata::DetectorClocksService
  larcore::ServiceUtil
  larcoreobj::SimpleTypesAndConstants
  art_root_io::TFileService_service
  ROOT::Tree
)

cet_build_plugin(
  RawDigitSaver art::EDProducer
  LIBRARIES
  PRIVATE
  lardataobj::RawData
)

cet_build_plugin(
  GPSOffsetAna art::EDAnalyzer
  LIBRARIES
  PRIVATE
  ubobj::RawData
  lardataobj::RawData
  ifdh_art::IFDH_service
  art_root_io::TFileService_service
  ROOT::Tree
)

install_headers()
install_fhicl()
install_source()

add_subdirectory(job)
