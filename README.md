# MiniTree Production

#### Set the CMSSSW release ####

* export SCRAM_ARCH=slc7_amd64_gcc700
* cmsrel CMSSW_10_4_0
* cd CMSSW_10_4_0/src/
* cmsenv

#### Download the MiniTree package ####

* git clone git@github.com:ravindkv/ExLep2016Tree.git

#### Compile and Run the codes ####

* cd ExLep2016Tree 
* scram b -j 20
* cd Selection/test
* cmsRun produceTree_cfg.py 

