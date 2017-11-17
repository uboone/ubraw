#!bin/bash

workdir=$MRB_TOP/srcs/uboonecode/uboone/LEEPhotonAnalysis

version=flash
file=flash.root

mv $workdir/runmv_sp_$version/$file $workdir/runmv_sp.root;
rm -r $workdir/runmv_sp_$version;

mv $workdir/runmv_sp_cosmic_$version/$file $workdir/runmv_sp_cosmic.root;
rm -r $workdir/runmv_sp_cosmic_$version;

mv $workdir/runmv_bnb_$version/$file $workdir/runmv_bnb.root;
rm -r $workdir/runmv_bnb_$version;

mv $workdir/runmv_bnb_cosmic_$version/$file $workdir/runmv_bnb_cosmic.root;
rm -r $workdir/runmv_bnb_cosmic_$version;

mv $workdir/runmv_cosmic_$version/$file $workdir/runmv_cosmic.root;
rm -r $workdir/runmv_cosmic_$version;