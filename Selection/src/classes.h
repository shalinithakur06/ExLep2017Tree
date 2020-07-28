#include "ExLep2017Tree/Selection/interface/MyEvent.h"
#include "ExLep2017Tree/Selection/interface/MyJet.h"
#include "ExLep2017Tree/Selection/interface/MyMET.h"
#include "ExLep2017Tree/Selection/interface/MyVertex.h"
#include "ExLep2017Tree/Selection/interface/MyElectron.h"
#include "ExLep2017Tree/Selection/interface/MyMuon.h"
//#include "ExLep2016Tree/Selection/interface/MyTau.h"
#include "ExLep2017Tree/Selection/interface/SampleInfo.h"
#include "ExLep2017Tree/Selection/interface/MomentumVec.h"

#include<vector>
#include "TROOT.h"

namespace{
  namespace{
    std::map<int   , std::vector<int> >        dummy1;
    std::pair<float, std::string>              dummy2;
    std::vector<std::pair<float, std::string> > dummy3;
    MyEvent          Myevt; 
    MyVertex         Myvertex; 
    MyJet            Myjet; 
    MyMET            Mymet; 
    MyElectron       Myele; 
    MyMuon           Mymuon; 
    SampleInfo       sampleinfo;
    std::vector<MyVertex>         Myvertices; 
    std::vector<MyJet>            Myjets; 
    std::vector<MyMET>            Mymets; 
    std::vector<MyElectron>       Myeles; 
    std::vector<MyMuon>           Mymuons; 
    std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > > lvec;
    MyLorentzVector         vec;
    Point3D               point;  
  }
}
