
#include "PTFAnalysis.hpp"
#include "Configuration.hpp"
#include "Utilities.hpp"
#include "TVirtualFFT.h"
#include "PulseFinding.hpp"
#include "TH2D.h"

#include <iostream>
#include <ostream>
#include <fstream>
#include <math.h>

// Pulse charge calculation (integrated pulse height over bin range {bin_low,bin_high})
// Optionally arguments: bin_low and bin_high (otherwise checks entire range from 0-8192ns)
// Note that time in waveform = bin number * 8 ns
void PTFAnalysis::ChargeSum( float ped, int bin_low, int bin_high ){
    if (bin_high==0) bin_high=hwaveform->GetNbinsX();
    fitresult->qped = ped;
    float sum = 0.;
    
    // Recalculate pedestal per waveform
    if (bin_high!=0) {
        ped=0;
        int ped_range = bin_low-50;
        if (ped_range<10) ped_range=10;
        for (int i=1; i<=ped_range; i++) ped+= hwaveform->GetBinContent(i);
        ped = ped/(bin_low-50);
        fitresult->qped = ped;
    }
    
    // Pulse charge calculation
    for( int ibin = bin_low; ibin<=bin_high; ibin++ ){
        auto to_add = ped - hwaveform->GetBinContent( ibin );
        sum += to_add;
    }
    fitresult->qsum = sum;
}
void PTFAnalysis::ChargeSumPTF(double baseline){
 float sum = 0.;
 float avg = 0.;
 int  bin_t=hwaveform->FindBin(40);
  for( int ibin_t = 1; ibin_t<=bin_t; ibin_t++ ){
    avg+=hwaveform->GetBinContent( ibin_t );

  }
  avg=avg/bin_t;
  fitresult->qped = avg;
  //cout<<avg<<endl;
  for( int ibin = 1; ibin<=hwaveform->GetNbinsX(); ibin++ ){
    sum += avg - hwaveform->GetBinContent( ibin );//8135.4
  }
  

  fitresult->qsum = sum;//Approximative offset
  //cout<<sum+200<<endl;
}

 

bool PTFAnalysis::MonitorCut( float cut ){
  float ped = 0.0;
  int nbins = 20;
  for( int ibin = 1; ibin<=nbins; ibin++ ){
      ped += hwaveform->GetBinContent( ibin )/(float)nbins;
  }
  fitresult->ped = ped;
  float amp = 0.0;
  float mean = 0.0;
  for( int ibin = 1; ibin<=hwaveform->GetNbinsX(); ibin++ ){
      if( ped - hwaveform->GetBinContent( ibin ) > amp ){
          amp = ped - hwaveform->GetBinContent( ibin );
          mean = (float)ibin - 1.;
      }
  }
  fitresult->amp = amp;
  fitresult->mean = mean;
  if( amp < cut ){
    return false;
  }
  else{
    return true;
  }
}

bool PTFAnalysis::FFTCut(){
  int nbins = hwaveform->GetNbinsX();

  int n = nbins-1;
  // Compute the transform
  TVirtualFFT::SetTransform(0);
  double range = hwaveform->GetXaxis()->GetXmax() - hwaveform->GetXaxis()->GetXmin();

  hfftm = hwaveform->FFT( hfftm, "MAG" ); // Magnitude
  hfftm->Scale(1./(n+1));
  hf_test=new TH1D("fft","fft",nbins,-500e6,500e6);
  //hfftm->SetBinContent(1, 0.0);

  TH1 *hp = 0;
  hp = hwaveform->FFT(hp, "PH");
  
  //TVirtualFFT *fft = TVirtualFFT::GetCurrentTransform();
  //Use the following method to get the full output:

  Double_t *re_full = new Double_t[n+1];
  Double_t *im_full = new Double_t[n+1];
  for (int i=0; i<n+1; ++i) {
    if(i<=(int)(n/2)) {
      // double freq = hm_c->GetBinCenter(i+1);
      re_full[i] = hfftm->GetBinContent(i+1)*TMath::Cos(hp->GetBinContent(i+1));//ffilter->Eval(freq);
      im_full[i] = hfftm->GetBinContent(i+1)*TMath::Sin(hp->GetBinContent(i+1));//ffilter->Eval(freq);
    } else {
      re_full[i]=0;
      im_full[i]=0;
    }
  } 

  //Now let's make a backward transform:                                                                                                                                         
  int m=n+1;
  TVirtualFFT *fft_back = TVirtualFFT::FFT(1, &m, "C2R M K");
  fft_back->SetPointsComplex(re_full,im_full);
  fft_back->Transform();
  TH1 *hb = 0;
  //Let's look at the output
  hb = TH1::TransformHisto(fft_back,hb,"Re");
  for (int i=0; i<n+1; ++i) {
    hwaveform->SetBinContent(i+1, hb->GetBinContent(i+1));
  }
  
  delete fft_back;
  delete [] re_full;
  delete [] im_full;
  
}

bool PTFAnalysis::PulseLocationCut( int cut ){
  // Cut if min bin in first or last bins
  int nbins = hwaveform->GetNbinsX();
  int minBin = hwaveform->GetMinimumBin();
  if( (minBin >= 1 && minBin <= cut) || (minBin >= nbins-cut+1 && minBin <= nbins) ){
    return false;
  }
  else{
    return true;
  }
}
  
// Returns constant reference to a particular fit result
// it is useable until next entry in TTree is added, or another get_fitresult
// is called again
const WaveformFitResult & PTFAnalysis::get_fitresult( int scanpt, unsigned long long wavenum ){
  ScanPoint scanpoint = scanpoints[ scanpt ];
  long long entry = scanpoint.get_entry();
  entry += wavenum;
  if ( entry > ptf_tree->GetEntries() ) throw std::runtime_error("WaveformFitResult::get_fitresult beyond end of TTree");
  ptf_tree->GetEvent( entry );
  return *fitresult;
}

