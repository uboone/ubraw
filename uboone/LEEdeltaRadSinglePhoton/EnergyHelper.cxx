#ifndef ENERGYHELPER_CXX
#define ENERGYHELPER_CXX

#include "EnergyHelper.h"

namespace lee {

double EnergyHelper::showerEnergy(const art::Ptr<recob::Shower> &shower,
                                  const art::Event &evt,
                                  std::string _pfp_producer) {
  auto const &shower_handle =
      evt.getValidHandle<std::vector<recob::Shower>>(_pfp_producer);
  auto const &pfparticle_handle =
      evt.getValidHandle<std::vector<recob::PFParticle>>(_pfp_producer);
  auto const &cluster_handle =
      evt.getValidHandle<std::vector<recob::Cluster>>(_pfp_producer);

  double _gain;
  if (evt.isRealData())
    _gain = _data_gain;
  else
    _gain = _mc_gain;

  art::FindOneP<recob::PFParticle> pfparticle_shower_ass(shower_handle, evt,
                                                         _pfp_producer);
  const art::Ptr<recob::PFParticle> pfparticle =
      pfparticle_shower_ass.at(shower.key());

  art::FindManyP<recob::Cluster> cluster_pfparticle_ass(pfparticle_handle, evt,
                                                        _pfp_producer);
  std::vector<art::Ptr<recob::Cluster>> clusters =
      cluster_pfparticle_ass.at(pfparticle.key());

  double energy[3] = {0, 0, 0};
  int n_hits[3] = {0, 0, 0};
  for (size_t icl = 0; icl < clusters.size(); icl++) {
    art::FindManyP<recob::Hit> hit_cluster_ass(cluster_handle, evt,
                                               _pfp_producer);
    std::vector<art::Ptr<recob::Hit>> hits =
        hit_cluster_ass.at(clusters[icl].key());

    for (size_t ihit = 0; ihit < hits.size(); ++ihit) {

      if (hits[ihit]->View() > 2 || hits[ihit]->View() < 0)
        continue;
      // 23 work function (23 eV/e- in the argon)
      // 0.62 recombination factor
      // TODO move to other function or use detector properties
      energy[hits[ihit]->View()] +=
          hits[ihit]->Integral() * _gain * (23. / 1e6) / 0.62;
      n_hits[hits[ihit]->View()]++;
    }
  }

  const int n = sizeof(n_hits) / sizeof(int);

  return energy[std::distance(n_hits, std::max_element(n_hits, n_hits + n))] /
         1000; // convert to GeV
}

double EnergyHelper::trackEnergy(const art::Ptr<recob::Track> &track,
                                 const art::Event &evt,
                                 std::string _pfp_producer) {
  auto const &track_handle =
      evt.getValidHandle<std::vector<recob::Track>>(_pfp_producer);
  art::FindManyP<anab::Calorimetry> calo_track_ass(track_handle, evt,
                                                   "pandoraNucalo");

  const std::vector<art::Ptr<anab::Calorimetry>> calos =
      calo_track_ass.at(track->ID());


  double E = 0;
  double Eapprox = 0;

  for (size_t ical = 0; ical < calos.size(); ++ical) {


    if (E != 0)
      continue;
    if (!calos[ical])
      continue;

    if (!calos[ical]->PlaneID().isValid)
      continue;

    int planenum = calos[ical]->PlaneID().Plane;

    if (planenum < 0 || planenum > 2)
      continue;
    if (planenum != 2)
      continue; // Use informartion from collection plane only


    // Understand if the calo module flipped the track
    // double dqdx_start = (calos[ical]->dQdx())[0] + (calos[ical]->dQdx())[1] +
    // (calos[ical]->dQdx())[2];
    // double dqdx_end   = (calos[ical]->dQdx())[calos[ical]->dQdx().size()-1] +
    // (calos[ical]->dQdx())[calos[ical]->dQdx().size()-2] +
    // (calos[ical]->dQdx())[calos[ical]->dQdx().size()-3];
    // bool caloFlippedTrack = dqdx_start < dqdx_end;

    double mean = 0;
    double dedx = 0;
    double prevresrange = 0;

    if (calos[ical]->ResidualRange().size() > 0) {
      if (calos[ical]->ResidualRange()[0] > track->Length() / 2) {
        prevresrange = track->Length();
      }
    }

    double currentresrange = 0;

    for (size_t iTrkHit = 0; iTrkHit < calos[ical]->dEdx().size(); ++iTrkHit) {
      dedx = calos[ical]->dEdx()[iTrkHit];
      currentresrange = calos[ical]->ResidualRange()[iTrkHit];
      if (dedx > 0 && dedx < 10) {
        mean += dedx;
        E += dedx * abs(prevresrange - currentresrange);
        prevresrange = currentresrange;
      }
    }

    // std::cout << "[PandoraLEE] " << "Length: " << track->Length() << "and
    // Energy approximation is " <<
    // mean/calos[ical]->dEdx().size()*track->Length()<< "MeV"<<std::endl;
    Eapprox = mean / calos[ical]->dEdx().size() * track->Length();
  }
  return Eapprox / 1000; // convert to GeV
}

void EnergyHelper::dQdx(size_t pfp_id,
                        const art::Event &evt,
                        std::vector<double> &dqdx,
                        std::vector<double> &dqdx_hits,
                        double m_dQdxRectangleLength,
                        double m_dQdxRectangleWidth,
                        std::string _pfp_producer) {

  double _gain;
  if (evt.isRealData())
    _gain = _data_gain;
  else
    _gain = _mc_gain;

  detinfo::DetectorProperties const *detprop =
      lar::providerFrom<detinfo::DetectorPropertiesService>();
  // art::ServiceHandle<geo::Geometry> geom;

  auto const &pfparticle_handle =
      evt.getValidHandle<std::vector<recob::PFParticle>>(_pfp_producer);
  auto const &cluster_handle =
      evt.getValidHandle<std::vector<recob::Cluster>>(_pfp_producer);

  art::FindOneP<recob::Shower> shower_per_pfpart(pfparticle_handle, evt,
                                                 _pfp_producer);
  auto const &shower_obj = shower_per_pfpart.at(pfp_id);

  TVector3 shower_dir(shower_obj->Direction().X(), shower_obj->Direction().Y(),
                      shower_obj->Direction().Z());

  geoHelper.correct_direction(pfp_id, evt);

  art::FindManyP<recob::Cluster> clusters_per_pfpart(pfparticle_handle, evt,
                                                     _pfp_producer);
  art::FindManyP<recob::Hit> hits_per_clusters(cluster_handle, evt,
                                               _pfp_producer);

  std::vector<art::Ptr<recob::Cluster>> clusters =
      clusters_per_pfpart.at(pfp_id);

  double drift = detprop->DriftVelocity() * 1e-3;
  //std::cout << drift << std::endl;
  //std::cout << "[dQdx] Clusters size " << clusters.size() << std::endl;

  for (size_t icl = 0; icl < clusters.size(); icl++) {
    std::vector<art::Ptr<recob::Hit>> hits =
        hits_per_clusters.at(clusters[icl].key());
    /*
    std::cout << "[dQdx] "
              << "Cluster " << icl << std::endl;

    std::cout << "[dQdx] "
              << "Wire coordinate " << clusters[icl]->StartWire() << std::endl;
    std::cout << "[dQdx] "
              << "Tick coordinate " << clusters[icl]->StartTick() << std::endl;
    */
    // TODO Use variable from detector properties!
    // To get the time in ns -> 4.8 ms / 9600 ticks * 1e6 = 500
    // 0.3 wire spacing

    double fromTickToNs = 4.8 / detprop->ReadOutWindowSize() * 1e6;
    double wireSpacing = 0.3;

    std::vector<double> cluster_start = {
        clusters[icl]->StartWire() * wireSpacing,
        drift * clusters[icl]->StartTick() * fromTickToNs};
    std::vector<double> cluster_end = {clusters[icl]->EndWire() * wireSpacing,
                                       drift * clusters[icl]->EndTick() *
                                           fromTickToNs};

    double cluster_length = sqrt(pow(cluster_end[0] - cluster_start[0], 2) +
                                 pow(cluster_end[1] - cluster_start[1], 2));
    if (cluster_length <= 0)
      continue;

    std::vector<double> cluster_axis = {cos(clusters[icl]->StartAngle()),
                                        sin(clusters[icl]->StartAngle())};

    // Build rectangle 4 x 1 cm around the cluster axis
    std::vector<std::vector<double>> points;
    geoHelper.buildRectangle(m_dQdxRectangleLength, m_dQdxRectangleWidth,
                             cluster_start, cluster_axis, points);
    /*
    std::cout << "[dQdx] Point 1 " << points[0][0] << " " << points[0][1] << std::endl;
    std::cout << "[dQdx] Point 2 " << points[1][0] << " " << points[1][1] << std::endl;
    std::cout << "[dQdx] Point 3 " << points[2][0] << " " << points[2][1] << std::endl;
    std::cout << "[dQdx] Point 4 " << points[3][0] << " " << points[3][1] << std::endl;
    */
    std::vector<double> dqdxs;

    bool first = true;

    for (auto &hit : hits) {

      // std::cout << "[PandoraLEE] Hit wire ID " << hit->WireID().Wire <<
      // std::endl;
      // std::cout << "Hit peak time " << hit->PeakTime() << std::endl;

      std::vector<double> hit_pos = {hit->WireID().Wire * wireSpacing,
                                     fromTickToNs * drift * hit->PeakTime()};


      double pitch =
          geoHelper.getPitch(shower_dir, clusters[icl]->Plane().Plane);

      bool is_within = geoHelper.isInside(hit_pos, points);

      // Hit within the rectangle. The function considers points on the border
      // as outside, so we manually add the first point

      if (is_within || first) {
        double q = hit->Integral() * _gain;
        dqdxs.push_back(q / pitch);
        if (clusters[icl]->Plane().Plane == 2) {
          dqdx_hits.push_back(q / pitch);
        }
      }
      first = false;

      // std::cout << "[dQdx] Hit pos " << is_within << " " << hit_pos[0] << " " << hit_pos[1] << std::endl;

    }

    // Get the median
    size_t n = dqdxs.size() / 2;

    std::nth_element(dqdxs.begin(), dqdxs.begin() + n, dqdxs.end());
    if (n > 0) {
      /*
      std::cout << "[dQdx] Plane dQdx " << clusters[icl]->Plane().Plane << " "
                << icl << " "
                << dqdxs[n] << " " << dqdxs[n] * (23. / 1e6) / 0.62
                << std::endl;
      */
      dqdx[clusters[icl]->Plane().Plane] = dqdxs[n];
    }

  }
}

void EnergyHelper::dEdxFromdQdx(std::vector<double> &dedx,
                                std::vector<double> &dqdx) {

  double work_function = 23;
  double recombination_factor = 0.62;

  for (size_t i = 0; i < dqdx.size(); i++) {
    if (dqdx[i] > 0)
      dedx[i] = dqdx[i] * (work_function / 1e6) / recombination_factor;
    //std::cout << "[dEdx] " << i << " " << dedx[i] << std::endl;
  }
}

void EnergyHelper::measureEnergy(size_t ipf, const art::Event &evt,
                                 double &energy, std::string _pfp_producer) {

  try {

    auto const &pfparticle_handle =
    evt.getValidHandle<std::vector<recob::PFParticle>>(_pfp_producer);
    auto const &pfparticles(*pfparticle_handle);


    try {

      art::FindManyP<recob::Shower> showers_per_pfparticle(pfparticle_handle, evt,
        _pfp_producer);
        std::vector<art::Ptr<recob::Shower>> showers = showers_per_pfparticle.at(ipf);

        for (size_t ish = 0; ish < showers.size(); ish++) {
          if (showerEnergy(showers[ish], evt) > 0) {
            energy += showerEnergy(showers[ish], evt);
          }
        }

      } catch (...) {
        std::cout << "[EnergyHelper] "
        << "SHOWER NOT AVAILABLE " << std::endl;
      }

    try {

        art::FindManyP<recob::Track> tracks_per_pfparticle(pfparticle_handle, evt,
          _pfp_producer);
        std::vector<art::Ptr<recob::Track>> tracks = tracks_per_pfparticle.at(ipf);

        for (size_t itr = 0; itr < tracks.size(); itr++) {
          if (trackEnergy(tracks[itr], evt) > 0) {
            energy += trackEnergy(tracks[itr], evt);
          }
        }


    } catch (...) {
      std::cout << "[EnergyHelper] "
      << "TRACK NOT AVAILABLE " << std::endl;
    }
    for (auto const &pfdaughter : pfparticles[ipf].Daughters()) {
      measureEnergy(pfdaughter, evt, energy, _pfp_producer);
    }

  } catch (...) {
    std::cout << "[EnergyHelper] "
    << "PFParticles not available " << std::endl;
  }
}

} // namespace lee

#endif
