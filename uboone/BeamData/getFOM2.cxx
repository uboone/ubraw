#include "getFOM2.h"
#include <iostream>
#include <math.h>
#include "TH1D.h"
#include "TF1.h"

using namespace gov::fnal::uboone::datatypes;

float bmd::getFOM2(std::string beam, const ub_BeamHeader& bh, const std::vector<ub_BeamData>& bd)
{
  double fom=0;
  if (beam=="bnb") {
    
    double hp875_offset= -3.40271;
    double vp875_offset= +1.47511;
    double hptg1_offset= +0.457014;
    double vptg1_offset= +0.38914;
    double hptg2_offset= +0.8;
    double vptg2_offset= +1.0;
    //double hptg2_offset= +0.79;
    //double vptg2_offset= +1.02;
    double hp875_zpos= 202.116104;
    double vp875_zpos= 202.3193205;
    double hptg1_zpos= 204.833267;
    double vptg1_zpos= 204.629608;
    double hptg2_zpos= 205.240662;
    double vptg2_zpos= 205.036835;
    double target_center_zpos= 206.870895;

    double p875x[]={0.431857, 0.158077, 0.00303551};
    double p875y[]={0.279128, 0.337048, 0};
    double p876x[]={0.166172, 0.30999, -0.00630299};
    double p876y[]={0.13425, 0.580862, 0};
    
    std::vector<double> tor860;
    std::vector<double> tor875;
    std::vector<double> hp875;
    std::vector<double> vp875;
    std::vector<double> hptg1;
    std::vector<double> vptg1;
    std::vector<double> hptg2;
    std::vector<double> vptg2;
    std::vector<double> mw875;
    std::vector<double> mw876;
    std::vector<double> mwtgt;

    for(auto& bdata : bd) {	// get toroid, BPM, and multiwire data
      if(bdata.getDeviceName().find("E:TOR860") != std::string::npos) {	// get E:TOR860 reading	
	tor860 = bdata.getData();
	continue;
      } else if(bdata.getDeviceName().find("E:TOR875") != std::string::npos) {	// get E:TOR875 reading
	tor875 = bdata.getData();
	continue;
      }	else if(bdata.getDeviceName().find("E:HP875") != std::string::npos) {	// get E:HP875 reading
	hp875 = bdata.getData();
	continue;
      }	else if(bdata.getDeviceName().find("E:VP875") != std::string::npos) {	// get E:VP875 reading
	vp875 = bdata.getData();
	continue;
      }	else if(bdata.getDeviceName().find("E:HPTG1") != std::string::npos) {	// get E:HPTG1 reading
	hptg1 = bdata.getData();
	continue;
      } else if(bdata.getDeviceName().find("E:VPTG1") != std::string::npos) {	// get E:VPTG1 reading
	vptg1 = bdata.getData();
	continue;
      }	else if(bdata.getDeviceName().find("E:HPTG2") != std::string::npos) {	// get E:HPTG1 reading
	hptg2 = bdata.getData();
	continue;
      } else if(bdata.getDeviceName().find("E:VPTG2") != std::string::npos) {	// get E:VPTG1 reading
	vptg2 = bdata.getData();
	continue;
      } else if(bdata.getDeviceName().find("E:M875BB") != std::string::npos) {	// get horizontal and vertical target multiwire reading
	mw875.resize(96);
	for (int ii=0;ii<96;ii++) 
	  mw875[ii]=bdata.getData()[ii];
	continue;
      } else if(bdata.getDeviceName().find("E:M876BB") != std::string::npos) {	// get horizontal and vertical target multiwire reading
	mw876.resize(96);
	for (int ii=0;ii<96;ii++) 
	  mw876[ii]=bdata.getData()[ii];
	continue;
      } else if(bdata.getDeviceName().find("E:MMBTBB") != std::string::npos) {	// get horizontal and vertical target multiwire reading
	mwtgt.resize(96);
	for (int ii=0;ii<96;ii++) 
	  mwtgt[ii]=bdata.getData()[ii];
	continue;    
      }
    }
    double tor;
    if (tor860.size()>0) 
      tor=tor860[0];
    else if (tor875.size()>0) 
      tor=tor875[0];
    else
      return -1;

    double horang,horpos;
    if (hptg1.size()>0 && hp875.size()>0) {
      horang=((hptg1[0]-hptg1_offset)-(hp875[0]-hp875_offset))/(hptg1_zpos-hp875_zpos);
      horpos=(hp875[0]-hp875_offset)+horang*(target_center_zpos-hp875_zpos);
    } else if (hptg2.size()>0 && hp875.size()>0) {
      horang=((hptg2[0]-hptg2_offset)-(hp875[0]-hp875_offset))/(hptg2_zpos-hp875_zpos);
      horpos=(hp875[0]-hp875_offset)+horang*(target_center_zpos-hp875_zpos);
    } else {
      //missing horizontal BPM data
      return 2;
    } 

    double verang,verpos;
    if (vptg1.size()>0 && vp875.size()>0) {
      verang=((vptg1[0]-vptg1_offset)-(vp875[0]-vp875_offset))/(vptg1_zpos-vp875_zpos);
      verpos=(vp875[0]-vp875_offset)+verang*(target_center_zpos-vp875_zpos);
    } else if (vptg2.size()>0 && vp875.size()>0) {
      verang=((vptg2[0]-vptg2_offset)-(vp875[0]-vp875_offset))/(vptg2_zpos-vp875_zpos);
      verpos=(vp875[0]-vp875_offset)+verang*(target_center_zpos-vp875_zpos);
    } else {
      //missing vertical BPM data
      return 3;
    } 
    
    horang=atan(horang);
    verang=atan(verang);

    double xx,yy,sx,sy,chi2x,chi2y;
    double tgtsx, tgtsy;
    if (bh.getSeconds()<1463677200) {
      //skip this if data from when tgt multiwire was already dead
      bmd::processBNBprofile(&mwtgt[0], xx, sx,chi2x);
      bmd::processBNBprofile(&mwtgt[48], yy, sy, chi2y);
    } 
    //target multiwire started failing after May 20 2016 (1463677200)
    if (bh.getSeconds()<1463677200 && sx>0.5 && sx<10 && sy>0.3 && sy<10 && chi2x<20 && chi2y<20) {
      tgtsx=sx;
      tgtsy=sy;
    } else {
      bmd::processBNBprofile(&mw876[0], xx,sx,chi2x);
      bmd::processBNBprofile(&mw876[48], yy,sy,chi2y);
      double tgtsx876=p876x[0]+p876x[1]*sx+p876x[2]*sx*sx;
      double tgtsy876=p876y[0]+p876y[1]*sy+p876y[2]*sy*sy;
      if (tgtsx876>0.5 && tgtsx876<10 && tgtsy876>0.3 && tgtsy876<10 && chi2x<20 && chi2y<20) {
	tgtsx=tgtsx876;
	tgtsy=tgtsy876;
      } else {
	bmd::processBNBprofile(&mw875[0], xx,sx,chi2x);
	bmd::processBNBprofile(&mw875[48], yy,sy,chi2y);
	double tgtsx875=p875x[0]+p875x[1]*sx+p875x[2]*sx*sx;
	double tgtsy875=p875y[0]+p875y[1]*sy+p875y[2]*sy*sy;
	if (tgtsx875>0.5 && tgtsx875<10 && tgtsy875>0.3 && tgtsy875<10 && chi2x<20 && chi2y<20) {
	  tgtsx=tgtsx875;
	  tgtsy=tgtsy875;
	} else {
	  //failed getting  multiwire data
	  return 4;
	}
      }
    }   

    fom=1-pow(10,bmd::calcFOM2(horpos,horang,verpos,verang,tor,tgtsx,tgtsy));
    
  } else if (beam=="numi") {
    //cuts copied from https://cdcvs.fnal.gov/redmine/projects/novaart/repository/entry/trunk/IFDBSpillInfo/IFDBSpillInfo.fcl
    double fMinPosXCut   = -2.00; //   #mm
    double fMaxPosXCut   =  2.00; //   #mm
    double fMinPosYCut   = -2.00; //   #mm
    double fMaxPosYCut   =  2.00; //   #mm
    double fMinWidthXCut =  0.57; //   #mm
    double fMaxWidthXCut =  1.58; //   #mm
    double fMinWidthYCut =  0.57; //   #mm
    double fMaxWidthYCut =  1.58; //   #mm

    std::vector<double> tor101;
    std::vector<double> tortgt;
    std::vector<double> hp121;
    std::vector<double> vp121;
    std::vector<double> hptgt;
    std::vector<double> vptgt;
    std::vector<double> hitgt;
    std::vector<double> vitgt;
    std::vector<double> mw121;
    std::vector<double> mwtgt;

    for(auto& bdata : bd) {	// get toroid, BPM, and multiwire data
      if(bdata.getDeviceName().find("E:TR101D") != std::string::npos) {	
	tor101 = bdata.getData();
	continue;
      } else if(bdata.getDeviceName().find("E:TRTGTD") != std::string::npos) {
	tortgt = bdata.getData();
	continue;
      }	else if(bdata.getDeviceName().find("E:HP121") != std::string::npos) {
	hp121 = bdata.getData();
	continue;
      }	else if(bdata.getDeviceName().find("E:VP121") != std::string::npos) {
	vp121 = bdata.getData();
	continue;
      }	else if(bdata.getDeviceName().find("E:HPTGT") != std::string::npos) {
	hptgt = bdata.getData();
	continue;
      } else if(bdata.getDeviceName().find("E:VPTGT") != std::string::npos) {
	vptgt = bdata.getData();
	continue;
      }	else if(bdata.getDeviceName().find("E:HITGT") != std::string::npos) {
	hitgt = bdata.getData();
	continue;
      } else if(bdata.getDeviceName().find("E:VITGT") != std::string::npos) {
	vitgt = bdata.getData();
	continue;
      } else if(bdata.getDeviceName().find("E:M121DS") != std::string::npos) {
	mw121.resize(bdata.getData().size());
	for (unsigned int ii=0;ii<mw121.size();ii++) 
	  mw121[ii]=bdata.getData()[ii];
	continue;
      } else if(bdata.getDeviceName().find("E:MTGTDS") != std::string::npos) {
	mwtgt.resize(bdata.getData().size());
	for (unsigned int ii=0;ii<mwtgt.size();ii++) 
	  mwtgt[ii]=bdata.getData()[ii];
	continue;    
      }
    }

    if (tor101.size()==0 && tortgt.size()==0) {
      //missing toroid info
      return -1;
    } else if (hp121.size()==0 || vp121.size()==0 || hptgt.size()==0 || vptgt.size() == 0 || hitgt.size() ==0 || vitgt.size()==0) {
      //missing bpms
      return 2;
    } else if (mwtgt.size()==0) {
      //missing multiwire
      return 3;
    }
    
    double temppot=0;
    if (tortgt.size()>0) temppot=tortgt[0];
    else if (tor101.size()>0) temppot=tor101[0];
    
    if (temppot<=0) 
      return 0; //no pot info or no beam, so no need to check beam position

    std::vector<std::vector<double>> BPMS;
    BPMS.push_back(vp121);
    BPMS.push_back(hp121);
    BPMS.push_back(vptgt);
    BPMS.push_back(hptgt);
    BPMS.push_back(vitgt);
    BPMS.push_back(hitgt);
 
    std::vector<double> xp, yp, xi, yi;
    std::vector<double> xpmm, ypmm;
    double totxi=0;
    double totyi=0;
    double convx=1;
    double convy=1;

    // Use Beam Position Monitors to estimate x and y position of
    // beam (xp,yp) and intensities (xi,yi) at target.	
    // Do this for all six readings of positions monitors
    NuMIBpmProjection(xp,yp,xi,yi,BPMS);
    for(int i=0;i<6;i++){
      totxi+=xi[i];
      totyi+=yi[i];
    }
    convx=totxi/temppot;
    convy=totyi/temppot;
    for(int i=0;i<6;i++){
      xi[i]=xi[i]/convx;
      yi[i]=yi[i]/convy;
    }
    double xpmean = -9., ypmean = -9., xpstdev = -9., ypstdev=-9.;
    // Use 6 position and intensity values to get mean and stdev
    // values of positions
    NuMIBpmAtTarget(xpmean,ypmean,xpstdev,ypstdev,xp,yp,xi,yi);

    double x = -9., y = -9., xstdev= -9., ystdev= -9.;
    // Use Beam Profile Monitors to estimate x and y widths and positions
    // of beam at target
    NuMIProfileProjection(x,y,xstdev,ystdev,mw121,mwtgt);

    if ((xpmean > fMinPosXCut) &&
	(xpmean < fMaxPosXCut) &&     // x position
	(ypmean > fMinPosYCut) &&	
	(ypmean < fMaxPosYCut) &&    // y position
	(xstdev > fMinWidthXCut) &&
	(xstdev < fMaxWidthXCut) && // x width
	(ystdev > fMinWidthYCut) &&
	(ystdev < fMaxWidthYCut)) // y width
      fom=1;
    else
      fom=0;
  }
  return fom;
}

