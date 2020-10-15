/// Analysis of PTF data
///
/// Definitions of terminology used, and associated structures:
///
/// Each input root file has a number of "ScanPoint" events, where each event has
/// several waveforms at a given location.
///
/// class  MeanRMSCalc           For calculating mean and rms
///
/// class  PTFErrorBarAnalysis   For calculating the error bar size to use on the waveforms
///
/// struct WaveformFitResult     Structure to hold one waveform fit result
///
/// class  ScanPoint             Holds location of scan point, first entry number in TTree of scan point, and number of waveforms
///
/// class  PTFAnalysis           For doing analysis of all of the waveforms, and keep track of scan points, store results in TTree
///
///
/// This program takes a PTF scan root file as input.
/// The analysis proceeds in these steps:
/// 1. Determine the uncertainties on the "collected charge" by studying the pedestal
///    of the waveforms in bins 1-20.  This part of the analysis produces two kinds of
///    pedestal histograms.
/// 2. Fit the waveforms to gaussians.  Each waveform is read in once into a Waveform object,
///    it is then fit to a gaussian using a fitter.  Fit results are stored in a TTee that
///    has one entry per scan-point
/// 3. The now filled TTree of fitted waveform parameters is analyzed to make histograms
///     
/// Author: Blair Jamieson (Sep. 2019)
/// Update by T. Lindner (June 2020) for mPMT analysis


#include "wrapper.hpp"
#include "MeanRMSCalc.hpp"
#include "ErrorBarAnalysis.hpp"
#include "WaveformFitResult.hpp"
#include "ScanPoint.hpp"
#include "PTFAnalysis.hpp"
#include "PTFQEAnalysis.hpp"
#include "Utilities.hpp"
#include <string>
#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <exception>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <unordered_set>

#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TF1.h"
#include "TVirtualFitter.h"
#include "TFitter.h"
#include "TMath.h"
#include "TStyle.h"
#include "Configuration.hpp"

using namespace std;

int main(int argc, char** argv) {
  if (argc != 4) {
    cerr << "give path to file to read" << endl;
    cerr << "usage: ptf_analysis filename.root run_number config_file" << endl;
    return 0;
  }

  std::cout << "Creating utilities " << std::endl;
  // Get utilities
  Utilities utils;

  // Set style
  utils.set_style();

  // Opening the output root file
  string outname = string("mpmt_Analysis_run0") + argv[2] + ".root";
  TFile * outFile = new TFile(outname.c_str(), "NEW");
  //TFile * outFile = new TFile("ptf_analysis.root", "NEW");

  // Set up PTF Wrapper
  vector<int> phidgets = {0, 1, 3};
  vector<PTF::Gantry> gantries = {PTF::Gantry0, PTF::Gantry1};

  bool first_ch = true;
  vector<PTF::PMT> activePMTs;

  // Choose which channels to analyze.  Default is channels 0 and 1.
  int first_channel = 0, last_channel = 1;
  Configuration config;
  
  config.Load(string(argv[3]));
  if(!config.Get("first_channel", first_channel) ){ first_channel = 0;};
  if(!config.Get("last_channel", last_channel) ){ last_channel = 0;};
  if(first_channel >= last_channel or first_channel < 0 or first_channel > 19
     or last_channel < 0 or last_channel > 19){
    std::cout << "Invalid choice of first and last channels: " << first_channel << " " << last_channel << std::endl;
    exit(0);
  }
  std::cout << "Analyzing channels " << first_channel << " to " << last_channel << std::endl;

  for(int ch = first_channel; ch < (last_channel+1); ch++){ // Loop over the channels you want to analyze    
    PTF::PMT PMT = {ch,ch,PTF::mPMT_REV0_PMT};
    activePMTs.push_back(PMT);
  }
  
  PTF::Wrapper wrapper = PTF::Wrapper(1, 1024, activePMTs, phidgets, gantries, PTF::mPMT_DIGITIZER);
  std::cout << "Open file: " << std::endl;
  wrapper.openFile( string(argv[1]), "scan_tree");
  cerr << "Num entries: " << wrapper.getNumEntries() << endl << endl;

  for(int ch = first_channel; ch < (last_channel+1); ch++){ // Loop over the channels you want to analyze    
    
    // Do analysis of waveforms for each scanpoint
    PTFAnalysis *analysis = new PTFAnalysis( outFile, wrapper, 2.1e-3, activePMTs[ch-first_channel], string(argv[3]), true );

    if(first_ch){
      analysis->write_scanpoints();
      first_ch = false;
    }

    std::cout << std::endl << "Finished channel " << ch << std::endl;  
  }
    
  outFile->Write();
  outFile->Close();
    
  cout << "Done" << endl; 

  return 0;
}

