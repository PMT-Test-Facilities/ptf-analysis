#include "wrapper.hpp"


using namespace std;
using namespace PTF;
using namespace Private;


Wrapper::Wrapper(unsigned long long maxSamples, unsigned long long sampleSize, const vector<PMTChannel>& activeChannels, const vector<int>& phidgets)
  : maxSamples(maxSamples), sampleSize(sampleSize)
{
  for (auto chPair : activeChannels) {
    double* data = new double[maxSamples * sampleSize];
    PMTSet* pmtSet  = new PMTSet();
    pmtSet->channel = chPair.channel;
    pmtSet->data    = data;
    pmtData[chPair.pmt] = pmtSet;
  }
  for (auto phidget : phidgets) {
    PhidgetSet* pSet = new PhidgetSet();
    phidgetData[phidget] = pSet;
  }

}


Wrapper::Wrapper(unsigned long long maxSamples, unsigned long long sampleSize, const vector<PMTChannel>& activeChannels, const vector<int>& phidgets, const string& fileName, const string& treeName)
  : Wrapper(maxSamples, sampleSize, activeChannels, phidgets) {
  openFile(fileName, treeName);
}


Wrapper::~Wrapper() {
  return;
  // for some reason doing cleanup below causes
  // program to crash as exiting?
  for (auto pmt : pmtData) {
    delete[] pmt.second->data;
    delete pmt.second;
  }
  
  for (auto phidget : phidgetData) {
    delete phidget.second;
  }
  if (tree) {
    unsetDataPointers();
    delete tree;
    tree = nullptr;
  }
  if (file) {
    file->Close();
    delete file;
    file = nullptr;
  }
}


/* Private functions */

//***********************
// Add a function to retrieve the waveform information.
// double* Wrapper::getDataForMultiPmt() const {
//   return mpmtreading->wvfm0;
// }
//***********************

double* Wrapper::getDataForPmt(int pmt) const {
  // Could be replaced with binary search, but probably list is small enough to not matter
  auto res = pmtData.find(pmt);

  if (res != pmtData.end()) {
    return res->second->data;
  }
  else {
    return nullptr;
  }
}


bool Wrapper::setDataPointers() {
  if (tree == nullptr || file == nullptr)
    return false;

  //******************
  // Set mPMT branches
  MultiPMTset* mpmtset = new MultiPMTset();
  MultiPmtReading* mpmtreading = new MultiPmtReading();
  mpmtset->waveform0 = nullptr;
  mpmtset->waveform1 = nullptr;
  mpmtset->waveform2 = nullptr;
  mpmtset->waveform3 = nullptr;
  mpmtset->waveform4 = nullptr;
  mpmtset->waveform5 = nullptr;
  mpmtset->waveform0 = tree->GetBranch("V1730_wave0");
  mpmtset->waveform1 = tree->GetBranch("V1730_wave1");
  mpmtset->waveform2 = tree->GetBranch("V1730_wave2");
  mpmtset->waveform3 = tree->GetBranch("V1730_wave3");
  mpmtset->waveform4 = tree->GetBranch("V1730_wave4");
  mpmtset->waveform5 = tree->GetBranch("V1730_wave5");
  mpmtset->waveform0->SetAddress(&mpmtreading->wvfm0);
  mpmtset->waveform0->SetAddress(&mpmtreading->wvfm1);
  mpmtset->waveform0->SetAddress(&mpmtreading->wvfm2);
  mpmtset->waveform0->SetAddress(&mpmtreading->wvfm3);
  mpmtset->waveform0->SetAddress(&mpmtreading->wvfm4);
  mpmtset->waveform0->SetAddress(&mpmtreading->wvfm5);

  cout<<mpmtreading->wvfm0[0][1]<<endl;
  //******************

  // Set PMT branches
  char branchName[64];
  for (auto pmt : pmtData) {
    snprintf(branchName, 64, PMT_CHANNEL_FORMAT, pmt.second->channel);
    pmt.second->branch = nullptr;
    pmt.second->branch = tree->GetBranch(branchName);
    if (pmt.second->branch == nullptr) {
      return false;
    }
    pmt.second->branch->SetAddress(pmt.second->data);
  }

  // Set phidget branches
  for (auto phidget : phidgetData) {
    snprintf(branchName, 64, PHIDGET_FORMAT_X, phidget.first);
    phidget.second->branchX = nullptr;
    phidget.second->branchX = tree->GetBranch(branchName);
    phidget.second->branchX->SetAddress(&phidget.second->data.Bx);

    snprintf(branchName, 64, PHIDGET_FORMAT_Y, phidget.first);
    phidget.second->branchY = nullptr;
    phidget.second->branchY = tree->GetBranch(branchName);
    phidget.second->branchY->SetAddress(&phidget.second->data.By);

    snprintf(branchName, 64, PHIDGET_FORMAT_Z, phidget.first);
    phidget.second->branchZ = nullptr;
    phidget.second->branchZ = tree->GetBranch(branchName);
    phidget.second->branchZ->SetAddress(&phidget.second->data.Bz);

    if (phidget.second->branchX == nullptr
        || phidget.second->branchY == nullptr
        || phidget.second->branchZ == nullptr) {
      return false;
    }
  }

  TBranch
    *g0X = tree->GetBranch("gantry0_x"), *g0Y = tree->GetBranch("gantry0_y"), *g0Z = tree->GetBranch("gantry0_z"),
      *g0Theta = tree->GetBranch("gantry0_rot"), *g0Phi = tree->GetBranch("gantry0_tilt"),
    *g1X = tree->GetBranch("gantry1_x"), *g1Y = tree->GetBranch("gantry1_y"), *g1Z = tree->GetBranch("gantry1_z"),
      *g1Theta = tree->GetBranch("gantry1_rot"), *g1Phi = tree->GetBranch("gantry1_tilt"),

    *brNumSamples = tree->GetBranch("num_points");

  g0X->SetAddress(&g0.x);
  g0Y->SetAddress(&g0.y);
  g0Z->SetAddress(&g0.z);
  g0Theta->SetAddress(&g0.theta);
  g0Phi->SetAddress(&g0.phi);

  g1X->SetAddress(&g1.x);
  g1Y->SetAddress(&g1.y);
  g1Z->SetAddress(&g1.z);
  g1Theta->SetAddress(&g1.theta);
  g1Phi->SetAddress(&g1.phi);

  brNumSamples->SetAddress(&numSamples);

  return true;
}