//Gaussian fitting function
// x[0] is x value
// par[0] = amplitude
// par[1] = mean
// par[2] = sigma
// par[3] = offset
// par[4] = sin amplitude
// par[5] = sin frequency (rad/nsec)
// par[6] = sin phase (rad)
double PTFAnalysis::pmt0_gaussian(double *x, double *par) {
  double arg=0;
  if(par[2]!=0) arg=(x[0]-par[1])/par[2];
  double gfunc=par[3] - par[0] * TMath::Exp( -0.5*arg*arg ) + par[4]*TMath::Sin( par[5]*x[0] + par[6] );
  return gfunc;
}

double PTFAnalysis::error_function(double *x, double *par){
	// x[0] is x value
	//par[0]=amplitude
	//par[1]=mean
	//par[2]=sigma^2

double arg=0;
	double gfunc=par[0]*TMath::Erf((x[0]-par[1])/par[2]);
return gfunc;
	

}
double PTFAnalysis::linear_fit(double *x, double *par){
        // x[0] is x value                                                                                                                                                                                                                                                     
        //par[0]=amplitude                                                                                                                                                                                                                                                     
        //par[1]=mean                                                                                                                                                                                                                                                                   //par[2]=sigma^2                                                                                                                                                                                                                                                        

double arg=0;
        double gfunc=par[0]*x[0]+par[1];
return gfunc;


}


//Gaussian fitting function
// x[0] is x value
// par[0] = amplitude
// par[1] = mean
// par[2] = sigma
// par[3] = offset
double PTFAnalysis::pmt1_gaussian(double *x, double *par) {
  double arg=0;
  if(par[2]!=0) arg=(x[0]-par[1])/par[2];
  double gfunc=par[3] - par[0] * TMath::Exp( -0.5*arg*arg );
  return gfunc;
}

double PTFAnalysis::funcEMG(double *x, double *p){
  //Exponential gaussian used for fitting
  // p[0]: amplitude
  // p[1]: gaussian mu
  // p[2]: gaussian sig
  // p[3]: exponential decay constant
  // p[4]: baseline
  
 double y = p[4] + (p[0]/0.3)*(p[3]/2.)*exp((p[1]+p[2]*p[2]/p[3]/2.-x[0])/(p[3]))*
    TMath::Erfc((p[1]+p[2]*p[2]/p[3] -x[0])/sqrt(2.)/p[2]);

  return y;
}

//Piece-wise linear fit to reference wave
// x[0] is x value
// par[0] = range1
// par[1] = range2
// par[2] = amplitude1
// par[3] = amplitude2
double PTFAnalysis::pmt2_piecewise(double *x, double *par) {
  if(x[0] < par[0]){
    return par[2];
  }
  else if(x[0] >= par[1]){
    return par[3];
  }
  else{
    double slope = (par[2] - par[3]) / (par[0] - par[1]);
    double intercept = par[2] - slope * par[0];
    return slope * x[0] + intercept;
  }

}

double PTFAnalysis::bessel(double *x, double *p){
  //Exponential gaussian used for fitting
  double xx = (x[0] - p[1]) * p[0];
  //
  double y;
  if(x[0] < p[1]){
    y = p[3];
  }else{
    //y = p[3] + p[2] * (TMath::Sin(xx) / (xx * xx) - TMath::Cos(xx)/xx);
    y = p[3] + p[2] / pow(xx,p[4]) *
      ((15/(xx*xx*xx) - 6/xx) * TMath::Sin(xx) / (xx) - ((15/(xx*xx) -1) *TMath::Cos(xx)/xx) );
  }
  
  return y ;
}
  

void PTFAnalysis::InitializeFitResult( int wavenum, int nwaves  ) {
  fitresult->Init();
  ScanPoint & scanpoint = scanpoints[ scanpoints.size()-1 ];
  fitresult->scanpt    = scanpoints.size()-1;
  fitresult->wavenum   = wavenum;
  fitresult->nwaves    = nwaves;
  fitresult->x         = scanpoint.x();
  fitresult->y         = scanpoint.y();
  fitresult->z         = scanpoint.z();
}
double p2_top = 0;
double p2_bottom = 0;
double p3_top = 0;
double p3_bottom = 0;

