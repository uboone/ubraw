// Framework includes
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDProducer.h"
#include "canvas/Persistency/Common/FindOneP.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "fhiclcpp/ParameterSet.h"


// LArSoft Includes
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h" // raw::ChannelID_t
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/CryostatGeo.h"
#include "larcore/Geometry/TPCGeo.h"
#include "larcore/Geometry/PlaneGeo.h"
#include "lardataobj/RecoBase/Wire.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardata/ArtDataHelper/HitCreator.h"

#include "waveform_tools.h"
#include <iostream>

 
namespace sn{
class SnHitFinder : public art::EDProducer {
  
public:
  explicit SnHitFinder(fhicl::ParameterSet const& pset); 
       
  void produce(art::Event& evt);
  void beginJob();
  void endJob();
  void reconfigure(fhicl::ParameterSet const& p);


protected:
  std::string fWireModuleLabel;
  
  double fThresh;
  double fWidth;
  
  std::vector<double> fPlane_time_adjust;
  std::vector<double> fPlane_adc_adjust;
  
  bool        fUseRawDigits;
  std::string fRawDigitAssociation;
}; 


// ctor
SnHitFinder::SnHitFinder(fhicl::ParameterSet const& pset)
{
  reconfigure(pset);
    
  recob::HitCollectionCreator::declare_products(*this,"",true,false);  
}


void SnHitFinder::reconfigure(fhicl::ParameterSet const& p)
{
    fWireModuleLabel = p.get< std::string  >("WireModuleLabel","sndaq");
    fThresh = p.get<double>("Thresh",5);
    fWidth  = p.get<double>("Width",3);
    fUseRawDigits = p.get<bool>("UseRawDigits",false);
    fRawDigitAssociation = p.get<std::string>("RawDigitAssociation");
    
    fPlane_time_adjust = p.get<std::vector<double>>("Plane_time_adjust");
    fPlane_adc_adjust = p.get<std::vector<double>>("Plane_adc_adjust");
    assert(fPlane_time_adjust.size()==3);
    assert(fPlane_adc_adjust.size()==3);
}


void SnHitFinder::beginJob() 
{
  // get access to the TFile service
  // art::ServiceHandle<art::TFileService> tfs;  
}

void SnHitFinder::endJob()
{
  
}



/////////////////////////////////////////////////
/// The meat //////
template<typename T>
void runPeakFinder( waveform_tools::peak_finder& peaks, T begin, T end)
{
  double ped = *begin;
  // Run the peak finder algorithm.
  for(T it = begin; it!=end; it++) {
    double val = *it;
    peaks(val-ped);
  }
}


// jobstartend
void SnHitFinder::produce(art::Event& evt)
{
  art::ServiceHandle<geo::Geometry> geom;
  
  recob::HitCollectionCreator hcol(*this, evt, true, false);

  art::Handle< std::vector<recob::Wire> > wireVecHandle;
  evt.getByLabel(fWireModuleLabel,wireVecHandle);

  std::unique_ptr< art::FindOneP<raw::RawDigit> > digitAssociation;
  if(fUseRawDigits){
     digitAssociation = std::unique_ptr< art::FindOneP<raw::RawDigit> >(
         new art::FindOneP<raw::RawDigit>(wireVecHandle, evt, fRawDigitAssociation ));
   }
  
  
  
  for(size_t wireIter = 0; wireIter < wireVecHandle->size(); wireIter++)
  {
    art::Ptr<recob::Wire>   wire(wireVecHandle, wireIter);

    raw::ChannelID_t channel = wire->Channel();
    // get the WireID for this hit
    std::vector<geo::WireID> wids = geom->ChannelToWire(channel);    
    geo::WireID wid = wids[0];  // For uboone, only one 

    geo::SigType_t signal_type = geom->SignalType(channel);  // induction or collection?

    // geo::PlaneGeo const& planegeo = geom->Plane(wid);
    
    const recob::Wire::RegionsOfInterest_t& signalROI = wire->SignalROI();
   
    for(const auto& range : signalROI.get_ranges())
    {
     
      // ROI start time
      raw::TDCtick_t roiFirstBinTick = range.begin_index();
      
      double sign = -1;    // induction wire
      if(signal_type == geo::kCollection) sign = +1; // collection wire
      waveform_tools::peak_finder peaks(fThresh/fPlane_adc_adjust[wid.Plane], sign, fWidth);
      
      if(fUseRawDigits) {
        // Instead of using data in the ROI, instead use the associated raw digit array.
        art::Ptr< raw::RawDigit > rawdigit(digitAssociation->at(wireIter));
        if(!rawdigit) throw std::runtime_error("Arg, no rawdigit associated with wire");
        runPeakFinder(peaks, 
                      rawdigit->ADCs().begin()+range.begin_index(),
                      rawdigit->ADCs().begin()+range.end_index()
                        );
        
      } else {
        const std::vector<float>& roi = range.data();
        // simple pedestal: assume the first sample is at pedestal.
        size_t n = roi.size();
        std::cout << "Running hitfinder on roi with " << n << " values.  Values:" << std::endl;
        if(n>10) n = 10;
        for(size_t i=0;i<n;i++) std::cout << roi[i] << "\t";
        std::cout << std::endl;
        
        // Run the peak finder algorithm.
        // float ped = roi[0];
        // for(const float val: roi) { peaks(val-ped);}     
        runPeakFinder(peaks,roi.begin(),roi.end());
        
      }
      
     
       
      // std::cout << "Found " << peaks.size() << " peaks" << std::endl;
      for(auto& peak: peaks) {
        recob::HitCreator hitcreator(
          *wire,                                  // wire reference
         wid,                                     // wire ID
         fPlane_time_adjust[wid.Plane] + peak.tstart+roiFirstBinTick,             // start_tick
         fPlane_time_adjust[wid.Plane] + peak.tstop+roiFirstBinTick,              // end_tick 
         (peak.tstop-peak.tstart)*0.5,                  // rms
         fPlane_time_adjust[wid.Plane] + peak.tpeak+roiFirstBinTick,              // peak_time
         (peak.tstop-peak.tstart)*0.5,            // sigma_peak_time
         fPlane_adc_adjust[wid.Plane]*peak.height,                             // peak_amplitude
         fPlane_adc_adjust[wid.Plane]*sqrt(peak.height),                       // sigma_peak_amplitude
         fPlane_adc_adjust[wid.Plane]*peak.integral,                          // hit_integral
         fPlane_adc_adjust[wid.Plane]*sqrt(peak.integral),                     // hit_sigma_integral
         fPlane_adc_adjust[wid.Plane]*peak.integral,                           // summedADC FIXME
        1,                 // multiplicity
        1,                // local_index TODO check that the order is correct
        1,                // goodness_of_fit
        1                     // dof
         );
 		    const recob::Hit hit(hitcreator.move());
        hcol.emplace_back(std::move(hit), wire);                   
         
      }
    }
  }
  hcol.put_into(evt);
  
  
}
  
  



DEFINE_ART_MODULE(SnHitFinder)

}