/****************************************************************
 * Program to convert Boone style ntuples into GSimple style
 ****************************************************************/

#include <iostream>
#include "BooNEtoGSimple.h"

using namespace std;

int main(int argc, char *argv[])
{
  char input_file[200];
  char output_file[200];

  if (argc==1 || argc >4) {
    cout << endl;
    cout << "USAGE:"<<endl;
    cout << endl;
    cout << "\t ConvertNtuple input_file_name [output_file_name]"<<endl;
    cout << endl;
    cout << "\t\t input_file_name  - Name of the input Boone style ntuple" << endl;
    cout << "\t\t output_file_name - Name of the output GSimple style ntuple" << endl;
    cout << "\t\t                    if not given it is input_file_name appended by _gsimple" << endl;
    exit(0);
  } else if (argc==2) {
    strcpy(input_file,argv[1]);
    strcpy(output_file,argv[1]);
    strcat(output_file,"_gsimple");
  } else {
    strcpy(input_file,argv[1]);
    strcpy(output_file,argv[2]);
  }

  BooNEtoGSimple* btg=new BooNEtoGSimple();
  btg -> OpenFiles(input_file, output_file); 
  btg -> DoIt(input_file, output_file, 500000000);
  btg -> CloseFiles(); 
  delete btg;

  return 0;
}
