#ifndef BooNEtoGSimple_h
#define BooNEtoGSimple_h

#include <string>
#include <iostream>

#include "BooNENtuple.h"
#include "BeamNtuple.h"

#include "FluxDrivers/GNuMIFlux.h"
#include "FluxDrivers/GSimpleNtpFlux.h"


class TTree;
class TFile;

class BooNEtoGSimple {
 
 public :
  BooNEtoGSimple();
  virtual ~BooNEtoGSimple();

  void DoIt(char* input_file, char* output_file, long int POTGoal);
  void OpenFiles(char* input_file, char* output_file);
  void CloseFiles();

 private:
  BooNENtuple fBooneNtp;
  BeamNtuple fBeamNtp;
  genie::flux::GSimpleNtpEntry* fentry;
  genie::flux::GSimpleNtpNuMI*  fnumi;
  genie::flux::GSimpleNtpAux*   faux;
  genie::flux::GSimpleNtpMeta*  fmeta;
  
  int fBooneNevents;

  int fNumiNevents;

  TFile* fBooNEfile;
  TTree* fBNBtree;
  TTree* fWindowtree;
  
  TFile* fGSimplefile;

  TTree* fluxntp;
  TTree* metantp;

  int fEventNo;
};

#endif
