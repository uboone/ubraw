////////////////////////////////////////////////////////////////////////
// Class:       CRTCoincidence
// Module Type: analyzer
// File:        CRTCoincidence_module.cc
// Description: Module to obtain XY (ZY, XZ) coincidences within a plane.
// Generated at Tue Jan 31 06:23:51 2017 by David Lorca Galindo using artmod
// from cetpkgsupport v1_10_02.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
//#include "art/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "bernfebdaq-core/Overlays/BernZMQFragment.hh"
#include "artdaq-core/Data/Fragments.hh"

#include "art/Framework/Services/Optional/TFileService.h"

#include "uboone/CRT/CRTAuxFunctions.hh"

#include "TTree.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TH3S.h"
#include "TProfile.h"
#include "TF1.h"
#include "TDatime.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <utility>

namespace bernfebdaq {
  class CRTCoincidence;
}

class bernfebdaq::CRTCoincidence : public art::EDAnalyzer {
public:
  explicit CRTCoincidence(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Required functions.
  void analyze(art::Event const & evt) override;

  // Selected optional functions.
  void beginJob() override;
  void endJob() override;

private:
  // Declare member data here.

  art::ServiceHandle<art::TFileService> tfs;
  TTree*       my_tree_;

  std::string  raw_data_label_;
  //unsigned int max_time_diff_;                                                                                                                
  std::string  SiPMpositions_;
  std::string  FEBDelays_;
  std::string  CRTGains_;
  std::string  CRTPedestals_;
  double max_time_difference_;//max time for coincidence
  double Emin_;//Energy threshold for hit


  std::map<std::pair<int,double>, BernZMQEvent const*> HitCollection;
  int counter=0;
  
  std::map <int, std::vector<double> > sensor_pos; //key = FEB*100+ch                                                                                
  std::map <int, double > FEBDel; //key = FEB;    
  std::map<int, std::pair<double,double> > SiPMgain;                                                                            
  std::map<int, std::pair<double,double> > SiPMpedestal;                                                                            

  TH2F* DKdistBot;
  TH2F* DKdistFT;
  TH2F* DKdistPS;
  TH3S* Dis3D;

  TH2F* FEBcoin;
  TH1F* TimDif;

  TH1F* pesHist0;
  TH1F* pesHist1;

  double Lbar = 10.8;
  //  void test(int a);

};


bernfebdaq::CRTCoincidence::CRTCoincidence(fhicl::ParameterSet const & p)
  : EDAnalyzer(p),
    raw_data_label_(p.get<std::string>("raw_data_label")),
    SiPMpositions_(p.get<std::string>("CRTpositions_file")),                                                                                
    FEBDelays_(p.get<std::string>("FEBDelays_file")),                                                                                
    CRTGains_(p.get<std::string>("CRTgains_file")),                                                                                
    CRTPedestals_(p.get<std::string>("CRTpedestals_file")),                                                                                
    max_time_difference_(p.get<double>("max_time_difference")),
    Emin_(p.get<double>("Emin"))//,                                                                                


{
  std::cout<<"Read parameters"<<std::endl;
}

void bernfebdaq::CRTCoincidence::analyze(art::Event const & evt)
{

  // std::cout<<"HitCollection size: "<<HitCollection.size()<<std::endl;
  // look for raw BernFEB data                                                                                                                   
  art::Handle< std::vector<artdaq::Fragment> > rawHandle;
  evt.getByLabel(raw_data_label_, "BernZMQ", rawHandle);

  //check to make sure the data we asked for is valid                                                                                             
  if(!rawHandle.isValid()){
    std::cout << "Run " << evt.run() << ", subrun " << evt.subRun()
              << ", event " << evt.event() << " has zero"
              << " BernFEB fragments " << " in module " << raw_data_label_ << std::endl;
    std::cout << std::endl;
    return;
  }

  //get better access to the data                                                                                                                 
  std::vector<artdaq::Fragment> const& rawFragments(*rawHandle);

  //loop over the raw data fragments                                                                                                              
  //There should be one fragment per FEB in each event.                                                                                           
  for(auto const& frag : rawFragments){

    //overlay this so it's in the "BernFragment" format. Same data!                                                                               
    BernZMQFragment bfrag(frag);

    //Grab the metadata.                                                                                                                          
    //See bernfebdaq-core/bernfebdaq-core/Overlays/BernFEBFragment.hh                                                                             
    auto bfrag_metadata = bfrag.metadata();
    size_t   nevents    = bfrag_metadata->n_events();   //number of BernFEBEvents in this packet                                                  
    auto time_start_ns = bfrag_metadata->time_start_nanosec();     //last second.
    //   auto time_end_ns = bfrag_metadata->time_end_nanosec();     //last second + 1s.
    auto FEB_ID =  bfrag_metadata->feb_id();     //mac addresss of this packet                                                                    
    auto IDN = crt::auxfunctions::getFEBN(FEB_ID); //FEB ID
    auto feb_tevt = IDN;                                                                                       
    double FEB_del = crt::auxfunctions::getFEBDel(IDN,FEBDel); //cable feb delay
    //    std::cout<<"FEB: "<<IDN<<" delay: "<<FEB_del<<std::endl;    

    time_start_ns = time_start_ns + FEB_del; //total time corrected
    //time_end_ns = time_end_ns + FEB_del; //

    for(size_t i_e=0; i_e<nevents; ++i_e){
      BernZMQEvent const* this_event = bfrag.eventdata(i_e); //get the single hit/event
      auto time_ts0 = this_event->Time_TS0();         //grab the event time from last second                                                              
           
      double corrected_time = GetCorrectedTime(time_ts0,*bfrag_metadata);      //correct this time
      double time_tevt = time_start_ns+corrected_time;     
      
      //previous some filter???
      std::pair<int,double> keypair_tevt;
      keypair_tevt = std::make_pair(feb_tevt,time_tevt);
      HitCollection[keypair_tevt]=this_event;
      
      //vacia el mapa cuando el tiempo del evento sea mayor que un par de segundos. esto nos hace perder eficiencia, pero reduce el tamaño de la hitcollection
      //std::cout<<"HitCillection size: "<<HitCollection.size()<<std::endl;
      //iterar sobre la collecccion y buscar coincidencias por tiempo y diferente feb                                                          

      for(auto itA = begin(HitCollection); itA != end(HitCollection); ++itA){
  
  std::pair<int,double> keypair_st = (*itA).first;
  BernZMQEvent const* event_st = (*itA).second;
  
  int feb_st = keypair_st.first;
  double time_st = keypair_st.second; 
  
  //std::cout<<"Time difference "<<abs(time_tevt - time_st)<<std::endl;
  //  std::cout<<"Coincidence "<<counter<<std::endl;
  //  getchar();
  
  if( (abs(time_tevt - time_st)<max_time_difference_)  && (feb_st !=  feb_tevt) ){//COINCIDENCE

    //tengo una coincidencia, sacar la info y crear un CRT HIT

    std::vector<std::pair<int,double> > pes_tevt;//to be store
    
    double max_temp1_tevt = -1;
    unsigned int maxSipm1_tevt = -1;
    double max_temp2_tevt = -1;
    unsigned int maxSipm2_tevt = -1;
    
    for(size_t i_chan=0; i_chan<32; ++i_chan){ //1st max
      if(this_event->adc[i_chan]>max_temp1_tevt){
        max_temp1_tevt=this_event->adc[i_chan];
        maxSipm1_tevt=i_chan;
      }
    }//1st max  

    if (maxSipm1_tevt % 2 == 0){//2nd mac
      maxSipm2_tevt = maxSipm1_tevt+1;
    } else if(maxSipm1_tevt % 2 == 1) {
      maxSipm2_tevt = maxSipm1_tevt-1;
    }
    max_temp2_tevt=this_event->adc[maxSipm2_tevt];
    //2ndmax    

    if(maxSipm2_tevt < maxSipm1_tevt){//swap
      unsigned int temp = maxSipm1_tevt;
      maxSipm1_tevt=maxSipm2_tevt;
      maxSipm2_tevt=temp;
      double maxtemp = max_temp1_tevt;
      max_temp1_tevt = max_temp2_tevt;
      max_temp2_tevt =maxtemp;
    }//swap
    
    int key_tevt1 = feb_tevt*100+maxSipm1_tevt;
    int key_tevt2 = feb_tevt*100+maxSipm2_tevt;

    std::pair<double,double> gain_tevt1 = crt::auxfunctions::getGain(key_tevt1, SiPMgain);      
    std::pair<double,double> pedestal_tevt1 = crt::auxfunctions::getGain(key_tevt1, SiPMpedestal);      
    double pesmax_tevt1 = (max_temp1_tevt - pedestal_tevt1.first) / gain_tevt1.first; 
    std::pair<int,double> max1_tevt = std::make_pair(maxSipm1_tevt,pesmax_tevt1);
    pes_tevt.push_back(max1_tevt);      
    std::vector<double> pos_tevt1 = crt::auxfunctions::getPos(key_tevt1, sensor_pos);   

    std::pair<double,double> gain_tevt2 = crt::auxfunctions::getGain(key_tevt2, SiPMgain);      
    std::pair<double,double> pedestal_tevt2 = crt::auxfunctions::getGain(key_tevt2, SiPMpedestal);      
    double pesmax_tevt2 = (max_temp2_tevt - pedestal_tevt2.first) / gain_tevt2.first; 
    std::pair<int,double> max2_tevt = std::make_pair(maxSipm2_tevt,pesmax_tevt2);
    pes_tevt.push_back(max2_tevt);      
    std::vector<double> pos_tevt2 = crt::auxfunctions::getPos(key_tevt2, sensor_pos);   

    //Interpolar!!!! 1
  
    std::vector<double> interpos_tevt = crt::auxfunctions::inter_X(pesmax_tevt1, pos_tevt1, pesmax_tevt2, pos_tevt2);
    double interpos_tevt_err = crt::auxfunctions::inter_X_error(pesmax_tevt1, pesmax_tevt2, Lbar);
        
    //Interpolar!!!! 1

    //------st

    std::vector<std::pair<int,double> > pes_st;//to be store

    double max_temp1_st = -1;
    unsigned int maxSipm1_st = -1;
    double max_temp2_st = -1;
    unsigned int maxSipm2_st = -1;

    for(size_t i_chan=0; i_chan<32; ++i_chan){ //1st max
      if(event_st->adc[i_chan]>max_temp1_st){
        max_temp1_st=event_st->adc[i_chan];
        maxSipm1_st=i_chan;
      }
    }//1st max

    if (maxSipm1_st % 2 == 0){//2nd max
      maxSipm2_st = maxSipm1_st+1;
    } else if(maxSipm1_st % 2 == 1) {
      maxSipm2_st = maxSipm1_st-1;
    }
    max_temp2_st=event_st->adc[maxSipm1_st];
    //2nd max

    if(maxSipm2_st < maxSipm1_st){//swap
      unsigned int temp = maxSipm1_st;
      maxSipm1_st=maxSipm2_st;
      maxSipm2_st=temp;
      double maxtemp = max_temp1_st;
      max_temp1_st = max_temp2_st;
      max_temp2_st =maxtemp;
    }//swap

    int key_st1 = feb_st*100+maxSipm1_st;
    int key_st2 = feb_st*100+maxSipm2_st;
    
    std::pair<double,double> gain_st1 = crt::auxfunctions::getGain(key_st1, SiPMgain);
    std::pair<double,double> pedestal_st1 = crt::auxfunctions::getGain(key_st1, SiPMpedestal);
    double pesmax_st1 = (max_temp1_st - pedestal_st1.first) / gain_st1.first;                                                                                       
    std::pair<int,double> max1_st = std::make_pair(maxSipm1_st,pesmax_st1);
    pes_st.push_back(max1_st);
    std::vector<double> pos_st1 = crt::auxfunctions::getPos(key_st1, sensor_pos);


    std::pair<double,double> gain_st2 = crt::auxfunctions::getGain(key_st2, SiPMgain);
    std::pair<double,double> pedestal_st2 = crt::auxfunctions::getGain(key_st2, SiPMpedestal);
    double pesmax_st2 = (max_temp2_st - pedestal_st2.first) / gain_st2.first;                                                                     
    std::pair<int,double> max2_st = std::make_pair(maxSipm2_st,pesmax_st2);
    pes_st.push_back(max2_st);
    std::vector<double> pos_st2 = crt::auxfunctions::getPos(key_st2, sensor_pos);
    

    //Interpolar y absolute position!!!! st                                                                                                                         
    std::vector<double> interpos_st = crt::auxfunctions::inter_X(pesmax_st1, pos_st1, pesmax_st2, pos_st2);
    double interpos_st_err = crt::auxfunctions::inter_X_error(pesmax_st1, pesmax_st2, Lbar);
    //Interpolar!!!! st    


    //------- st


    double Epes0 = max_temp1_tevt = max_temp2_tevt + max_temp1_st + max_temp1_st;
    pesHist0->Fill(Epes0);    

    //pos vector/ 0=x 1=y 2=z 3=plane 4=layer 5=orientation;
    if((pos_tevt1[3]==pos_st1[3]) && (pos_tevt1[4]!=pos_st1[4]) && (abs(pos_tevt1[4]-pos_st1[4])<2)  && (pos_tevt1[5]!=pos_st1[5])  ){
      //same plane && different layer && layer-continutiy && different orientation

      ///llenar CRTevent product
      counter++;
      /*      
      std::cout<<"Coincidences "<<counter<<std::endl;
      std::cout<<"Time difference "<<abs(time_tevt - time_st)<<std::endl;
      std::cout<<"Coincidence between feb_tevt "<<feb_tevt<<" and feb_st: " <<feb_st <<std::endl;
      std::cout<<"Interpos_tevt x: "<<interpos_tevt[0]<<" y: "<<interpos_tevt[1]<<" z: "<<interpos_tevt[2]<< " in plane "<<interpos_tevt[3]<<" and layer "<<interpos_tevt[4]<<" with orientation "<<interpos_tevt[5]<<" interpolada en eje: "<<interpos_tevt[6]<<std::endl;
      std::cout<<"Interpos_st   x: "<<interpos_st[0]<<" y: "<<interpos_st[1]<<" z: "<<interpos_st[2]<< " in plane "<<interpos_st[3]<<" and layer "<<interpos_st[4]<<" with orientation "<<interpos_st[5]<<" interpolada en eje: "<<interpos_st[6]<<std::endl;
      getchar();
*/
      FEBcoin->Fill(feb_tevt,feb_st);
      TimDif->Fill(abs(time_tevt - time_st));



      double Epes1 = max_temp1_tevt = max_temp2_tevt + max_temp1_st + max_temp1_st;
      pesHist1->Fill(Epes1);


      double xtot=-10000., ytot=-10000., ztot=-10000.;

      if( (interpos_tevt[6]==1) && (interpos_st[6] != 1) ){xtot=interpos_tevt[0];}
      if( (interpos_tevt[6]==2) && (interpos_st[6] != 2) ){ytot=interpos_tevt[1];}
      if( (interpos_tevt[6]==3) && (interpos_st[6] != 3) ){ztot=interpos_tevt[2];}
      
      if( (interpos_st[6]==1) && (interpos_tevt[6] != 1) ){xtot=interpos_st[0];}
      if( (interpos_st[6]==2) && (interpos_tevt[6] != 2) ){ytot=interpos_st[1];}
      if( (interpos_st[6]==3) && (interpos_tevt[6] != 3) ){ztot=interpos_st[2];}
      
      if( (interpos_st[6] !=1) && (interpos_tevt[6] != 1) ){xtot=(interpos_tevt[0] + interpos_st[0])/2;}
      if( (interpos_st[6] !=2) && (interpos_tevt[6] != 2) ){ytot=(interpos_tevt[1] + interpos_st[1])/2;}
      if( (interpos_st[6] !=3) && (interpos_tevt[6] != 3) ){ztot=(interpos_tevt[2] + interpos_st[2])/2;}
      
      //std::cout<<"Interaction 3D point--> x:"<<xtot<<"  y: "<<ytot<<"  z: "<<ztot<<std::endl;
      //getchar();


      crt::CRTData::CRTHit CRTevent;  

      CRTevent.x_pos= xtot;    //light atenuation missing
      CRTevent.x_err= sqrt( pow(interpos_tevt_err,2) + pow(interpos_st_err,2) );
      CRTevent.y_pos= ytot;
      CRTevent.y_err= sqrt( pow(interpos_tevt_err,2) + pow(interpos_st_err,2) );
      CRTevent.z_pos= ztot;
      CRTevent.z_err= sqrt( pow(interpos_tevt_err,2) + pow(interpos_st_err,2) );

      CRTevent.ts0_ns = (time_tevt + time_st)/2; //errors!!


      std::vector<uint8_t> ids;
      ids.push_back(feb_tevt);
      ids.push_back(feb_st);
      CRTevent.feb_id = ids;


      std::map< uint8_t, std::vector<std::pair<int,double> > > pesmap;
      pesmap[feb_tevt] = pes_tevt;
      pesmap[feb_st] = pes_st;
      CRTevent.pesmap = pesmap;
      //calculate CRTProduct components

      /*      int a = (int)xtot;
      int b = (int)ytot;
      int c = (int)ztot;
      std::cout<<"flag 1"<<std::endl;
      std::cout<<a<<"  "<<b<<"  "<<c<<std::endl;
      Dis3D->Fill(a,b,c);
      std::cout<<"flag 2"<<std::endl;     
      getchar();
      */
      if(pos_tevt1[3]==0){
        DKdistBot->Fill( ztot, xtot);
      }                                                                                                                                                                                       
      if(pos_tevt1[3]==1){                                                                                                                                                        DKdistFT->Fill(ztot, ytot);
      }                                
      if(pos_tevt1[3]==2){                                                                                                                                                        DKdistPS->Fill( ztot, ytot);
      }         
      
      
    }//same plane && different layer && layer-continutiy && different orientation
    
  }//COINCIDENCE
  /*else if((abs(time_tevt - time_st)) > 5*max_time_difference_){
    HitCollection.clear();
    std::cout<<" Empty collection"<<std::endl;}*/
      }
    }
  }
}//end analyzer 





void bernfebdaq::CRTCoincidence::beginJob()
{

  my_tree_ = tfs->make<TTree>("my_tree","CRT Analysis Tree");

  crt::auxfunctions::FillPos(SiPMpositions_, sensor_pos); //key = FEB*100+ch  //fill sipm positions
  crt::auxfunctions::FillFEBDel(FEBDelays_, FEBDel); //key = FEB  //fill FEB delays
  crt::auxfunctions::FillGain(CRTGains_, SiPMgain); //key = FEB*100+ch  //fill sipms gain
  crt::auxfunctions::FillGain(CRTPedestals_, SiPMpedestal); //key = FEB*100+ch  //same for pedestals



  DKdistBot = tfs->make<TH2F>("Bottom","Bottom",1400,200,900, 1400, -200, 500);
  DKdistBot->GetXaxis()->SetTitle("Z");
  DKdistBot->GetYaxis()->SetTitle("X");

  DKdistFT = tfs->make<TH2F>("Feedthrough Side","Feedthrough Side",2800,-200,1200, 500, -300, 200);
  DKdistFT->GetXaxis()->SetTitle("Z");
  DKdistFT->GetYaxis()->SetTitle("Y");

  DKdistPS = tfs->make<TH2F>("Pipe Side","Pipe Side",2800,-200,1200, 1400, -300, 400);
  DKdistPS->GetXaxis()->SetTitle("Z");
  DKdistPS->GetYaxis()->SetTitle("Y");

  Dis3D = tfs->make<TH3S>("3D","3D",1000,-500,500, 1400, -300, 400, 2800,-200,1200);
  Dis3D->GetXaxis()->SetTitle("X (cm)");
  Dis3D->GetYaxis()->SetTitle("Y (cm)");
  Dis3D->GetZaxis()->SetTitle("Z (cm)");

  FEBcoin = tfs->make<TH2F>("FEBvsFEB","FEBvsFEB",100,0,100,100,0,100);
  FEBcoin->GetXaxis()->SetTitle("FEB ID");
  FEBcoin->GetYaxis()->SetTitle("FEB ID");

  TimDif = tfs->make<TH1F>("Coincidence time difference","Coincidence time difference",400,0,400);
  TimDif->GetXaxis()->SetTitle("Time Difference (ns)");
  TimDif->GetYaxis()->SetTitle("Entries/bin");

  pesHist0 = tfs->make<TH1F>("Coincidence pes no cut","Coincidence pes no cut",4000,0,16000);
  pesHist0->GetXaxis()->SetTitle("Coincidence E (pes)");
  pesHist0->GetYaxis()->SetTitle("Entries/bin");

  pesHist1 = tfs->make<TH1F>("Coincidence pes","Coincidence pes",4000,0,4000);
  pesHist1->GetXaxis()->SetTitle("Coincidence E (pes)");
  pesHist1->GetYaxis()->SetTitle("Entries/bin");



}

void bernfebdaq::CRTCoincidence::endJob()
{
  // Implementation of optional member function here.


  std::cout<<"HitCollection size: "<<HitCollection.size()<<std::endl;
  std::cout<<counter<<" coincidences found..."<<std::endl;

  // std::cout<<"CoincidenceCollection size: "<<CoincidenceCollection.size()<<std::endl;
  //  std::cout<<"counter: "<<counter<<std::endl;

  /*
  for(auto it = begin(CoincidenceCollection); it != end(CoincidenceCollection); ++it){
    
    std::pair< BernZMQEvent const*, BernZMQEvent const* > CoincidencePair = (*it).second;
    crt::CRTData::CRTProduct CRTevent;  
    
    //calculate CRTProduct components
    
    //    std::pair<double,int> keypair2 = (*itA).first;
    BernZMQEvent const* event_one = CoincidencePair.first;
    BernZMQEvent const* event_two = CoincidencePair.second;
    
    CRTevent.x_pos=(*it).first;


    //calculate CRTProduct components
    

  }*/

}

DEFINE_ART_MODULE(bernfebdaq::CRTCoincidence)
