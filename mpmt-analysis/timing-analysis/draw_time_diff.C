//Created by Mohit Gola
//9/08/2022
#include "TROOT.h"
#include "TFile.h"
#include "TBrowser.h"
#include "TH2.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <TTree.h>
#include <TCanvas.h>
/*
double Gaussian(double *x, double *par) {
  double arg=0;
  if(par[2]!=0) arg=(x[0]-par[1])/par[2];
  double gfunc=par[3] - par[0] * TMath::Exp( -0.5*arg*arg ) + par[4]*TMath::Sin( par[5]*x[0] + par[6] );
  return gfunc;
}
*/
//void draw_time_diff()
{

     TFile *fi = new TFile("root_run_000221_0000.root");

     TH1D *waveforms[8];
     int timestamp;
     int serialnumber;
     int freqsetting;
     int triggerTime;


     //     TH1* Check = new TH1D("check", "Check",250,0,350);
     TH1* Check = new TH1D("check", "Check",1000,-1000,1000); 
     TF1 *f1 = new TF1("f1","gaus",-1,1);

     //     TF1 *fitfunc = new TF1("mygauss","Gaussian",0,147,7);


     TTree *midas_data = (TTree*)fi->Get("midas_data1");
     midas_data->SetBranchAddress("timestamp",&timestamp);
     midas_data->SetBranchAddress("serialnumber",&serialnumber);
     midas_data->SetBranchAddress("freqsetting",&freqsetting);
     midas_data->SetBranchAddress("triggerTime",&triggerTime);

     for(int i=0; i<2; i++) midas_data->SetBranchAddress(Form("Channel%d",i),&(waveforms[i]));

     std::cout<<midas_data->GetEntries()<<std::endl;

     TH1D *lgc0 = waveforms[0];
     TH1D *lgc1 = waveforms[1];

     double min_bin_trigger = 0;
     double min_bin_pulse = 0;
	  
     for(Long_t i=0; i<((Long_t)midas_data->GetEntries())/4; i++){
       //for (int i=0; i<2;i++){

	 midas_data->GetEntry(i);
	 
	 double ped0 = 0.;
	 double ped1 = 0.;
	 double maxHeight0 = 5000.;
	 double maxHeight1 = 5000.;
	 double check = 3000.;
	 


	 for(int j=1; j<=lgc0->GetNbinsX(); j++){
	   if(j<=20) ped0 += lgc0->GetBinContent(j);

	   if(lgc0->GetBinContent(j)<check) {
	     maxHeight0 = lgc0->GetBinContent(j);
	     check = lgc0->GetBinContent(j);
	     min_bin_trigger = j;

	   }
	 }

	 double cfd_threshold_trigger = ped0/20. -  (ped0/20. - maxHeight0)/2.0;
	 double crossing_time_trigger;
	 bool found_cfd_trigger = false;
	 int ii = min_bin_trigger;
	 double x1=0,x2=0,y1=0,y2=0;
	 double m=0, b=0;

	 while(!found_cfd_trigger){
	   y2 = lgc0->GetBinContent(ii);
	   y1 = lgc0->GetBinContent(ii-1);
	   //	   std::cout<<"Left bin to minimum bin value: "<<y1<<" Minimum bin value: "<<y2<<std::endl;
	   if(y2 < cfd_threshold_trigger && y1 > cfd_threshold_trigger){
	     x1 = ii-1;
	     x2 = ii;
	      b=y1;
	      m=(y2-y1)/(x2-x1);
	      
	      crossing_time_trigger = (cfd_threshold_trigger - b)/m + x1;
	      found_cfd_trigger = true;
	   }
	   ii--;
	   if (ii < 10) found_cfd_trigger = true;
	 }
	 //	 std::cout<<"CFD Threshold: "<<cfd_threshold<<" Left bin to minimum bin: "<<x1<<" Minimum bin: "<<x2<<std::endl;	 
	 //	 std::cout<<"Found the pulse: "<<found_cfd_trigger<<" Trigger Pulse peaking at(ns): "<< ii*0.3125 << " Leading edge time: " << crossing_time_trigger*0.3125 << std::endl;
	 //	 Check->Fill(crossing_time_trigger);

	 check = 5000;

	 for(int j=1; j<=lgc1->GetNbinsX(); j++){
           if(j<=20) ped1 += lgc1->GetBinContent(j);
	   
           if(lgc1->GetBinContent(j)<check) {
             maxHeight1 = lgc1->GetBinContent(j);
	     check = lgc1->GetBinContent(j);
             min_bin_pulse = j;
             //      std::cout<<"Mininum bin value"<<min_bin_trigger<<std::endl;                                                                                                                                      
           }
         }
	 //	 std::cout<<"Pulse amplitude: "<<check-ped1/20.<<std::endl;
	 double cfd_threshold_pulse = ped1/20. -  (ped1/20. - maxHeight1)/2.0;
         double crossing_time_pulse;
         bool found_cfd_pulse = false;
         int jj = min_bin_pulse;
         double x1_1=0,x2_1=0,y1_1=0,y2_1=0;
	 double m_1=0, b_1=0;

	 if (-50 < (check-ped1/20.) && (check-ped1/20.) < -15){
	 while(!found_cfd_pulse){
	   y2_1 = lgc1->GetBinContent(jj);
	   y1_1 = lgc1->GetBinContent(jj-1);
	   //	   std::cout<<"Left bin to minimum bin value: "<<y1<<" Minimum bin value: "<<y2<<std::endl;
	   if(y2_1 < cfd_threshold_pulse && y1_1 > cfd_threshold_pulse){
	     x1_1 = jj-1;
	     x2_1 = jj;
	      b_1=y1_1;
	      m_1=(y2_1-y1_1)/(x2_1-x1_1);
	      
	      crossing_time_pulse = (cfd_threshold_pulse - b_1)/m_1 + x1_1;
	      found_cfd_pulse = true;
	   }
	   jj--;
	   if (jj < 10) found_cfd_pulse = true;
	 }

	 }//	 std::cout<<"CFD Threshold: "<<cfd_threshold<<" Left bin to minimum bin: "<<x1<<" Minimum bin: "<<x2<<std::endl;	 
	 //	 std::cout<<" Trigger time: " << crossing_time_trigger*0.3125 << " Pulse Time: "<<crossing_time_pulse*0.3125<< " Time Resolution: "<<(crossing_time_pulse*0.3125 - crossing_time_trigger*0.3125)<<std::endl;
	 if (-50 < (check-ped1/20.) && (check-ped1/20.) < -15)	 std::cout<<" Trigger time: " << crossing_time_trigger*0.3125 << " Pulse Time: "<<crossing_time_pulse*0.3125<< " Time Resolution: "<<(crossing_time_pulse*0.3125 - crossing_time_trigger*0.3125)<<std::endl; 
	 //	 if (-50 < (check-ped1/20.) && (check-ped1/20.) < -15)	 Check->Fill(crossing_time_pulse-crossing_time_trigger);
	 Check->Fill(maxHeight1-ped1/20.);

     }
     //     Check->Fit("f1");
     Check->Draw();
     
}
