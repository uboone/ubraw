#ifndef _GETFOM2_H
#define _GETFOM2_H

#include "datatypes/ub_BeamHeader.h"
#include "datatypes/ub_BeamData.h"
#include <string>
#include "TMath.h"
#include "TH1.h"
#include "TGraph.h"

namespace bmd
{
  float getFOM2(std::string beam, const  gov::fnal::uboone::datatypes::ub_BeamHeader& bh, const std::vector<gov::fnal::uboone::datatypes::ub_BeamData>& bd);
  
  double calcFOM2(double horpos,double horang,double verpos,double verang,double tor,double tgtsx,double tgtsy);
  void swimBNB(const double centroid1[6], const double sigma1[6][6], 
	       const double xferc[6][6], const double xfers[6][6],
	       double &cx, double& cy, double &sx, double &sy, double &rho);
  double func_intbivar(const double cx, const double cy, const double sx, const double sy, const double rho );
  void processBNBprofile(const double* mwdata, double &x, double& sx, double& chi2); 
  
  //NuMI code copied from NOvA http://nusoft.fnal.gov/nova/novasoft/doxygen/html/IFDBSpillInfo__module_8cc_source.html
  double NuMIExtrapolatePosition(double t1, double z1, double t2, double z2, double z3);
  void NuMIBpmProjection(std::vector<double> &xp, std::vector<double> &yp, 
			 std::vector<double> &xi, std::vector<double> &yi,
			 std::vector<std::vector<double>> BPMS);
  double NuMIBpmAtTarget(double &xpmean, double &ypmean, 
			 double &xpstdev, double &ypstdev,
			 std::vector<double>TargBpmX,
			 std::vector<double>TargBpmY,
			 std::vector<double>BpmIntX, 
			 std::vector<double>BpmIntY);
  int NuMIProfileProjection(double &x, double &y, 
			    double &xstdev, double &ystdev,
			    std::vector<double> PM121,
			    std::vector<double> PMTGT);
  double NuMIGetGaussFit(double &mean, double &sigma, std::vector<double> profile);
  double NuMIGetStats(TGraph *prof, double &mean, double &stdev);

}

#endif
