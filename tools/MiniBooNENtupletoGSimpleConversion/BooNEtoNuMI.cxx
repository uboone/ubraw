#include <iostream>
#include <stdlib.h>
#include <cmath>

//BooNEtoNuMI stuff
#include "BooNEtoNuMI.h"
#include "BooNENtuple.h"
#include "NuMINtuple.h"

//root stuff
#include <TROOT.h>
#include "TStopwatch.h"
#include "TDatime.h"
#include "TTree.h"
#include "TFile.h"

using namespace std;

BooNEtoNuMI::BooNEtoNuMI()
{
}

BooNEtoNuMI::~BooNEtoNuMI()
{
}

void BooNEtoNuMI::OpenFiles(char* input_file, char* output_file)
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

  fNuMIfile=new TFile(output_file,"RECREATE");
  fNuMItree=new TTree("h10","Neutrino ntuple");
  fNuMItree->Branch("run",&fNumiNtp.run,"run/I");
  fNuMItree->Branch("evtno",&fNumiNtp.evtno,"evtno/I");
  fNuMItree->Branch("Ndxdz",&fNumiNtp.Ndxdz,"Ndxdz/F");
  fNuMItree->Branch("Ndydz",&fNumiNtp.Ndydz,"Ndydz/F");
  fNuMItree->Branch("Npz",&fNumiNtp.Npz,"Npz/F");
  fNuMItree->Branch("Nenergy",&fNumiNtp.Nenergy,"Nenergy/F");
  fNuMItree->Branch("Ndxdznea",&fNumiNtp.NdxdzNea,"NdxdzNea/F");
  fNuMItree->Branch("Ndydznea",&fNumiNtp.NdydzNea,"NdydzNea/F");
  fNuMItree->Branch("Nenergyn",&fNumiNtp.NenergyN,"NenergyN/F");
  fNuMItree->Branch("Nwtnear",&fNumiNtp.NWtNear,"NWtNear/F");
  fNuMItree->Branch("Ndxdzfar",&fNumiNtp.NdxdzNea,"NdxdzFar/F");
  fNuMItree->Branch("Ndydzfar",&fNumiNtp.NdydzNea,"NdydzFar/F");
  fNuMItree->Branch("Nenergyf",&fNumiNtp.NenergyN,"NenergyF/F");
  fNuMItree->Branch("Nwtfar",&fNumiNtp.NWtFar,"NWtFar/F");
  fNuMItree->Branch("Norig",&fNumiNtp.Norig,"Norig/I");
  fNuMItree->Branch("Ndecay",&fNumiNtp.Ndecay,"Ndecay/I");
  fNuMItree->Branch("Ntype",&fNumiNtp.Ntype,"Ntype/I");
  fNuMItree->Branch("Vx",&fNumiNtp.Vx,"Vx/F");
  fNuMItree->Branch("Vy",&fNumiNtp.Vy,"Vy/F");
  fNuMItree->Branch("Vz",&fNumiNtp.Vz,"Vz/F");
  fNuMItree->Branch("pdpx",&fNumiNtp.pdPx,"pdPx/F");
  fNuMItree->Branch("pdpy",&fNumiNtp.pdPy,"pdPy/F");
  fNuMItree->Branch("pdpz",&fNumiNtp.pdPz,"pdPz/F");
  fNuMItree->Branch("ppdxdz",&fNumiNtp.ppdxdz,"ppdxdz/F");
  fNuMItree->Branch("ppdydz",&fNumiNtp.ppdydz,"ppdydz/F");
  fNuMItree->Branch("pppz",&fNumiNtp.pppz,"pppz/F");
  fNuMItree->Branch("ppenergy",&fNumiNtp.ppenergy,"ppenergy/F");
  fNuMItree->Branch("ppmedium",&fNumiNtp.ppmedium,"ppmedium/I");
  fNuMItree->Branch("ptype",&fNumiNtp.ptype,"ptype/I");
  fNuMItree->Branch("ppvx",&fNumiNtp.ppvx,"ppvx/F");
  fNuMItree->Branch("ppvy",&fNumiNtp.ppvy,"ppvy/F");
  fNuMItree->Branch("ppvz",&fNumiNtp.ppvz,"ppvz/F");
  fNuMItree->Branch("muparpx",&fNumiNtp.muparpx,"muparpx/F");
  fNuMItree->Branch("muparpy",&fNumiNtp.muparpy,"muparpy/F");
  fNuMItree->Branch("muparpz",&fNumiNtp.muparpz,"muparpz/F");
  fNuMItree->Branch("mupare",&fNumiNtp.mupare,"mupare/F");
  fNuMItree->Branch("Necm",&fNumiNtp.Necm,"Necm/F");
  fNuMItree->Branch("Nimpwt",&fNumiNtp.Nimpwt,"Nimpwt/F");
  fNuMItree->Branch("xpoint",&fNumiNtp.xpoint,"xpoint/F");
  fNuMItree->Branch("ypoint",&fNumiNtp.ypoint,"ypoint/F");
  fNuMItree->Branch("zpoint",&fNumiNtp.zpoint,"zpoint/F");
  fNuMItree->Branch("tvx",&fNumiNtp.tvx,"tvx/F");
  fNuMItree->Branch("tvy",&fNumiNtp.tvy,"tvy/F");
  fNuMItree->Branch("tvz",&fNumiNtp.tvz,"tvz/F");
  fNuMItree->Branch("tpx",&fNumiNtp.tpx,"tpx/F");
  fNuMItree->Branch("tpy",&fNumiNtp.tpy,"tpy/F");
  fNuMItree->Branch("tpz",&fNumiNtp.tpz,"tpz/F");
  fNuMItree->Branch("tptype",&fNumiNtp.tptype,"tptype/I");
  fNuMItree->Branch("tgen",&fNumiNtp.tgen,"tgen/I");
  
  fNuMItree->Branch("tgptype",&fNumiNtp.tgptype,"tgptype/I");
  
  fNuMItree->Branch("tgppx",&fNumiNtp.tgppx,"tgppx/F");
  fNuMItree->Branch("tgppy",&fNumiNtp.tgppy,"tgppy/F");
  fNuMItree->Branch("tgppz",&fNumiNtp.tgppz,"tgppz/F");
  fNuMItree->Branch("tprivx",&fNumiNtp.tprivx,"tprivx/F");

  fNuMItree->Branch("tprivy",&fNumiNtp.tprivy,"tprivy/F");
  fNuMItree->Branch("tprivz",&fNumiNtp.tprivz,"tprivz/F");
  fNuMItree->Branch("beamx",&fNumiNtp.beamx,"beamx/F");
  fNuMItree->Branch("beamy",&fNumiNtp.beamy,"beamy/F");
  fNuMItree->Branch("beamz",&fNumiNtp.beamz,"beamz/F");
 
  fNuMItree->Branch("beampx",&fNumiNtp.beampx,"beampx/F");
  fNuMItree->Branch("beampy",&fNumiNtp.beampy,"beampy/F");
  fNuMItree->Branch("beampz",&fNumiNtp.beampz,"beampz/F");
 
}

