// -*- C++ -*-
//
// Package:    EventSelection
// Class:      EventSelection
// 
/**\class EventSelection EventSelection.cc MyAnalysis/EventSelection/src/EventSelection.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Aruna Kumar Nayak
//         Created:  Fri Mar 11 12:42:51 WET 2011
// $Id$
//
//

#ifndef MINITREE_PRODUCER_H
#define MINITREE_PRODUCER_H


// system include files
#include <memory>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TH1D.h"
#include "TH2D.h"

//
// class declaration
//
#include "ExLep2017Tree/Selection/interface/MyEventSelection.h"

class TFile;
class TTree;

class MiniTreeProducer : public edm::EDAnalyzer {
public:
  explicit MiniTreeProducer(const edm::ParameterSet&);
  virtual ~MiniTreeProducer();
  
  
  //fill tree
  virtual void beginJob() ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;
  
  // ----------member data ---------------------------
private:
  void book(const edm::ParameterSet& ps);
  void analyzeEvent(const edm::Event& e, const edm::EventSetup&);
  
  // Event info
  MyEventSelection* pMyevt; 
  MyEvent* pMyevt_data;
  //std::map<std::string, TH1D*>histos_;
  std::map<TString, TH1D*>histos_;
  std::map<std::string, TH2D*>histos2D_;
  
  edm::InputTag hltSource_;
  
  //   
  edm::Service<TFileService> tfs_;
  std::vector<TFileDirectory> tdirs_;
  TTree* tree_;

  int minEventQualityToStore_;
};

#endif
