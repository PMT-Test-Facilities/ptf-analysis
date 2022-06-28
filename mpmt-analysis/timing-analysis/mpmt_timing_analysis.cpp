#include "WaveformFitResult.hpp"
#include "ScanPoint.hpp"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TF1.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TProfile.h"

#include <iostream>
#include <fstream>
#include <vector>

///////////////////// defining the Fit functions for the tts////////////////////////////

double funcEMG(double *x, double *p){
  //exponential gaussian used for Fitting
  // p[0]: amplitude
  // p[1]: gaussian mu
  // p[2]: gaussian sig
  // p[3]: exponential decay constant
  // p[4]: baseline
  
 double y = p[4] + (p[0]/0.3)*(p[3]/2.)*exp((p[1]+p[2]*p[2]/p[3]/2.-x[0])/(p[3]))*
   TMath::Erfc((p[1]+p[2]*p[2]/p[3] -x[0])/sqrt(2.)/p[2]) *
   TMath::Sin(p[5] * (x[0]-p[6]));

 return y ;
}

Double_t fAssymGauss(Double_t *x, Double_t *p)
{
	Double_t f=0,
			 y1=0,
			 y2=0,
			 stot=0,
			 n=0;

	y1=x[0]-p[1];
	y1*=y1;
	y1/=2*p[2]*p[2];

	y2=x[0]-p[1];
	y2*=y2;
	y2/=2*p[3]*p[3];

	stot=p[2]+p[3];
	n=2.;
	n/=stot;
	n*=p[0];
	if( x[0]<p[1] ){ f+=TMath::Exp( -y1 ); }
	else{ f+=TMath::Exp( -y2 ); }

	return n*f;
}

double bessel(double *x, double *p){
  //exponential gaussian used for Fitting
  double xx = (x[0] - p[1]) * p[0];

  double y;
  if(x[0] < p[1]){
    y = p[3];
  }else{
    //y = p[3] + p[2] * (TMath::Sin(xx) / (xx * xx) - tmath::cos(xx)/xx); 
    y = p[3] + p[2] / pow(xx,p[4]) *
      ((15/(xx*xx*xx) - 6/xx) * TMath::Sin(xx) / (xx) - ((15/(xx*xx) -1) *TMath::Cos(xx)/xx) ); 
  }

  return y ;
}