void bmd::processBNBprofile(const double* mwdata, double &x, double& sx, double& chi2) 
{
   /* 
      function takes multiwire data as input (48 channels horizontal or vertical, 
      finds the min and max 
      finds the first and last bin where amplitude is greater than 20%
      fits the peak between first and last bin with gaussian (assuming 2% relative errors)
      returns:
      x  - mean position from the fit
      sx - sigma x from the fit
      chi2 - chi2/NDF for the gaussian fit
   */ 

  double minx=0;
  double maxx=0;
  for (unsigned int i=0;i<48;i++) {
    minx=std::min(-mwdata[i],minx);
    maxx=std::max(-mwdata[i],maxx);
  }
  int first_x=-1;
  int last_x=-1;
  static TH1D* hProf=new TH1D("hProf","",48,-12,12);
  for (unsigned int i=0;i<48;i++) {
    hProf->SetBinContent(i+1,-mwdata[i]-minx);
    if (-mwdata[i]-minx    > (maxx-minx)*0.2 && first_x==-1) first_x=i;
    if (-mwdata[i]-minx    > (maxx-minx)*0.2)                last_x=i+1;
    hProf->SetBinError(i+1,(maxx-minx)*0.02);
  }
  hProf->Fit("gaus","Q0","",-12+first_x*0.5,-12+last_x*0.5);
  
  x   =hProf->GetFunction("gaus")->GetParameter(1);
  sx  =hProf->GetFunction("gaus")->GetParameter(2);
  chi2=hProf->GetFunction("gaus")->GetChisquare()/hProf->GetFunction("gaus")->GetNDF();

}

