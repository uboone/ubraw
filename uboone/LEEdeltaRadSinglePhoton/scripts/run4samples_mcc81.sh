
#!bin/bash

workdir=$MRB_TOP/srcs/uboonecode/uboone/LEEPhotonAnalysis
echo $workdir

listdir=$DATA/filelist/mcc81

mkdir runmv_sp_mcc81; cd runmv_sp_mcc81;
lar -c $workdir/LEEPhotonAnalysis.fcl -S $listdir/filelist_bnb_single_photon.txt &> log &
cd ..

mkdir runmv_bnb_mcc81; cd runmv_bnb_mcc81;
lar -c $workdir/LEEPhotonAnalysis.fcl -S $listdir/filelist_bnb.txt &> log &
cd ..

mkdir runmv_bnb_cosmic_mcc81; cd runmv_bnb_cosmic_mcc81;
lar -c $workdir/LEEPhotonAnalysis.fcl -S $listdir/filelist_bnb_cosmic_330.txt &> log &
cd ..

mkdir runmv_cosmic_mcc81; cd runmv_cosmic_mcc81;
lar -c $workdir/LEEPhotonAnalysisNoPOT.fcl -S $listdir/filelist_cosmic_intime_330.txt &> log &
cd ..