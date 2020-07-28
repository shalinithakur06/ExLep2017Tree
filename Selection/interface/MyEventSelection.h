// -*- C++ -*-
//
// Original Author:  Aruna Kumar Nayak
//         Created:  Fri Mar 11 12:42:51 WET 2011
// $Id: MyEventSelection.h,v 1.4 2012/10/15 13:17:24 anayak Exp $
//
//

#ifndef __MYEVENTSELECTION_H__
#define __MYEVENTSELECTION_H__

// system include files
#include <memory>
#include <string>

// user include files
#include <TROOT.h>
#include <vector>
#include <TString.h>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "DataFormats/TrackReco/interface/DeDxData.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"

#include "PhysicsTools/SelectorUtils/interface/PFJetIDSelectionFunctor.h"
#include "PhysicsTools/SelectorUtils/interface/JetIDSelectionFunctor.h"
#include "PhysicsTools/SelectorUtils/interface/strbitset.h"

#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/MET.h"
//#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Particle.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"

#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//jet resolution and scale factors
#include "JetMETCorrections/Modules/interface/JetResolution.h"
#include "CondFormats/JetMETObjects/interface/JetResolutionObject.h"

#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "PhysicsTools/SelectorUtils/interface/CutApplicatorWithEventContentBase.h"
#include "PhysicsTools/SelectorUtils/interface/CutApplicatorBase.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/PatCandidates/interface/VIDCutFlowResult.h"

#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"


#include "MyEvent.h"

#include "TH1.h"
#include "TH2.h"
#include "TString.h"
#include <vector>

using namespace std;

//namespace edm {
//  class ConsumesCollector;
//}

class MyEventSelection
{
public:
  MyEventSelection(const edm::ParameterSet&, edm::ConsumesCollector&& );
  ~MyEventSelection();
  
  //void Print(std::ostream &os = std::cout) const;
  void Set(const edm::Event& e, const edm::EventSetup& es);
  void Reset() {event_.Reset();}
  MyEvent * getData() {return &event_;}
  
  //user functions
  std::vector<std::string> getHLT(const edm::Event&, const edm::EventSetup&);
  std::vector<MyVertex> getVertices(const edm::Event&, const edm::EventSetup&);
 
  MyVertex MyVertexConverter(const reco::Vertex& iVertex, double rhoAll, int totVtx);
  std::vector<MyJet> getJets(const edm::Event&, const edm::EventSetup&);
  MyJet MyJetConverter(const pat::Jet&, TString&, double JER);
  std::vector<MyMET> getMETs(const edm::Event&, const edm::EventSetup&);
  MyMET MyMETConverter(const pat::MET&, TString&);
  std::vector<MyElectron> getElectrons(const edm::Event&, const edm::EventSetup&);
  MyElectron MyElectronConverter(const pat::Electron&, TString&);
  std::vector<MyMuon> getMuons(const edm::Event&, const edm::EventSetup&);
  MyMuon MyMuonConverter(const pat::Muon&, TString&);
 
  SampleInfo getSampleInfo(const edm::Event&, const edm::EventSetup&);
  static bool sumPtOrder(const reco::Vertex *, const reco::Vertex *);
  std::vector<double> defaultMuonIsolation(const pat::Muon&, bool isPF=false);
  std::vector<double> defaultPFMuonIsolation(const pat::Muon&);
  std::vector<double> defaultPFElectronIsolation(const pat::Electron&);
  float relCombPFIsoWithEAcorr(const pat::Electron& iEle, double rho_, TString& dirtag);

  int assignDYchannel(const edm::Event&, const edm::EventSetup&);
  int assignWJets(const edm::Event&, const edm::EventSetup&);
  int assignTTEvent(const edm::Event&, const edm::EventSetup&);
  void BookHistos();

private:
  bool fillHLT_;
  MyEvent event_;
  
  //configuration parameters
  edm::ParameterSet configParamsVertex_;
  edm::ParameterSet configParamsElectrons_;
  edm::ParameterSet configParamsMuons_;
  edm::ParameterSet configParamsJets_;
  edm::ParameterSet configParamsMETs_;
  edm::ParameterSet configParamshlt_;
  edm::ParameterSet configParamsMC_;
  edm::ParameterSet configParamsKFPs_;
  //PVx
  //  edm::Handle<reco::VertexCollection> vtxSource;
  edm::EDGetTokenT<reco::VertexCollection> vtxSource;
  edm::EDGetTokenT<reco::BeamSpot> bsSource; // new 76x
  edm::EDGetTokenT<double> rhoSource; // new 76x

  //MET filter                                                        
  edm::EDGetTokenT<bool> BadChCandFilterToken_;                       
  edm::EDGetTokenT<bool> BadPFMuonFilterToken_;    

  // Muon, Electrons, Jets, MET, Trigger
  edm::EDGetTokenT<pat::MuonCollection> Muonsources; 
  edm::EDGetTokenT<pat::ElectronCollection> Elesources;
  edm::EDGetTokenT<double> eventrhoToken_;
  edm::EDGetTokenT<reco::ConversionCollection> conversionsMiniAODToken_;
  edm::EDGetTokenT<reco::BeamSpot> beamSpotToken_;

  edm::EDGetTokenT<pat::JetCollection> Jetsources;
  edm::EDGetTokenT<pat::TriggerEvent> TrigEvent_;
  edm::EDGetTokenT<reco::JetIDValueMap> jetIDMapToken_;
  edm::EDGetTokenT<pat::METCollection> Metsources;
  edm::EDGetTokenT<edm::TriggerResults>  hlt_;
  edm::EDGetTokenT<edm::TriggerResults>  hltFilter_;

  const reco::Vertex *bestPrimVertex_;
  reco::Vertex refVertex_;
  math::XYZPoint refPoint_;
  edm::ESHandle<TransientTrackBuilder> trackBuilder;

  // MC and PU
  edm::EDGetTokenT<vector<PileupSummaryInfo>> PUInfoTag_;
  edm::EDGetTokenT<reco::GenParticleCollection> GenParticle_;
  ////////////////////
  edm::EDGetTokenT<GenEventInfoProduct> genEventInfo_;
  ////////////////
  edm::EDGetTokenT<LHEEventProduct> externalLHEProducer_;

  //JET id functors
  JetIDSelectionFunctor jetIDFunctor_;
  PFJetIDSelectionFunctor pfjetIDFunctor_;
  edm::Handle<reco::JetIDValueMap> hJetIDMap;
  //for pt resolution
  std::string m_resolutions_file;
  std::string m_scale_factors_file;
  edm::EDGetTokenT<double> m_rho_token;
  edm::Service<TFileService> fs_;
  std::vector<TFileDirectory> dirs_;
  std::map<TString, TH1D*>myhistos_;
  std::map<TString, TH2D*>myhistos2_;

  bool initTriggerHistos_;
  //is data flag
  bool isData_;
  int mcEvtType_;
  int inputDataSampleCode_;
  bool runKineFitter_; 

  //pu re-weighting
  edm::LumiReWeighting LumiWeights_, LumiWeightsDefault_;
  //define data and MC pu vector
  std::vector< float > MCPUDist, MCPUTrueGen;
  std::vector< float > DataPUDist;

};
#endif