void BooNEtoNuMI::CloseFiles()
{
  //Numi ntuple
  fNuMItree->Write();
  fNuMIfile->Close();

  //Boone ntuple
  fBooNEfile->Close();
}

void BooNEtoNuMI::DoIt()
{
  fEventNo=0;
  fBooneNevents=fBNBtree->GetEntries();
  cout << "Reading " << fBooneNevents << " events" << endl;
  for ( int iev = 0 ; iev < fBooneNevents ; iev++ ) {
    fBNBtree->GetEntry(iev);
    ConvertBooNEtoNuMI(fBooneNtp, fNumiNtp);
    fEventNo++;
    
    fNuMItree->Fill();
  }
}


void BooNEtoNuMI::ConvertBooNEtoNuMI(BooNENtuple &in_ntp, NuMINtuple &out_ntp)
{
  //the order of indices is reversed (3,npart) -> [npart][3]
  //also index shifted in c++ (1, 2, ... -> 0, 1, ...)
  int npart = in_ntp.npart;    
  //id(0)     = 4 (neutrino)
  //id(1)     = 5,6,8,9,10,11,12 (nu parent)
  //..
  //id(npart-1) = 14 (primary proton)
  out_ntp.evtno    = fEventNo;
  out_ntp.beamx    = in_ntp.ini_pos[npart-1][0];
  out_ntp.beamy    = in_ntp.ini_pos[npart-1][1];
  out_ntp.beamz    = in_ntp.ini_pos[npart-1][2];
  
  out_ntp.beampx   = in_ntp.ini_mom[npart-1][0];
  out_ntp.beampy   = in_ntp.ini_mom[npart-1][1];
  out_ntp.beampz   = in_ntp.ini_mom[npart-1][2];
  out_ntp.tgen     = in_ntp.npart;
  //tvX and tpX should be position and momenta of the particle leaving the target
  //I'll just fill in the values for first particle created in primary proton interaction.
  out_ntp.tvx      = in_ntp.ini_pos[npart-2][0];
  out_ntp.tvy      = in_ntp.ini_pos[npart-2][1];
  out_ntp.tvz      = in_ntp.ini_pos[npart-2][2];
  out_ntp.tpx      = in_ntp.ini_mom[npart-2][0];
  out_ntp.tpy      = in_ntp.ini_mom[npart-2][1];
  out_ntp.tpz      = in_ntp.ini_mom[npart-2][2];
  out_ntp.tptype   = in_ntp.id[npart-2];

  out_ntp.tprivx   = in_ntp.ini_pos[npart-2][0];
  out_ntp.tprivy   = in_ntp.ini_pos[npart-2][1];
  out_ntp.tprivz   = in_ntp.ini_pos[npart-2][2];
  
  out_ntp.Vx       = in_ntp.ini_pos[0][0] ;
  out_ntp.Vy       = in_ntp.ini_pos[0][1] ;
  out_ntp.Vz       = in_ntp.ini_pos[0][2] ;
  out_ntp.pdPx     = in_ntp.fin_mom[1][0] ;
  out_ntp.pdPy     = in_ntp.fin_mom[1][1] ;
  out_ntp.pdPz     = in_ntp.fin_mom[1][2] ;
  
  out_ntp.ppvx     = in_ntp.ini_pos[1][0] ;
  out_ntp.ppvy     = in_ntp.ini_pos[1][1] ;
  out_ntp.ppvz     = in_ntp.ini_pos[1][2] ;
  out_ntp.ppdxdz   = in_ntp.ini_mom[1][0]/in_ntp.ini_mom[1][2] ;
  out_ntp.ppdydz   = in_ntp.ini_mom[1][1]/in_ntp.ini_mom[1][2] ;
  out_ntp.pppz     = in_ntp.ini_mom[1][2] ;
  out_ntp.ppenergy = in_ntp.ini_eng[1] ;
  
  out_ntp.ptype    = in_ntp.id[1] ; 
 
  //if parent is a muon fill muon parent info
  if ( in_ntp.id[1] == 5 ||
       in_ntp.id[1] == 6) {
    out_ntp.mupare  = in_ntp.ini_eng[2];
    out_ntp.muparpx = in_ntp.fin_mom[2][0];
    out_ntp.muparpy = in_ntp.fin_mom[2][1];
    out_ntp.muparpz = in_ntp.fin_mom[2][2];
  } else {
    out_ntp.mupare  = -9999.;
    out_ntp.muparpx = -9999.;
    out_ntp.muparpy = -9999.;
    out_ntp.muparpz = -9999.;
  }

  //NEUTRINO info:
  if ( in_ntp.ntp == 1 )
    out_ntp.Ntype = 53 ;
  else if ( in_ntp.ntp == 2 )
    out_ntp.Ntype = 52 ;
  else if ( in_ntp.ntp == 3 )
    out_ntp.Ntype = 56 ;
  else if ( in_ntp.ntp == 4 )
    out_ntp.Ntype = 55 ;
  else
    cout<<"Neutrino type not recognized!!! ntp = "<<in_ntp.ntp<<endl;
  
  out_ntp.Nenergy = in_ntp.ini_eng[0] ;
  out_ntp.Ndxdz   = in_ntp.ini_mom[0][0]/in_ntp.ini_mom[0][2] ;
  out_ntp.Ndydz   = in_ntp.ini_mom[0][1]/in_ntp.ini_mom[0][2] ;
  out_ntp.Npz     = in_ntp.ini_mom[0][2] ;
  
  //Get the neutrino energy in the parent decay cm
  double parent_mass=sqrt(out_ntp.ppenergy*out_ntp.ppenergy-
			  out_ntp.pppz*out_ntp.pppz*(out_ntp.ppdxdz*out_ntp.ppdxdz +
						     out_ntp.ppdydz*out_ntp.ppdydz +
						     1.));
  double parent_energy = sqrt(out_ntp.pdPx*out_ntp.pdPx +
			      out_ntp.pdPy*out_ntp.pdPy +
			      out_ntp.pdPz*out_ntp.pdPz + parent_mass*parent_mass);
  
  double gamma         = parent_energy / parent_mass;
  double beta[3];
  beta[0] = out_ntp.pdPx/parent_energy;
  beta[1] = out_ntp.pdPy/parent_energy;
  beta[2] = out_ntp.pdPz/parent_energy;
  double partial = in_ntp.ini_mom[0][2] * gamma * ( beta[0] * out_ntp.Ndxdz + beta[1] * out_ntp.Ndydz + beta[2] );
  out_ntp.Necm = gamma * out_ntp.Nenergy - partial;
  
  out_ntp.Nimpwt = in_ntp.beamwgt;

  //Fill Ndecay (check parent type, neutrino type and if it is a 2 or 3 body decay)
  if (in_ntp.id[1] == 10 && in_ntp.ntp == 1) 
    out_ntp.Ndecay = 1;
  else if (in_ntp.id[1] == 10 && in_ntp.ntp == 2) 
    out_ntp.Ndecay = 2;
  else if (in_ntp.id[1] == 10 && in_ntp.ntp == 3) 
    out_ntp.Ndecay = 3;
  else if (in_ntp.id[1] == 10 && in_ntp.ntp == 4) 
    out_ntp.Ndecay = 4;
  else if (in_ntp.id[1] == 11 && in_ntp.ntp == 3) {
    //check if it is a two or three body decay
    if (fabs((parent_mass*parent_mass-0.105658389*0.105658389)/(2.*parent_mass)-out_ntp.Necm)/out_ntp.Necm <= 0.001)
      //two body decay (numu + mu+)
      out_ntp.Ndecay = 5;
    else
      //three body decay (numu + pi0 + mu+)
      out_ntp.Ndecay = 7;
  } else if (in_ntp.id[1] == 11 && in_ntp.ntp == 1) 
    out_ntp.Ndecay = 6;
  else if (in_ntp.id[1] == 12 && in_ntp.ntp == 4) {
    if (fabs((parent_mass*parent_mass-0.105658389*0.105658389)/(2.*parent_mass)-out_ntp.Necm)/out_ntp.Necm <= 0.001)
      //two body decay (numu + mu+)
      out_ntp.Ndecay = 8;
    else
      //three body decay (numu + pi0 + mu+)
       out_ntp.Ndecay = 10;
  } else if (in_ntp.id[1] == 12 && in_ntp.ntp == 2) 
    out_ntp.Ndecay = 9;
  else if (in_ntp.id[1] == 5 ) 
     out_ntp.Ndecay = 11;
  else if (in_ntp.id[1] == 6 ) 
    out_ntp.Ndecay = 12;
  else if (in_ntp.id[1] == 8 ) 
    out_ntp.Ndecay = 13;
  else if (in_ntp.id[1] == 9 ) 
    out_ntp.Ndecay = 14;


}

