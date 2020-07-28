#include "ExLep2017Tree/Selection/interface/MyEventSelection.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "PhysicsTools/PatUtils/interface/TriggerHelper.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include "DataFormats/BTauReco/interface/SecondaryVertexTagInfo.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

std::vector<MyJet> MyEventSelection::getJets(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace std; 
  std::vector<MyJet> selJets; 
  selJets.clear();
  iEvent.getByToken( jetIDMapToken_, hJetIDMap ); // 76x
  //config parameters
  double minPt = configParamsJets_.getParameter<double>("minPt");
  double maxEta = configParamsJets_.getParameter<double>("maxEta");
  try{
    TString rawtag="Jets";
    //std::string tag(rawtag);
    TString tag(rawtag);
    edm::Handle<pat::JetCollection>ijets;
    iEvent.getByToken( Jetsources, ijets);  
    //Jet resolution and scale factors
    //https://github.com/cms-jet/JRDatabase/tree/master/textFiles/Spring16_25nsV10_MC
    //https://insight.io/github.com/cms-sw/cmssw/blob/master/JetMETCorrections/Modules/plugins/JetResolutionDemo.cc
    edm::Handle<double> rho;
    iEvent.getByToken(m_rho_token, rho);
    JME::JetResolution resolution;
    JME::JetResolutionScaleFactor res_sf;
    //cout<<"m_resolutions_file = "<<m_resolutions_file<<endl;
    resolution = JME::JetResolution(m_resolutions_file);
    res_sf = JME::JetResolutionScaleFactor(m_scale_factors_file);
    if(ijets.isValid()){
      edm::ESHandle<JetCorrectorParametersCollection> JetCorParColl;
      //get the jet corrector parameters collection from the globaltag
      //iSetup.get<JetCorrectionsRecord>().get("slimmedJetsAK8PFPuppiSoftDropPacked",JetCorParColl);
      iSetup.get<JetCorrectionsRecord>().get("AK8PF",JetCorParColl);
      // get the uncertainty parameters from the collection
      JetCorrectorParameters const & JetCorPar = (*JetCorParColl)["Uncertainty"]; 
      // instantiate the jec uncertainty object
      JetCorrectionUncertainty *jecUnc = new JetCorrectionUncertainty(JetCorPar); 
      for(size_t iJet = 0; iJet < ijets->size(); iJet++){
        const pat::Jet jIt = ((*ijets)[iJet]);
        //Jet resolution and scale factors
        JME::JetParameters parameters_5 = {{JME::Binning::JetPt, jIt.pt()}, {JME::Binning::JetEta, jIt.eta()}, {JME::Binning::Rho, *rho}};
        float reso = resolution.getResolution(parameters_5);
        float sf = res_sf.getScaleFactor({{JME::Binning::JetEta, jIt.eta()}});
        //JEC uncertainty
        jecUnc->setJetEta(jIt.eta());
        jecUnc->setJetPt(jIt.pt());  
        float JECUncertainty = jecUnc->getUncertainty(true);
        MyJet newJet = MyJetConverter(jIt, rawtag, reso);
        newJet.jetName = tag;
        newJet.scaleFactor = sf;
        newJet.resolution = reso; 
        newJet.JECUncertainty = JECUncertainty;
        //make selections
        if(jIt.pt() > minPt && fabs(jIt.eta()) < maxEta)
        selJets.push_back(newJet);
      } // for loop
      delete jecUnc;
      fs_->cd();
    }
  }catch(std::exception &e){
    std::cout << "[Jet Selection] : check selection " << e.what() << std::endl;
  }
  return selJets;
}