double bmd::calcFOM2(double horpos, double horang, double verpos, double verang, double ppp, double tgtsx, double tgtsy)
{
  //code from MiniBooNE AnalysisFramework with the addition of scaling the beam profile to match tgtsx, tgtsy 
  //form DQ_BeamLine_twiss_init.F
  double bx  =  4.68;
  double ax  =  0.0389;
  double gx  = (1+ax*ax)/bx;
  double nx  =  0.0958;
  double npx = -0.0286;
  double by  = 59.12;
  double ay  =  2.4159;
  double gy  = (1+ay*ay)/by;
  double ny  =  0.4577;
  double npy = -0.0271;

  //from DQ_BeamLine_make_tgt_fom2.F
  double ex = 0.1775E-06 + 0.1827E-07*ppp;
  double ey = 0.1382E-06 + 0.2608E-08*ppp;
  double dp = 0.4485E-03 + 0.6100E-04*ppp;
    
  double tex = ex;
  double tey = ey;
  double tdp = dp;

  //from DQ_BeamLine_beam_init.F
  double sigma1[6][6]={0};
  double centroid1[6]={0};
  centroid1[0] = horpos;
  centroid1[1] = horang;
  centroid1[2] = verpos;
  centroid1[3] = verang;
  centroid1[4] = 0.0;
  centroid1[5] = 0.0;

  sigma1[5][5] =  tdp*tdp;

  sigma1[0][0] =  tex*bx+ nx*nx *tdp*tdp;
  sigma1[0][1] = -tex*ax+ nx*npx*tdp*tdp;
  sigma1[1][1] =  tex*gx+npx*npx*tdp*tdp;
  sigma1[0][5] =  nx*tdp*tdp;
  sigma1[1][5] =  npx*tdp*tdp;
  sigma1[1][0] =  sigma1[0][1];
  sigma1[5][0] =  sigma1[0][5];
  sigma1[5][1] =  sigma1[1][5];

  sigma1[2][2] =  tey*by+ny*ny*tdp*tdp;
  sigma1[2][3] = -tey*ay+ny*npy*tdp*tdp;
  sigma1[3][3] =  tey*gy+npy*npy*tdp*tdp;
  sigma1[2][5] =  ny*tdp*tdp;
  sigma1[3][5] =  npy*tdp*tdp;
  sigma1[3][2] =  sigma1[2][3];
  sigma1[5][2] =  sigma1[2][5];
  sigma1[5][3] =  sigma1[3][5];
  
  double begtocnt[6][6]={
    { 0.65954,  0.43311,  0.00321,  0.10786, 0.00000,  1.97230},
    { 0.13047,  1.60192,  0.00034,  0.00512, 0.00000,  1.96723},
    {-0.00287, -0.03677, -0.35277, -4.68056, 0.00000,  0.68525},
    {-0.00089, -0.00430, -0.17722, -5.18616, 0.00000,  0.32300},
    {-0.00104,  0.00232, -0.00001, -0.00224, 1.00000, -0.00450},
    { 0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  1.00000}
  };
  double cnttoups[6][6]={0};
  double cnttodns[6][6]={0};
  double identity[6][6]={0};
  double begtoups[6][6]={0};
  double begtodns[6][6]={0};

  for (int i=0;i<6;i++) {
    for (int j=0;j<6;j++) {
      if (i==j) {
	cnttoups[i][j] = 1.0;
	cnttodns[i][j] = 1.0;
	identity[i][j] = 1.0;
      } else {
	cnttoups[i][j] = 0.0;
	cnttodns[i][j] = 0.0;
	identity[i][j] = 0.0;
      }
    }  
  }

  cnttoups[0][1] = -0.35710;
  cnttoups[2][3] = -0.35710;
  cnttodns[0][1] = +0.35710;
  cnttodns[2][3] = +0.35710;

  for (int i=0;i<6;i++) {
    for (int j=0;j<6;j++) {
      for (int k=0;k<6;k++) {
	begtoups[i][k] = begtoups[i][k] + cnttoups[i][j]*begtocnt[j][k];
	begtodns[i][k] = begtodns[i][k] + cnttodns[i][j]*begtocnt[j][k];
      }
    }
  }

  //swim to upstream of target
  double cx, cy, sx, sy, rho;
  bmd::swimBNB(centroid1,sigma1,
	       cnttoups, begtoups,
	       cx, cy, sx, sy, rho);

  double scalex=tgtsx/sx;
  double scaley=tgtsy/sy;
  double fom_a=bmd::func_intbivar(cx, cy, sx*scalex, sy*scaley, rho);

  //swim to center of target
  bmd::swimBNB(centroid1,sigma1,
	       identity, begtocnt,
	       cx, cy, sx, sy, rho);
  double fom_b=bmd::func_intbivar(cx, cy, sx*scalex, sy*scaley, rho);
  
  //swim to downstream of target
  bmd::swimBNB(centroid1,sigma1,
	       cnttodns, begtodns,
	       cx, cy, sx, sy, rho);
  double fom_c=bmd::func_intbivar(cx, cy, sx*scalex, sy*scaley, rho);

  double fom2=fom_a*0.6347 +
              fom_b*0.2812 +
              fom_c*0.0841;
  
  return fom2;
} 

