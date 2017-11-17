
#!bin/bash

workdir=$MRB_TOP/srcs/uboonecode/uboone/LEEPhotonAnalysis
echo $workdir

listdir=$RM/filelist/mcc82

mkdir runmv_sp_mcc82; cd runmv_sp_mcc82;
lar -c $workdir/LEEPhotonAnalysisMC.fcl -S $listdir/bnb_nc_delta_rad.txt &> log &
cd ..

mkdir runmv_sp_cosmic_mcc82; cd runmv_sp_cosmic_mcc82;
lar -c $workdir/LEEPhotonAnalysisMC.fcl -S $listdir/bnb_nc_delta_rad_cosmic.txt &> log &
cd ..

mkdir runmv_bnb_cosmic_mcc82; cd runmv_bnb_cosmic_mcc82;
lar -c $workdir/LEEPhotonAnalysisMC.fcl -S $listdir/bnb_nu_cosmic_1000.txt &> log &
cd ..
