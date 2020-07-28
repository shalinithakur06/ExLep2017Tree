#include "ExLep2017Tree/Selection/interface/MyEventSelection.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "DataFormats/JetReco/interface/JPTJet.h"

MyEventSelection::MyEventSelection(const edm::ParameterSet& iConfig, edm::ConsumesCollector&& cc)  
{
  configParamsVertex_ = iConfig.getParameter<edm::ParameterSet>("Vertex");
  configParamsJets_ = iConfig.getParameter<edm::ParameterSet>("Jets");
  configParamsMETs_ = iConfig.getParameter<edm::ParameterSet>("Mets");
  configParamsMuons_ = iConfig.getParameter<edm::ParameterSet>("Muons");
  configParamsElectrons_ = iConfig.getParameter<edm::ParameterSet>("Electrons");
  configParamshlt_ = iConfig.getParameter<edm::ParameterSet>("Trigger");
  configParamsMC_ = iConfig.getParameter<edm::ParameterSet>("MCTruth");
  
  // vertex
  vtxSource = cc.consumes<reco::VertexCollection>(configParamsVertex_.getParameter<edm::InputTag>("vertexSource"));
  bsSource = cc.consumes<reco::BeamSpot>(configParamsVertex_.getParameter<edm::InputTag>("beamSpotSource"));
  rhoSource = cc.consumes<double>( configParamsVertex_.getParameter<edm::InputTag>("rho"));

  // Muon
  Muonsources = cc.consumes<pat::MuonCollection>(configParamsMuons_.getParameter<edm::InputTag>("sources")); 

  // Elctrons
  Elesources = cc.consumes<pat::ElectronCollection>(configParamsElectrons_.getParameter<edm::InputTag>("sources"));
  //EleConversion_ = cc.consumes<reco::ConversionCollection>(edm::InputTag("reducedEgamma"));
  eventrhoToken_ = cc.consumes<double>(configParamsElectrons_.getParameter<edm::InputTag>("rhoIso"));
  beamSpotToken_ = cc.consumes<reco::BeamSpot>(configParamsElectrons_.getParameter<edm::InputTag>("beamSpot"));
  jetIDMapToken_ = cc.consumes<reco::JetIDValueMap>(edm::InputTag("ak5JetID"));
  conversionsMiniAODToken_ = cc.consumes<reco::ConversionCollection>(configParamsElectrons_.getParameter<edm::InputTag>("conversionsMiniAOD"));
 

  // Jets
  Jetsources = cc.consumes<pat::JetCollection>(configParamsJets_.getParameter<edm::InputTag>("sources"));
  m_rho_token  = cc.consumes<double>(configParamsJets_.getParameter<edm::InputTag>("jet_rho"));
  //local .txt file
  m_resolutions_file = configParamsJets_.getParameter<std::string>("resolutionsFile");
  m_scale_factors_file = configParamsJets_.getParameter<std::string>("scaleFactorsFile");
  // Mets
  Metsources = cc.consumes<pat::METCollection>(configParamsMETs_.getParameter<edm::InputTag>("sources"));
  //get MET filters from the event                                    
  BadChCandFilterToken_ = cc.consumes<bool>(configParamsMETs_.getParameter<edm::InputTag>("filter1"));
  BadPFMuonFilterToken_ =cc.consumes<bool>(configParamsMETs_.getParameter<edm:: InputTag>("filter2"));

  // Trigger
  hlt_ = cc.consumes<edm::TriggerResults>(configParamshlt_.getParameter<edm::InputTag>("source"));
  hltFilter_ = cc.consumes<edm::TriggerResults>(configParamshlt_.getParameter<edm::InputTag>("sourceFilter"));
  // MC and PU
  PUInfoTag_ = cc.consumes<vector<PileupSummaryInfo> >(edm::InputTag("slimmedAddPileupInfo"));
  GenParticle_ = cc.consumes<reco::GenParticleCollection>(edm::InputTag("prunedGenParticles"));
  externalLHEProducer_ = cc.consumes<LHEEventProduct>(edm::InputTag("externalLHEProducer"));
  ///////////////
  genEventInfo_ = cc.consumes<GenEventInfoProduct>(edm::InputTag("generator"));
 ////////////////
  std::string code = configParamsMC_.getParameter<std::string>("sampleCode");
  cout<<"code = "<<code<<endl;
  if(code!=std::string("DATA")) { isData_=false; }
  else{ isData_=true; inputDataSampleCode_ = MyEvent::DATA; }
}