void bmd::swimBNB(const double centroid1[6], const double sigma1[6][6], 
		  const double xferc[6][6], const double xfers[6][6],
		  double &cx, double& cy, double &sx, double &sy, double &rho)
{
  //centroid
  double centroid2[6]={0};
  for (int i=0;i<6;i++) {
    for (int j=0;j<6;j++) {
      centroid2[i] = centroid2[i] + xferc[i][j]*centroid1[j];
    }
  }
  
  cx = centroid2[0];
  cy = centroid2[2];
  
  //sigma
  double sigma2[6][6]={0};
  
  for (int i = 0; i<6;i++) {
    for (int j = 0;j<6;j++) {
      for (int k = 0;k<6;k++) {
	for (int m = 0;m<6;m++) {
	  sigma2[i][m] = sigma2[i][m] +
	    xfers[i][j]*sigma1[j][k]*xfers[m][k];
	}
      }    
    }
  }
  //get beam sigma
  
  sx  = sqrt(sigma2[0][0])*1000.0;
  sy  = sqrt(sigma2[2][2])*1000.0;
  rho = sigma2[0][2]/sqrt(sigma2[0][0]*sigma2[2][2]);

  //  cout<<"Swim "<<sx<<"\t"<<sy<<"\t"<<rho<<endl;
}
 
