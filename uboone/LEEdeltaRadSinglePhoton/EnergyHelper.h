////////////////////////////////////////////////////////////////////////
// Class:       EnergyHelper
// Module Type: filter
// File:        EnergyHelper.h
//
////////////////////////////////////////////////////////////////////////

#ifndef ENERGYHELPER_H
#define ENERGYHELPER_H

#include "HelperBase.h"

#include "GeometryHelper.h"

#include "lardataobj/AnalysisBase/Calorimetry.h"
#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/RecoBase/Shower.h"
#include "lardataobj/RecoBase/Track.h"

#include "canvas/Persistency/Common/FindManyP.h"
#include "canvas/Persistency/Common/FindOneP.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"

namespace lee {

class EnergyHelper : public HelperBase {
public:
  EnergyHelper() = default;
  ~EnergyHelper() = default;

  /**
   * @brief      Measure the energy of a track
   *
   * @param[in]  track  The track
   * @param[in]  evt    The art::event
   *
   * @return     Energy in units of [ TODO: units? ]
   */
  double trackEnergy(const art::Ptr<recob::Track> &track, const art::Event &evt,
                     std::string _pfp_producer = "pandoraNu");

  /**
   * @brief      Measure the energy of a shower
   *
   * @param[in]  shower  The shower
   * @param[in]  evt     The art::event
   *
   * @return     Energy in units of [ TODO: units? ]
   */
  double showerEnergy(const art::Ptr<recob::Shower> &shower,
                      const art::Event &evt,
                      std::string _pfp_producer = "pandoraNu");

  /**
   * @brief      Measure the energy of a pfparticle neutrino candidate
   *
   * @param[in]  ipf     The pfparticle ID
   * @param[in]  evt     The art::event
   * @param      energy  The energy
   */
  void measureEnergy(size_t ipf, const art::Event &evt, double &energy,
                     std::string _pfp_producer = "pandoraNu");

  void dQdx(size_t pfp_id, const art::Event &evt,
            std::vector<double> &dqdx,
            std::vector<double> &dqdx_hits,
            double m_dQdxRectangleLength, double m_dQdxRectangleWidth,
            std::string _pfp_producer = "pandoraNu");

  void dEdxFromdQdx(std::vector<double> &dedx, std::vector<double> &dqdx);

private:
  double _data_gain = 240;
  double _mc_gain = 200;

  GeometryHelper geoHelper;
};
} // namespace lee

#endif
