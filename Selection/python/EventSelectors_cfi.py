import FWCore.ParameterSet.Config as cms

from ExLep2017Tree.Selection.ObjectSelectors_cfi import *
allEventsFilter = cms.EDFilter("AllEventsFilter")

#----------------------------------------------------------
# MET filters
#----------------------------------------------------------
# https://github.com/cms-sw/cmssw/blob/CMSSW_8_0_X/PhysicsTools/PatAlgos/python/slimming/metFilterPaths_cff.py
# https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2#2016_data
# Muon
from RecoMET.METFilters.metFilters_cff import BadChargedCandidateFilter, BadPFMuonFilter #2016 post-ICHEPversion
def addMETFilters(process):
    # Muon
    process.BadChargedCandidateFilter = BadChargedCandidateFilter.clone()
    process.BadChargedCandidateFilter.muons  = cms.InputTag("slimmedMuons")
    process.BadChargedCandidateFilter.PFCandidates = cms.InputTag("packedPFCandidates")
    process.BadChargedCandidateFilter.taggingMode = True
    process.BadPFMuonFilter = BadPFMuonFilter.clone()
    process.BadPFMuonFilter.muons = cms.InputTag("slimmedMuons")
    process.BadPFMuonFilter.PFCandidates = cms.InputTag("packedPFCandidates")
    process.BadPFMuonFilter.taggingMode = True
    process.metFilterSequence = cms.Sequence(process.BadPFMuonFilter*
            process.BadChargedCandidateFilter
            )

# base configuration ----------------------------------------
myMiniTreeProducer = cms.EDAnalyzer("MiniTreeProducer",
                                    MCTruth = BaseMCTruthSet,
                                    Trigger = BaseTriggerSet,
                                    Vertex = BaseVertexSet,
                                    Muons = BaseMuonsSet,
                                    Electrons = BaseElectronsSet,
                                    Jets = BaseJetsSet,
                                    Mets = BaseMetsSet,
                                    minEventQualityToStore = cms.int32(0)
                                    )