double bmd::func_intbivar(const double cx, const double cy, const double sx, const double sy, const double rho )
{
  //integrate beam overlap with target cylinder
  double x0  =  cx;
  double y0  =  cy;
  
  double dbin = 0.1;
  double dx = dbin;
  double dy = dbin;

  double r    = 4.75;
  double rr   = r*r;
  double rho2 = rho*rho;

  double xmin = -r;
  double ymin = -r;
  int imax = round((2.0*r)/dx);
  int jmax = round((2.0*r)/dy);
  double sum =  0.0;
  double x = xmin;
  //cout <<sx<<"\t"<<sy<<"\t"<<imax<<"\t"<<jmax<<endl;
  for (int i=0;i<=imax;i++) {
    double y = ymin;
    for (int j=0;j<=jmax;j++) {
      if ( (x*x+y*y)<rr ) {
	double tx = (x+x0)/sx;
	double ty = (y+y0)/sy;
	double z = tx*tx - 2.0*rho*tx*ty + ty*ty;
	double t = exp(-z/(2.0*(1.0-rho2)));
	sum = sum + t;
      } 
      //      cout << i<<"\t"<<j<<"\t"<<x<<"\t"<<y<<"\t"<<sum<<endl;
      y = y + dy;
    }
    x = x + dx;
  }
  sum = sum*dx*dy/(2.0*3.14159*sx*sy*sqrt(1.0-rho2));
  //  cout<<"sum = "<<sum<<endl;
  return log10(1-sum);
}

