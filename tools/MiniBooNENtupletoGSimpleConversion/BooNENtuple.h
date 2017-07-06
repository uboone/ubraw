#ifndef BooNENtuple_h
#define BooNENtuple_h

//boone ntuple structure
struct BooNENtuple {  
  float beamwgt;          /// Magic Weight : CrossSect * BooBeamNT
  int ntp;                /// 1,2,3,4 : nue, nuebar, numu, numubar
  int npart;              /// number of particles in the chain
                          ///    npart-1 == proton
                          ///    0 == neutrino
  int id[20];             /// id of each particle in before chain, array length 'npart'
  float ini_pos[20][3];   /// 3-pos of particle in before chain, array length 'npart'
  float ini_mom[20][3];   /// 3-mom of particle in before chain, array length 'npart'
  float ini_eng[20];      /// E of particle in before chain, array length 'npart'
  float ini_t[20];        /// "decay" time of particle (starting from proton) 
                          ///             in before chain, array length 'npart'
  float fin_mom[20][3];   /// final 3-mom of particle in before chain, array length 'npart'
  float fin_pol[20][3];   /// final 3-polarization of particle in before chain, array length 'npart'  

};

#endif