MyEventSelection::~MyEventSelection()
{
}

void MyEventSelection::Set(const edm::Event& e, const edm::EventSetup& es)
{
  event_.eventNb = e.id().event();
  event_.lumiblock = e.luminosityBlock();
  event_.isData = isData_;
  //get trigger, vertex, muon, electron, jet, met
  event_.hlt = getHLT(e, es);
  event_.PrimaryVtxs = getVertices(e, es);
  event_.Muons = getMuons(e, es);
  event_.Electrons = getElectrons(e, es);
  event_.Jets = getJets(e, es);
  event_.mets = getMETs(e, es);
  if(!isData_){
    event_.sampleInfo = getSampleInfo(e, es);
  }
 //make event selection
  bool passTrig = false;
  std::vector<std::string> trigs = event_.hlt;
  for(size_t itrig = 0; itrig < trigs.size(); itrig++){
    if(trigs[itrig].find("Ele") != std::string::npos || 
	trigs[itrig].find("Mu") != std::string::npos) passTrig = true;
  }
  int EventQuality = 0;
  if(passTrig)EventQuality++;
  event_.eventQuality = EventQuality;
  fs_->cd();
}

void MyEventSelection::BookHistos(){
  ///book histograms
  initTriggerHistos_ = true;
  //selection
  dirs_.push_back( fs_->mkdir("selection") );

  ///MC
  dirs_.push_back( fs_->mkdir("MCINFO") );
  myhistos_["intimepu"] = dirs_[dirs_.size() - 1].make<TH1D>("intimepu", "intime pileup", 6000, 0, 1000);
  myhistos_["outoftimepu"] = dirs_[dirs_.size() - 1].make<TH1D>("outoftimepu", "out of time pileup", 6000, 0, 1000);
  myhistos_["totalpu"] = dirs_[dirs_.size() - 1].make<TH1D>("totalpu", "total pileup", 6000, 0, 1000);
  myhistos_["trueintimepu"] = dirs_[dirs_.size() - 1].make<TH1D>("trueintimepu", "intime pileup", 6000, 0., 1000.); 
  myhistos_["trueoutoftimepu"] = dirs_[dirs_.size() - 1].make<TH1D>("trueoutoftimepu", "out of time pileup",6000,0.,1000.); 
  myhistos_["truetotalpu"] = dirs_[dirs_.size() - 1].make<TH1D>("truetotalpu", "total pileup", 6000, 0., 1000.);

  ////https://github.com/rappoccio/usercode/blob/Dev_53x/EDSHyFT/plugins/BTaggingEffAnalyzer.cc
  double ptNBins = 100;
  double ptMin = 0;
  double ptMax = 1000;
  double etaNBins = 50;
  double etaMin = -5;
  double etaMax = 5; 
  //--------------------------------------
  //For BTag efficiency
  //--------------------------------------
  TString jetrawtag="Jets";
  //Total b, c and other jets
  myhistos2_["h2_BTagEff_Denom_b_"+jetrawtag]    = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTagEff_Denom_b", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  myhistos2_["h2_BTagEff_Denom_c_"+jetrawtag]    = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTagEff_Denom_c", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  myhistos2_["h2_BTagEff_Denom_udsg_"+jetrawtag] = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTagEff_Denom_udsg", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  //Loose BTag efficiency 2D histos          
  myhistos2_["h2_BTagEff_Num_bL_"+jetrawtag]      = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTagEff_Num_bL", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  myhistos2_["h2_BTagEff_Num_cL_"+jetrawtag]      = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTagEff_Num_cL", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  myhistos2_["h2_BTagEff_Num_udsgL_"+jetrawtag]   = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTagEff_Num_udsgL", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  //Medium BTag efficiency 2D histos          
  myhistos2_["h2_BTagEff_Num_bM_"+jetrawtag]      = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTagEff_Num_bM", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  myhistos2_["h2_BTagEff_Num_cM_"+jetrawtag]      = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTagEff_Num_cM", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  myhistos2_["h2_BTagEff_Num_udsgM_"+jetrawtag]   = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTagEff_Num_udsgM", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  //Tight BTag efficiency 2D histos          
  myhistos2_["h2_BTagEff_Num_bT_"+jetrawtag]      = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTagEff_Num_bT", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  myhistos2_["h2_BTagEff_Num_cT_"+jetrawtag]      = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTagEff_Num_cT", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  myhistos2_["h2_BTagEff_Num_udsgT_"+jetrawtag]   = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTagEff_Num_udsgT", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
}
