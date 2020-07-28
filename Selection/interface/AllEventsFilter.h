#ifndef AllEventsFilter_h
#define AllEventsFilter_h
//
// Original Author:  Nayak Aruna
//         Created:  Wed Apr 18 11:57:38 CEST 2012
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TH1F.h"

//
// class declaration
//

class AllEventsFilter : public edm::EDFilter {
public:
  explicit AllEventsFilter(const edm::ParameterSet&);
  ~AllEventsFilter();
  
  
private:
  virtual bool filter(edm::Event&, const edm::EventSetup&);
  TH1F* totalEvents_;
  // ----------member data ---------------------------
  
};

#endif
