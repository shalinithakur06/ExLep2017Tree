#include "ExLep2017Tree/Selection/interface/MyEventSelection.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"

SampleInfo MyEventSelection::getSampleInfo(const edm::Event& iEvent, const edm::EventSetup& iSetup){
  SampleInfo mcInfo;
  mcInfo.sampleName = configParamsMC_.getParameter<std::string>("sampleCode");
  //----------------------------------------------
  //store GenPileup info
  //----------------------------------------------
  double npuVertices_=0, nOOTpuVertices_=0;
  double nTruePuVertices_=0, nTrueOOTPuVertices_=0;
  edm::Handle<std::vector<PileupSummaryInfo> > puInfoH;
  iEvent.getByToken(PUInfoTag_, puInfoH);
  if(puInfoH.isValid()){ 
    //std::cout << "valid puInfoH = " << std::endl;
    double nOOTpuVertices(0),npuVertices(0);
    double nTrueOOTPuVertices(0),nTruePuVertices(0);
    for(std::vector<PileupSummaryInfo>::const_iterator it =puInfoH->begin(); it != puInfoH->end(); it++){
	  if(it->getBunchCrossing() ==0){
	    npuVertices += it->getPU_NumInteractions();
	    nTruePuVertices += it->getTrueNumInteractions();
	  }
	  else{
	    nOOTpuVertices += it->getPU_NumInteractions();
	    nTrueOOTPuVertices += it->getTrueNumInteractions();
	  }
	}
    npuVertices_ = npuVertices;
    nOOTpuVertices_ = nOOTpuVertices;
    nTruePuVertices_ = nTruePuVertices;
    nTrueOOTPuVertices_ = nTrueOOTPuVertices;
    // std::cout<<"npuVertices_ =  "<<npuVertices_<<std::endl;
  }
  std::vector<double>pu; pu.clear();
  pu.push_back(npuVertices_);
  pu.push_back(nOOTpuVertices_);
  std::vector<double>putrue; putrue.clear(); 
  putrue.push_back(nTruePuVertices_); 
  putrue.push_back(nTrueOOTPuVertices_);
  mcInfo.pileup = pu;
  mcInfo.truepileup = putrue;
  myhistos_["intimepu"]->Fill(npuVertices_);
  myhistos_["outoftimepu"]->Fill(nOOTpuVertices_);
  myhistos_["totalpu"]->Fill(npuVertices_+nOOTpuVertices_);
  myhistos_["trueintimepu"]->Fill(nTruePuVertices_); 
  myhistos_["trueoutoftimepu"]->Fill(nTrueOOTPuVertices_); 
  myhistos_["truetotalpu"]->Fill(nTruePuVertices_+nTrueOOTPuVertices_);
  //----------------------------------------------
  //get total number of particles (in, inter, out) 
  //in an event at gen level. It is called hepNuP
  //----------------------------------------------
  int hepNUP_ = -99;
  std::vector<int> hepIDUP_;
  std::vector<int> hepISTUP_;
  edm::Handle<LHEEventProduct > LHEHandle;
  const LHEEventProduct* LHE = 0;
  iEvent.getByToken(externalLHEProducer_, LHEHandle);
  if(LHEHandle.isValid()){
    LHE = LHEHandle.product();
    hepNUP_ = (LHE->hepeup()).NUP;
    for(size_t i=0; i<(LHE->hepeup()).IDUP.size(); i++){
      hepIDUP_.push_back((LHE->hepeup()).IDUP[i]);
      hepISTUP_.push_back((LHE->hepeup()).ISTUP[i]);
    }
  }
  mcInfo.hepNUP = hepNUP_;
  mcInfo.hepIDUP = hepIDUP_;
  mcInfo.hepISTUP = hepISTUP_;
  //----------------------------------------------
  //add weights for top pT re-weighting
  //----------------------------------------------
  edm::Handle<reco::GenParticleCollection> genParticles;
  try{
    iEvent.getByToken(GenParticle_, genParticles); //76x
  }catch(...) {;}

  if(genParticles.isValid()){
    double pt_top = -1.0; double pt_antitop = -1.0;
    for(size_t i = 0; i < genParticles->size(); ++ i){
      const reco::GenParticle & part = (*genParticles)[i];
	  //int id = part.pdgId();
      //if(part.status() == 3 &&  part.pdgId() == 6)
      if(part.pdgId() == 6)
	    pt_top = part.pt();
	  //else if(part.status() == 3 &&  part.pdgId() == -6)
	  else if(part.pdgId() == -6)
	    pt_antitop = part.pt();
    }
    if(pt_top >= 0 && pt_antitop >= 0){
      //https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopPtReweighting
      double topPtWeight_comb = sqrt(exp(0.0615 - 0.0005*pt_top)*exp(0.0615 - 0.0005*pt_antitop));
      std::vector<double>topPtW; topPtW.clear();
      topPtW.push_back(topPtWeight_comb);
      mcInfo.topPtWeights = topPtW;
    }
  }

/////////////////////////////////////////////////////////////////////////
///am@nlo -ve weights
//https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMCatNLOInterface
////////////////////////////////////////////////////////////////////////
  edm::Handle<GenEventInfoProduct> genEventInfo;
  iEvent.getByToken(genEventInfo_, genEventInfo); 
  double genwt = genEventInfo->weight();
  if(genwt>0) mcInfo.gen_weight = 1.0;
  else mcInfo.gen_weight = -1.0;
  return mcInfo;
}

