#ifndef SNHITCOMPARE_CLASSES_H_B14333B0
#define SNHITCOMPARE_CLASSES_H_B14333B0

namespace sn {

  class tuple_hit {
  public:
    double t;
    double integral;
    double height;
    double tstart;
    double tend;
    double tsigma;
    double heightsigma;
    double integralsigma;
  };

  class tuple_entry {
  public:
    int run;
    int subrun;
    int event;
    int wire;
    int plane;
    tuple_hit hit1;
    tuple_hit hit2;
    double dt;
  };
}


#endif /* end of include guard: SNHITCOMPARE_CLASSES_H_B14333B0 */
