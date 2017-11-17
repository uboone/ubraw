#!bin/bash

workdir=$MRB_TOP/srcs/uboonecode/uboone/LEEPhotonAnalysis

version=mcc82

mv $workdir/runmv_sp_$version/LEEPhotonAnalysisMC.root $workdir/runmv_sp.root;
rm -r $workdir/runmv_sp_$version;

mv $workdir/runmv_sp_cosmic_$version/LEEPhotonAnalysisMC.root $workdir/runmv_sp_cosmic.root;
rm -r $workdir/runmv_sp_cosmic_$version;

mv $workdir/runmv_bnb_$version/LEEPhotonAnalysisMC.root $workdir/runmv_bnb.root;
rm -r $workdir/runmv_bnb_$version;

mv $workdir/runmv_bnb_cosmic_$version/LEEPhotonAnalysisMC.root $workdir/runmv_bnb_cosmic.root;
rm -r $workdir/runmv_bnb_cosmic_$version;

mv $workdir/runmv_cosmic_$version/LEEPhotonAnalysisNoPOTMC.root $workdir/runmv_cosmic.root;
rm -r $workdir/runmv_cosmic_$version;