void PTFAnalysis::FitWaveform( int wavenum, int nwaves, PTF::PMT pmt) {
  // assumes hwaveform already defined and filled
  // assumes fit result structure already setup
  // Fit waveform for main PMT
  if( pmt.type == PTF::Hamamatsu_R3600_PMT ){
    //float conversion_volt=0.0001220703125;
    // check if we need to build the function to fit
    if( ffitfunc == nullptr ) ffitfunc = new TF1("mygauss",pmt0_gaussian,0,140,7);
    ffitfunc->SetParameters( 1.0, 70, 7.2, 8135, 10, 0.2, 0.0 );
    ffitfunc->SetParNames( "Amplitude", "Mean", "Sigma", "Offset",
      		 "Sine-Amp",  "Sin-Freq", "Sin-Phase" );

    ffitfunc->SetParLimits(0, 0.0, 8500);
    ffitfunc->SetParLimits(1, 50, 110 );
    ffitfunc->SetParLimits(2, 0.25, 12.0 );
    ffitfunc->SetParLimits(3, 7500, 8500 );
    ffitfunc->SetParLimits(4, 0.0, 8500);
    ffitfunc->SetParLimits(5, 0.1, 50.0);
    ffitfunc->SetParLimits(6, -TMath::Pi(), TMath::Pi() );
 
    // first fit for sine wave:
    ffitfunc->FixParameter(0,0.0);//Set it to  0
    ffitfunc->FixParameter(1,80);
    ffitfunc->FixParameter(2,3.6);
    hwaveform->Fit( ffitfunc, "Q", "", 0,40.0);

    // then fit gaussian
    ffitfunc->ReleaseParameter(0);
    ffitfunc->ReleaseParameter(1);
    ffitfunc->ReleaseParameter(2);
    ffitfunc->SetParLimits(0, 0.0,8500);
    ffitfunc->SetParLimits(1, 50, 110 );
    ffitfunc->SetParLimits(2, 0.25, 12.0 );
    ffitfunc->FixParameter(3, ffitfunc->GetParameter(3) );
    ffitfunc->FixParameter(4, ffitfunc->GetParameter(4));
    ffitfunc->FixParameter(5, ffitfunc->GetParameter(5));
    ffitfunc->FixParameter(6, ffitfunc->GetParameter(6));
     
    int fitstat= hwaveform->Fit( ffitfunc, "Q", "", 50.0, 110);//Adjust the timing window

    ffitfunc->FixParameter(0,ffitfunc->GetParameter(0));
    ffitfunc->FixParameter(1,ffitfunc->GetParameter(1));
    ffitfunc->FixParameter(2,ffitfunc->GetParameter(2));
    ffitfunc->FixParameter(3, ffitfunc->GetParameter(3) );
    ffitfunc->FixParameter(4, ffitfunc->GetParameter(4));
    ffitfunc->FixParameter(5, ffitfunc->GetParameter(5));
    ffitfunc->FixParameter(6, ffitfunc->GetParameter(6));


    /*    if (ffitfunc->GetParameter(0)<0.0 || ffitfunc->GetParameter(0)>1.1){
    //  cout<<"0  "<<ffitfunc->GetParameter(0)<<endl;}
    //if (ffitfunc->GetParameter(1)<40.0 || ffitfunc->GetParameter(0)>90.0){
    //  cout<<"1  "<<ffitfunc->GetParameter(1)<<endl;}
    // if (ffitfunc->GetParameter(2)<0.25 || ffitfunc->GetParameter(2)>10.0){
      cout<<"2  "<<ffitfunc->GetParameter(2)<<endl;}
    if (ffitfunc->GetParameter(3)<0.9 || ffitfunc->GetParameter(3)>1.1){
      cout<<"3  "<<ffitfunc->GetParameter(3)<<endl;}
   if (ffitfunc->GetParameter(4)<0.0 || ffitfunc->GetParameter(4)>1.1){
      cout<<"4  "<<ffitfunc->GetParameter(4)<<endl;}
   if (ffitfunc->GetParameter(5)<0.0 || ffitfunc->GetParameter(5)>50.0){
      cout<<"5  "<<ffitfunc->GetParameter(5)<<endl;}
 if (ffitfunc->GetParameter(6)<-TMath::Pi() || ffitfunc->GetParameter(6)>TMath::Pi()){
      cout<<"6  "<<ffitfunc->GetParameter(6)<<endl;}
    */
   
    // then fit sine and gaussian together    

     hwaveform->Fit( ffitfunc, "Q", "", 0, 140);
    //  ffitfunc->SetParameters( ffitfunc->GetParameter(0), ffitfunc->GetParameter(1), ffitfunc->GetParameter(2), ffitfunc->GetParameter(3),
    //			     ffitfunc->GetParameter(4), ffitfunc->GetParameter(5), ffitfunc->GetParameter(6) );
  
    //ffitfunc->ReleaseParameter(0);
    //ffitfunc->ReleaseParameter(1);
    //ffitfunc->ReleaseParameter(2);
    /*
    ffitfunc->ReleaseParameter(3);
    ffitfunc->ReleaseParameter(4);
    ffitfunc->ReleaseParameter(5);
     ffitfunc->ReleaseParameter(6);
     ffitfunc->SetParLimits(0, 0.0, 1.1);
    ffitfunc->SetParLimits(1, 40, 90 );
    ffitfunc->SetParLimits(2, 0.25, 10.0 );
    ffitfunc->SetParLimits(3, 0.9, 1.1 );
    ffitfunc->SetParLimits(4, 0.0, 1.1);
    ffitfunc->SetParLimits(5, 0.0, 50.0);
    ffitfunc->SetParLimits(6, -TMath::Pi(), TMath::Pi() );
    */
    
   // collect fit results
    fitresult->ped       = ffitfunc->GetParameter(3);
    fitresult->mean      = ffitfunc->GetParameter(1);
    fitresult->sigma     = ffitfunc->GetParameter(2);
    fitresult->amp       = ffitfunc->GetParameter(0);
    fitresult->sinamp    = ffitfunc->GetParameter(4);
    fitresult->sinw      = ffitfunc->GetParameter(5)*(1e3/(2*TMath::Pi()));
    fitresult->sinphi    = ffitfunc->GetParameter(6);
    fitresult->ped_err   = ffitfunc->GetParError(3);
    fitresult->mean_err  = ffitfunc->GetParError(1);
    fitresult->sigma_err = ffitfunc->GetParError(2);
    fitresult->amp_err   = ffitfunc->GetParError(0);
    fitresult->sinamp_err= ffitfunc->GetParError(4);
    fitresult->sinw_err  = ffitfunc->GetParError(5);
    fitresult->sinphi_err= ffitfunc->GetParError(6);
    fitresult->chi2      = ffitfunc->GetChisquare();
    fitresult->ndof      = 30-4;
    fitresult->prob      = TMath::Prob( ffitfunc->GetChisquare(), 30-4 );
    fitresult->fitstat   = fitstat;
  }
  // Simpler analysis for monitor PMT
  // Fit with simple gaussian
  // OR find bin furthest from pedestal
  //else if( pmt == 1 ){
  //else if( pmt.type == PTF::PTF_Monitor_PMT ){
    //if( ffitfunc == nullptr ) ffitfunc = new TF1("mygauss",pmt1_gaussian,0,140,4);
    //ffitfunc->SetParameters( 10, 70, 0.2, 8172 );
    //ffitfunc->SetParNames( "Amplitude", "Mean", "Sigma", "Offset" );

    //ffitfunc->SetParLimits(0, 0.0, 8500.0);
    //ffitfunc->SetParLimits(1, 0.0, 140.0 );
    //ffitfunc->SetParLimits(2, 0.01, 3.0 );
    //ffitfunc->SetParLimits(3, 7500.0, 9000.0 );

  //  // then fit gaussian
    //int fitstat = hwaveform->Fit( ffitfunc, "Q", "", 60.0, 100.0);

  //  // collect fit results
    //fitresult->ped       = ffitfunc->GetParameter(3);
    //fitresult->mean      = ffitfunc->GetParameter(1);
    //fitresult->sigma     = ffitfunc->GetParameter(2);
    //fitresult->amp       = ffitfunc->GetParameter(0);
    //fitresult->chi2      = ffitfunc->GetChisquare();
    //fitresult->ndof      = 30-4;
    //fitresult->prob      = TMath::Prob( ffitfunc->GetChisquare(), 30-4 );
    //fitresult->fitstat   = fitstat;
  //}
     else if( pmt.type == PTF::PTF_Monitor_PMT ){
       // cout<<"going into the loop"<<endl;
    float ped = 0.0;
    int nbins = 40;
    for( int ibin = 1; ibin<=nbins; ibin++ ){
      ped += hwaveform->GetBinContent( ibin )/(float)nbins;
    }
    fitresult->ped = ped;
    float amp = 0.0;
    float mean = 0.0;
    for( int ibin = 1; ibin<=hwaveform->GetNbinsX(); ibin++ ){
      if( ped - hwaveform->GetBinContent( ibin ) > amp ){
        amp = ped - hwaveform->GetBinContent( ibin );
        mean = hwaveform->GetXaxis()->GetBinCenter(ibin);
      }
    }
    fitresult->amp = amp;
    fitresult->mean = mean;
     
 }
 
  else if( pmt.type == PTF::Reference ){ 
    /*
    if( ffitfunc == nullptr ) ffitfunc = new TF1("error function", error_function,0,90,3);
      ffitfunc->SetParameters( 7000.0, 45, 1.0);
      ffitfunc->SetParNames( "Amplitude", "Mean", "Sigma");

      ffitfunc->SetParLimits(0, 0.0, 8500);
      ffitfunc->SetParLimits(1, 30, 70 );
      ffitfunc->SetParLimits(2, 0.1, 10.0 );
	 int fitstat=hwaveform->Fit( ffitfunc, "Jitters", "", 0, 90.0);
     fitresult->mean      = ffitfunc->GetParameter(1);
     fitresult->sigma     = ffitfunc->GetParameter(2);
     fitresult->amp       = ffitfunc->GetParameter(0);
     
     fitresult->fitstat   = fitstat;
    
     float mean = 0.0;
      for( int ibin = 1; ibin<=hwaveform->GetNbinsX(); ibin++ ){
      if( hwaveform->GetBinContent( ibin ) < 0.5 ){
        mean = hwaveform->GetXaxis()->GetBinCenter(ibin);
        break;
      }
    }
    fitresult->mean = mean;
    *///Method used to get the minimum of the distribution 
    //float ped = 0.0;
    //int nbins = 40;
    //for( int ibin = nbins; ibin<=nbins+30; ibin++ ){
    //  ped += hwaveform->GetBinContent( ibin )/(float)30.0;
    // }
    //fitresult->ped = ped;
    /*
    float amp =hwaveform->GetBinContent(1);
    float mean = 0.0;
    for( int ibin = 1; ibin<=hwaveform->GetNbinsX(); ibin++ ){
      if( hwaveform->GetBinContent( ibin ) < amp ){
        mean =hwaveform->GetXaxis()->GetBinCenter(ibin);
	amp=hwaveform->GetBinContent( ibin );
      }

    }
    // if (mean>2){
    //  cout<<mean<<endl;
    */
    float mean = 0.0;
    for( int ibin = 3; ibin<=hwaveform->GetNbinsX()-3; ibin++ ){
      if( hwaveform->GetBinContent( ibin ) - hwaveform->GetBinContent( ibin-1) > 300 ){//Invert
	 if ( hwaveform->GetBinContent( ibin+1 )- hwaveform->GetBinContent( ibin )<100  ){ //Invert
	   TF1 *f1 = new TF1("f1", linear_fit, hwaveform->GetXaxis()->GetBinCenter(ibin-2) ,hwaveform->GetXaxis()->GetBinCenter(ibin),2);
	   hwaveform->Fit("f1","Q","", hwaveform->GetXaxis()->GetBinCenter(ibin-2) ,hwaveform->GetXaxis()->GetBinCenter(ibin));
	   double m=f1->GetParameter(0);
	   double b=f1->GetParameter(1);  
	   mean=(12000.0-b)/m;//(hwaveform->GetXaxis()->GetBinCenter(ibin-2 )+hwaveform->GetXaxis()->GetBinCenter(ibin+2))/2;
	   fitresult->mean = mean;
	   fitresult->amp=f1->GetParameter(0);
	   fitresult->sigma=f1->GetParameter(1);
	   break;
	 }
	 else{
	   TF1 *f1 = new TF1("f1", linear_fit, hwaveform->GetXaxis()->GetBinCenter(ibin-1) ,hwaveform->GetXaxis()->GetBinCenter(ibin+1),2);
	   hwaveform->Fit("f1","Q","", hwaveform->GetXaxis()->GetBinCenter(ibin-1) ,hwaveform->GetXaxis()->GetBinCenter(ibin+1));
	   double m=f1->GetParameter(0);
           double b=f1->GetParameter(1);
           mean=(12000.0-b)/m;//(hwaveform->GetXaxis()->GetBinCenter(ibin-2 )+hwaveform->GetXaxis()->GetBinCenter(ibin+2))/2;   
           fitresult->mean = mean;
	   fitresult->amp=f1->GetParameter(0);
           fitresult->sigma=f1->GetParameter(1);
           break;


	 }
	   

	  }
       else{
	 mean=0.0;
	 fitresult->mean=mean;
       } 
	   //else{
	   // mean=(hwaveform->GetBinContent( ibin-1 )+ hwaveform->GetBinContent(ibin+1))/2;
	   //break;
	   // }
       }
       // fitresult->amp = amp;
    //fitresult->mean = mean;
      
    //    }
  }
   

  
  //else if( pmt == PTF::Reference ){
  //  if( ffitfunc == nullptr ) ffitfunc = new TF1("mygauss",pmt2_piecewise,0,140,4);
  //  ffitfunc->SetParameters( 30.0, 40., 1.0, 0.1 );
  //  ffitfunc->SetParNames( "range1", "range2", "amplitude1", "amplitude2" );
  //  ffitfunc->SetParLimits(0, 10., 60. );
  //  ffitfunc->SetParLimits(1, 10., 60. );
  //  ffitfunc->SetParLimits(2, 0.9, 1.1 );
  //  ffitfunc->SetParLimits(3, 0.0, 0.2 );

  //  // then fit gaussian
  //  int fitstat = hwaveform->Fit( ffitfunc, "Q", "", 0.0, 140.0);

  //  // collect fit results
  //  // Set mean to value at 0.5
  //  double slope = (ffitfunc->GetParameter(2) - ffitfunc->GetParameter(3)) / (ffitfunc->GetParameter(0) - ffitfunc->GetParameter(1));
  //  double intercept = ffitfunc->GetParameter(2) - slope * ffitfunc->GetParameter(0);
  //  if( fabs(slope) > 1e-8 )
  //    fitresult->mean      = (0.5 - intercept) / slope;
  //  fitresult->chi2      = ffitfunc->GetChisquare();
  //  fitresult->ndof      = 30-5;
  //  fitresult->prob      = TMath::Prob( ffitfunc->GetChisquare(), 30-5 );
  //  fitresult->fitstat   = fitstat;
  //}
  else if( pmt.type == PTF::mPMT_REV0_PMT ){ /// Add a new PMT type for the mPMT analysis. ensure PTF won't get into this

    // Find the mininum bin between 2040.0ns (bin 255) and  2320.0ns (bin 290)
    double min_bin = 2400;
    double min_bini = 0;
    double min_value = 1999.0;
    //    for(int i = 280; i < 320; i++){
    for(int i = 250; i < 320; i++){
      double value = hwaveform->GetBinContent(i);
      if(value < min_value){
        min_value = value;
        min_bin = hwaveform->GetBinCenter(i);
	min_bini = i;
      }
    }

    double fit_minx = min_bin - 40.0;
    double fit_maxx = min_bin + 8.0*2.5;

    int fitstat;
    double amplitude;

    // ellipitcall modified gaussian
    if(pmt.channel >= 16){
      if( ffitfunc == nullptr ) ffitfunc = new TF1("mygauss",funcEMG,fit_minx-30,fit_maxx+30,5);
      ffitfunc->SetParameters( fitresult->amp, fitresult->mean, 8.0, 1.0, fitresult->ped );
      ffitfunc->SetParNames( "Amplitude", "Mean", "Sigma", "exp decay", "Offset" );
      
      
      ffitfunc->SetParameter(1, min_bin );
      //ffitfunc->SetParameter(2, 13 );
      //ffitfunc->SetParameter(3, 1 );    
      //ffitfunc->FixParameter(2, 13 );
      //ffitfunc->FixParameter(3, 1 );
      
      
      ffitfunc->FixParameter(2, 9.6 );
      	ffitfunc->SetParameter(3, 15.8 );
      //ffitfunc->FixParameter(3, 6.0 );
      
      
      double sbaseline = 0.9908;
      //      if(pmt.channel == 1){sbaseline = 0.9961; }
      if(pmt.channel == 1){sbaseline = 1.0034; }
      if(pmt.channel == 16){sbaseline = 1.0015; }
      //      if(pmt.channel == 17){sbaseline = 0.9932; }
      if(pmt.channel == 17){sbaseline = 0.9975; }
      if(pmt.channel == 18){sbaseline = 1.004; }   
      if(pmt.channel == 19){sbaseline = 1.0025; } 
      
      amplitude = sbaseline - min_value;
      ffitfunc->SetParameter(0, amplitude*-10.0);
      if(pmt.channel >= 0){
	ffitfunc->SetParameter(0, amplitude*-0.63);
      }
      ffitfunc->FixParameter(4, sbaseline);
      ffitfunc->SetParLimits(0, -1000, 100);
      ffitfunc->SetParLimits(1, 1800.0, 2600.0 );
      //ffitfunc->SetParLimits(2, 10.56, 10.58 );
      //ffitfunc->SetParLimits(3, 0.1, 0.9 );
      //    ffitfunc->SetParLimits(4, 0.99, 1.01 );
      
      // then fit gaussian
      fitstat = hwaveform->Fit( ffitfunc, "Q", "", fit_minx, fit_maxx);


      
    }

    // Bessel fit
    if(pmt.channel < 16){

      fit_minx = min_bin - 8*6.5;
      //fit_maxx = min_bin + 8.0*3.5;
      fit_maxx = min_bin + 8.0*0.5;
      

      if( ffitfunc == nullptr ) ffitfunc = new TF1("mygauss",bessel,fit_minx-32,fit_maxx+36,5);
      ffitfunc->SetParameters( fitresult->amp, fitresult->mean, 8.0, fitresult->ped );
      //      ffitfunc->SetParNames( "Amplitude", "Mean", "Sigma", "exp decay", "Offset" );
      
      
      ffitfunc->SetParameter(1, min_bin - 28.0 );
      //ffitfunc->SetParameter(2, 13 );
      //ffitfunc->SetParameter(3, 1 );    
      //ffitfunc->FixParameter(0, 0.113 );
      //ffitfunc->FixParameter(4, 0.5 );
      //      ffitfunc->SetParameter(0, 0.113 ); // 1PE
      //ffitfunc->SetParameter(4, 0.5 ); // 1PE
      ffitfunc->FixParameter(0, 0.113 ); // 32PE
      ffitfunc->FixParameter(4, -0.3 ); // 32PE
      
      
      double sbaseline = 0.9908;
      if(pmt.channel == 1){sbaseline = 0.9961; }
      //      if(pmt.channel == 1){sbaseline = 1.0034; }
      if(pmt.channel == 16){sbaseline = 1.0015; }
      if(pmt.channel == 17){sbaseline = 0.9932; }
      if(pmt.channel == 18){sbaseline = 1.0044; }   
      if(pmt.channel == 19){sbaseline = 1.0025; } 
      
      double basebase = 0;
      int strt = min_bini - 16;
      int stp = min_bini - 6;

      for(int ii = strt; ii < stp; ii++){
	basebase += hwaveform->GetBinContent(ii);
      }
      basebase /= 10.0;
      sbaseline = basebase;

      double amplitude = sbaseline - min_value;
      //      ffitfunc->SetParameter(0, amplitude*-10.0);
            ffitfunc->SetParameter(2, -5.6* amplitude );
      //ffitfunc->SetParameter(2, -1.6* amplitude );

      ffitfunc->FixParameter(3, sbaseline);
      //      ffitfunc->SetParLimits(0, -100, 100);
      ffitfunc->SetParLimits(1, 1900.0, 2600.0 );
      //ffitfunc->SetParLimits(2, 10.56, 10.58 );
      //ffitfunc->SetParLimits(3, 0.1, 0.9 );
      //    ffitfunc->SetParLimits(4, 0.99, 1.01 );
      
      // then fit gaussian
      int fitstat = hwaveform->Fit( ffitfunc, "Q", "", fit_minx, fit_maxx);
      
      if(pmt.channel == 1 && 1) std::cout  << "FF " << ffitfunc->GetParameter(0)<< " "
				     << ffitfunc->GetParameter(1)<< " "
				     << ffitfunc->GetParameter(2)<< " "
				     << amplitude << " " 
				     << ffitfunc->GetParameter(2)/amplitude << " " 
				     << ffitfunc->GetParameter(3)<< " "
				     << ffitfunc->GetParameter(4)<< "   |||||"
				     << std::endl;

    }
    
    if(pmt.channel == 17 && 0)std::cout << "FF " << ffitfunc->GetParameter(0)<< " " 
					<< ffitfunc->GetParameter(1)<< " " 
					<< ffitfunc->GetParameter(2)<< " " 
					<< ffitfunc->GetParameter(3)<< "   |||||" 
					<< std::endl;
    
    if(pmt.channel == 16 && 0){
      p2_top += ffitfunc->GetParameter(2);
      p2_bottom += 1.0;
      p3_top += ffitfunc->GetParameter(3);
      p3_bottom += 1.0;
      
      std::cout << "Amp " << amplitude << " " << ffitfunc->GetParameter(0) 
		<< " " << ffitfunc->GetParameter(0)/amplitude
		<< " " << ffitfunc->GetParameter(2) 
		<< " " << p2_top/p2_bottom 
		  << " " << ffitfunc->GetParameter(3)
		<< " " << p3_top/p3_bottom << " "
		<< std::endl;
    }
  
  

    // collect fit results
    fitresult->ped       = ffitfunc->GetParameter(4);
    fitresult->mean      = ffitfunc->GetParameter(1);
    fitresult->sigma     = ffitfunc->GetParameter(2);
    fitresult->amp       = ffitfunc->GetParameter(0);
    fitresult->chi2      = ffitfunc->GetChisquare();
    fitresult->ndof      = ffitfunc->GetParameter(3);
    fitresult->prob      = TMath::Prob( ffitfunc->GetChisquare(), 30-4 );

    fitresult->fitstat   = fitstat;
	
    // Do CFD analysis on the fitted pulse
    double baseline = ffitfunc->GetParameter(4);
    if(pmt.channel == 0) baseline = 0.991;
    if(pmt.channel == 1) baseline = 0.9966;
    double pulse_amplitude = ffitfunc->GetParameter(0) / 3.3;   
    pulse_amplitude = min_value-baseline;

    double cfd_threshold = baseline + pulse_amplitude/2.0;
    if(0)std::cout << "amp " << pulse_amplitude << " " << baseline-min_value
		   << " " << (baseline-min_value)/0.008 << " "
		   << (baseline-min_value) / pulse_amplitude << " "
		   << ffitfunc->GetParameter(2) << " " << ffitfunc->GetParameter(3)
		   << " " << cfd_threshold 
		   << " " << std::endl;
    double crossing_time;
    // Step back from min_bin
    bool found_cfd = false;
    int ii = min_bini;
    double x1=0,x2=0,y1=0,y2=0;
    double m=0, b=0;
    while(!found_cfd){
      
      // check if this bin is < CFD threshold and previous bin > CFD threshold
      y2 = hwaveform->GetBinContent(ii);
      y1 = hwaveform->GetBinContent(ii-1);
      if(y2 < cfd_threshold && y1 > cfd_threshold){ // found cross-over

	x1 = hwaveform->GetBinCenter(ii-1);
	x2 = hwaveform->GetBinCenter(ii);

	b=y1;
	m=(y2-y1)/(x2-x1);
	  
	// y = m * (x-x1) + b
	// (y - b)/m + x1 = x
	crossing_time = (cfd_threshold - b)/m + x1;
	found_cfd = true;
      }

      ii--;
      if (ii < 200) found_cfd = true;

    }

    fitresult->sinw = crossing_time;    
    if(0)std::cout << "CFD: " << pulse_amplitude << " " << baseline << " " 
	      << cfd_threshold << " " << ii << " " 
	      << " X1/Y1: " << x1<<":"<<y1 
	      << " X2/Y2: " << x2<<":"<<y2 
	      << " crossing : " << m << " " << b << " " << crossing_time 
	      << std::endl;
    
    


  }

  

  else{
    cout << "PTFAnalysis::FitWaveForm Error: No fit function for PMT type!" << endl;
    exit( EXIT_FAILURE );
  }
}