//NuMI code copied from NOvA http://nusoft.fnal.gov/nova/novasoft/doxygen/html/IFDBSpillInfo__module_8cc_source.html
double bmd::NuMIExtrapolatePosition(double t1, double z1, double t2, double z2, double z3)
{
  return t1+(t2-t1)*(z3-z1)/(z2-z1);
}

void bmd::NuMIBpmProjection(std::vector<double> &xp, std::vector<double> &yp, 
			    std::vector<double> &xi, std::vector<double> &yi,
			     std::vector<std::vector<double>> BPMS)
{
  xp.clear();
  yp.clear();
  xi.clear();
  yi.clear();
  
  // In NOvA era (for now) we will assume the following
  // In MINOS era, target location was 3937 after time period 1155564632
  double z_targ = 0;
  
  // Return shortest array length of all BPM positions and intensities used
  int size = BPMS.size();
  int n    = BPMS[0].size();
  for(int ind = 1; ind < size; ++ind) {
    int n2 = BPMS[ind].size();
    if(n2 < n) n = n2;
  }
  
  int start = 0;
  // Skip first BPMS value (average used in auto-tune) but only if 
  // there are multiple batches
  
  if(n > 1) ++start; 
  // Check to see if elements of BPMS[4] and BPMS[5] (BPM intensities) are 0.
  // If they are, set position values to 0.  Otherwise, use corresponding values of
  // vertical and horizontal position monitors to extrapolate vertical and 
  // horizontal position at target.  Simply transfer intensities over.
  
  for(int ind = start; ind < n; ++ind) {
    
    if(BPMS[4][ind] == 0.0 || BPMS[5][ind]== 0.0) {
      yp.push_back(0.0);
      xp.push_back(0.0);
      yi.push_back(0.0);
      xi.push_back(0.0);
      
      continue;
    }
    
    // Returns transverse position (x or y) at z_targ assuming a linear 
    // extrapolation from z_vp121 to z_vptgt
    double ft_to_m=0.3048;
    double z_hp121     = -68.04458*ft_to_m;//-72.2309*Munits::foot,  // HP121
    double z_vp121     = -66.99283*ft_to_m;//-71.3142*Munits::foot,  // VP121
    double z_hptgt     = -31.25508*ft_to_m;//-33.1564*Munits::foot,  // HPTGT
    double z_vptgt     = -30.16533*ft_to_m;//-32.2397*Munits::foot,  // VPTGT
    yp.push_back
      (NuMIExtrapolatePosition(BPMS[0][ind],z_vp121,BPMS[2][ind],z_vptgt,z_targ));
    xp.push_back
      (NuMIExtrapolatePosition(BPMS[1][ind],z_hp121,BPMS[3][ind],z_hptgt,z_targ));
    
    // Returns the horizontal or vertical intensity of beam        
    yi.push_back(BPMS[4][ind]);
    xi.push_back(BPMS[5][ind]);
    
  }
  return;
} // BpmProjection

