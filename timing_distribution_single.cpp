#include "WaveformFitResult.hpp"
#include "ScanPoint.hpp"
#include "Utilities.hpp"
#include "FindCircle.hpp"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TF1.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

//Gaussian fitting function
// x[0] is x value
// par[0] = amplitude
// par[1] = mean
// par[2] = sigma
double my_gaussian(double *x, double *par) {
  double arg=0;
  if(par[2]!=0) arg=(x[0]-par[1])/par[2];
  double gfunc=par[0] * TMath::Exp( -0.5*arg*arg );
  return gfunc;
}

TString ToString(double num){
  ostringstream start;
  start<<num; //Double check if position works                                                                                                                                                                                                
  TString start1=start.str();
  return start1;

}

int main( int argc, char* argv[] ) {

  if ( argc != 3 ){
    std::cerr<<"Usage: ptf_timing_analysis.app ptf_analysis.root run_number\n";
    exit(0);
  }

  // Get utilities
  Utilities utils;

  // Set style
  utils.set_style();

  std::cout<<"Input file "<<argv[1]<<std::endl;
  TFile * fin = new TFile( argv[1], "read" );

  // Opening the output root file
  string outname = string("ptf_timing_analysis_run0") + argv[2] + ".root";
  //TFile * fout = new TFile(outname.c_str(), "NEW");
  //std::cout<<"Output file "<<outname<<std::endl;

  // get the scanpoints information
  std::vector< ScanPoint > scanpoints = ReadScanPoints( fin );


  // Make 2d histograms
  // Transit time, transit time spread
     TH1D* histos_pmt0= new TH1D("histos_pmt0", "Transit time (nsec); T (nsec); Counts/bin", 100,40,150 );
      TH1D* histos_pmt1= new TH1D("histos_pmt1", "Transit time (nsec); T (nsec); Counts/bin", 100,0,100 );
     // get the waveform fit TTree for PMT0 (The signal pmt)
  TTree * tt0 = (TTree*)fin->Get("ptfanalysis0");
  if ( !tt0 ){
    std::cerr<<"Failed to read TTree called ptfanalysis0, exiting"<<std::endl;
    return 0;
  }
  WaveformFitResult * wf0 = new WaveformFitResult;
  wf0->SetBranchAddresses( tt0 );

  // get the waveform fit TTree for PMT1 (The reference pmt)
    TTree * tt2 = (TTree*)fin->Get("ptfanalysis2");
  if ( !tt2 ){
    std::cerr<<"Failed to read TTree called ptfanalysis1, exiting"<<std::endl;
    return 0;
  }
  WaveformFitResult * wf2 = new WaveformFitResult;
  wf2->SetBranchAddresses( tt2 );
  
  //Loop through scanpoints to fill histograms
  for(unsigned int iscan=0; iscan<scanpoints.size(); iscan++){
    if (iscan%1000==0) std::cout<<"Filling histograms for iscan = "<<iscan<<" / "<<scanpoints.size()<<std::endl;
    ScanPoint scanpoint = scanpoints[ iscan ];
    //Loop over scanpoint
    for ( unsigned iev = 0; iev < scanpoint.nentries(); ++iev ){
      
      tt0->GetEvent( scanpoint.get_entry() + iev );
      tt2->GetEvent( scanpoint.get_entry() + iev );  
       if ( wf0->haswf ){
         histos_pmt0->Fill( wf0->mean );
	 histos_pmt1->Fill( wf2->mean );
	 // if(wf2->mean!=0){
	 // cout<<wf2->mean<<endl;
	 // }
	 //}
      
      // tt1->GetEvent( scanpoint.get_entry() + iev );
      //if ( wf1->haswf ){
      //  histos_pmt1->Fill( wf1->mean );
       }
    }
  }

  // Fit for each scanpoint
  std::vector< TF1* > vecpmtresponse;
  //double min_time = 70.;
  for ( unsigned iscan=0; iscan<scanpoints.size(); ++iscan ){
    // if ( histos_pmt0->GetEntries() < 100 ) {
    //  vecpmtresponse.push_back( nullptr );
      //std::cout<<"Skip "<<iscan
	  //     <<" with "<<h_pmt0_tscanpt[iscan]->GetEntries()
	  //     <<" entries"<<std::endl;
    // continue;
    // }
    std::ostringstream fname;
    fname << "pmt_response_" << iscan;
    //std::cout<<"Fitting "<<fname.str()
	//     <<" with "<<h_pmt0_tscanpt[iscan]->GetEntries()
	//     <<std::endl;
    TF1* ftmp = new TF1("gaussian_fit", my_gaussian, 40., 150., 3 );
    ftmp->SetParNames("Amplitude","Mean","Sigma");
    ftmp->SetParameter(0, 10000.);
    ftmp->SetParameter(1, 60.);
    ftmp->SetParameter(2, 10.);
    ftmp->SetParLimits(0, 0.0, 20000.0);
    ftmp->SetParLimits(1, 20.0, 80.0 );
    ftmp->SetParLimits(2, 0.0, 100.0 );
    histos_pmt0->Fit( ftmp, "Q", "", 50.,100.0);
    //vecpmtresponse.push_back( ftmp );
    //if( ftmp->GetParameter(1) > 25. &&
    //  ftmp->GetParameter(1) < min_time ) min_time = ftmp->GetParameter(1);
  }

  //Now fill 2d plots

  
  //Remove data outside circle
  //TH2D* h_rtt_grad;
  //Circle_st circ = find_circle_max_grad( h_rtt, h_rtt_grad, 0.5 );
  //zero_outside_circle( h_rtt, circ );
  //zero_outside_circle( h_tts, circ );

  //Set plot ranges
  //h_rtt->SetMinimum(34.5);
  //h_rtt->SetMaximum(40.0);
  //h_tts->SetMinimum(2.2);
  //h_tts->SetMaximum(4.4);


  //Make plots
  TCanvas* c = new TCanvas("canvas");
  string plotname;
  //TFile * fout = new TFile(outname.c_str(), "RECREATE");

  TFile * f1=new TFile("timing_distribution.root","RECREATE");
  histos_pmt0->SetDirectory(f1);
  histos_pmt1->SetDirectory(f1);

  // histos_pmt0->SetStats(1);
   // gStyle->SetOptStat(1);
  // gStyle->SetOptFit(1);
   gPad->Modified(); gPad->Update();
   histos_pmt0->GetYaxis()->SetTitle("Counts/bin");
   histos_pmt0->GetXaxis()->SetTitle("T(nsec)");
   histos_pmt0->Draw();
   
   //   histos_pmt0->Write();
   // histos_pmt1->Write();
   plotname = string("/home/vgousy/projects/def-pdeperio/vgousy/ptf-analysis/timing_distribution/timing_single.pdf");
   c->SaveAs(plotname.c_str(),"pdf");

   f1->Write();
   f1->Close();

  cout << "Done" << endl; 

  return 0;
}
