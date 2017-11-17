
#!bin/bash

workdir=$MRB_TOP/srcs/uboonecode/uboone/LEEPhotonAnalysis
echo $workdir

listdir=$DATA/filelist/mcc82

mkdir runmv_sp_flash; cd runmv_sp_flash;
lar -c $workdir/flash.fcl -S $listdir/filelist_bnb_delta_rad.txt &> log &
cd ..

mkdir runmv_sp_cosmic_flash; cd runmv_sp_cosmic_flash;
lar -c $workdir/flash.fcl -S $listdir/filelist_bnb_delta_rad_cosmic.txt &> log &
cd ..

mkdir runmv_bnb_cosmic_flash; cd runmv_bnb_cosmic_flash;
lar -c $workdir/flash.fcl -S $listdir/filelist_bnb_cosmic_330.txt &> log &
cd ..
