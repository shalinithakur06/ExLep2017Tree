import FWCore.ParameterSet.Config as cms
#------------------------------------------------------
# The process object
#------------------------------------------------------
process = cms.Process('MiniTree')
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100

isData=False
##isData=True
#------------------------------------------------------
# Input root files and number of events
#------------------------------------------------------
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring("/store/mc/RunIIFall17MiniAODv2/ExcitedLepton_MuMuZ-250_TuneCP5_13TeV_pythia8/MINIAODSIM/PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/80000/FE840902-D773-E811-B18A-0090FAA569C4.root")
    ##fileNames = cms.untracked.vstring("/store/mc/RunIIFall17MiniAODv2/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PU2017_12Apr2018_94X_mc2017_realistic_v14_ext1-v1/00000/0014BAAA-E142-E811-8068-A4BF01125458.root")
    ##fileNames = cms.untracked.vstring("/store/mc/RunIISummer16MiniAODv2/ExcitedLepton_EEZ-250_TuneCUETP8M1_13TeV-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/40000/0C4445C1-FC94-E811-9466-1866DA85DC5B.root")
    #fileNames = cms.untracked.vstring("file:00AE0629-1F98-E611-921A-008CFA1112CC.root")
    ##fileNames = cms.untracked.vstring("/store/data/Run2017B/SingleMuon/MINIAOD/31Mar2018-v1/90000/FEC62083-1E39-E811-B2A1-0CC47A4D75F8.root")
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1))

#------------------------------------------------------
# Output file
#------------------------------------------------------
process.load("PhysicsTools.UtilAlgos.TFileService_cfi")
process.TFileService.fileName = cms.string("outFile_.root")

#-----------------------------
#START PROCESS CONFIGURATION
#-----------------------------
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
###process.GlobalTag.globaltag  = cms.string('80X_mcRun2_asymptotic_2016_TrancheIV_v6')
process.GlobalTag.globaltag  = cms.string('94X_dataRun2_ReReco_EOY17_v2')

#-----------------------------
# ADD THE ANALYSIS MODULE
#-----------------------------
process.load('ExLep2017Tree.Selection.EventSelectors_cfi')
process.myMiniTreeProducer.MCTruth.isData = cms.bool(isData)
if isData:
    process.myMiniTreeProducer.MCTruth.sampleCode = cms.string("DATA")
else:
    #for multi CRAB
    process.myMiniTreeProducer.MCTruth.sampleCode = cms.string("sampCode_")
process.myMiniTreeProducer.minEventQualityToStore = cms.int32(1)
##https://github.com/cms-jet/JRDatabase/tree/master/textFiles/Fall17_V3_MC
process.myMiniTreeProducer.Jets.resolutionsFile = cms.string('Fall17_V3_MC_PtResolution_AK8PF.txt')
process.myMiniTreeProducer.Jets.scaleFactorsFile = cms.string('Fall17_V3_MC_SF_AK8PF.txt')
process.myMiniTreeProducer.Trigger.source = cms.InputTag('TriggerResults::HLT')
process.myMiniTreeProducer.Trigger.trigBits = cms.vstring("HLT_IsoMu24",
"HLT_IsoTkMu24",
"HLT_Mu50",
"HLT_TkMu50",
"HLT_DoubleEle33",
"HLT_Ele27_WPTight_Gsf")

#------------------------------------------------------
# apply partial MET filters via trigger selection.
# BadPFMuonFilter and BadChargedCandidateFilter are
# run on fly through addMETFilters (process)
#------------------------------------------------------
from ExLep2017Tree.Selection.EventSelectors_cfi import *
addMETFilters(process)
process.myMiniTreeProducer.Trigger.sourceFilter = cms.InputTag('TriggerResults::HLT')
process.myMiniTreeProducer.Trigger.metFilterBits = cms.vstring("Flag_goodVertices",
"Flag_globalSuperTightHalo2016Filter",
"Flag_HBHENoiseFilter",
"Flag_HBHENoiseIsoFilter",
"Flag_EcalDeadCellTriggerPrimitiveFilter"
)
if(isData):
    process.myMiniTreeProducer.Trigger.metFilterBits.extend(["Flag_eeBadScFilter"])

#-----------------------------
#ANALYSIS SEQUENCE
#-----------------------------
process.p  = cms.Path(process.allEventsFilter*process.metFilterSequence*process.myMiniTreeProducer)
process.schedule = cms.Schedule(process.p)