///////////////////////////////////////////////////////////////////////////
int main( int argc, char* argv[] ) {

  if ( argc != 2 ){
    std::cerr<<"Usage: ptf_TTree_analysis.app ptf_analysis.root\n";
    exit(0);
  }

  //TFile * fin = new TFile( argv[1], "update" );
  TFile * fin = new TFile( argv[1], "read" );

  ///////////////////////Getting the TTrees/////////////////////////////////////
  // Get the waveform Fit TTree
  std::cout << "TTree 0" << std::endl;
  TTree * tt0 = (TTree*)fin->Get("ptfanalysis0"); // mpmt signal
  WaveformFitResult * wf0 = new WaveformFitResult;
  if(tt0) wf0->SetBranchAddresses( tt0 );

  std::cout << "TTree 1" << std::endl;
  TTree * tt1 = (TTree*)fin->Get("ptfanalysis1"); // injected pulse
  WaveformFitResult * wf1 = new WaveformFitResult;
  wf1->SetBranchAddresses( tt1 );

  std::cout << "TTree 17" << std::endl;  
  TTree * tt17 = (TTree*)fin->Get("ptfanalysis17");
  if(!tt17) tt17 = (TTree*)fin->Get("ptfanalysis17");
  WaveformFitResult * wf17 = new WaveformFitResult;
  if(tt17)  wf17->SetBranchAddresses( tt17 );

  TTree * tt18 = (TTree*)fin->Get("ptfanalysis18");
  WaveformFitResult * wf18 = new WaveformFitResult;
  if(tt18)wf18->SetBranchAddresses( tt18 );
  std::cout << "TTree done" << std::endl;

  TTree * tt19 = (TTree*)fin->Get("ptfanalysis19");
  WaveformFitResult * wf19 = new WaveformFitResult;
  if(tt19) wf19->SetBranchAddresses( tt19 );
  std::cout << "TTree done" << std::endl;

  /////////////////histograms to calculate the time difference (transit time)/////////////////////
  TH1F *tdiff = new TH1F("time diff","ch 0 minus ch 1 time difference",100,-5,1);
  TH1F *tdiff0 = new TH1F("time diff0","pmt0 time relative to trigger time",200,316,326);
  TH1F *tdiff1 = new TH1F("time diff1","pmt0 time relative to trigger time",100,54,61); //42,52
  //  TH1F *tdiff1 = new TH1F("time diff1","pmt1 time relative to trigger time",200,70,80);
  TH1F *tdiff2 = new TH1F("time diff2","timediff2",800,-6,1);
  TH1F *tdiff_inj = new TH1F("time diff inj","time difference injected pulses",200,-9.3,-8.8);


  TH1F *tdiff00 = new TH1F("time diff00","pmt1 time relative to trigger time",50,325,333);
  TH1F *tdiff01 = new TH1F("time diff01","pmt1 time relative to trigger time",50,325,333);
  TH1F *tdiff02 = new TH1F("time diff02","pmt1 time relative to trigger time",50,325,333);
  TH1F *tdiff03 = new TH1F("time diff03","pmt1 time relative to trigger time",50,325,333);

  TH1F *tdiff_phase[8];
  for (int i = 0; i < 8; i++){
    char name1[100];
    sprintf(name1,"time diff phase %i",i);
    tdiff_phase[i]  = new TH1F(name1,"pmt1 time relative to trigger time - phase",100,91.5,93.5);
  }

  TH1F *tdiff_phase_inj[8];
  for (int i = 0; i < 8; i++){
    char name1[100];
    sprintf(name1,"time diff phase %i inj",i);
    tdiff_phase_inj[i]  = new TH1F(name1,"time difference injected pulses - by phase",200,-9.3,-8.8);
  }


  TH2F *tdiff_vs_ph = new TH2F("tdiff_vs_ph","time difference vs pulse height",40,0,0.000488/0.018*80,50,70, 80);
  TProfile *tdiff_vs_ph_prof = new TProfile("tdiff_vs_ph_prof", "time difference vs pulse height - profile", 20,0,0.000488/0.018);
  
  TH1F *ph[2];
  ph[0] = new TH1F("ph0","pulse heights",400,0,0.000488/0.05685219014*400);
  ph[1] = new TH1F("ph1","pulse heights",2000,0,0.000488/0.018*2000);//1 adc count == 0.00048v, 1 adc count == 0.48/0.018 mpe, 0.48 mv == 26 mpe => 1 pe == 18 mv 
  std::cout << "looping tree " << tt0->GetEntries() << " " << tt1->GetEntries() << std::endl;
  int total_hits0 = 0, success_Fits0 = 0;
  int total_hits1 = 0, success_Fits1 = 0;

  int total_pe = 0.0;
  int total_nohits = 0.0;
  int input, slices, runNo;
  std::cout << "input iteration :";
  std::cin >> input;
  std::cout << "Divide into how many slices? ";
  std::cin >> slices;
  std::cout << "Enter Run Number ";
  std::cin >> runNo;
  float pulseCount = 0.;
  float pulseCount2 = 0.;
  // loop the first scan point and print something 
  for(int i = input*tt0->GetEntries()/slices; i < (input+1)*tt0->GetEntries()/slices-1; i++){
    //for(int i = 0; i < 50000; i++){
    tt0->GetEvent(i );
    tt1->GetEvent(i );
    //if(tt16) tt16->GetEvent(i );
    if(tt17) tt17->GetEvent(i );
    if(tt18) tt18->GetEvent(i );
    if(tt19) tt19->GetEvent(i );

    // find the pulses in list of pulses
    double pulse_time[2] = {-1,-1};
    double pulse_height[2] {-1,-1};
   
    for(int j =0; j < 2; j++){
      WaveformFitResult *wf;
      if(j==0) wf = wf0;
      if(j==1) wf = wf1;
      for(int k = 0; k < wf->numPulses; k++){
        //if(wf->pulseTimes[k] > 2420 && wf->pulseTimes[k] < 2480){ // look for laser pulse
        if(wf->pulseTimes[k] > 2020 && wf->pulseTimes[k] < 2240){ // look for laser pulse
	  pulseCount += 1.0;
          pulse_time[j] = wf->pulseTimes[k];
          if(j == 0 && 0) std::cout << "found " << i << " " << wf->pulseTimes[k] << " "
                               << (wf->pulseCharges[k])/0.018 << std::endl;

          //pulse_height[j] = (baseline[j] - wf->pulseCharges[k])/0.01;
          //pulse_height[j] = (wf->pulseCharges[k])/0.018 ;
          pulse_height[j] = (wf->pulseCharges[k])/(0.05685219014);
          //pulse_height[j] = (wf->pulseCharges[k])/0.016;
          //          std::cout << "pulse charge: " << wf->pulseCharges[k] << std::endl;
        }
      }
    }
    ph[0]->Fill(pulse_height[0]);
    ph[1]->Fill(pulse_height[1]);

    if(pulse_height[1] < 0.5) total_nohits += 1.0;
    if(pulse_height[1] > 0.5 && pulse_height[1] < 1.5) total_pe += 1.0;
    if(pulse_height[0] > 0.5 && pulse_height[0] < 1.5) pulseCount2 += 1.0;
    if(pulse_height[1] > 1.5 && pulse_height[1] < 2.5) total_pe += 2.0;
    if(pulse_height[1] > 2.5 && pulse_height[1] < 3.5) total_pe += 3.0;
    if(pulse_height[1] > 3.5 && pulse_height[1] < 4.5) total_pe += 4.0;
    if(pulse_height[1] > 4.5 && pulse_height[1] < 5.5) total_pe += 5.0;
    if(pulse_height[1] > 5.5 && pulse_height[1] < 6.5) total_pe += 6.0;
    if(pulse_height[1] > 6.5 && pulse_height[1] < 7.5) total_pe += 7.0;
    if(pulse_height[1] > 7.5 && pulse_height[1] < 8.5) total_pe += 8.0;
    if(pulse_height[1] > 8.5 && pulse_height[1] < 9.5) total_pe += 9.0;
    if(pulse_height[1] > 9.5 && pulse_height[1] < 10.5) total_pe += 10.0;
   
    double time0 = wf0->mean; // Fitted time for pmt
    double time1 = wf1->mean;// Fitted time for injected pulsex
    double time16 = 0.0;//wf16->mean;
    double time17 = wf17->mean;
    double time18 = wf18->mean;
    double time19 = wf19->mean;
    double cfd_time0 = 0.0;//wf16->sinw;
    double cfd_time1 = 0.0;//wf18->sinw;
    double time_diff = 0.0;//time16 - time18;

    if(1 &&  i < 1000)std::cout << i << " times: " << time0 << " "
              << time1 << " "
              << time1 - time0 << " "
              << time1 - time0 -75.6<< " "
              << pulse_height[0] << " "
              << pulse_height[1] << " "
              << cfd_time0 << " " 
              << cfd_time1 << " " 
              << std::endl;

    // ch 0 to trigger
    //    if(pulse_height[0] < 1.5 && pulse_height[0] > 0.5){  tdiff0->Fill(time0-time16 + 1.5); }

    //    if(pulse_height[0] < 1.5 && pulse_height[0] > 1.25){  tdiff0->Fill(time0-time16); }
    if(pulse_height[0] <4.5 && pulse_height[0] > 3.5){
      double mtdiff = time0-time18;
      tdiff0->Fill(mtdiff);
      total_hits0++;
      if(mtdiff > 312 && mtdiff < 330) {success_Fits0++;}
    }
    
    double td = time1-time18;


    if(pulse_height[1] < 0.75 && pulse_height[1] > 0.5){  tdiff00->Fill(time1-time18); }
    if(pulse_height[1] < 1.0 && pulse_height[1] > 0.75){  tdiff01->Fill(time1-time18); }
    if(pulse_height[1] < 1.25 && pulse_height[1] > 1.0){  tdiff02->Fill(time1-time18); }
    if(pulse_height[1] < 1.5 && pulse_height[1] > 1.25){  tdiff03->Fill(time1-time18); }

    tdiff_vs_ph->Fill(pulse_height[1],td);
    if(td > 327.5 && td < 330){
      tdiff_vs_ph_prof->Fill(pulse_height[1],td);
    }// ch 1 to trigger

    int phase = ((int)(time18-0.5)) % 8 ;
    if(pulse_height[0] < 3.5 && pulse_height[0] > 0.5){

      double extra = 0.0;
      if(phase == 0) extra = 0.16;
      if(phase == 1) extra = 0.14;
      if(phase == 2) extra = 0.09;
      if(phase == 3) extra = 0.11;
      if(phase == 4) extra = 0.05;
      if(phase == 5) extra = 0.00;
      if(phase == 6) extra = 0.07;
      if(phase == 7) extra = 0.18;

      double mtdiff = time0-time1;
      //double mtdiff = time1-time18 - extra;

      //      if(phase == 2 || phase==3)
      //if(phase != 6)
      tdiff1->Fill(mtdiff);

      tdiff_phase[phase]->Fill(mtdiff);

      total_hits1++;
      if(mtdiff > 60 && mtdiff < 340) {success_Fits1++;}
    

    }

    // check injected pulse timing
    double tdiff_i = time1-time17;
    //std::cout << "injected: " << tdiff_i << std::endl;
    tdiff_inj->Fill(tdiff_i);
    tdiff_phase_inj[phase]->Fill(tdiff_i);
      
    // ch 0 - ch 1
    if(pulse_height[0] < 200.5 && pulse_height[1] < 200.5 &&
       pulse_height[0] > -2 && pulse_height[1] > -2       ){
      tdiff->Fill(time16-time18);
      tdiff2->Fill(cfd_time0-cfd_time1);

      if(i < 1000 && 1 && (time16-time18) > -1.25 &&
         (time16-time18) < -1.15 
         ){
        std::cout << i << " time difference: " << time16 << " - " << time18
                  << " : " << time16-time18 << std::endl;
        std::cout << "charge: " << wf0->amp << " " << wf1->amp << " "
                  << pulse_time[0] << " " << pulse_time[1] << " "
                  << pulse_height[0] << " " << pulse_height[1] << " "
                  << std::endl;
        std::cout << "ratio "
                  << wf0->amp/pulse_height[0] << " " 
                  << wf1->amp/pulse_height[1] << " " << std::endl;
        
      }
    }     
  } 

  std::cout << "successful Fit (chan 0) = " << success_Fits0 << " / " << total_hits0
            << " : " << (((double) success_Fits0) /((double)total_hits0) * 100.0) << "%" << std::endl;
  std::cout << "successful Fit (chan 1) = " << success_Fits1 << " / " << total_hits1
            << " : " << (((double) success_Fits1) /((double)total_hits1) * 100.0) << "%" << std::endl;

  std::cout << "total events " << tt0->GetEntries() << " total pe " << total_pe << std::endl;
  std::cout << "mean: " << total_pe/(double)tt0->GetEntries() << std::endl;
  std::cout << "p(0) meas: " << total_nohits/(double)tt0->GetEntries() << std::endl;
  double lambda = total_pe/(double)tt0->GetEntries();
  double calc_p0 = exp(-lambda);// https://en.wikipedia.org/wiki/poisson_distribution#poisson_approximation
  std::cout << "p(0) calc: " << calc_p0 << std::endl;
  double better_lambda = - log(total_nohits/(double)tt0->GetEntries());
  std::cout << "better lamba: " << better_lambda << std::endl;
  std::cout << "Total number of 1 PE pulses: " << pulseCount2 << std::endl;
  std::cout << "Test number: " << pulseCount << std::endl;
    
    TCanvas *c = new TCanvas("c");

  tdiff->Draw();
  //TF1 *gaus = new TF1("gaus","gaus",-5,2);

  //tdiff->Fit("gaus","r");
  tdiff->SetLineColor(2);
  //tdiff2->Draw("same");
  //tdiff->Fit("gaus","r");
  tdiff->SetXTitle("time difference (ns)"); 

  gStyle->SetOptFit(1111);


  
  c->SaveAs("tdiff.png");
  if(0){
  TCanvas *c1 = new TCanvas("c1");
  tdiff0->Draw();
  //TF1 *gaus = new TF1("gaus","gaus",321.5,324);
  TF1 *gaus1 = new TF1("gaus1","gaus",319.5,321.5);
  tdiff0->Fit("gaus1","r");
  tdiff0->SetXTitle("time difference (ns)"); 

  gStyle->SetOptFit(1111);

  double mean0 = gaus1->GetParameter(0);
  double hm0 = mean0/2.0;
  double start0 = -1, end0 = -1;
  for (int i = 1; i < tdiff0->GetNbinsX(); i++){
    double tmp = tdiff0->GetBinContent(i);
    if(tmp > hm0 && start0 < 0) start0 = tdiff0->GetBinCenter(i);
    if(tmp < hm0 && start0 > 0 && end0 < 0) end0 = tdiff0->GetBinCenter(i);
    
  }
  double fwhm0 = end0-start0;
  std::cout << "mean0 : " << mean0 << " "
            << start0 << " "
            << end0 << " "
            << fwhm0 
            << std::endl;


  
  c1->SaveAs("tdiff_injected0.png");
  }
  TCanvas *c2 = new TCanvas("c2");
  tdiff1->Draw();
  //TF1 *gaus = new TF1("gaus","gaus",321.5,324);
  
  int bin1 = tdiff1->FindFirstBinAbove(tdiff1->GetMaximum()*0.3);
  int bin2 = tdiff1->FindLastBinAbove(tdiff1->GetMaximum()*0.3);

  TF1 *gaus2 = new TF1("gaus2","gaus", tdiff1->GetBinCenter(bin1), tdiff1->GetBinCenter(bin2));//45.5,48
  TF1 *fagaus = new TF1("fagaus", fAssymGauss, tdiff1->GetBinCenter(bin1), tdiff1->GetBinCenter(bin2), 4);
  //  TF1 *fagaus = new TF1("fagaus","gaus",74,75.6);
  gaus2->SetLineWidth(0);  
  tdiff1->Fit("gaus2", "R+");
  //gStyle->SetOptFit(0);
  fagaus->SetParameter(0, gaus2->GetParameter(0));
  fagaus->SetParameter(1, gaus2->GetParameter(1));
  fagaus->SetParameter(2, gaus2->GetParameter(2));
  fagaus->SetParameter(3, gaus2->GetParameter(2));
  tdiff1->Fit("fagaus", "R+");
  tdiff1->SetXTitle("time difference (ns)"); 

  gStyle->SetOptFit(1111);

  double maxX = fagaus->GetMaximumX(tdiff1->GetBinCenter(bin1),tdiff1->GetBinCenter(bin2));
  double max = fagaus->GetMaximum(tdiff1->GetBinCenter(bin1),tdiff1->GetBinCenter(bin2));
  double mean1 = fagaus->GetParameter(0);
  double start1 = fagaus->GetX(max/2, 0, maxX);
  double end1 = fagaus->GetX(max/2, maxX, 2*maxX);
  std::cout << "Start at " << start1 << " and end at " << end1 << std::endl;
  double fwhm = end1 - start1;
  double hm1 = mean1/2.0;
  double value;
  //std::cout << "Debugging...." << std::endl;
  //std::cout << "Mean value is :" << hm1 << std::endl;
  //std::cout << "FWHM value is :" << fagaus->GetParameter(3)/2.35482 << std::endl;
  /*for (int step = 0; step <= 5.e8; step++) {
    value = fagaus->Eval(50+step*(20/(5.e8)));
    if (value>hm1-1.e-3 && value<hm1+1.e-3){
      start1 = 50+step*(20/(5.e8));
      std::cout << value << " " << start1 << std::endl;
    }
  }*/
  
  /*for (int i = 1; i < tdiff1->GetNbinsX(); i++){
    double tmp = tdiff1->GetBinContent(i);
    if(tmp > hm1 && start1 < 0) start1 = tdiff1->GetBinCenter(i);
    if(tmp < hm1 && start1 > 0 && end1 < 0) end1 = tdiff1->GetBinCenter(i);
    
    }*/
  
  int binmax = ph[0]->GetMaximumBin();
  double x = ph[0]->GetXaxis()->GetBinCenter(binmax);

  std::ofstream file1;
  file1.open("TimingResolutionRecord.txt", std::ios::app);
  file1 << "Run number : " << TString::Itoa(runNo,10) << " mean1 : " << mean1 << " " << hm1 << " " 
            << start1 << " "
            << end1 << ". FWHM is "
            << fwhm  
            << "ns   !! " << tdiff1->GetRMS() << " "
            << std::endl << "Pulse height hist peaks at " << x 
            << std::endl;
  file1.close();

  std::cout << "mean1 : " << mean1 << " " << hm1 << " " 
            << start1 << " "
            << end1 << ". FWHM is "
            << fwhm  
            << "ns   !! " << tdiff1->GetRMS() << " "
            << std::endl << "Pulse height hist peaks at " << x 
            << std::endl;

  std::ofstream file2;
  if (input==0) file2.open("TimingData_" + TString::Itoa(runNo,10) + ".txt");
  else file2.open("TimingData_" + TString::Itoa(runNo,10) + ".txt", std::ios::app);
  file2 << fwhm << std::endl;
  file2.close();

  c2->SaveAs("tdiff_injected1_run_" + TString::Itoa(runNo,10) + "_slice_" + TString::Itoa(input,10) + ".png");

  if(0){

  TCanvas *c2inj = new TCanvas("C2inj");
  tdiff_inj->Draw();
  //TF1 *gaus = new TF1("gaus","gaus",321.5,324);
  TF1 *gaus3 = new TF1("gaus3","gaus",-9.1,-9.05);
  tdiff_inj->Fit("gaus3","R");
  tdiff_inj->SetXTitle("time difference (ns)"); 

  gStyle->SetOptFit(1111);

  double mean3 = gaus3->GetParameter(0);
  double hm3 = mean3/2.0;
  double start3 = -9999, end3 = -9999;
  for (int i = 1; i < tdiff_inj->GetNbinsX(); i++){
    double tmp = tdiff_inj->GetBinContent(i);
    if(tmp > hm3 && start3 < -1000) start3 = tdiff_inj->GetBinCenter(i);
    if(tmp < hm3 && start3 > -1000 && end3 < -1000) end3 = tdiff_inj->GetBinCenter(i);
    
  }
  double fwhm3 = end3-start3;
  std::cout << "mean (inj) : " << mean3 << " " << hm3 << " " 
            << start3 << " "
            << end3 << " "
            << fwhm3
            << " " << tdiff_inj->GetRMS() << " " 
            << std::endl;
  

  c2inj->SaveAs("tdiff_injected_injected.png");

  }
  TCanvas *c3 = new TCanvas("C3");
  ph[0]->Draw();
  ph[0]->SetXTitle("Pulse height (PE)");

  //  ph[1]->Draw("SAME");
  //ph[1]->SetLineColor(2);

  /*TLegend *leg = new TLegend(0.5,0.7,0.79,0.89);
  leg->AddEntry(ph[0],"Channel 0");
  leg->AddEntry(ph[1],"Channel 1");
  leg->Draw("SAME");*/    
  
  c3->SaveAs("pulse_heights_" + TString::Itoa(runNo,10) + "_slice_" + TString::Itoa(input,10) \
+ ".png");
  
  if(0){

  TCanvas *c4 = new TCanvas("C4");

  char name[100];
  sprintf(name,"PMT1_NoWaveforms/hwf_%i;1",103);
  TH1 *ch0 = (TH1*) fin->Get(name);

  if(ch0){
    ch0->Draw();
    ch0->GetXaxis()->SetRangeUser(2000,2450);


    TF1 *fFitfunc1 = new TF1("mygauss1",bessel,2100,2300,5);
    

    
    fFitfunc1->SetParameter(0, 0.12);
    fFitfunc1->SetParameter(1, 2200 );
    fFitfunc1->SetParameter(2, -0.3 );
    fFitfunc1->FixParameter(3, 0.996);
    fFitfunc1->SetParameter(4, 1.4 );

       
    ch0->Fit( fFitfunc1, "", "");

    
    
  }
  c4->SaveAs("waveform_example.png");


  TCanvas *c5 = new TCanvas("C5","Timing Diff",1000,800);
  tdiff03->Draw("HIST");
  tdiff03->SetLineColor(4);
  tdiff03->SetXTitle("Timing difference (ns)");
  
  tdiff00->Draw("SAMEHIST");
  tdiff01->Draw("SAMEHIST");
  tdiff01->SetLineColor(2);
 
  tdiff02->Draw("SAMEHIST");
  tdiff02->SetLineColor(3);

  tdiff01->Scale(tdiff00->GetEntries()/tdiff01->GetEntries());
  tdiff02->Scale(tdiff00->GetEntries()/tdiff02->GetEntries());
  tdiff03->Scale(tdiff00->GetEntries()/tdiff03->GetEntries());

  TLegend *leg4 = new TLegend(0.6,0.6,0.89,0.89);
  leg4->AddEntry(tdiff00,"0.5PE - 0.75PE");
  leg4->AddEntry(tdiff01,"0.75PE - 1.0PE");
  leg4->AddEntry(tdiff02,"1.0PE - 1.25PE");
  leg4->AddEntry(tdiff03,"1.25PE - 1.5PE");

  leg4->Draw("SAME");
  
  c5->SaveAs("tdiff_injected0_many.png");
  
  TCanvas *c6 = new TCanvas("C6","Timing Diff vs PH",1000,800);
  tdiff_vs_ph->Draw("COLZ");
  tdiff_vs_ph->SetXTitle("Pulse Height (PE)");
  tdiff_vs_ph->SetYTitle("Time Difference (ns)");
  //tdiff_vs_ph_prof->Draw("SAME");

  
  c6->SaveAs("tdiff_vs_ph_1pe.png");


    
  TCanvas *c7 = new TCanvas("C7","Timing Diff vs PH",1000,800);
  tdiff_vs_ph_prof->Draw();
  tdiff_vs_ph_prof->SetXTitle("Pulse Height (PE)");
  tdiff_vs_ph_prof->SetYTitle("Time Difference (ns)");
  tdiff_vs_ph_prof->GetYaxis()->SetRangeUser(328.6,328.9);

  

  c7->SaveAs("tdiff_vs_ph_1pe_prof.png");

  TCanvas *c8 = new TCanvas("C7","Timing Diff vs PH",1000,800);

  TLegend *legp = new TLegend(0.6,0.6,0.89,0.89);
  for(int i = 0; i < 8; i++){

    if(i == 0){
      tdiff_phase[0]->Draw();
      tdiff_phase[0]->GetYaxis()->SetRangeUser(0,tdiff_phase[0]->GetMaximum()*1.2);
    }else{
      tdiff_phase[i]->Scale((float)tdiff_phase[0]->GetEntries()/((float)tdiff_phase[i]->GetEntries()));
      tdiff_phase[i]->Draw("SAMEHIST");
      tdiff_phase[i]->SetLineColor(i+1);
    }
    char nnn[100];
    sprintf(nnn,"Phase %i",i);
    legp->AddEntry(tdiff_phase[i],nnn);

    std::cout << i << " " << tdiff_phase[i]->GetEntries()
              << " " << tdiff_phase[i]->GetMean()
              << " " << tdiff_phase[i]->GetRMS()
              << std::endl;
    
  }
  legp->Draw("SAME");

  c8->SaveAs("tdiff_vs_phase.png");


  TCanvas *c9 = new TCanvas("C7","Timing Diff vs PH",1000,800);

  TLegend *legp2 = new TLegend(0.6,0.6,0.89,0.89);
  for(int i = 0; i < 8; i++){

    if(i == 0){
      tdiff_phase_inj[0]->Draw();
      tdiff_phase_inj[0]->GetYaxis()->SetRangeUser(0,tdiff_phase_inj[0]->GetMaximum()*1.2);
    }else{
      tdiff_phase_inj[i]->Scale((float)tdiff_phase_inj[0]->GetEntries()/((float)tdiff_phase_inj[i]->GetEntries()));
      tdiff_phase_inj[i]->Draw("SAMEHIST");
      tdiff_phase_inj[i]->SetLineColor(i+1);
    }
    char nnn[100];
    sprintf(nnn,"Phase %i",i);
    legp2->AddEntry(tdiff_phase_inj[i],nnn);

    std::cout << i << " " << tdiff_phase[i]->GetEntries()
              << " " << tdiff_phase[i]->GetMean()
              << " " << tdiff_phase[i]->GetRMS()
              << std::endl;
    
  }
  legp2->Draw("SAME");

  c9->SaveAs("tdiff_vs_phase_inj.png");
  }
  tdiff0->Write();
  fin->Close();
  return 0;
}

