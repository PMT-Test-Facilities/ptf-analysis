
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

double calculate_charge( const WaveformFitResult& wf ){
  // integral of gaussian a * exp( -(x-b)^2 / 2 c^2 )  = sqrt(2*pi) a * c
  return sqrt2pi * fabs( wf.amp * wf.sigma/2 );
}

TString ToString(double num){
  ostringstream start;
  start<<num; //Double check if position works
  TString start1=start.str();
  return start1;

}

int main( int argc, char* argv[] ) {
 
  if ( argc != 3 && argc != 4){
    std::cerr<<"Usage: calculate_charge.app ptf_analysis.root run_number"<<std::endl;
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
  TTree * tt1 = (TTree*)fin->Get("ptfanalysis0");
  if ( !tt1 ){
    std::cerr<<"Failed to read TTree called ptfanalysis0, exiting"<<std::endl;
    return 0;
  }
TH1D* hq_fit = new TH1D("hq_fit", "Charge deposit for fit events; Q (ADC); Counts/bin", 100, -500., 5000. );

  WaveformFitResult * wf = new WaveformFitResult;
  wf->SetBranchAddresses( tt1 );
  int nb_points=scanpoints.size();
  TH1D* histos_fitted[nb_points];
  TH1D* histos_raw[nb_points];
      for (unsigned int iscan=0; iscan<scanpoints.size(); iscan++){

	TString numstr_x=ToString(round(scanpoints[iscan].x()*1000)/1000);
        TString numstr_y=ToString(round(scanpoints[iscan].y()*1000)/1000);
	//TString histoname="histo"+"x:"+numstr_x+"y: "+numstr_y;
        TString histoname_fitted="histo_fitted"+numstr_x+numstr_y; 
        TString histoname_raw="histo_raw"+numstr_x+numstr_y;
	//std::cout<<histoname<<endl;
	histos_fitted[iscan]=new TH1D(histoname_fitted,histoname_fitted,50,0,5000);  
        histos_raw[iscan]=new TH1D(histoname_raw,histoname_raw,100,-500,5000);
	histos_raw[iscan]->Sumw2();
	histos_fitted[iscan]->SetLineColor(6); //fill color
        histos_raw[iscan]->SetLineColor(4);
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
      
      tt1->GetEvent( scanpoint.get_entry() + iev );
      double charge = calculate_charge( *wf );
      //fft_cut=FFTCut();
      //fft_cut=FFTCut();
      //if(do_fit==true){ How does the cut work ?

       histos_fitted[iscan]->Fill( charge );   
       hq_fit->Fill( charge );

       //}
      //if ( wf->haswf ) {
      // histos_fitted[iscan]->Fill( charge );
       
       //hqscanpt[ iscan ]->Fill( charge );
      //	hqsum->Fill( charge );
      // }//Using the fit
      //histos_fitted[iscan]->Fill( charge );
       histos_raw[iscan]->Fill(wf->qsum);
    }
    
  }
    PMTResponsePed::set_binwid( hq_fit->GetBinWidth(1) );
  //PMTResponsePed::set_pedestal( "nofftcut_pedestal_4554.root", "hqall_nofftcut" );                                                                                                 

  // Do initial fit to the overall charge histogram                                                                                                                                  
  // With everything freely floating                                                                                                                                                 
  TF1* ff = new TF1( "pmt_response", model1, 0., 5000., 6 );
  double Nfix  = hq_fit->Integral( 1, hq_fit->GetNbinsX()+1, "width" );
  double N0    = hq_fit->Integral( 1, 1 );
  double Nrest = hq_fit->Integral( 2, hq_fit->GetNbinsX()+1 );
//double mufix = Nrest / N0;                                                                                                                                                         
   double mufix = 0.1;//log( mufix + 1 ); // corrected to get estimated mu                                                                                                           
  ff->SetNpx(1000);
  ff->SetParNames("N","Q_{1}","#sigma_{1}", "#mu", "w", "#alpha" );
  ff->SetParameters( Nfix, 400.0, 152.0, mufix,5.048e-5,0.004683);
  ff->SetLineWidth(3);                                                                                                                                                              \

  ff->SetLineColor(kRed+1);
  TFitResultPtr  result = hq_fit->Fit(ff, "S", "", 0, 5000.0 );

  // TFile f1("charge_distribution_sum.root","RECREATE");
  string outname = string("charge_distribution_sum") + argv[2] + ".root";
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
	 histos_raw[iscan]->Write();
	 histos_fitted[iscan]->Write();
	continue;
   }

     std::cout<<"Skip scan point "<<iscan<<" inside circle " <<std::endl;

  double Nfix_2  = histos_raw[iscan]->Integral( 1, histos_raw[iscan]->GetNbinsX()+1, "width" );
  double N0_2    = histos_raw[iscan]->Integral( 1, 1 );
  double Nrest_2 = histos_raw[iscan]->Integral( 2, histos_raw[iscan]->GetNbinsX()+1 );