bool Wrapper::unsetDataPointers() {
  if (tree == nullptr || file == nullptr)
    return false;
  
  for (auto pmt : pmtData) {
    // if (pmt.second.branch)
    //   delete pmt.second.branch;
    pmt.second->branch = nullptr;
  }

  for (auto phidget : phidgetData) {
    // if (phidget.second.branchX)
    //   delete phidget.second.branchX;
    phidget.second->branchX = nullptr;

    // if (phidget.second.branchY)
    //   delete phidget.second.branchY;
    phidget.second->branchY= nullptr;

    // if (phidget.second.branchZ)
    //   delete phidget.second.branchZ;
    phidget.second->branchZ = nullptr;
  }

  return true;
}


/* Public functions */


void Wrapper::openFile(const string& fileName, const string& treeName) {
  file = new TFile(fileName.c_str(), "READ");

  if (!file->IsOpen()) {
    delete file;
    file = nullptr;
    throw new Exceptions::FileDoesNotExist(fileName);
  }

  tree = nullptr;
  file->GetObject(treeName.c_str(), tree);
  // tree->Print();

  if (!tree) {
    throw new Exceptions::InvalidTreeName(treeName);
  }

  cout<<treeName<<endl;
  auto res = setDataPointers();

  if (!res) {
    throw new Exceptions::DataPointerError();
  }

  numEntries = tree->GetEntries();

  cout << numEntries << endl;
  tree->GetEntry(0);
  // cout << "Opening root file." << endl;
  // cout << tree->GetEntry(0)<<endl;
  // entry = 0;
}


bool Wrapper::isFileOpen() const {
  return file && tree;
}


void Wrapper::closeFile() {
  if (tree) {
    unsetDataPointers();
    delete tree;
    tree = nullptr;
  }
  if (file) {
    file->Close();
    delete file;
    file = nullptr;
  }
  entry = UINT32_MAX;
}


int Wrapper::getChannelForPmt(int pmt) const {
  // Could be replaced with binary search, but probably list is small enough to not matter
  auto res = pmtData.find(pmt);

  if (res != pmtData.end()) {
    return res->second->channel;
  }
  else {
    return -1;
  }
}


int Wrapper::getPmtForChannel(int channel) const {
  for (auto pmt : pmtData) {
    if (pmt.second->channel == channel) {
      return pmt.first;
    }
  }
  return -1;
}


unsigned long long Wrapper::getCurrentEntry() const {
  if (!isFileOpen()) {
    throw new Exceptions::NoFileIsOpen();
  }
  return entry;
}


unsigned long long Wrapper::getNumEntries() const {
  if (!isFileOpen()) {
    throw new Exceptions::NoFileIsOpen();
  }
  return numEntries;
}


void Wrapper::setCurrentEntry(unsigned long long entry) {
  if (!isFileOpen()) {
    throw new Exceptions::NoFileIsOpen();
  }
  if (entry >= numEntries) {
    throw new Exceptions::EntryOutOfRange();
  }

  this->tree->GetEntry(entry);
  this->entry = entry;
}


unsigned long long Wrapper::getNumSamples() const {
  if (!isFileOpen()) {
    throw new Exceptions::NoFileIsOpen();
  }
  return numSamples;
}


double* Wrapper::getPmtSample(int pmt, unsigned long long sample) const {
  if (sample > numSamples) {
    throw new Exceptions::SampleOutOfRange();
  }
  auto pmtData = getDataForPmt(pmt);
  if (pmtData == nullptr) {
    throw new Exceptions::InvalidPMT();
  }
  return pmtData + (sample * sampleSize);
}


int Wrapper::getSampleLength() const {
  return sampleSize;
}


GantryPos Wrapper::getDataForCurrentEntry(Gantry whichGantry) const {
  if (!isFileOpen()) {
    throw new Exceptions::NoFileIsOpen();
  }

  return whichGantry == Gantry0 ? g0 : g1;
}


PhidgetReading Wrapper::getReadingForPhidget(int phidget) const {
  if (!isFileOpen()) {
    throw new Exceptions::NoFileIsOpen();
  }
  auto res = phidgetData.find(phidget);
  if (res == phidgetData.end()) {
    throw new Exceptions::InvalidPhidget();
  }
  else {
    return res->second->data;
  }
}