double bmd::NuMIBpmAtTarget(double &xpmean, double &ypmean, 
			    double &xpstdev, double &ypstdev,
			    std::vector<double>TargBpmX,
			    std::vector<double>TargBpmY,
			    std::vector<double>BpmIntX, 
			    std::vector<double>BpmIntY)
{
 
  //xpmean = ypmean = xpstdev = ypstdev = 0.0;
  double IXtot = 0.0, IYtot = 0.0;
  double ix = 0.0, iy = 0.0, ix2 = 0.0, iy2 = 0.0;
  
  // For intensity-weighted position values...
  // Remember, TargBpmX, TargBpmY, BpmIntX and BpmIntY have one fewer element than
  // the devices because first device value was an average used for beam tuning
  for(size_t ind = 0; ind < BpmIntX.size(); ++ind) {
    
    // If average intensity is 0, skip to next value
    if(BpmIntX[ind] == 0.0 || BpmIntY[ind] == 0.0) continue;
    
    double X  = TargBpmX[ind]; //xp[ind]
    double Y  = TargBpmY[ind]; //yp[ind]
    double IX = BpmIntX[ind];  //xi[ind]
    double IY = BpmIntY[ind];  //yi[ind]
    
    // if I>0, set I=I, else set I=0  
    IX = IX > 0 ? IX : 0;
    IY = IY > 0 ? IY : 0;
                   
    // sum intensities
    IXtot += IX;
    IYtot += IY;
         
    // intensity-weighted position value
    ix += IX*X;
    iy += IY*Y;
    
    ix2 += IX*X*X;
    iy2 += IY*Y*Y;
        
  } // end for-loop over elements of xp, yp, xi and yi
       
  if(IXtot <= 0.0) return 0.0;
  if(IYtot <= 0.0) return 0.0;
      
  // intensity-weighted average positions
  xpmean = ix/IXtot;
  ypmean = iy/IYtot;
  
  // standard deviation
  double xvar = ix2/IXtot-xpmean*xpmean;
  if(xvar > 0.0) xpstdev = sqrt(xvar);
  double yvar = iy2/IYtot-ypmean*ypmean;
  if(yvar > 0.0) ypstdev = sqrt(yvar);
  
  return xpmean;
  
}
int bmd::NuMIProfileProjection(double &x, double &y, 
			       double &xstdev, double &ystdev,
			       std::vector<double> PM121,
			       std::vector<double> PMTGT)
{
  double xtgt = 0., ytgt = 0., xstdevtgt = 0., ystdevtgt = 0.;
  
  std::vector<double> hchannels121, vchannels121, hchannelstgt, vchannelstgt;
  // Need to add line here to return 0 if no data from devices 
  // Get relevant channels from the device                
  for(int ind = 0; ind <= 47; ++ind) {
    hchannels121.push_back(PM121[103+ind]);
    vchannels121.push_back(PM121[103+48+ind]);
    hchannelstgt.push_back(PMTGT[103+ind]);
    vchannelstgt.push_back(PMTGT[103+48+ind]);
  }
  
  NuMIGetGaussFit(xtgt,xstdevtgt,hchannelstgt);
  NuMIGetGaussFit(ytgt,ystdevtgt,vchannelstgt);
  
  // If values at MTGTDS are reasonable, set them to values at target 
  if(std::abs(xtgt) < 11 && std::abs(ytgt) < 11) {
    x = xtgt;
    y = ytgt;
  }
  // Standard deviation values are not extrapolated to target
  xstdev = xstdevtgt;
  ystdev = ystdevtgt;
  return 1;
}// ProfileProjection