//double mufix = Nrest / N0;                                                                                                                                                        \
                                                                                                                                                                                     
   double mufix_2 = 0.2;
   TF1* ff2 = new TF1( "pmt_response2", pmtresponseped, -500, 5000., 8 );

  ff2->SetParNames("N","Q_0","sigma_0","Q_{1}","#sigma_{1}", "#mu", "w", "#alpha" );
  ff2->SetParameters( Nfix_2,0.0,45, 400.0, 250.0, mufix_2, 5.048e-5, 0.0004683  );//w=5.048e-5, alpha0.004683                                                                       

  ff2->SetNpx(1000);
  ff2->SetLineWidth(3);
  ff2->SetLineColor(kRed+2);
  //  histos_raw[iscan]->Fit(ff2, "SQL", "", -200, 5000.0 );
  //std::vector< TF1* > fcmp_2 = get_pmt_response_components_ped( ff2->GetParameters() );
  //for ( TF1* f2 : fcmp_2 ){
  //  histos_raw[iscan]->GetListOfFunctions()->Add( f2 );
  //  f2->Draw("same");
  // }

     
  PMTResponsePed::set_binwid( histos_fitted[iscan]->GetBinWidth(1) );
  TF1* ftmp = new TF1( "pmt_response", model1, 0., 5000., 6 );
  double curNfix  = hqallscanpt[iscan]->Integral(1, hqallscanpt[iscan]->GetNbinsX()+1, "width" );
  double curN0    = hqallscanpt[iscan]->Integral(1,1);
  double curNrest = hqallscanpt[iscan]->Integral(2, hqallscanpt[iscan]->GetNbinsX()+1 );
  double curmufix = curNrest / curN0;
    ftmp->FixParameter(0, curNfix );
    ftmp->SetParameter(1, ff->GetParameter(1) );
    ftmp->SetParameter(2, ff->GetParameter(2) );
    ftmp->FixParameter(3, curmufix );
    ftmp->FixParameter(4, ff->GetParameter(4) );
    ftmp->FixParameter(5, ff->GetParameter(5) );
    ftmp->SetParLimits(1, 0., 1000. );
    ftmp->SetParLimits(2, 0., 1000. );

  TFitResultPtr  result = histos_fitted[iscan]->Fit(ftmp, "SQ", "", 0, 2000.0 );

  std::vector< TF1* > fcmp = get_model1_components( ftmp->GetParameters() );
  for ( TF1* f : fcmp ){
    histos_fitted[iscan]->GetListOfFunctions()->Add( f );
    f->Draw("same");
  }

   //c->cd(iscan+1);
  histos_fitted[iscan]->SetStats(1);
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);
   histos_fitted[iscan]->GetYaxis()->SetTitle("Counts/bin");
   histos_fitted[iscan]->GetXaxis()->SetTitle("Q(ADC)");
   histos_fitted[iscan]->Draw();
    histos_raw[iscan]->Draw("SAME");
   histos_raw[iscan]->SetDirectory(f1);
   histos_fitted[iscan]->SetDirectory(f1);
   TString numstr_x=ToString(round(scanpoints[iscan].x()*1000)/1000);
   TString numstr_y=ToString(round(scanpoints[iscan].y()*1000)/1000);
   //c->SetLogy(); 
   //histos_raw[iscan]->Write();
   // histos_fitted[iscan]->Write();
   // plotname = string("~/projects/def-pdeperio/vgousy/ptf/ptf-analysis/charge_distribution/charge_distribution_")+numstr_x+numstr_y+".pdf";
   // c->SaveAs(plotname.c_str(),"pdf");
 }
 //gPad->Modified();
 //gPad->Update();      

 f1->Write();
 f1->Close();
//For more file, define a canva after a certain number of distribution 
}
 


 