MyJet MyEventSelection::MyJetConverter(const pat::Jet& iJet, TString& dirtag, double JER){
  MyJet newJet;
  newJet.Reset();
  ///basic
  const reco::GenJet *genJet = iJet.genJet();
  if(genJet){
    newJet.Genp4.SetCoordinates(genJet->px(), genJet->py(), genJet->pz(), genJet->energy());
  }
  newJet.jetCharge = iJet.jetCharge();
  newJet.p4.SetCoordinates(iJet.px(), iJet.py(), iJet.pz(), iJet.energy());
  //MC truth
  const reco::GenParticle *genParton = iJet.genParton();
  newJet.parton_id = (genParton != 0 ? genParton->pdgId() : 0 );
  /*
    if(genParton){
    if(genParton->numberOfMothers() > 0){
      newJet.parton_mother_id = genParton->mother()->pdgId();
    }
  } */
  newJet.ak8Tau1 = iJet.userFloat("NjettinessAK8Puppi:tau1"); 
  newJet.ak8Tau2 = iJet.userFloat("NjettinessAK8Puppi:tau2"); 
  newJet.ak8Tau3 = iJet.userFloat("NjettinessAK8Puppi:tau3"); 
  newJet.ak8Pmass = iJet.userFloat("ak8PFJetsCHSValueMap:ak8PFJetsCHSPrunedMass");
  newJet.partonFlavour = double(iJet.partonFlavour());
  newJet.hadronFlavour = double(iJet.hadronFlavour());
  //vertex.fCoordinates.fXYZ are zero, in the MINIAOD
  newJet.vertex.SetCoordinates(iJet.vx(), iJet.vy(), iJet.vz());
  ///ids
  if(iJet.isPFJet())
    {     
      newJet.neutralHadronEnergyFraction = iJet.neutralHadronEnergyFraction(); 
      newJet.neutralEmEnergyFraction = iJet.neutralEmEnergyFraction();
      newJet.NumConst = iJet.chargedMultiplicity()+ iJet.neutralMultiplicity();
      newJet.muonEnergyFraction = iJet.muonEnergyFraction();
      newJet.chargedHadronEnergyFraction = iJet.chargedHadronEnergyFraction();
      newJet.chargedMultiplicity = iJet.chargedMultiplicity();
      newJet.chargedEmEnergyFraction = iJet.chargedEmEnergyFraction();
      newJet.neutralMultiplicity = iJet.neutralMultiplicity();
    }
  //https://github.com/rappoccio/usercode/blob/Dev_53x/EDSHyFT/plugins/BTaggingEffAnalyzer.cc
  //2D histos to calculate Btag efficiency
  double partonFlavor = newJet.partonFlavour;
  ///double csv = iJet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
  double csv1 = iJet.bDiscriminator("pfDeepCSVJetTags:probb");
  double csv2 = iJet.bDiscriminator("pfDeepCSVJetTags:probbb");
  double csv = csv1 + csv2;
  double csvL = 0.1522;
  double csvM = 0.4941;
  double csvT = 0.8001;
  //b-quarks
  if( abs(partonFlavor)==5 ){
    myhistos2_["h2_BTagEff_Denom_b_"+dirtag]->Fill(iJet.pt(), iJet.eta());
    if( csv > csvL ) myhistos2_["h2_BTagEff_Num_bL_"+dirtag]->Fill(iJet.pt(), iJet.eta());
    if( csv > csvM ) myhistos2_["h2_BTagEff_Num_bM_"+dirtag]->Fill(iJet.pt(), iJet.eta());
    if( csv > csvT ) myhistos2_["h2_BTagEff_Num_bT_"+dirtag]->Fill(iJet.pt(), iJet.eta());
  }
  //c-quarks
  else if( abs(partonFlavor)==4 ){
    myhistos2_["h2_BTagEff_Denom_c_"+dirtag]->Fill(iJet.pt(), iJet.eta());
    if( csv > csvL ) myhistos2_["h2_BTagEff_Num_cL_"+dirtag]->Fill(iJet.pt(), iJet.eta());
    if( csv > csvM ) myhistos2_["h2_BTagEff_Num_cM_"+dirtag]->Fill(iJet.pt(), iJet.eta());
    if( csv > csvT ) myhistos2_["h2_BTagEff_Num_cT_"+dirtag]->Fill(iJet.pt(), iJet.eta());
  }
  //other quarks and gluon
  else{
    myhistos2_["h2_BTagEff_Denom_udsg_"+dirtag]->Fill(iJet.pt(), iJet.eta());
    if( csv > csvL ) myhistos2_["h2_BTagEff_Num_udsgL_"+dirtag]->Fill(iJet.pt(), iJet.eta());
    if( csv > csvM ) myhistos2_["h2_BTagEff_Num_udsgM_"+dirtag]->Fill(iJet.pt(), iJet.eta());
    if( csv > csvT ) myhistos2_["h2_BTagEff_Num_udsgT_"+dirtag]->Fill(iJet.pt(), iJet.eta());
  }
 //btag : https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation80XReReco
  std::map<std::string, double> discr; discr.clear();
  ///discr["pfCombinedInclusiveSecondaryVertexV2BJetTags"] = iJet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
  discr["pfDeepCSVb"] = iJet.bDiscriminator("pfDeepCSVJetTags:probb");
  discr["pfDeepCSVbb"] = iJet.bDiscriminator("pfDeepCSVJetTags:probbb");
  newJet.bDiscriminator = discr;
  //JECs
  std::map<std::string, double>jetCorrections; jetCorrections.clear();
  const std::vector<std::string> jeclevels = iJet.availableJECLevels();
  for(size_t j = 0; j < jeclevels.size(); j++){
    std::string levelName = jeclevels[j];
    if(levelName.find("L5Flavor") != std::string::npos ||
       levelName.find("L7Parton") != std::string::npos ){
      jetCorrections[levelName] = iJet.jecFactor(levelName, "bottom");
    }
    else{ jetCorrections[levelName] = iJet.jecFactor(levelName); }
  }
  newJet.JECs = jetCorrections;
  newJet.JECUncertainty = 1.0;  //default, get it later from CondDB.
  return newJet;
}

