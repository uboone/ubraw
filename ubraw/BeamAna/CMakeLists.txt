cet_build_plugin(
  BeamAna art::EDAnalyzer
  LIBRARIES
  PRIVATE
  larcoreobj::SummaryData
)

install_headers()
install_fhicl()
install_source()
