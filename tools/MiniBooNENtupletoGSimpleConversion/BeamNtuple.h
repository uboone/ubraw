#ifndef BeamNtuple_h
#define BeamNtuple_h

//boone beam ntuple structure
struct BeamNtuple {  
  float tank_pos_beam[3]; /// 3-position of the flux window;
  float targ_pos_beam[3]; /// Frame conversion from beam to flux frame
  float pot;              /// pot in the file

  float windowbase[3];
  float windowdir1[3];
  float windowdir2[3];
  
  
};

#endif