double bmd::NuMIGetGaussFit(double &mean, double &sigma, 
			    std::vector<double> profile)
{              
  double  wirespacing = 0.5;
  TGraph Prof;        
  Prof.Set(0);
  //Prof.Clear("");
  int npoints = 0;
  // For each of 48 channels of profile monitor
  for(int ch = 1; ch <= 48; ++ch) {
    // Get position of wire. Origin is halfway between channels 24 and 25
    double X = (ch-24.5)*wirespacing;
    // Get the voltage value.
    double Qx = profile[ch-1];
    // ensure that they are greater than or equal to zero
    // NOTE: FOR NOW DON'T DO THIS - INSTEAD FIT TO AN OFFSET
    // Qx = Qx > 0 ? Qx : 0;
    // Set values of points in TGraph, x=X/Munits::mm and y=-1*Qx/Munits::millivolt
    // Invert voltage to make it positive in Gaussian peak - may lead to 
    // better fitting
    Prof.SetPoint(npoints,X,-1*Qx);
    ++npoints;
  } // for channels 1 to 48
  
    // Now that all of channel information has been loaded...
    // Remove dead and hot channels from profiles being fitted.
    // I DON'T DO THIS BUT I WILL REMOVE DEAD CHANNELS (IE THOSE WITH ZERO READING)
    // SuppressDeadHot(&xProf);
  
  for(int i = 0; i < Prof.GetN(); ++i) {
    if (Prof.GetY()[i] == 0.0){
      Prof.RemovePoint(i);
      --i;
    }
  }

  // For some reason this is faster than if I use a TMath::Gaus:
  TF1 gausF("gausF","[3]+([0]*exp((-1*(x-[1])*(x-[1]))/(2*[2]*[2]))/(sqrt(2*3.142)*[2]))");
  //TF1 gausF("gausF","[3]+[0]*TMath::Gaus(x,[1],[2],1)");
  
  gausF.SetParName(0,"Area");
  gausF.SetParName(1,"Mean");
  gausF.SetParName(2,"Sigma");
  gausF.SetParName(3,"Offset");
  
  // NOT SURE WE NEED THIS
  //gausF.SetParLimits(0,0.00,100000000);
  //gausF.SetParLimits(2,0,1000);
  
  // Gaussian fit parameters
  double area = -9.;
  double offset = -9.;
  mean  = -9., sigma = -9.;
  
  // Get statistical mean and standard deviation of profile distribution
  double StatMean = 0.0, stdev = 0.0;
  area = NuMIGetStats(&Prof,StatMean,stdev);
  // Check we have enough points and enough charge for a reasonable fit 
  if(Prof.GetN() > 5 && std::abs(area) > 1.0 &&
     std::abs(StatMean) < 20*wirespacing &&
     std::abs(stdev) < 12*wirespacing) {
    gausF.SetParameter(0,area);
    gausF.SetParameter(1,StatMean);
    gausF.SetParameter(2,stdev/2);
    //offset value should be fixed to the baseline, get avg value of the neagtive readings
    int nbneg=0;
    double avgneg=0.;
    for(int i = 0; i < Prof.GetN(); ++i) {
      if (Prof.GetY()[i] < 0.0){
	avgneg=avgneg+Prof.GetY()[i];
	nbneg++;
      }
    }
    
    if(nbneg!=0){
      offset=avgneg/nbneg;
    }else{
      // Starting offset value is a guess based on looking at data
      offset = -3.0;
    }
    gausF.SetParameter(3,offset);
    if(Prof.Fit("gausF","q","",-24*wirespacing,24*wirespacing) == 0) {
      bool goodfit = true;
      // Are fit results physical?
      if(gausF.GetParameter(0) < 0.1){
	goodfit = false;
      }
      
      if(std::abs(gausF.GetParameter(1)) > 24*wirespacing){
	goodfit = false;
      }
      
      if(gausF.GetParameter(2) > 24*wirespacing || gausF.GetParameter(2) < 0.0){
	goodfit = false;
      }
      if(goodfit) {
	// Remember to correct fit result for area by dividing by "bin size" and correcting units
	area   = gausF.GetParameter(0)/wirespacing;
	mean   = gausF.GetParameter(1);
	sigma  = gausF.GetParameter(2);
	offset = gausF.GetParameter(3);                                        
      } // if goodfit
    }
  } // if we can make a reasonable fit
    //    else{
    //      //mf::LogInfo("IFDBSpillInfo") << "POINTS DID NOT PASS BASIC CUTS in profile fit to Gaussian" << std::endl;
    //    }
    
  return area;
} // GetGaussFit
  

// Calculate mean and standard deviation for points in TGraph to use as 
// starting parameters values in Fit
double bmd::NuMIGetStats(TGraph *prof, double &mean, double &stdev)
{
  double qx = 0, qtot = 0;
  double X = 0.0;                
  double Qx = 0.0;                
  
  double minprof = 1E6;
  double maxprof = -1E6;
  
  // Find min and max values of profile.  Profile is upside down Gaussian 
  // with offset.  All profile values have already been inverted to make 
  // Gaussian right-side up - offset is now negative.
  // To get good starting param values, shift all profile values up by the 
  // most negative of all profile values, ie minprof, as found above.
  for(int i = 0; i < prof->GetN(); ++i) {
    Qx = prof->GetY()[i];
    if (Qx <= minprof) minprof = Qx;
    if (Qx >= maxprof) maxprof = Qx;
  }

  for(int i = 0; i < prof->GetN(); ++i) {
    // For each point in profile, get wire position and charge values
    X = prof->GetX()[i];
    Qx = prof->GetY()[i];
    prof->SetPoint(i,X,Qx-minprof);
    Qx = prof->GetY()[i];
    // Total charge on all wires
    qtot += Qx;
    // Total charge-weighted position value
    qx += Qx*X;
  }

  // If total charge is 0, mean and stdev are 0
  if(qtot == 0.0) {
    mean = 0.0;
    stdev = 0.0;
  }
  
  // Otherwise, mean position is charge-weighted mean
  else{
    mean = qx/qtot;
    // Charge-weighted variance
    double var = 0.0;
    for(int i = 0; i < prof->GetN(); ++i) {
      X = prof->GetX()[i];
      Qx = prof->GetY()[i];
      var +=  Qx*(X-mean)*(X-mean);
    }
    
    var = var/qtot;
    if(var > 0.0){
      stdev = std::sqrt(var);
    } else {
      stdev = 0.0;
    }
  }
  return qtot;        
} // GetStats
