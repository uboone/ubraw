cet_make_library(
  SOURCE
  bnbAutoTune.cxx
  getFOM.cxx
  getFOM2.cxx
  LIBRARIES
  PUBLIC
  UB_DATA_TYPES
  lardataobj::RawData
  ROOT::Spectrum
  ROOT::Hist
  ROOT::MathCore
  ROOT::Tree
)

cet_build_plugin(
  BeamData art::EDProducer
  LIBRARIES
  PRIVATE
  ubraw::BeamDAQ
  ubraw::BeamData
  larcoreobj::SummaryData
  art_root_io::TFileService_service
  ifdh_art::IFDH_service
)

cet_build_plugin(
  BeamDataQualityFilter art::EDFilter
  LIBRARIES
  PRIVATE
  ubraw::BeamData
  art_root_io::TFileService_service
)

install_headers()
install_fhicl()
install_source()
