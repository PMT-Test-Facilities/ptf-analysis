#include "pmt_response_function.hpp"

#include "WaveformFitResult.hpp"
#include "ScanPoint.hpp"
#include "Utilities.hpp"
#include "TFile.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TMatrixDSym.h"
#include "FindCircle.hpp"

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

#include <sstream>

using namespace std;

const double sqrt2pi = std::sqrt( 2 * std::acos(-1) );

double calculate_charge( const WaveformFitResult& wf ){
  // integral of gaussian a * exp( -(x-b)^2 / 2 c^2 )  = sqrt(2*pi) a * c
  return sqrt2pi * fabs( wf.amp * wf.sigma/2 ); //Conversion of the time
}
int main( int argc, char* argv[] ) {
  if ( argc != 3 && argc != 4){
    std::cerr<<"Usage: ptf_charge_analysis.app ptf_analysis.root run_number [T/F/I]"<<std::endl;
    std::cerr<<"Where the T/F/I is for True to do/not do circle fit to find PMT, I to cut inside circle (default T)\n"<<std::endl;
    exit(0);
  }

  // Get utilities
  Utilities utils;

  // Set style
  utils.set_style();

  std::cout<<"Input file "<<argv[1]<<std::endl;
  TFile * fin = new TFile( argv[1], "read" );

  // get the scanpoints information
  std::vector< ScanPoint > scanpoints = ReadScanPoints( fin );

  //   vector< double > xbins = utils.get_bins( scanpoints, 'x' );
  //vector< double > ybins = utils.get_bins( scanpoints, 'y' );

   TTree * tt1 = (TTree*)fin->Get("ptfanalysis0");
  if ( !tt1 ){
    std::cerr<<"Failed to read TTree called ptfanalysis0, exiting"<<std::endl;
    return 0;
  }
  WaveformFitResult * wf = new WaveformFitResult;
  wf->SetBranchAddresses( tt1 );

  
TH1D* hq_raw = new TH1D("hq_raw", "Charge deposit for sum events; Q (ADC); Counts/bin", 125, -750., 5000. );
TH1D* hq_fit = new TH1D("hq_fit", "Charge deposit for fit events; Q (ADC); Counts/bin", 100, -500., 5000. );
hq_raw->Sumw2();
hq_raw-> SetLineColor(kBlue);
hq_fit->SetLineColor(kPink+10);
//hq_fit->Sumw2();
for(unsigned int iscan=0; iscan<scanpoints.size(); iscan++){

  if (iscan%100==0) std::cout<<"pass 2 filling histograms for iscan = "<<iscan<<" / "<<scanpoints.size()<<std::endl;

    ScanPoint scanpoint = scanpoints[ iscan ];
  
  //hq_fit->Sumw2();
 for ( unsigned iev = 0; iev < scanpoint.nentries(); ++iev ){
      
      tt1->GetEvent( scanpoint.get_entry() + iev );
      double charge = calculate_charge( *wf );
      // cout<<wf->qsum<<endl;
      //if ( wf->haswf ) {
      //hqscanpt[ iscan ]->Fill( charge );
	hq_fit->Fill( charge );
	hq_raw->Fill(wf->qsum);
	// } else {
	//hqped->Fill( charge );
	//hpedscanpt[ iscan ]->Fill( charge );
	// }

	//hphall->Fill( wf->amp );
	//hqall->Fill( charge );
	//hqallfine->Fill( charge );
	// hqallfinescanpt [ iscan ]->Fill(charge);

    }
 }




//Charge sum
  double Nfix_2  = hq_raw->Integral( 1, hq_raw->GetNbinsX()+1, "width" );
  double N0_2    = hq_raw->Integral( 1, 1 );
  double Nrest_2 = hq_raw->Integral( 2, hq_raw->GetNbinsX()+1 );
//double mufix = Nrest / N0;                                                                                                                                                                                                                  
   double mufix_2 = 0.2;
   TF1* ff2 = new TF1( "pmt_response2", pmtresponseped, -500, 5000., 8 );

  ff2->SetParNames("N","Q_0","sigma_0","Q_{1}","#sigma_{1}", "#mu", "w", "#alpha" );
  ff2->SetParameters( Nfix_2,0.0,45, 400.0, 250.0, mufix_2, 5.048e-5, 0.0004683  );//w=5.048e-5, alpha0.004683

  ff2->SetNpx(1000);
  ff2->SetLineWidth(3);
  ff2->SetLineColor(kRed+2);
  // ff2->FixParameter(0, Nfix_2 );
  // ff2->FixParameter(1, 0.0 );
  //ff2->FixParameter(2, 45.0 );
  //ff2->FixParameter(3, 400.0 );
  //ff2->FixParameter(4, 200.0 );
  //ff2->FixParameter(5, 0.0 );

 //hq_raw->Fit(ff2, "L", "", 2000., 5000.0 );

 //for ( unsigned ipar=0; ipar < 8; ++ipar ) ff2->ReleaseParameter(ipar);
//ff2->FixParameter( 0, Nfix_2 );
 // ff2->FixParameter( 6, ff2->GetParameter(6) ) ;
 // ff2->FixParameter( 7, ff2->GetParameter(7) );

 // TFitResultPtr  result2=hq_raw->Fit(ff2, "SL", "", -400, 2000.0 );//-300
//Monitor PMT -50
/* ff2->FixParameter(0, ff2->GetParameter(0) );
 ff2->FixParameter(1, ff2->GetParameter(1) );
 ff2->FixParameter(2, ff2->GetParameter(2));
  ff2->FixParameter(3, ff2->GetParameter(3) );
 ff2->FixParameter(4, ff2->GetParameter(4) );
 ff2->FixParameter(5, ff2->GetParameter(5) );
 ff2->FixParameter(6, ff2->GetParameter(6) );
 ff2->FixParameter(7, ff2->GetParameter(7) );
*/
// for ( unsigned ipar=0; ipar < 8; ++ipar ) ff2->ReleaseParameter(ipar);
  hq_raw->Fit(ff2, "SL", "", -200, 5000.0 );
    std::vector< TF1* > fcmp_2 = get_pmt_response_components_ped( ff2->GetParameters() );
  for ( TF1* f2 : fcmp_2 ){
    hq_raw->GetListOfFunctions()->Add( f2 );
    f2->Draw("same");
   }
  double mu_test;
  double ped=hq_raw->Integral(hq_raw->FindBin(-400),hq_raw->FindBin(100));
  double peak=hq_raw->Integral(hq_raw->FindBin(100),hq_raw->FindBin(5000));			      
  cout<<"approximation of mu"<<peak<<" "<<ped<<" "<<peak/ped<<endl;


//Fitted charge distribution 
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
  ff->SetLineWidth(3);                                                                                                                                                                                                                        
  ff->SetLineColor(kRed+1);  
  /*
  ff->FixParameter( 0, Nfix );
  ff->FixParameter( 1, 400.0 );
  ff->FixParameter( 2, 148. );
  ff->FixParameter( 3, mufix );
  hq_fit->Fit(ff, "", "", 2000., 5000.0 );

  for ( unsigned ipar=0; ipar < 6; ++ipar ) ff->ReleaseParameter(ipar);
  ff->FixParameter( 0, Nfix );
  ff->FixParameter( 4, ff->GetParameter(4) ) ;
  ff->FixParameter( 5, ff->GetParameter(5) );

  ff->SetLineWidth(3);
  ff->SetLineColor(kRed+2);

  hq_fit->Fit(ff, "", "", 0., 2000. );

  for ( unsigned ipar=0; ipar < 6; ++ipar ) ff->ReleaseParameter(ipar);
  */ 
  TFitResultPtr  result = hq_fit->Fit(ff, "S", "", 0, 5000.0 );

  std::vector< TF1* > fcmp = get_model1_components( ff->GetParameters() );
  for ( TF1* f : fcmp ){
    hq_fit->GetListOfFunctions()->Add( f );
    f->Draw("same");
  }
string outname = string("charge_distribution") + argv[2] + ".root";
TFile * fout = new TFile(outname.c_str(), "RECREATE");

hq_raw->SetDirectory(fout);
hq_fit->SetDirectory(fout); 
hq_fit->SetStats(1);
hq_raw->SetStats(1);
fout->Write();
//hq_fit->Write();
 fout->Close();
  
}