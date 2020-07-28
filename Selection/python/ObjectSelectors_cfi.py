import FWCore.ParameterSet.Config as cms

#my base values for trigger bit selection -------------------
#my MC truth matching sets -------------------------------------------------------
BaseMCTruthSet = cms.PSet( isData = cms.bool(False),
                       sampleCode = cms.string("SEECODES")
                       )

BaseTriggerSet = cms.PSet( source = cms.InputTag("TriggerResults::HLT"),
                           trigBits = cms.vstring('HLT_IsoMu24'),
			   sourceFilter = cms.InputTag("TriggerResults"),
			   metFilterBits = cms.vstring('Flag_goodVertices')
                           )

#my base values for vertex selection ---------------------------------
BaseVertexSet = cms.PSet( vertexSource = cms.InputTag("offlineSlimmedPrimaryVertices"),
                          maxZ = cms.double(24),
                          rho =  cms.InputTag('fixedGridRhoAll'),
                          maxRho = cms.double(2.0),
                          minNDOF = cms.int32(4),
                          beamSpotSource = cms.InputTag("offlineBeamSpot"),
                          useBeamSpot = cms.bool(True)
                          )

#my base values for muon selection ---------------------------------------
BaseMuonsSet =  cms.PSet( sources = cms.InputTag("slimmedMuons"),
                          minPt = cms.double(10),
                          maxEta = cms.double(2.5)
                          )

# base values for electron selection ----------------------------------------------
BaseElectronsSet =  cms.PSet(sources = cms.InputTag("slimmedElectrons"),
                        minEt = cms.double(10),
                        maxEta = cms.double(2.5),
                        rhoIso = cms.InputTag("fixedGridRhoFastjetAll"),
                        conversionsMiniAOD  = cms.InputTag('reducedEgamma:reducedConversions'),
                        beamSpot = cms.InputTag('offlineBeamSpot')
                        )

#my base values for jet selection -----------------------------------------------
BaseJetsSet = cms.PSet(sources = cms.InputTag("slimmedJetsAK8"), #slimmedJetsAK8PFPuppiSoftDropPacked
                       dedxSource = cms.InputTag("dedxHarmonic2"),
                       minPt = cms.double(17),
                       maxEta = cms.double(2.5),
                       jet_rho = cms.InputTag('fixedGridRhoAll'),
                       resolutionsFile = cms.string("Fall17_V3_MC_PtResolution_AK8PF.txt"),
                       scaleFactorsFile= cms.string("Fall17_V3_MC_SF_AK8PF.txt")
                       )

#my base values for met selection ------------------------------------------------
BaseMetsSet = cms.PSet(sources = cms.InputTag("slimmedMETs"),
                       minMET = cms.double(10),
		       filter1 = cms.InputTag('BadChargedCandidateFilter'),
		       filter2 = cms.InputTag('BadPFMuonFilter')
                       )


