#!bin/bash

workdir=$MRB_TOP/srcs/uboonecode/uboone/LEEPhotonAnalysis

version=mcc81

mv $workdir/runmv_sp_$version/LEEPhotonAnalysis.root $workdir/runmv_sp_$version.root;
rm -r $workdir/runmv_sp_$version;

mv $workdir/runmv_bnb_$version/LEEPhotonAnalysis.root $workdir/runmv_bnb_$version.root;
rm -r $workdir/runmv_bnb_$version;

mv $workdir/runmv_bnb_cosmic_$version/LEEPhotonAnalysis.root $workdir/runmv_bnb_cosmic_$version.root;
rm -r $workdir/runmv_bnb_cosmic_$version;

mv $workdir/runmv_cosmic_$version/LEEPhotonAnalysis.root $workdir/runmv_cosmic_$version.root;
rm -r $workdir/runmv_cosmic_$version;