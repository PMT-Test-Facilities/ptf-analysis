
#include "pmt_response_function.hpp"

#include "PTFAnalysis.hpp"
#include "Configuration.hpp"

#include "WaveformFitResult.hpp"

#include "PulseFinding.hpp"
#include "ScanPoint.hpp"
#include "Utilities.hpp"
#include "TFile.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TFitResult.h"
#include "TVirtualFFT.h"
#include "TFitResultPtr.h"
#include "TMatrixDSym.h"
#include "FindCircle.hpp"
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "TString.h"
#include <sstream>

using namespace std;

const double sqrt2pi = std::sqrt( 2 * std::acos(-1) );

TString ToString(double num){
  ostringstream start;
  start<<num; //Double check if position works
  TString start1=start.str();
  return start1;

}

double my_gaussian(double *x, double *par) {
  double arg=0;
  if(par[2]!=0) arg=(x[0]-par[1])/par[2];
  double gfunc=par[0] * TMath::Exp( -0.5*arg*arg );
  return gfunc;
}
int main( int argc, char* argv[] ) {
 
  if ( argc != 3 && argc != 4){
    std::cerr<<"Usage: timing_distribution_total.app ptf_analysis.root run_number"<<std::endl;
    // std::cerr<<"Where the T/F/I is for True to do/not do circle fit to find PMT, I to cut inside circle (default T)\n"<<std::endl;
    exit(0);
  }

   // Get utilities
  Utilities utils;

  // Set style
  utils.set_style();

  std::cout<<"Input file "<<argv[1]<<std::endl;
  TFile * fin = new TFile( argv[1], "read" );

  // Opening the output root file
  //string outname = string("ptf_charge_analysis_run0") + argv[2] + ".root";
  //TFile * fout = new TFile(outname.c_str(), "NEW");
  //std::cout<<"Output file "<<outname<<std::endl;

  // get the scanpoints information
  std::vector< ScanPoint > scanpoints = ReadScanPoints( fin );

  vector< double > xbins = utils.get_bins( scanpoints, 'x' );
  vector< double > ybins = utils.get_bins( scanpoints, 'y' );

  // get the waveform fit TTree for PMT1 (The signal pmt)
    TTree * tt2 = (TTree*)fin->Get("ptfanalysis2");
  if ( !tt2 ){
    std::cerr<<"Failed to read TTree called ptfanalysis2, exiting"<<std::endl;
    return 0;
   }
   TTree * tt0 = (TTree*)fin->Get("ptfanalysis0");
  if ( !tt0 ){
    std::cerr<<"Failed to read TTree called ptfanalysis0, exiting"<<std::endl;
    return 0;
  }
  WaveformFitResult * wf0 = new WaveformFitResult;
  wf0->SetBranchAddresses( tt0 );

     WaveformFitResult * wf2 = new WaveformFitResult;
  wf2->SetBranchAddresses( tt2 );
  
  int nb_points=scanpoints.size();
  TH1D* histos_tt[nb_points];
  TH1D* histos_tts[nb_points];
      for (unsigned int iscan=0; iscan<scanpoints.size(); iscan++){

	TString numstr_x=ToString(round(scanpoints[iscan].x()*1000)/1000);
        TString numstr_y=ToString(round(scanpoints[iscan].y()*1000)/1000);
	//TString histoname="histo"+"x:"+numstr_x+"y: "+numstr_y;
        TString histoname_tt="histo_tt"+numstr_x+numstr_y; 
        TString histoname_tts="histo_tts"+numstr_x+numstr_y;
	//std::cout<<histoname<<endl;
	histos_tt[iscan]=new TH1D(histoname_tt,histoname_tt,50,50,110);  
        histos_tts[iscan]=new TH1D(histoname_tts,histoname_tts,50,0,50);
	//histos_raw[iscan]->Sumw2();
	histos_tt[iscan]->SetLineColor(6); //fill color
        histos_tts[iscan]->SetLineColor(4);
      }
      
      //TH1D* hq_x = new TH1D("hqall", "Charge deposit for all events (position X); Q (ADC); Counts/bin", 50, 0., 5000. );   
      
      unsigned int N=scanpoints.size();
    for(unsigned int iscan=0; iscan<N; iscan++){
    if (iscan%100==0) std::cout<<"pass 1: Filling histograms for iscan = "<<iscan<<" / "<<scanpoints.size()<<std::endl;
    ScanPoint scanpoint = scanpoints[ iscan ];
    //Loop over scanpoint
    double sum=0;
    bool fft_cut;
    // double conversion=(2.0/pow(2.0, 14.0))*(500/50); 
    for ( unsigned iev = 0; iev < scanpoint.nentries(); ++iev ){ //That's the number of pulse per point
      
      tt0->GetEvent( scanpoint.get_entry() + iev );
      tt2->GetEvent( scanpoint.get_entry() + iev );
      if ( wf0->haswf ){
         histos_tt[iscan]->Fill( wf0->mean-wf2->mean );
      }

      //      tt3->GetEvent( scanpoint.get_entry() + iev );
      //if ( wf3->haswf ){
      //  histos_->Fill( wf1->mean );
      // }
       
    }
    
  }
string outname = string("timing_distribution_sum") + argv[2] + ".root";
TFile * f1 = new TFile(outname.c_str(), "RECREATE");
 // TCanvas *c1 = new TCanvas("c1","multipads",900,700);
 // gStyle->SetOptStat(0);
 // c1->Divide(20,20,0,0);
 //  for(unsigned int i; i<=400; i++){
 //  c1->cd(i);
 //  histos[i]->Draw();
 //  }
 //scanpoints.size(
 string plotname;
 TCanvas* c = new TCanvas("canvas");
 
 // c->Divide(20,20,0,0);  
 TH2D* hgrad;
  Circle_st circ;

  bool docirclefit = true;
  // if ( argc == 4 && argv[3][0] == 'F' ) docirclefit = false;                                                                                                                                                                               
  if ( docirclefit ){
    //circ = find_circle_max_grad( hqavg, hgrad, 0.5 );//0.25                                                                                                                                                                                 
    // } //else {                                                                                                                                                                                                                             
    circ.r  = 0.25;
    circ.xc = 0.3725;
    circ.yc = 0.25;//                                                                                                                                                                                                                         
  }

 for(unsigned int iscan=0; iscan<N; iscan++){
    ScanPoint scanpoint = scanpoints[ iscan ];
    // cout<<scanpoint.x()<<"  "<<scanpoint.y()<<endl;
    if ( !circ.is_inside( scanpoint.x(), scanpoint.y() ) ) {
        std::cout<<"Skip scan point "<<iscan<<" outside circle " <<std::endl;
	 histos_tt[iscan]->Write();
	continue;
   }

     std::cout<<"Do scan point "<<iscan<<" inside circle " <<std::endl;

    TF1* ftmp = new TF1( "gaussian_fit", my_gaussian, 40., 80., 3 );
    ftmp->SetParNames("Amplitude","Mean","Sigma");
    ftmp->SetParameter(0, 20.);
    ftmp->SetParameter(1, 60.);
    ftmp->SetParameter(2, 5.);
    ftmp->SetParLimits(0, 0.0, 5000.0);
    ftmp->SetParLimits(1, 20.0, 80.0 );
    ftmp->SetParLimits(2, 0.0, 100.0 );
    histos_tt[iscan]->Fit( ftmp, "Q", "", 40., 80. );

   //c->cd(iscan+1);
  histos_tt[iscan]->SetStats(1);
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);
   histos_tt[iscan]->GetYaxis()->SetTitle("Counts");
   histos_tt[iscan]->GetXaxis()->SetTitle("time [nsec]");
   histos_tt[iscan]->Draw();
   //histos_fitted[iscan]->Draw("SAME");
   //histos_raw[iscan]->SetDirectory(f1);
   //histos_fitted[iscan]->SetDirectory(f1);
   TString numstr_x=ToString(round(scanpoints[iscan].x()*1000)/1000);
   TString numstr_y=ToString(round(scanpoints[iscan].y()*1000)/1000);
   //c->SetLogy(); 
   histos_tt[iscan]->SetDirectory(f1);
   //histos_fitted[iscan]->Write();
   // plotname = string("~/projects/def-pdeperio/vgousy/ptf/ptf-analysis/charge_distribution/charge_distribution_")+numstr_x+numstr_y+".pdf";
   // c->SaveAs(plotname.c_str(),"pdf");
 }
 //gPad->Modified();
 //gPad->Update();      

 f1->Write();
 f1->Close();
//For more file, define a canva after a certain number of distribution 
}
 


 
