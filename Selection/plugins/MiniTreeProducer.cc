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
#include "ExLep2017Tree/Selection/plugins/MiniTreeProducer.h"

//---------------
// C++ Headers --
//---------------

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>


//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

//
// constructors and destructor
//
MiniTreeProducer::MiniTreeProducer(const edm::ParameterSet& ps) : tree_(0) 

{
   //now do what ever initialization is needed
  pMyevt                = new MyEventSelection(ps, consumesCollector());
  
  pMyevt_data           = pMyevt->getData();
  
  tree_ = tfs_->make<TTree>("MyTree", "MyTree");
  tree_->SetAutoSave(10000000000);
  tree_->SetAutoFlush(1000000);
  
  book(ps);
  
  minEventQualityToStore_ = ps.getParameter<int>("minEventQualityToStore");
  
}


MiniTreeProducer::~MiniTreeProducer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
  delete pMyevt;

}

// ------------ method called once each job just before starting event loop  ------------
void 
MiniTreeProducer::beginJob(){
}

// ------------ method called once each job just after ending the event loop  ------------
void 
MiniTreeProducer::endJob() {
}

// ------------ method called to for each event  ------------
void
MiniTreeProducer::analyze(const edm::Event& e, const edm::EventSetup& es)
{
  analyzeEvent(e, es);    
  
  int eventQuality = pMyevt_data->eventQuality;
  if(eventQuality < minEventQualityToStore_) return;
  tree_->Fill();
}

void MiniTreeProducer::analyzeEvent(const edm::Event& e, const edm::EventSetup& es) { 
  

    pMyevt->Reset();
    pMyevt->Set(e, es); 

}

//--------------
// Operations --
//--------------
void MiniTreeProducer::book(const edm::ParameterSet& ps) {
  //book tree
   int bufsize = 256000;
   int split   = 1;
   tree_->Branch("MyEvent", "MyEvent", &pMyevt_data, bufsize, split);
  //pMyevt->BookHistos(tfs_);
  pMyevt->BookHistos();
}

//define this as a plug-in
DEFINE_FWK_MODULE(MiniTreeProducer);
