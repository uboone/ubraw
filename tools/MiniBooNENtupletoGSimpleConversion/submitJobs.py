#!/usr/bin/env python

import argparse
import os.path
import sys
import tarfile
import re
import subprocess

parser = argparse.ArgumentParser(description='Submit beam data jobs.')

parser.add_argument("-f", "--first-run", dest="firstrun",
                    required=True,
                    help="First run.")
parser.add_argument("-l", "--last-run", dest="lastrun",
                    required=True,
                    help="Last run.")
parser.add_argument("-i", "--input-path", dest="inputpath",
                    default="/pnfs/uboone/persistent/uboonebeam/bnb_redecay/",
                    help="Path to input redecay files. (default=/pnfs/uboone/persistent/uboonebeam/bnb_redecay/)")
parser.add_argument("-o", "--output-path", dest="outputpath",
                    default="/pnfs/uboone/scratch/users/%s/bnb_redecay_to_gsimple"%os.environ['USER'],
                    help="Path where to copy final output. (default=/pnfs/uboone/scratch/users/%s/bnb_redecay_to_gsimple)"%os.environ['USER'])
parser.add_argument("-d", "--debug",action='store_true',
                    help="Will not delete submission files in the end. Useful for debugging and will only print the submission command on screen.")

args = parser.parse_args()

#now create jobfiles_*.tar that is shipped with the job
#this includes the executable
#tar -cf jobfiles.tar --transform '!^[^/]*/!!' file1 file2 file3
tarfilename="jobfiles_%i.tar.bz2"%os.getpid()
outtar = tarfile.open(tarfilename, mode='w:bz2')
outtar.add("ConvertNtuple",arcname="ConvertNtuple")
outtar.close()

ofstr='''
#!/bin/bash
echo "Running $0 on "$HOSTNAME
echo "Cluster: " ${CLUSTER}
echo "Process: " ${PROCESS}

source /nusoft/app/alt/setup
setup gsimple v2_8_6d -q e9:debug
source /grid/fermiapp/products/common/etc/setup
setup ifdhc v2_0_7

cd ${_CONDOR_SCRATCH_DIR}
mkdir ${CLUSTER}
cd ${CLUSTER}
export _RUN_NUMBER=$((PROCESS+%(firstrun)s))
mkdir ${_RUN_NUMBER}
cd ${_RUN_NUMBER}
cp $INPUT_TAR_FILE .
tar -jvxf `basename ${INPUT_TAR_FILE}`
inpfile=`printf beammc_%%04d.root ${_RUN_NUMBER}`
ifdh cp %(inputdir)s/$inpfile $inpfile
./ConvertNtuple $inpfile converted_$inpfile > ${_RUN_NUMBER}.out

rm $inpfile
rm ConvertNtuple
rm `basename ${INPUT_TAR_FILE}`
cd ../

ifdh mkdir %(outputdir)s/
ifdh mkdir %(outputdir)s/${_RUN_NUMBER}
ifdh cp -r ${_RUN_NUMBER} %(outputdir)s/${_RUN_NUMBER}/

'''%{'inputdir':args.inputpath,'firstrun':args.firstrun,'outputdir':args.outputpath}

runjobfname="runjob_%i.sh"%os.getpid()
of=open(runjobfname,'w')
of.write(ofstr)
of.close()

cmd="jobsub_submit --memory=1000MB --group=uboone -N %i --tar_file_name=dropbox://%s file://%s"%(int(args.lastrun)-int(args.firstrun)+1,os.path.abspath(tarfilename),os.path.abspath(runjobfname))

if (not args.debug):
    print "Running submit cmd:"
    print cmd
    os.system(cmd)
else:
    print "Would have ran:"
    print cmd

#Delete temp files unless debugging
if (not args.debug):
    os.remove(tarfilename)
    os.remove(runjobfname)
