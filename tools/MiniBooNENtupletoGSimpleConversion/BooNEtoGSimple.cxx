#include <iostream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <cmath>
#include <regex>

//BooNEtoGSimple stuff
#include "BooNEtoGSimple.h"
#include "BooNENtuple.h"
#include "BeamNtuple.h"
#include "FluxDrivers/GNuMIFlux.h"
#include "FluxDrivers/GSimpleNtpFlux.h"

//GEANT to PDG 
#include "PDG/PDGCodes.h"
#include "PDG/PDGCodeList.h"

//root stuff
#include <TROOT.h>
#include "TStopwatch.h"
#include "TDatime.h"
#include "TTree.h"
#include "TFile.h"
#include "TRandom3.h"

using namespace std;

BooNEtoGSimple::BooNEtoGSimple()
{
}

BooNEtoGSimple::~BooNEtoGSimple()
{
}

void BooNEtoGSimple::OpenFiles(char* input_file, char* output_file)
{
  cout << "*******************************************************" << endl;
  cout << "Input (BooNE) ntuple : "<< input_file                    << endl;
  cout << "Output (NuMI) ntuple : "<< output_file                   << endl;
  cout <<  endl;
  cout << "*******************************************************" << endl;

  fBooNEfile=new TFile(input_file);
  fBNBtree=dynamic_cast<TTree*> (fBooNEfile->Get("h201"));  
  fBNBtree->SetBranchAddress("beamwgt",&fBooneNtp.beamwgt);
  fBNBtree->SetBranchAddress("ntp",&fBooneNtp.ntp);
  fBNBtree->SetBranchAddress("npart",&fBooneNtp.npart);
  fBNBtree->SetBranchAddress("id",fBooneNtp.id);
  fBNBtree->SetBranchAddress("ini_pos",&fBooneNtp.ini_pos[0][0]);
  fBNBtree->SetBranchAddress("ini_mom",&fBooneNtp.ini_mom[0][0]);
  fBNBtree->SetBranchAddress("ini_eng",fBooneNtp.ini_eng);
  fBNBtree->SetBranchAddress("ini_t",fBooneNtp.ini_t);
  fBNBtree->SetBranchAddress("fin_mom",&fBooneNtp.fin_mom[0][0]);
  fBNBtree->SetBranchAddress("fin_pol",&fBooneNtp.fin_pol[0][0]);
  
  fWindowtree=dynamic_cast<TTree*> (fBooNEfile->Get("h220"));
  fWindowtree->SetBranchAddress("tank_pos_beam",&fBeamNtp.tank_pos_beam[0]);
  fWindowtree->SetBranchAddress("targ_pos_beam",&fBeamNtp.targ_pos_beam[0]);
  fWindowtree->SetBranchAddress("pot",&fBeamNtp.pot);
  fWindowtree->SetBranchAddress("windowbase",&fBeamNtp.windowbase[0]);
  fWindowtree->SetBranchAddress("windowdir1",&fBeamNtp.windowdir1[0]);
  fWindowtree->SetBranchAddress("windowdir2",&fBeamNtp.windowdir2[0]);
 
  fGSimplefile=new TFile(output_file,"RECREATE");
  fluxntp = new TTree("flux","a simple flux n-tuple");
  metantp = new TTree("meta","metadata for flux n-tuple");
  fentry = new genie::flux::GSimpleNtpEntry;
  fnumi  = new genie::flux::GSimpleNtpNuMI;
  faux   = new genie::flux::GSimpleNtpAux;
  fmeta  = new genie::flux::GSimpleNtpMeta;

  fEventNo=0;

}

void BooNEtoGSimple::CloseFiles()
{
  //Numi ntuple
  fluxntp->Write();
  metantp->Write();
  fGSimplefile->Close();

  //Boone ntuple
  fBooNEfile->Close();
}

