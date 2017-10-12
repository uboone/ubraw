#ifndef _GETFOM2_H
#define _GETFOM2_H

#include "datatypes/ub_BeamHeader.h"
#include "datatypes/ub_BeamData.h"
#include <string>
#include "TMath.h"
#include "TH1.h"

namespace bmd
{
  float getFOM2(std::string beam, const  gov::fnal::uboone::datatypes::ub_BeamHeader& bh, const std::vector<gov::fnal::uboone::datatypes::ub_BeamData>& bd);
  
  double calcFOM2(double horpos,double horang,double verpos,double verang,double tor,double tgtsx,double tgtsy);
  void swimBNB(const double centroid1[6], const double sigma1[6][6], 
	       const double xferc[6][6], const double xfers[6][6],
	       double &cx, double& cy, double &sx, double &sy, double &rho);
  double func_intbivar(const double cx, const double cy, const double sx, const double sy, const double rho );
  void processBNBprofile(const double* mwdata, double &x, double& sx, double& chi2); 
  

}

#endif