PTFAnalysis::PTFAnalysis( TFile* outfile, Wrapper & wrapper, double errorbar, PTF::PMT & pmt, string config_file, bool savewf ){

  // Load config file
  Configuration config;
  bool terminal_output;
  bool pulse_location_cut;
  bool fft_cut;
  bool do_pulse_finding;
  bool dofit = true;

  config.Load(config_file);
  if( !config.Get("terminal_output", terminal_output) ){
    cout << "Missing terminal_output parameter from config file." << endl;
    exit( EXIT_FAILURE );
  }
  if( !config.Get("pulse_location_cut", pulse_location_cut) ){
    cout << "Missing pulse_location_cut parameter from config file." << endl;
    exit( EXIT_FAILURE );
  }
  if( !config.Get("fft_cut", fft_cut) ){
    cout << "Missing fft_cut parameter from config file." << endl;
    exit( EXIT_FAILURE );
  }
  if( !config.Get("do_pulse_finding", do_pulse_finding) ){
    cout << "Disabling pulse finding." << std::endl;
    do_pulse_finding = false;
  }
  if( !config.Get("do_pulse_fitting", dofit) ){
    std::cout <<"Disabling pulse fitting"<< std::endl;
    dofit = true;
  }else{
    if(dofit){
      std::cout <<"Enabling pulse fitting"<< std::endl;
    }else{
      std::cout <<"Disabling pulse fitting"<< std::endl;
    }
  }

  static int instance_count =0;
  int savewf_count =0;
  int savenowf_count = 0;
  ++instance_count;
  save_waveforms = savewf;
  
  // Get utilities
  Utilities utils;

  // Get digitizer settings
  Digitizer digi = wrapper.getDigitizerSettings();
  double digiCounts = pow(2.0, digi.resolution);

  // get length of waveforms
  wrapper.setCurrentEntry(0);
  int  numTimeBins= wrapper.getSampleLength();
  
  // build the waveform histogram
  std::string hname = "hwaveform" + std::to_string(instance_count);
  std::string hname_fft = "hfftm" + std::to_string(instance_count);
  outfile->cd();
  hwaveform = new TH1D( hname.c_str(), "Pulse waveform; Time (ns); Voltage (V)", numTimeBins, 0., float(numTimeBins)*1000/digi.samplingRate );
  hfftm = new TH1D( hname_fft.c_str(), "Fast Fourier Transform; Frequency; Coefficient", numTimeBins, -5.0e8, 5.0e8 );
  
  // set up the output TTree
  string ptf_tree_name = "ptfanalysis" + std::to_string(pmt.pmt);
  ptf_tree = new TTree(ptf_tree_name.c_str(), ptf_tree_name.c_str());
  fitresult = new WaveformFitResult();
  fitresult->MakeTTreeBranches( ptf_tree );
  
  // Create output directories
  // Directories for waveforms
  string wfdir_name = "PMT" + std::to_string(pmt.pmt) + "_Waveforms";
  string nowfdir_name = "PMT" + std::to_string(pmt.pmt) + "_NoWaveforms";
  if ( save_waveforms && wfdir==nullptr ) wfdir = outfile->mkdir(wfdir_name.c_str());
  if ( save_waveforms && nowfdir==nullptr ) nowfdir = outfile->mkdir(nowfdir_name.c_str());
  // Directories for FFTs
  string wfdir_fft_name = "FFT" + std::to_string(pmt.pmt) + "_Waveforms";
  string nowfdir_fft_name = "FFT" + std::to_string(pmt.pmt) + "_NoWaveforms";
  if ( save_waveforms && wfdir_fft==nullptr ) wfdir_fft = outfile->mkdir(wfdir_fft_name.c_str());
  if ( save_waveforms && nowfdir_fft==nullptr ) nowfdir_fft = outfile->mkdir(nowfdir_fft_name.c_str());
  outfile->cd();
    
  // Loop over scan points (index i)
  unsigned long long nfilled = 0;// number of TTree entries so far

  for (unsigned i = 2; i < wrapper.getNumEntries(); i++) {
    //if ( i>2000 ) continue;
    if( terminal_output ){
      cerr << "PTFAnalysis scan point " << i << " / " << wrapper.getNumEntries() << "\u001b[34;1m (" << (((double)i)/wrapper.getNumEntries()*100) << "%)\u001b[0m\033[K";
      cerr << "\r";
    }
    else{
      if ( i % 10 == 0 ){
        std::cout << "PTFAnalysis scan point " << i << " / " << wrapper.getNumEntries() << std::endl;
      }
    }
    wrapper.setCurrentEntry(i);
    
    auto location = wrapper.getDataForCurrentEntry(PTF::Gantry0);
    auto T=wrapper.getReadingTemperature();
    auto time_F=wrapper.getReadingTime();
    scanpoints.push_back( ScanPoint( location.x, location.y, location.z,time_F.time_c, T.ext_2, nfilled ) );
    
    ScanPoint& curscanpoint = scanpoints[ scanpoints.size()-1 ];
    // loop over the number of waveforms at this ScanPoint (index j)
    int numWaveforms = wrapper.getNumSamples();
    for ( int j=0; j<numWaveforms; j++) {
      //if( j>20 ) continue;
      double* pmtsample=wrapper.getPmtSample( pmt.pmt, j );
      // set the contents of the histogram
      hwaveform->Reset();
      for ( int ibin=1; ibin <= numTimeBins; ++ibin ){
        hwaveform->SetBinContent( ibin, pmtsample[ibin-1] );
	    hwaveform->SetBinError( ibin, errorbar );
      }
      // hwaveform->Scale(digi.fullScaleRange/digiCounts);
      InitializeFitResult( j, numWaveforms );
      
      // Do pulse finding (if requested)
      if(do_pulse_finding){
        find_pulses(0, hwaveform, fitresult, pmt);
      }else{
        fitresult->numPulses = 0;
      }

//       Do simple charge sum calculation
        if( pmt.pmt == 0 ) {
            ChargeSumPTF(8135.4); //original PTF function call here
        }
        if (pmt.type == PTF::PTF_Monitor_PMT ){
	  ChargeSumPTF(8135.4);
	  //2500 offset
 	}
        // Added by Yuka June 2021 for PMT pulse charge calculation
        if (pmt.type == PTF::mPMT_REV0_PMT) {
            if (pmt.pmt==1) ChargeSum(1.0034,260,271);    //2080 to 2170 ns
            if (pmt.pmt==2) ChargeSum(1.00146,272,287);   //2180 to 2300 ns
        }
        
        
        
      // For main PMT do FFT and check if there is a waveform
      // If a waveform present then fit it
      if( dofit && pulse_location_cut && pmt.pmt == 0 ) dofit = PulseLocationCut(10);
      if( dofit && fft_cut && pmt.pmt == 0 ) dofit = FFTCut();
      //if( dofit && pmt.pmt == 1 ) dofit = MonitorCut( 25. );
      dofit =true; //true;
	  //ButterwothFilter(28.0);//MHz, apply the filter  
      if( dofit ){
        FitWaveform( j, numWaveforms, pmt ); // Fit waveform and copy fit results into TTree
      }
      fitresult->haswf = utils.HasWaveform( fitresult, pmt.pmt );
      ptf_tree->Fill();
      if(0)std::cout << "Check save waveform: " << save_waveforms << " " << savewf_count
<< " " << savenowf_count << " " << curscanpoint.x() << std::endl; 
      // check if we should clone waveform histograms
      // if ( save_waveforms && savewf_count<500 && savenowf_count<500 ){
      //    if  ( fabs( curscanpoint.x() - 0.46 ) < 0.0005 && 
      //      fabs( curscanpoint.y() - 0.38 ) < 0.0005 ) {
           //   std::cout << "Success:" << std::endl;
          std::string hwfname = "hwf_" + std::to_string( nfilled );
          std::string hfftmname = "hfftm_" + std::to_string( nfilled );
          if ( fitresult->haswf && savewf_count<500 ) {
            wfdir->cd();
            TH1D* hwf = (TH1D*) hwaveform->Clone( hwfname.c_str() );
            hwf->SetName( hwfname.c_str() );
            hwf->SetTitle("HAS a pulse; Time (ns); Voltage (V)");
            hwf->SetDirectory( wfdir );
            wfdir_fft->cd();
            TH1D* hfftm_tmp = (TH1D*) hfftm->Clone( hfftmname.c_str() );
            hfftm_tmp->SetName( hfftmname.c_str() );
            hfftm_tmp->SetTitle("HAS a pulse; Frequency; Coefficient");
            hfftm_tmp->SetDirectory( wfdir_fft );
            ++savewf_count;	  
          } else if ( !fitresult->haswf && savenowf_count<500 ){
            nowfdir->cd();
            TH1D* hwf = (TH1D*) hwaveform->Clone( hwfname.c_str() );
            hwf->SetName( hwfname.c_str() );
            hwf->SetTitle("Noise pulse; Time (ns); Voltage (V)");
            hwf->SetDirectory( nowfdir );
            nowfdir_fft->cd();
            TH1D* hfftm_tmp = (TH1D*) hfftm->Clone( hfftmname.c_str() );
            hfftm_tmp->SetName( hfftmname.c_str() );
            hfftm_tmp->SetTitle("Noise pulse; Frequency; Coefficient");
            hfftm_tmp->SetDirectory( nowfdir_fft );
            ++savenowf_count;	  
          }

	    outfile->cd();


	    //}

	    // }
      ++curscanpoint;  // increment counters
      ++nfilled;
    }
  }
  //cout << endl;
  // Done.
}