void BooNEtoGSimple::DoIt(char* input_file, char* output_file, long int POTGoal)
{
  
  fBooneNevents=fBNBtree->GetEntries();
  cout << "Reading " << fBooneNevents << " events" << endl;
  
  double MaxWeight = 0;
  double MinWeight = 100;
  double SumWeights = 0;
  int nok = 0;
  int nwrite = 0;

  std::string run_number = input_file;
  std::regex r("\\d\\d\\d\\d");
  std::smatch m;
  std::regex_search(run_number,m,r);

  int runNumber;
  std::istringstream buffer(m[0].str().c_str());
  buffer >> runNumber;
  
  //  runNumber = 1;
  std::cout << "Run Number : " << runNumber << std::endl; 
  
  
  for ( int iev = 0 ; iev < fBooneNevents ; iev++ ) {
    fBNBtree->GetEntry(iev);
    SumWeights += fBooneNtp.beamwgt;
    if( MaxWeight < fBooneNtp.beamwgt) MaxWeight = fBooneNtp.beamwgt;
    if( MinWeight > fBooneNtp.beamwgt) MinWeight = fBooneNtp.beamwgt;
  }
  
  std::cout << "Max Weight : " << MaxWeight << std::endl; 
  std::cout << "Min Weight : " << MinWeight << std::endl; 
  
  /*///
    / 
    / This is where we copy from "gnumi2simple_basic.C"
    /
  *///
  
  fluxntp->Branch("entry",&fentry);
  fluxntp->Branch("numi",&fnumi);
  fluxntp->Branch("aux",&faux);
  metantp->Branch("meta",&fmeta);
  
  long int ngen = 0;
  
  set<int> pdglist;
  double maxe = 0;
  double minwgt = +1.0e10;
  double maxwgt = -1.0e10;
  
  UInt_t metakey = TString::Hash(output_file,strlen(output_file));
  
  TRandom3* rnd = new TRandom3();
  rnd->SetSeed(runNumber);
  double SumPOT = 0;
  for (int iter = 0; SumPOT < POTGoal; iter++){

    for ( int iev = 0 ; iev < fBooneNevents ; iev++ ) {
      fWindowtree->GetEntry(0);   
      fBNBtree->GetEntry(iev);
      
      // Convert weight into neutrino count
      double RAND = rnd->Uniform(MaxWeight);
      
      if( fBooneNtp.beamwgt < RAND){ continue; }
      
      ngen++;
      fentry->Reset();
      fnumi->Reset();
      faux->Reset();
      
      fentry->metakey = metakey;
      
      int pdg = fBooneNtp.id[0]; 
      
      if ( fBooneNtp.ntp == 1 ){
	fentry->pdg = 12; //nue
	//fentry->ntype = 12; //nue
      }
      else if ( fBooneNtp.ntp == 2 ){
	fentry->pdg = -12; //nuebar
	//	fentry->ntype = -12; //nue
      }
      else if ( fBooneNtp.ntp == 3 ){
	fentry->pdg = 14; //numu
	//	fentry->ntype = 14; //nue
      }
      else if ( fBooneNtp.ntp == 4 ){
	fentry->pdg = -14; //numubar
	//	fentry->ntype = -14; //nue
      }
      else{
	cout<<"Neutrino type not recognized!!! ntp = "<< fBooneNtp.ntp <<endl;
      }

      fentry->wgt     = 1; // Do the rejection before
      
      //Convert all these to meters
      double nu_x = fBooneNtp.ini_pos[0][0]/100;
      double nu_y = fBooneNtp.ini_pos[0][1]/100;
      double nu_z = fBooneNtp.ini_pos[0][2]/100;
      
      double nu_momx = fBooneNtp.ini_mom[0][0];
      double nu_momy = fBooneNtp.ini_mom[0][1];
      double nu_momz = fBooneNtp.ini_mom[0][2];
      
      double targ_x = fBeamNtp.targ_pos_beam[0]/100;
      double targ_y = fBeamNtp.targ_pos_beam[1]/100;
      double targ_z = fBeamNtp.targ_pos_beam[2]/100;

      double tank_z = (fBeamNtp.tank_pos_beam[2]+fBeamNtp.windowbase[2])/100;
            
      fentry->vtxx    = nu_x + (nu_momx/nu_momz)*(tank_z-nu_z) + targ_x;
      fentry->vtxy    = nu_y + (nu_momy/nu_momz)*(tank_z-nu_z) + targ_y;
      fentry->vtxz    = tank_z+targ_z; 
      fentry->dist    = sqrt( pow(nu_x-fentry->vtxx,2) +
			      pow(nu_y-fentry->vtxy,2) +
			      pow(nu_z-tank_z,2) );
      
      fentry->px      = fBooneNtp.ini_mom[0][0];
      fentry->py      = fBooneNtp.ini_mom[0][1];
      fentry->pz      = fBooneNtp.ini_mom[0][2];
      fentry->E       = fBooneNtp.ini_eng[0];
      
      fnumi->run      = runNumber;
      fnumi->evtno    = fEventNo; 
      fnumi->entryno  = iev; 
      
      int npart = fBooneNtp.npart;
      fnumi->tpx      = fBooneNtp.ini_mom[npart-2][0]; //npart-2
      fnumi->tpy      = fBooneNtp.ini_mom[npart-2][1]; //npart-2
      fnumi->tpz      = fBooneNtp.ini_mom[npart-2][2]; //npart-2
      fnumi->vx       = fBooneNtp.ini_pos[0][0];  //0
      fnumi->vy       = fBooneNtp.ini_pos[0][1];  //0
      fnumi->vz       = fBooneNtp.ini_pos[0][2];  //0
      
      fnumi->pdpx     = fBooneNtp.fin_mom[1][0]; //1 final
      fnumi->pdpy     = fBooneNtp.fin_mom[1][1]; //1 final
      fnumi->pdpz     = fBooneNtp.fin_mom[1][2]; //1 final
      
      fnumi->pppx     = fBooneNtp.ini_mom[1][0]; //1 init
      fnumi->pppy     = fBooneNtp.ini_mom[1][1]; //1 init
      fnumi->pppz     = fBooneNtp.ini_mom[1][2]; //1 init
      
      /* ////
	 /
	 /  Now need to  calculate ndecay
	 /      fnumi->ndecay   = gnumi->PassThroughInfo().ndecay;
      */ ////
      
      double Nenergy = fBooneNtp.ini_eng[0] ;
      double Ndxdz   = fBooneNtp.ini_mom[0][0]/fBooneNtp.ini_mom[0][2] ;
      double Ndydz   = fBooneNtp.ini_mom[0][1]/fBooneNtp.ini_mom[0][2] ;
      double Npz     = fBooneNtp.ini_mom[0][2] ;
      
      double ppenergy = fBooneNtp.ini_eng[1];
      double pdPx     = fBooneNtp.fin_mom[1][0] ;
      double pdPy     = fBooneNtp.fin_mom[1][1] ;
      double pdPz     = fBooneNtp.fin_mom[1][2] ;
      
      double ppvx     = fBooneNtp.ini_pos[1][0] ;
      double ppvy     = fBooneNtp.ini_pos[1][1] ;
      double ppvz     = fBooneNtp.ini_pos[1][2] ;    
      
      double ppdxdz   = fBooneNtp.ini_mom[1][0]/fBooneNtp.ini_mom[1][2] ;
      double ppdydz   = fBooneNtp.ini_mom[1][1]/fBooneNtp.ini_mom[1][2] ;
      double pppz     = fBooneNtp.ini_mom[1][2] ;
      
      //Get the neutrino energy in the parent decay cm
      double parent_mass=sqrt(ppenergy*ppenergy-
			      pppz*pppz*(ppdxdz*ppdxdz +
					 ppdydz*ppdydz +
					 1.));
      
      double parent_energy = sqrt(pdPx*pdPx +
				  pdPy*pdPy +
				  pdPz*pdPz + 
				  parent_mass*parent_mass);
      
      double gamma         = parent_energy / parent_mass;
      double beta[3];
      beta[0] = pdPx/parent_energy;
      beta[1] = pdPy/parent_energy;
      beta[2] = pdPz/parent_energy;

      double partial = fBooneNtp.ini_mom[0][2] * gamma * ( beta[0] * Ndxdz + 
							   beta[1] * Ndydz + 
							   beta[2] );

      double Necm = gamma * Nenergy - partial;
      
      if (fBooneNtp.id[1] == 10 && fBooneNtp.ntp == 1) 
	fnumi->ndecay = 1;
      else if (fBooneNtp.id[1] == 10 && fBooneNtp.ntp == 2) 
	fnumi->ndecay = 2;
      else if (fBooneNtp.id[1] == 10 && fBooneNtp.ntp == 3) 
	fnumi->ndecay = 3;
      else if (fBooneNtp.id[1] == 10 && fBooneNtp.ntp == 4) 
	fnumi->ndecay = 4;
      else if (fBooneNtp.id[1] == 11 && fBooneNtp.ntp == 3) {
	//check if it is a two or three body decay
	if (fabs((parent_mass*parent_mass-0.105658389*0.105658389)/(2.*parent_mass)-Necm)/Necm <= 0.001)
	  //two body decay (numu + mu+)
	  fnumi->ndecay = 5;
	else
	  //three body decay (numu + pi0 + mu+)
	  fnumi->ndecay = 7;
      } else if (fBooneNtp.id[1] == 11 && fBooneNtp.ntp == 1) 
	fnumi->ndecay = 6;
      else if (fBooneNtp.id[1] == 12 && fBooneNtp.ntp == 4) {
	if (fabs((parent_mass*parent_mass-0.105658389*0.105658389)/(2.*parent_mass)-Necm)/Necm <= 0.001)
	  //two body decay (numu + mu+)
	  fnumi->ndecay = 8;
	else
	  //three body decay (numu + pi0 + mu+)
	  fnumi->ndecay = 10;
      } else if (fBooneNtp.id[1] == 12 && fBooneNtp.ntp == 2) 
	fnumi->ndecay = 9;
      else if (fBooneNtp.id[1] == 5 ) 
	fnumi->ndecay = 11;
      else if (fBooneNtp.id[1] == 6 ) 
	fnumi->ndecay = 12;
      else if (fBooneNtp.id[1] == 8 ) 
	fnumi->ndecay = 13;
      else if (fBooneNtp.id[1] == 9 ) 
	fnumi->ndecay = 14;
 
      //ptype and tptype
      int ptype_input = fBooneNtp.id[1];
      int tptype_input = fBooneNtp.id[npart-2];
           
      if ( ptype_input != 0 ) ptype_input = genie::pdg::GeantToPdg(ptype_input);
      if ( tptype_input!= 0 ) tptype_input= genie::pdg::GeantToPdg(tptype_input);
     
      fnumi->ptype    = ptype_input;
      fnumi->tptype   = tptype_input; // npart-2 id

      fnumi->ppmedium = 0.; // empty

      
      faux->auxint.push_back(fBooneNtp.npart);//tgen     
      faux->auxint.push_back(-9999.);//tgptype
      
      faux->auxdbl.push_back(-9999.);//fgXYWgt    
      faux->auxdbl.push_back(fBooneNtp.beamwgt);//nimpwt
      
      double mupare;
      double muparpx;
      double muparpy;
      double muparpz;
      
      if ( fBooneNtp.id[1] == 5 ||
	   fBooneNtp.id[1] == 6) {
	mupare  = fBooneNtp.ini_eng[2];
	muparpx = fBooneNtp.fin_mom[2][0];
	muparpy = fBooneNtp.fin_mom[2][1];
	muparpz = fBooneNtp.fin_mom[2][2];
      } else {
	mupare  = -9999.;
	muparpx = -9999.;
	muparpy = -9999.;
	muparpz = -9999.;
      }
      
      faux->auxdbl.push_back(muparpx);//muparpx
      faux->auxdbl.push_back(muparpy);//muparpy
      faux->auxdbl.push_back(muparpz);//muparpz
      faux->auxdbl.push_back(mupare);//mupare
      faux->auxdbl.push_back(Necm);//necm
      //beam px, py, pz, E
      faux->auxdbl.push_back(fBooneNtp.ini_mom[npart-1][0]);
      faux->auxdbl.push_back(fBooneNtp.ini_mom[npart-1][1]);
      faux->auxdbl.push_back(fBooneNtp.ini_mom[npart-1][2]);
      faux->auxdbl.push_back(fBooneNtp.ini_eng[npart-1]);
      
      if ( fentry->E > 0 ) ++nok;
      fluxntp->Fill();
      ++nwrite;
      
      pdglist.insert(fentry->pdg);
      minwgt = TMath::Min(MinWeight,fentry->wgt);
      maxwgt = TMath::Max(MaxWeight,fentry->wgt);
      maxe   = TMath::Max(maxe,fentry->E);
      
      fEventNo++;
      
      SumPOT += (fBeamNtp.pot / SumWeights);

      if(SumPOT > POTGoal) {
	std::cout << "internal fraction: " << double(iev) / double(fBooneNevents) << std::endl;
	std::cout << "external fraction: " << iter              << std::endl;
	break;
      } 
    }
    
    if(SumPOT > POTGoal) break; 
  
  }
  std::cout << "N wrote " << nwrite << " to Entries " <<  fBooneNevents <<  " with POT " << SumPOT << std::endl;  

  std::cout << "Fraction of events written : " << (double(nwrite)/double(fBooneNevents)) << std::endl;
  std::cout << "Fraction of events written over sum of weights : " << (nwrite/SumWeights) << std::endl;

  std::cout << "Fraction of POT used       : " << SumPOT/fBeamNtp.pot << std::endl; 

  fmeta->pdglist.clear();
  set<int>::const_iterator setitr = pdglist.begin();
  for ( ; setitr != pdglist.end(); ++setitr)  fmeta->pdglist.push_back(*setitr);
  
  fmeta->maxEnergy = maxe;
  fmeta->minWgt    = minwgt;
  fmeta->maxWgt    = maxwgt;

  fWindowtree->GetEntry(0);   

  fmeta->protons   = nwrite*(fBeamNtp.pot / SumWeights);
  TVector3 p0, p1, p2;
  p0 = TVector3(fBeamNtp.windowbase[0],
                fBeamNtp.windowbase[1],
                fBeamNtp.windowbase[2]);
  p1 = p0 + TVector3(fBeamNtp.windowdir1[0],
                     fBeamNtp.windowdir1[1],
                     fBeamNtp.windowdir1[2]);
  p2 = p0 + TVector3(fBeamNtp.windowdir2[0],
                     fBeamNtp.windowdir2[1],
                     fBeamNtp.windowdir2[2]);

  TVector3 d1 = p1 - p0;
  TVector3 d2 = p2 - p0;
  fmeta->windowBase[0] = p0.X();
  fmeta->windowBase[1] = p0.Y();
  fmeta->windowBase[2] = p0.Z();
  fmeta->windowDir1[0] = d1.X();
  fmeta->windowDir1[1] = d1.Y();
  fmeta->windowDir1[2] = d1.Z();
  fmeta->windowDir2[0] = d2.X();
  fmeta->windowDir2[1] = d2.Y();
  fmeta->windowDir2[2] = d2.Z();
  
  // Poop //
  
  fmeta->auxintname.push_back("tgen");
  fmeta->auxintname.push_back("tgptype");
  
  fmeta->auxdblname.push_back("fgXYWgt");
  fmeta->auxdblname.push_back("nimpwt");
  fmeta->auxdblname.push_back("muparpx");
  fmeta->auxdblname.push_back("muparpy");
  fmeta->auxdblname.push_back("muparpz");
  fmeta->auxdblname.push_back("mupare");
  fmeta->auxdblname.push_back("necm");
  //beam px, py, pz, E
  fmeta->auxdblname.push_back("beampx");
  fmeta->auxdblname.push_back("beampy");
  fmeta->auxdblname.push_back("beampz");
  fmeta->auxdblname.push_back("beame");


  // Poop //
  /*
  std::string rotinfo, posinfo;
  encode_transform(gnumi,rotinfo,posinfo);
  fmeta->infiles.push_back(rotinfo);
  fmeta->infiles.push_back(posinfo);
  
  if ( srcpatt != "" ) {
    string srcstring = std::string("srcpatt=") + srcpatt;
    fmeta->infiles.push_back(srcstring);
  }
  
  string fname = std::string("flux_pattern=") + fluxpatt; // fluxfname;
  fmeta->infiles.push_back(fname);
  // should get this expanded from gnumi
  
  vector<string> flist = gnumi->GetFileList();
  for (size_t i = 0; i < flist.size(); ++i) {
    const char* onefile = flist[i].c_str();
    fname = std::string("flux_infile=") +
      std::string(gSystem->BaseName(onefile));
    fmeta->infiles.push_back(fname);
  }
  */
  
  fmeta->seed    = rnd->GetSeed();
  fmeta->metakey = metakey;
  
  metantp->Fill();
      
}
