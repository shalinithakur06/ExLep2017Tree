#include "ExLep2017Tree/Selection/interface/MyEventSelection.h"
#include <typeinfo>
std::vector<MyVertex> MyEventSelection::getVertices(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  
  std::vector<MyVertex> selVertices; 
  selVertices.clear();

  try{
    bestPrimVertex_ = 0;
    refPoint_ = math::XYZPoint(0,0,0);
    double maxZ = configParamsVertex_.getParameter<double>("maxZ");
    double maxRho = configParamsVertex_.getParameter<double>("maxRho");
    int minNDOF = configParamsVertex_.getParameter<int>("minNDOF");
    edm::Handle<reco::VertexCollection>vtx_;
    iEvent.getByToken(vtxSource, vtx_);
    std::vector<const reco::Vertex *> selVtx; selVtx.clear();
    for(size_t ivtx = 0; ivtx < vtx_->size(); ivtx++){
      const reco::Vertex *vIt = &((*vtx_)[ivtx]);
      //base quantities
      bool isReal = !(vIt->isFake());
      double z = fabs(vIt->z());
      double rho = vIt->position().Rho();
      int ndof = vIt->ndof();
      //https://twiki.cern.ch/twiki/bin/view/CMSPublic/TrackingPOGPlotsVertex2014
      if( isReal && z < maxZ && rho < maxRho && ndof >= minNDOF )
      selVtx.push_back(vIt);
    }
    //std::sort(selVtx.begin(), selVtx.end(), &sumPtOrder);
    if(selVtx.size()!=0){
      bestPrimVertex_ = selVtx[0];
      refPoint_ = bestPrimVertex_->position();
      refVertex_ = *bestPrimVertex_;
    }
    //fixedGridRhoAll: 
    //https://github.com/cms-analysis/flashgg/blob/e2fac35487f23fe05b20160d7b51f34bd06b0660/Taggers/python/globalVariables_cff.py
    edm::Handle<double>rhoAll;
    iEvent.getByToken(rhoSource, rhoAll);
    const reco::Vertex *vIt; 
    if(selVtx.size()!=0){
      vIt = selVtx[0];
      int totVtx = selVtx.size();
      MyVertex newVertex = MyVertexConverter(*vIt, *rhoAll, totVtx);
      selVertices.push_back(newVertex);
    }
  }catch(std::exception &e){
    std::cout << "[Vertex Selection] : check selection " << e.what() << std::endl;
  }
  
  return selVertices;
}


MyVertex MyEventSelection::MyVertexConverter(const reco::Vertex& iVertex, double rhoAll, int totVtx)
{
  MyVertex newVertex;
  newVertex.Reset(); 
  newVertex.chi2 = iVertex.chi2();
  newVertex.totVtx = totVtx;
  newVertex.normalizedChi2 = iVertex.chi2()/iVertex.ndof();
  newVertex.ndof = iVertex.ndof();
  newVertex.rho = iVertex.position().Rho();
  newVertex.rhoAll = rhoAll;
  newVertex.ErrXYZ.SetCoordinates(iVertex.xError(), iVertex.yError(), iVertex.zError());
  newVertex.isValid = !(iVertex.isFake());
  newVertex.XYZ.SetCoordinates(iVertex.x(), iVertex.y(), iVertex.z());
  return newVertex;
}