const std::vector< double > PTFAnalysis::get_bins( char dim ){

  vector< double > positions;
  for(unsigned int iscan=0; iscan<get_nscanpoints(); iscan++){
    ScanPoint scanpoint = scanpoints[ iscan ];
    if( scanpoint.x() < 1e-5 ) continue; // Ignore position (0,0,0)
    if( dim == 'x' ){
      positions.push_back( scanpoint.x() );
    }
    else if( dim == 'y' ){
      positions.push_back( scanpoint.y() ); 
    }
    else if( dim == 'z' ){
      positions.push_back( scanpoint.z() );
    }
    else{
      cout << "PTFAnalysis::get_bins Error: input must be x, y or z!" << endl;
      exit( EXIT_FAILURE );
    }
  }

  sort( positions.begin(), positions.end() );
  positions.erase( unique( positions.begin(), positions.end(), comparison ), positions.end() ); // comparison function may not be necessary but was concerned about the comparison of doubles

  //std::cout << "positions contains:";
  //vector<double>::iterator it;
  //for (it=positions.begin(); it!=positions.end(); ++it)
  //  std::cout << ' ' << *it;
  //std::cout << '\n';

  if( positions.size() < 3 ){
    cout << "PTFAnalysis::get_bins Error: fewer than 3 positions a problem for automatic binning!" << endl;
    exit( EXIT_FAILURE );
  }

  vector< double > bins;
  for(unsigned int i=0; i<positions.size()-1; i++){
    bins.push_back( (positions[i]+positions[i+1])/2. );
  }
  bins.push_back( 2.*bins[ bins.size()-1 ] - bins[ bins.size()-2 ] );
  bins.insert( bins.begin(), 2.*bins[0] - bins[1] );

  //std::cout << "bins contains:";
  //vector<double>::iterator it;
  //for (it=bins.begin(); it!=bins.end(); ++it)
  //  std::cout << ' ' << *it;
  //std::cout << '\n';

  return bins;
}
 
