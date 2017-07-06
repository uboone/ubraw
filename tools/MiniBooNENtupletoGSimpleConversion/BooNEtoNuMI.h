#ifndef BooNEtoNuMI_h
#define BooNEtoNuMI_h

#include <string>

#include "BooNENtuple.h"
#include "NuMINtuple.h"

class TTree;
class TFile;

class BooNEtoNuMI {
 
 public :
  BooNEtoNuMI();
  virtual ~BooNEtoNuMI();

  void DoIt();
  void OpenFiles(char* input_file, char* output_file);
  void CloseFiles();
  void ConvertBooNEtoNuMI(BooNENtuple &boone_ntp, NuMINtuple &numi_ntp);

 private:
  NuMINtuple  fNumiNtp;
  BooNENtuple fBooneNtp;

  int fBooneNevents;
  int fNumiNevents;

  TFile* fBooNEfile;
  TTree* fBNBtree;

  TFile* fNuMIfile;
  TTree* fNuMItree;
  int fEventNo;
};

#endif
