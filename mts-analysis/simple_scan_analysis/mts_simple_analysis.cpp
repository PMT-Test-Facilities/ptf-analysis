// Ashley/Thomas/Angela 2D histograms
//Modified by Mohit Gola
// 2022-07-04


#include "WaveformFitResult.hpp"
#include "ScanPoint.hpp"
#include "TCanvas.h"
#include "TFile.h"
#include "TF1.h"
#include "TF2.h"
#include "TMath.h"
#include "TH1D.h"
#include "TH2F.h"
#include "THStack.h"
#include "TGraph.h"
#include "TGaxis.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TLine.h"
#include "TLegend.h"
#include "TPaveStats.h"
#include "TPad.h"
#include "TProfile.h"
#include "TH2F.h"
#include "TEllipse.h"
#include "TPaletteAxis.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <math.h> 
#include <fstream>
#include <nlohmann/json.hpp>

//using namespace std;


// //Functions for drawing PMTs and plotting;
// /*---------------------------------------------------------------------------------*/

// void drawPMT(Double_t x, Double_t y){
//   TEllipse pmt(x,y,0.03,0.03);
//   pmt.SetLineColor(1);
//   pmt.SetFillStyle(0);
//   pmt.SetLineWidth(2);
//   pmt.DrawClone();
  
//   }

// void plot2D( TCanvas *c,bool plotting_on,TH2F *hist,char *title,const char *zt,char *name){

//   if(plotting_on == true){
//     c->SetRightMargin(0.2);
//     c->SetLeftMargin(0.1);
//     c->SetBottomMargin(0.10);
//     hist->GetXaxis()->SetTitle("X (m)");
//     //hist->GetXaxis()->SetRangeUser(0.,1.);
//     //hist->GetYaxis()->SetRangeUser(0.25,0.36);
//     hist->GetYaxis()->SetTitle("Y (m)");
//     hist->GetZaxis()->SetTitle(zt);
//     hist->SetTitle(title);
//     hist->Draw("COLZ");
//     hist->SetTitleSize(50,"t");
//     c->SetRealAspectRatio();
//     c->Modified();
//     c->Update();

//     // drawPMT(0.22,0.23); //PMT0 Location
//     // drawPMT(0.19,0.14); //PMT1 Location
//     // drawPMT(0.145,0.29); //PMT2 Location
//     // drawPMT(0.11,0.17); //PMT3 Location
//     // drawPMT(0.17,0.38); //PMT4 Location
//     // drawPMT(0.24,0.32); //PMT5 Location
//     // drawPMT(0.28,0.14); //PMT7 Location
//     // drawPMT(0.34,0.34); //PMT8 Location
//     // drawPMT(0.365,0.45); //PMT9 Location
//     // drawPMT(0.415,0.27); //PMT10 Location
//     // drawPMT(0.31,0.25); //PMT11 Location
//     // drawPMT(0.265,0.41); //PMT13 Location
//     // drawPMT(0.41,0.35); //PMT15 Location
//     // drawPMT(0.06,0.35); //PMT16 Location

//     // drawPMT(0.215,0.227-0.015); //PMT0 Location
//     // drawPMT(0.193,0.144-0.015); //PMT1 Location
//     // drawPMT(0.145,0.29-0.015); //PMT2 Location
//     // drawPMT(0.111,0.191-0.015); //PMT3 Location
//     // drawPMT(0.172,0.387-0.015); //PMT4 Location
//     // drawPMT(0.24,0.32-0.015); //PMT5 Location
//     // drawPMT(0.287,0.144-0.015); //PMT7 Location
//     // drawPMT(0.332,0.344-0.015); //PMT8 Location
//     // drawPMT(0.368,0.448-0.015); //PMT9 Location
//     // drawPMT(0.415,0.273-0.015); //PMT10 Location
//     // drawPMT(0.307,0.252-0.015); //PMT11 Location 
//     // drawPMT(0.264,0.412-0.015); //PMT13 Location
//     // drawPMT(0.415,0.367-0.015); //PMT15 Location
//     // drawPMT(0.064,0.367-0.015); //PMT16 Location
//     c->SaveAs(name);}
  
// }

// void plot1D( TCanvas *c,bool plotting_on,TH1F *hist,char *title,char *name){

//   if(plotting_on == true){
//     hist->Draw();
//     hist->GetXaxis()->SetTitle("Pulse height (mV)");
//     hist->GetYaxis()->SetTitle("Number of events");
//     hist->SetTitle(title);
//     gStyle->SetOptFit(11);
//     c->SaveAs(name);}
  
// }


// int main( int argc, char* argv[] )
// {//START MAIN 

//    //Specify desired plots
//   /*---------------------------------------------------------------------------------*/
//   bool plot_mph = true;
//   bool plot_p = true;
//   bool plot_RMS = false;
//   bool plot_scan_pt = false;
//   bool plot_events = true;
//   bool plot_eff = true;
//   bool plot_pP = true;
//   bool plot_Pw = true;
//   bool plot_h = true;
 
//   //Define paramaters for scan. (m)
//   /*---------------------------------------------------------------------------------*/
//   float step_size = 0.002;
//   float xstart = 0.170;
//   float ystart = 0.320;
//   float x_scan_dist = 0.120;
//   float y_scan_dist = 0.130;
//   float dwelltime = 8000;
//   int num_ch = 19; //number of active channels
//   int f_ch = 0; //first channel
//   int run_num = 1026;
//   int dead_ch = 0;
  
//   int num_bins_x = static_cast<int>(x_scan_dist/step_size)+1 ;
//   //  int num_bins_x = 6;  
//   //  int num_bins_y = 11;
//   int num_bins_y = static_cast<int>(y_scan_dist/step_size)+1 ;

//   float x_low = xstart - step_size/2;
//   //float x_low = 0.;
//   float x_high = xstart + x_scan_dist + step_size/2;
//   //float x_high = 1.;
//   float y_low = ystart - step_size/2;;
//   //float y_low = 0.;
//   float y_high = ystart + y_scan_dist + step_size/2;
//   //float y_high = 1.;
  
//   std::cout << "There are approx" << num_bins_x * num_bins_y << "scan points" << std::endl;
//   std::cout << "We expect" << dwelltime * num_bins_x * num_bins_y << "events in total" << std::endl;

//   //Specify the coordinate you want to look at for the 1D histogram;
//   /*---------------------------------------------------------------------------------*/
//   //  float POI_y = 0.1; //scan point of interest y
//   //  float POI_x = 0.1;
//   //scan point of interest x
//   float POI_y = 0.2; //scan point of interest y
//   float POI_x = 0.2; //scan point of interest x
//   //int BOI_x = 0.5;
//   //  int BOI_y = 0.5;
//   int BOI_x = static_cast<int>((POI_x-xstart)/step_size); //Scan bin of interest in x direction
//   int BOI_y = static_cast<int>((POI_y-ystart)/step_size); //Scan bin of interest in y direction

//   //  TH1::AddDirectory(false);
//   //  TH2::AddDirectory(false);
//   //Initialize histograms and bins of combined channels
//   /*---------------------------------------------------------------------------------*/
//   TH2F *h_eff_sum = new TH2F("Total Efficiency","LED pulses/waveform",num_bins_x,x_low,x_high,num_bins_y,y_low,y_high);
//   TH2F *h_pP_sum = new TH2F(" LED pulses / Raw Pulse Count","",num_bins_x,x_low,x_high,num_bins_y,y_low,y_high);
//   TH2F *h_Pw_sum = new TH2F("Pulse / waveforms","",num_bins_x,x_low,x_high,num_bins_y,y_low,y_high) ;
//   TH2F *h_p_sum = new TH2F("pulses","",num_bins_x,x_low,x_high,num_bins_y,y_low,y_high) ;
//   TH2F *h_mph_sum = new TH2F("mean pulse height","",num_bins_x,x_low,x_high,num_bins_y,y_low,y_high) ;
//   TH2F *h_eff_bin[num_ch];
//   TH2F *h_pP_bin[num_ch];
//   TH2F *h_Pw_bin[num_ch];
//   TH2F *h_p_bin[num_ch];
//   TH2F *h_mph_bin[num_ch];
//   double onePE_p[num_ch];

//   //Define place to store mean pulse heights for all channels
//   double mph_arr[14];

  
//   //Read ROOT file;
//   if ( argc != 2 ){
//     std::cerr<<"Usage: ptf_ttree_analysis.app ptf_analysis.root\n";
//     exit(0); }
//   TFile * fin = new TFile( argv[1], "read" );
//   TTree * tt0;    
//   WaveformFitResult * wf0;
 

//  //Start the loops
//   /*---------------------------------------------------------------------------------*/
  
//   for (int ch=0;ch<num_ch;ch++)
//     {//START CHANNEL LOOP
//       int ch_name = ch + f_ch;
//       std::cout << "CHANNEL " << ch_name << std::endl;

//       // if (ch_name == 1) continue;
//       //if (ch_name == 3) continue;
//       //if (ch_name == 6) {dead_ch+=1; continue;}
//       //if (ch_name == 7) continue;
//       //if (ch_name == 9) continue;
//       //if (ch_name == 10) continue;
//       //if (ch_name == 12) {dead_ch+=1; continue;}
//       //if (ch_name == 14) {dead_ch+=1; continue;}
//       //if (ch_name == 1) continue;
//       //if (ch_name == 16) continue;
//       /*
//       if (ch_name == 0) continue;
//       if (ch_name == 1) continue;
//       if (ch_name == 2) continue;
//       if (ch_name == 4) continue;
//       if (ch_name == 4) continue;
//       if (ch_name == 5) continue;
//       if (ch_name == 7) continue;
//       if (ch_name == 8) continue;
//       if (ch_name == 9) continue;
//       if (ch_name == 10) continue;
//       if (ch_name == 11) continue;
//       if (ch_name == 13) continue;
//       if (ch_name == 14) continue;
//       */
      
  
//       // Getting the waveform fit TTree for desired channel;
//       char channel[50];
//       sprintf(channel,"ptfanalysis%i",ch_name);
//       tt0 = (TTree*)fin->Get(channel);
//       wf0 = new WaveformFitResult;
//       if(tt0) wf0->SetBranchAddresses( tt0 );
  
//       /*----------------------------------------------------------------------------------*/
//       //Initialize
//       /* --------------------------------------------------------------------------------- */
//       //2D Histograms for separate channels; 
//       TH2F *h_mph = new TH2F("mean pulse height distribution","Mean Pulse Height",num_bins_x,x_low,x_high,num_bins_y,y_low,y_high);
//       TH2F *h_p = new TH2F("Number of Pulses","Number of Pulses",num_bins_x,x_low,x_high,num_bins_y,y_low,y_high);
//       //      TH2F *h_p = new TH2F("Number of Pulses","Number of Pulses",11,0.,1.,11,0.,1.);
//       TH2F *h_RMS = new TH2F("RMS", "Standard dev. dist.",num_bins_x,x_low,x_high,num_bins_y,y_low,y_high);
//       TH2F *h_scan_pt = new TH2F("scan point","scan point",num_bins_x,x_low,x_high,num_bins_y,y_low,y_high);
//       TH2F *h_events = new TH2F("Events","Number of events",num_bins_x,x_low,x_high,num_bins_y,y_low,y_high);
//       TH2F *h_P = new TH2F("Raw Pulse Count","Number of Pulses",num_bins_x,x_low,x_high,num_bins_y,y_low,y_high);
//       TH2F *h_eff = new TH2F("Efficiency","laser pulses/waveform",num_bins_x,x_low,x_high,num_bins_y,y_low,y_high);
//       //TH2F *h_eff = new TH2F("Efficiency","laser pulses/waveform",11,0.,1.,11,0.,1.);
//       TH2F *h_pP = new TH2F("laser pulse / Raw Pulse Count","",num_bins_x,x_low,x_high,num_bins_y,y_low,y_high);
//       TH2F *h_Pw = new TH2F("Pulse / waveforms","",num_bins_x,x_low,x_high,num_bins_y,y_low,y_high) ;
 

  
//       //2D array bins for constructing the 2D histograms;
//       TH1F *h[num_bins_x][num_bins_y];
//       Double_t events_bin[num_bins_x][num_bins_y];
//       Double_t pulse_bin[num_bins_x][num_bins_y];
      
//       for (int x=0;x<num_bins_x;x++) {
// 	for (int y=0;y<num_bins_y;y++){
// 	  char name1[100];
// 	  sprintf(name1,"h bin %i %i %i",x,y,ch);
// 	  h[x][y] = new TH1F(name1,"pulse heights",200,0,200*0.48828125);
// 	  events_bin[x][y]=0;
// 	  pulse_bin[x][y]=0;
// 	}
//       }
      
//       /* --------------------------------------------------------------------------------- */


//       //Create histogram to get full pulse height distribution at each point with no timing cut
//       // TH1F *h_p_all[num_bins_x][num_bins_y];

//       // for (int x=0;x<num_bins_x;x++){
//       //  	for (int y=0;y<num_bins_y;y++)
//       //  	  h_p_all[x][y] = new TH1F("h_p_all","pulse heights all",200,0,200*0.48828125);

//       //  }

//       TH1F *h_p_sum_all = new TH1F("h_p_sum_all","pulse heights all",200,0,200*0.48828125);

      
//       // //Create position pulse time histograms
//       // TH1F *h_time[num_bins_x][num_bins_y];

//       // //Create 2D mean pulse time histogram
//       // TH2F *h_mtime = new TH2F("Mean Pulse Times","",num_bins_x,x_low,x_high,num_bins_y,y_low,y_high);
      
//       // //Initialize the timing histograms
//       // for (int y_time=0; y_time<num_bins_y; y_time++){
//       // 	for (int x_time=0; x_time<num_bins_x; x_time++)
//       // 	  h_time[x_time][y_time] = new TH1F(Form("Timing %i%i",x_time,y_time), "", 400.0,0.0,400.0);

//       // }
      
//       std::cout << "Analyzing " << tt0->GetEntries() << " waveforms" << std::endl;
//       for(int i = 0; i < tt0->GetEntries()-1 ; i++)
// 	{//START WAVEFORM LOOP

      
// 	  tt0->GetEvent(i);
// 	  //std::cout << "Number of pulses found: " << wf0->numPulses << std::endl;


// 	  for(int ypoint = 0; ypoint < num_bins_y ; ypoint++)
// 	    {//START YLOOP

// 	      float ycenter = step_size*ypoint + ystart;
// 	      //	      float ycenter = 0.38;
// 	      float y_l = ycenter - step_size/2.0;
// 	      float y_r = ycenter + step_size/2.0;
											  
// 	      for(int xpoint = 0; xpoint < num_bins_x ; xpoint++)
// 		{//START XLOOP
// 		  float xcenter = step_size*xpoint + xstart;
// 		  //		  float xcenter = 0.46;
// 		  float x_l = xcenter - step_size/2.0;
// 		  float x_r = xcenter + step_size/2.0;

// 		  if((wf0->x>x_l && wf0->x < x_r) and (wf0->y >y_l && wf0->y < y_r))
// 		    {//FILTER XY COORDS
		      
// 		      events_bin[xpoint][ypoint] += 1;
// 		      pulse_bin[xpoint][ypoint] += wf0->numPulses;

// 		      for(int k = 0; k < wf0->numPulses; k++ )
// 			{//START PULSE LOOP

		      
// 			  //Fill timing histogram for current position and current waveform
// 			  // h_time[xpoint][ypoint]->AddBinContent(wf0->pulseTimes[k]);

// 			  //Fill all pulse height histogram at this point
// 			  //h_p_all[xpoint][ypoint]->Fill(wf0->pulseCharges[k]*1000.0);

			  

// 			  h_p_sum_all->Fill(wf0->pulseCharges[k]*1000.0);
			  
	     
	    
// 			  if(wf0->pulseTimes[k] > 250 and wf0->pulseTimes[k] < 350 and wf0->pulseCharges[k]*1000.0 > 2.0 )
// 			    {//FILTER PULSE TIME

// 			      //			      std::cout << "Number of pulses found: " << wf0->pulseCharges[k]*1000 << std::endl;
// 			      h[xpoint][ypoint]->Fill(wf0->pulseCharges[k]*1000.0);
// 			      h_scan_pt->SetBinContent(xpoint+1,ypoint+1,wf0->scanpt);
			  			 
// 			    }//DONE FILTER PULSE TIME

// 			}//DONE PULSE LOOP
		  
// 		    }//DONE FILTER XY COORDS
		 
// 		}//DONE XLOOP
		    
// 	    }//DONE YLOOP


// 	}//DONE WAVEFORM LOOP

//       //Loop again through xy bins to fill mean pulse time histogram
      
//       // for (int y_time=1; y_time<num_bins_y; y_time++){
//       // 	for (int x_time=1; x_time<num_bins_x; x_time++){
	  
//       // 	  h_time[x_time][y_time]->SetEntries(1);
//       // 	  h_time[x_time][y_time]->SetStats(false);
//       // 	  h_time[x_time][y_time]->Rebin(4);
//       // 	  h_mtime -> SetBinContent(x_time,y_time, h_time[x_time][y_time]->GetMean());

//       // 	}
//       // }
      
//       // //Draw the timing histograms
//       // TCanvas *c = new TCanvas("c","",1200,500);

//       // h_time[20][10]->Draw();
//       // c->SaveAs(Form("1d_time_%i.png",ch_name));

//       // TCanvas *c1 = new TCanvas("c1","",1200,500);

//       // h_mtime->SetStats(false);
//       // h_mtime->Draw("COLZ");
//       // c1->SaveAs(Form("2d_time_%i.png",ch_name));

      

  
 	  
//       //Constructing 2D histograms from h (array of histograms)
//       /* --------------------------------------------------------------------------------- */

//       //Define hist for each channel that counts pulse height with no timing cut
//       //TH1F *h_p_sum_all = new TH1F("h_p_sum","pulse heights all",200,0,200*0.48828125);
      
//       //Want to get mean pulse height cross all bins, so sum across all positions and then get mean
//       double mph_pos = 0;
//       int counter = 0;
      
//       for(int x = 0; x < num_bins_x ; x++){
// 	for(int y = 0; y < num_bins_y ; y++){
//        	  h_p->SetBinContent(x+1,y+1,h[x][y]->GetEntries());
// 	  h_mph->SetBinContent(x+1,y+1,h[x][y]->GetMean());
// 	  h_RMS->SetBinContent(x+1,y+1,h[x][y]->GetRMS());
// 	  h_events->SetBinContent(x+1,y+1,events_bin[x][y]);
// 	  h_P->SetBinContent(x+1,y+1,pulse_bin[x][y]);
// 	  //h_p_sum_all->Add(h_p_all[x][y]);
// 	  if (h_mph->GetBinContent(x+1,y+1)>1) {
// 	    mph_pos += h_mph->GetBinContent(x+1,y+1);
// 	    counter += 1;
// 	  }
// 	}
//       }

//       //Get mean over all positions
//       // std::cout << "Mean pulse height for channel " << ch_name << ": "<< mph_pos/counter << std::endl;
//       mph_arr[ch] = mph_pos/counter;

//       h_p_bin[ch] = h_p;
//       h_p_sum->Add(h_p_bin[ch]);

//       h_mph_bin[ch] = h_mph;
//       h_mph_sum->Add(h_mph_bin[ch]);
      
//       h_eff = (TH2F*)h_p->Clone();
//       h_eff->Divide(h_events);
//       h_eff->Scale(100.0);
//       h_eff_bin[ch] = h_eff;
//       h_eff_sum->Add(h_eff_bin[ch]);
      
//       //h_eff->GetXaxis()->SetRangeUser(0.28,0.39);
//       //h_eff->GetYaxis()->SetRangeUser(0.250,0.360);
//       //h_eff->SetMaximum(40.);

//       h_Pw = (TH2F*)h_P->Clone();
//       h_Pw->Divide(h_events);
//       h_Pw->Scale(100.0);
//       h_Pw_bin[ch]=h_Pw;
//       h_Pw_sum->Add(h_Pw_bin[ch]);

//       h_pP = (TH2F*)h_p->Clone();
//       h_pP->Divide(h_P);
//       h_pP->Scale(100.0);
//       h_pP_bin[ch]=h_pP;
//       h_pP_sum->Add(h_pP_bin[ch]);

//       //For the pulse height distributions,let's find the 1pe peak by first finding the highest bin, and
//       //then taking a weighted average between it and the highest peak adjacent to it
      

//       double avg = 0;
//       h_p_sum_all->Rebin(4);
      
//       double gaus_min = 0;
//       //double gaus_min = (h_p_sum_all->GetBinCenter(h_p_sum_all->GetMaximumBin()))/1.; //min of range for fitting gaussian
//       //if (h_p_sum_all->GetBinContent(h_p_sum_all->FindBin(gaus_min))>0. && gaus_min>2.9){
//       //if (ch_name==7) std::cout<<"CHANNEL 7: "<<h_p_sum_all->GetMean()<<std::endl;
//       if (h_p_sum_all->GetBinCenter(h_p_sum_all->GetMaximumBin())>7.0 || h_p_sum_all->GetMean()>7.0){
// 	h_p_sum_all->SetAxisRange(0.,80.0);
// 	if (h_p_sum_all->GetBinCenter(h_p_sum_all->GetMaximumBin()) >7.0){
// 	   gaus_min = (h_p_sum_all->GetBinCenter(h_p_sum_all->GetMaximumBin()))/1.5;
// 	} //min of range for fitting gaussian
// 	else {
// 	  h_p_sum_all->GetXaxis()->SetRange(5,50);
// 	  gaus_min = (h_p_sum_all->GetBinCenter(h_p_sum_all->GetMaximumBin()))/1.5;
// 	  h_p_sum_all->GetXaxis()->SetRange(0,50);
// 	}
	
// 	h_p_sum_all->Fit("gaus","W","",gaus_min,80.0);
// 	onePE_p[ch_name] = h_p_sum_all->GetFunction("gaus")->GetParameter(1);
//       }
      
//       else {


     

// 	  double max_val = h_p_sum_all->GetBinCenter(h_p_sum_all->GetMaximumBin());
// 	  // if (h_p_sum_all->GetBinContent(h_p_sum_all->GetMaximumBin()-1) > h_p_sum_all->GetBinContent(h_p_sum_all->GetMaximumBin()+1)) {
// 	  //   double sec_val = h_p_sum_all->GetBinCenter(h_p_sum_all->GetMaximumBin()-1);
// 	  //   double weight = h_p_sum_all->GetBinContent(h_p_sum_all->GetMaximumBin()-1)/h_p_sum_all->GetBinContent(h_p_sum_all->GetMaximumBin());
// 	  //   avg = (max_val + weight*sec_val)/(1+weight);
// 	  //   onePE_p[ch_name] = avg;
// 	  //   //std::cout<<"max val: "<<max_val;
	  

// 	  // }

// 	  // else {
// 	    double sec_val = h_p_sum_all->GetBinCenter(h_p_sum_all->GetMaximumBin()+1);
// 	    double weight = h_p_sum_all->GetBinContent(h_p_sum_all->GetMaximumBin()+1)/h_p_sum_all->GetBinContent(h_p_sum_all->GetMaximumBin());
// 	    avg = (max_val + weight*sec_val)/(1+weight);
// 	    onePE_p[ch_name] = avg;
// 	    //std::cout<<"max val: "<<max_val;
	  
// 	    // }


	
//       }

//       //std::cout<< "ch " << ch_name <<" 1pe pulse height is: "<<avg << " mv"<<std::endl;
       
//       /* --------------------------------------------------------------------------------- */

  
//       //Plotting
//       /* --------------------------------------------------------------------------------- */

//       // TCanvas *c0 = new TCanvas("C0");
//       // char *title0 = Form("pulse height distribution channel %d (%f,%f) bin (%d,%d)",ch_name,POI_x,POI_y,BOI_x,BOI_y);
//       // char *name0 = Form("pulse_height_distribution%d.png",ch_name);
//       // plot1D(c0,plot_h,h[BOI_x][BOI_y],title0, name0);

           
	    

      
//       //Style;
//       gStyle->SetPalette(1);
//       gStyle->SetOptTitle(1); gStyle->SetOptStat(0);
//       gStyle->SetOptFit(1111); gStyle->SetStatBorderSize(0);
//       gStyle->SetStatX(.89); gStyle->SetStatY(.89);

//       // TF1 *gaussian = new TF1("gaussian","[0]*TMath::Exp(-0.5*pow(TMath::Abs((x-[1])/[2]),[3]))",0.0,50);
//       // gaussian->SetParameter(3,2.0);
//       // gaussian->SetParameter(2,0.1);
//       // gaussian->SetParLimits(2,0.01,1.0);
//       // gaussian->FixParameter(3,2.0);
  

//       //double gaus_min = (h_p_sum_all->GetBinCenter(h_p_sum_all->GetMaximumBin()))/1.5;
//       TCanvas *a = new TCanvas("a","",1200,500);
//       //TF1 * f1 = new TF1("f1","gaus"); 
//       //h_p_sum_all->Fit("gaussian");
//       //h_p_all[30][30]->Draw("hist");
//       h_p_sum_all->Fit("gaus","W","",gaus_min,80.0);
//       //h_p_sum_all->Draw("hist");
//       a->SaveAs(Form("Pulse_Height_CH%d.png",ch_name));
  
//       TCanvas *c1 = new TCanvas("C1","",1200,500);
//       char *title1 = Form("Scan points channel %d",ch_name);
//       char *name1 = Form("scan_points_%d.png",ch_name);
//       plot2D(c1,plot_scan_pt,h_scan_pt,title1,"scan point", name1);
 
//       TCanvas *c2 = new TCanvas("C2","",1200,500);
//       char *title2 = Form("Pulse counts channel %d",ch_name);
//       char *name2 = Form("2D_p_%d.png",ch_name);
//       plot2D(c2,plot_p,h_p,title2,"pulses", name2);
 
//       TCanvas *c3 = new TCanvas("C3","",1200,500);
//       char *title3 = Form("Mean Pulse height (mV) channel %d",ch_name);
//       char *name3 = Form("2D_mph_%d.png",ch_name);
//       plot2D(c3,plot_mph,h_mph,title3,"mV", name3);

//       TCanvas *c4 = new TCanvas("C4","",1200,500);
//       char *title4 = Form("RMS (mV) channel %d",ch_name);
//       char *name4 = Form("2D_RMS_%d.png",ch_name);
//       plot2D(c4,plot_RMS,h_RMS,title4,"mV", name4);

//       //h_eff->SetMaximum(10); //Do this if you want all histograms to have the same scale
//       TCanvas *c5 = new TCanvas("C5","",1200,500);
//       char *title5 = Form("Efficiency (laser pulses / waveforms) channel %d",ch_name);
//       char *name5 = Form("2D_Efficiency_%d_r%d.png",ch_name,run_num);
//       plot2D(c5,plot_eff,h_eff,title5,"%", name5);

//       TCanvas *c6 = new TCanvas("C6","",1200,500);
//       char *title6 = Form("Number of waveforms channel %d",ch_name);
//       char *name6 = Form("2D_Events_%d.png",ch_name);
//       plot2D(c6,plot_events,h_events,title6,"events", name6);

//       TCanvas *c7 = new TCanvas("C7","",1200,500);
//       char *title7 = Form("laser pulses / Pulses channel %d",ch_name);
//       char *name7 = Form("2D_pulse.Pulse_%d.png",ch_name);
//       plot2D(c7,plot_pP,h_pP,title7,"%", name7);

//       TCanvas *c8 = new TCanvas("C8","",1200,500);
//       char *title8 = Form("Pulses / waveforms channel %d",ch_name);
//       char *name8 = Form("2D_Pulse.waveform_%d.png",ch_name);
//       plot2D(c8,plot_Pw,h_Pw,title8,"%", name8);		




//       //Try counting number of bins in 2D efficiency plots that are above a given threshold. This is to understand effect of reflectors
//       int ref_eff_count = 0;
//       double tot_eff = 0; //Count the total efficiency above threshold so we can find the average
      
//       for (int y=1; y<=num_bins_y; y++){

// 	for (int x=1; x<=num_bins_x; x++){

// 	  if (h_eff->GetBinContent(x,y)>8){
// 	      ref_eff_count += 1;
// 	      tot_eff += h_eff->GetBinContent(x,y);
// 	  }


// 	}

//       }

//       std::cout<< "Channel "<<ch_name<< " has "<< ref_eff_count << " bins above threshold"<<std::endl;
//       std::cout<< "Channel "<<ch_name<< " has average efficiency"<< tot_eff/ref_eff_count <<std::endl;

      



//       //Do the following to get range of effective light exposure to determine how diffuse laser is

//       // for (int i=0; i<200;i++){

//       // std::cout<<"X-coord: "<< h_eff->GetXaxis()->GetBinLowEdge(i+1)<< ", Bin Content: "<<h_eff->GetBinContent(i+1,1)<<std::endl; 
//       // }

//       // for (int i=1; i<=num_bins_x; i++) {

//       // 	std::cout<<h_eff->GetBinContent(i,20)<<","<<std::endl;
	
//       // }

//       //std::cout<<"Bin eff: "<<h_eff->GetBinContent(10,2)<<std::endl;
      
//      }//END CHANNEL LOOP

 
  
//   /*Plotting over sum of all channels*/
//   TCanvas *c9 = new TCanvas("C9","",1200,500);
//   char *title9 = Form("Efficiency sum over %d channels",num_ch-dead_ch);
//   char *name9 = Form("2D_SUM_Efficiency_%d_r%d.png",num_ch-dead_ch,run_num);
//   plot2D(c9,true,h_eff_sum,title9,"%",name9);

//   TCanvas *c10 = new TCanvas("C10","",1200,500);
//   char *title10 = Form("LED pulse / total (sum over %d channels)",num_ch);
//   char *name10 = Form("2D_SUM_pP_%d.png",num_ch);
//   plot2D(c10,true,h_pP_sum,title10,"%",name10);

//   TCanvas *c11 = new TCanvas("C11","",1200,500);
//   char *title11 = Form("Pulses / waveforms (sum over %d channels)",num_ch);
//   char *name11 = Form("2D_SUM_Pw_%d.png",num_ch);
//   plot2D(c11,true,h_Pw_sum,title11,"%",name11);

//   TCanvas *c12 = new TCanvas("C12","",1200,500);
//   char *title12 = Form("number of pulses (sum over %d channels)",num_ch);
//   char *name12 = Form("2D_SUM_p_%d.png",num_ch);
//   plot2D(c12,true,h_p_sum,title12,"%",name12);

//   TCanvas *c13 = new TCanvas("C13","",1200,500);
//   char *title13 = Form("mean pulse height (sum over %d channels)",num_ch);
//   char *name13 = Form("2D_SUM_mph_%d.png",num_ch);
//   plot2D(c13,true,h_mph_sum,title13,"%",name13);


//   // TF1 *g = (TF1*)h_p_sum_all->GetListOfFunctions()->FindObject("gaus");
//   // double c = g->GetParameter(0);
//   //Print pulse height for 1PE peak
//   std::cout<<"1PE peaks:"<<std::endl;
//   for (int i = 0; i<19; i++){
//     //if (i==3) continue;
//     std::cout<<onePE_p[i]<<","<<std::endl;
    
//   }
  

//   //std::cout<<"Mean pulse height" << ":"<< std::endl;

//   // for (int i = 0; i<=num_ch; i++){

//   //   std::cout<< mph_arr[i] << std::endl;
//   // }
//   // return 0;

// }//DONE MAIN

//Below is the code for runs that aren't scans (no movement of gantry)

/* LEXXIE DID THIS
void plot1D( TCanvas *c,bool plotting_on,TH1F *hist,char *title,char *name){

  if(plotting_on == true){
    hist->Draw();
    hist->GetXaxis()->SetTitle("Pulse height (mV)");
    hist->GetYaxis()->SetTitle("Number of events");
    hist->SetTitle(title);
    gStyle->SetOptFit(11);
    c->SaveAs(name);}
  
} */

// Vector to hold the value of the means for a group of runs for a particular channel 
std::vector<double> meansVector;

// Vector to hold the value of the run that has 0 change in the hamamatsu value 
std::vector<double> graphVector1;

// Vector to hold the value of the run that has 0 change in the hamamatsu value 
std::vector<double> graphVector2;

// Vector to hold the value of the run that has 0 change in the hamamatsu value 
std::vector<double> graphVector3;

// Vector to hold the value of the run that has 0 change in the hamamatsu value 
std::vector<double> graphVector4;

// Vector to hold the value of the run that has 0 change in the hamamatsu value 
std::vector<double> graphVector5;

// Vector to hold the value of the run that has 25 change in the hamamatsu value 
std::vector<double> Vector;

// The array hold the values that are put in the Json file
nlohmann::json jsonArray;

double gain=0;

int rightBounds = 0;



// Function that will create a fit to find the right boundaries to use on the final fit
void fitBounds(TH1F *hist, TF1 *fitFunction, double &xLower, double &xUpper, int rightBounds) {
  //int totalBins = hist->GetNbinsX(); // The total number of bins in the histogram

  // We watch to look in a range of 40% the mean to find the tallest bin
  double meanValue = fitFunction->GetParameter(1); // Sets the variable to the value of the mean
  double meanRange = meanValue * 0.4; // Sets the variable to 40% of the mean
  double rightMean = ceil(meanValue + meanRange); // Sets the variable to the mean plus 40% of the mean
  double leftMean = floor(meanValue - meanRange); // Sets the variable to the mean minus 40% of the mean

  int rightMeanBin = hist->GetXaxis()->FindBin(rightMean); // Sets the variable the bin number where you would find rightMean
  int leftMeanBin = hist->GetXaxis()->FindBin(leftMean); // Sets the variable the bin number where you would find leftMean


  double maxHeightRange = 0; // This will hold the the tallest bin
  double maxBinRange = 0; // This will hold the position of the tallest bin
  double height = 0; // This will hold the height of a particular bin

  for (int i = 0; i <= rightMeanBin; i++) {
    height = hist->GetBinContent(i); // Sets the variable to the height of a particular bin

    if (height > maxHeightRange) { 
        maxHeightRange = height; // Sets the variable maxHeightRange to the height of a the tallest bin yet
        maxBinRange = i; // Sets the variable maxBinRange to the position of the tallest bit yet

    }
  }

  int fitRange = maxBinRange * 0.4; // Sets the range that we want to look at to 55% from wear the tallest bin is

  int lower = floor(maxBinRange - fitRange); // Sets the lower range to the position of the tallest bin minus 55% from wear the tallest bin

  xLower = hist->GetBinCenter(lower);  

  xUpper = rightBounds; // Sets the upper bound to the size of the graph
  
}

// Function that takes the run number from the name of the argument
int extractIntFromArgv(char *argv[], int arg_num) {
    std::string myString = argv[arg_num];
    size_t startPos = myString.length() - 9;
    std::string extractedString = myString.substr(startPos, 4);
    int extractedInt = std::stoi(extractedString);
    return extractedInt;
}

void setPlotStyle() {
    
    gStyle->SetPalette(6);
    gStyle->SetOptTitle(1);
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1111);
    gStyle->SetStatBorderSize(0);
    gStyle->SetStatX(.89);
    gStyle->SetStatY(.89);
}

void processWaveform(TTree *tree, WaveformFitResult *waveformResult, TH1F *hist, int run_num) {
    for (int i = 0; i < tree->GetEntries() - 1; i++) {
        tree->GetEvent(i);
        //waveformResult->numPulses;

        for (int k = 0; k < waveformResult->numPulses; k++) {
            if(run_num == 1247 || run_num == 3){
            if (waveformResult->pulseTimes[k] > 0 and waveformResult->pulseTimes[k] < 400 and waveformResult->pulseCharges[k] * 1000.0 > 20.0) {
                hist->Fill(waveformResult->pulseCharges[k] * 1000.0);
                // FILTER PULSE TIME
            }
            }
            else{
            if (waveformResult->pulseTimes[k] > 0 and waveformResult->pulseTimes[k] < 400 and waveformResult->pulseCharges[k] * 1000.0 > 12.0) {
                hist->Fill(waveformResult->pulseCharges[k] * 1000.0);
                // FILTER PULSE TIME
            }}



        }
    }
}

struct ChannelParameters {
    double efficiency;
    double mean;
    double sigma;
    double amplitude;

    // Add any other parameters you need
};

ChannelParameters processChannel(TFile *file, int ch_name, int run_num, int graph_num, TH1F **hist_all, int rightBounds) {
    std::cout << "CHANNEL " << ch_name << std::endl;

    char channel[50];
    sprintf(channel, "ptfanalysis%i", ch_name);
    TTree *tree = (TTree *)file->Get(channel);
    WaveformFitResult *wfResult = new WaveformFitResult;

    if (tree) wfResult->SetBranchAddresses(tree);

    //This sets the title of the graph, the number of bins in the histogram and the size of the x axis 
    TH1F *hist = new TH1F(Form("h_p_sum_all_CH%d_Run%d", ch_name, run_num),
                          Form("All Pulse Heights for CH %d Run %d", ch_name, run_num),
                          350, 0, rightBounds/* * 0.48828125*/); 
 
    processWaveform(tree, wfResult, hist, run_num);

    hist->Rebin(4);
    gStyle->SetOptFit(111);

    TCanvas *canvas = new TCanvas("canvas", "", 1200, 500);
    hist->Draw();

    // This is the first fit
    TF1 *fitFunction = new TF1("fit", "gaus");
    fitFunction->SetLineColor(kBlue);
    hist->Fit(fitFunction, "R+", "", 10, rightBounds);

    double xLower, xUpper;
    // This fit uses the first fit as a reference to make it more acurate
    fitBounds(hist, fitFunction, xLower, xUpper, rightBounds);
    hist->Draw();
    TF1 *fitFunction2 = new TF1("fit", "gaus");
    fitFunction2->SetLineColor(kRed);
    hist->Fit(fitFunction2, "R+", "", xLower, xUpper);

    meansVector.push_back(fitFunction2->GetParameter(1));
    
    canvas->Update();
    canvas->SaveAs(Form("Pulse_Height_Fit_Run%d_CH%d.png", run_num, ch_name));
    ChannelParameters params; // Create an instance to store the parameters

    params.efficiency = 100 * hist->GetEntries() / tree->GetEntries();

    std::cout << "The efficiency for channel " << ch_name << " is " << params.efficiency << " %" << std::endl;

    params.amplitude = fitFunction2->GetParameter(0);
    params.mean = fitFunction2->GetParameter(1);
    params.sigma = fitFunction2->GetParameter(2);

    hist_all[ch_name] = hist;

    return params; // Return the parameters
}

// MAKE THIS A HISTOPGRAM
// This graphs the means from each run for a particular channel. This is to calulate the gain. Ex: For CH 1 it will graph the mean from run 1001, 1002, 1003, 1004 and 1005
void graphMeans(const std::vector<double>& y, int ch_name, int num_ch, nlohmann::json& graphArray, double gain){

  std::cout << "CHANNEL " << ch_name << std::endl;

  std::vector<double> x = {-50, -25, 0, 25, 50}; // The values from the x axis in V.
    
  TGraph *scatterPlot = new TGraph(x.size(), &x[0], &y[0]);

  gStyle->SetPalette(6);  gStyle->SetOptTitle(1); gStyle->SetOptStat(0);  gStyle->SetOptFit(1111);  
  gStyle->SetStatBorderSize(0); gStyle->SetStatX(0);  gStyle->SetStatY(.89);

  // Create a canvas to draw the scatter plot
  TCanvas *canvas = new TCanvas("canvas", "Scatter Plot with Linear Fit", 800, 600);

  scatterPlot->SetMarkerStyle(20); // Marker style (circle)
  scatterPlot->SetMarkerSize(0.5); // Marker size

  // Draw the scatter plot
  scatterPlot->Draw("AP"); // "AP" option indicates points

  // Sets the graph titles
  scatterPlot->SetTitle(Form("Mean Pulse Height vs. Voltage Change from Hamamatsu Value for CH%d", ch_name));
  scatterPlot->GetXaxis()->SetTitle("Voltage Change from Hamamatsu Value (V)");
  scatterPlot->GetYaxis()->SetTitle("Mean Pulse Height");

  // Define a fitting function (Linear fit)
  TF1 *quadraticFitFunction = new TF1("linearFit", "pol1");

  // Set initial parameters for the linear fit if needed
  quadraticFitFunction->SetParameter(0, 1); // Slope
  quadraticFitFunction->SetParameter(1, 20); // Intercept

  // Sets the colour of the fit
  quadraticFitFunction->SetLineColor(kBlue);

  // Perform the linear fit
  scatterPlot->Fit(quadraticFitFunction);

  //This is the target mean after gain equalisation
  double targetY = 39.82;
  double b = quadraticFitFunction->GetParameter(0);
  double m = quadraticFitFunction->GetParameter(1);

  // This is the calculation of the gain
  gain = (targetY-b)/m;

  /*  For quadradic fit
  // Print the fit parameters
  double c = quadraticFitFunction->GetParameter(0);
  double b = quadraticFitFunction->GetParameter(1);
  double a = quadraticFitFunction->GetParameter(2);

  // Calculate the discriminant
  double discriminant = b * b - (4 * a * (c - targetY));

  double x1 = (-b + sqrt(discriminant)) / (2 * a);
  double x2 = (-b - sqrt(discriminant)) / (2 * a);

  if (x1 >= -50 && x1 <= 50){
    std::cout << "For y = " << targetY << ", x = " << x1 << std::endl;
    gain = x1;
  }
  else if (x2 >= -50 && x2 <= 50){
    std::cout << "For y = " << targetY << ", x = " << x2 << std::endl;
    gain = x2;
  }*/

  TLine *targetLine = new TLine(x.front(), 37, x.back(), 37);
  targetLine->SetLineColor(kRed);
  targetLine->SetLineStyle(2);  // Dashed line
  targetLine->Draw();

  //This will add the gain value to the Json file
  for (nlohmann::json& graph : graphArray) {

    if (graph["channel"] == ch_name) {
      graph["delta_hv_to_get_nominal_ph"] = gain; 
     
      std::string jsonString = graphArray.dump();

      std::cout << "The decrease/increase amount " << gain << std::endl;
    }
  }

  // Update the canvas
  canvas->Update();

  // Save the canvas as a PNG file
  canvas->SaveAs(Form("Means_Fit_CH%d.png", ch_name));

  // Keep the program running to display the scatter plot
  canvas->WaitPrimitive();
}

//This graphs the means of all the channels in a particular run. Ex. For run 1000 it will graph the mean for CH1, CH2, CH3 ...
void meanDistribution(const std::vector<double>& y, int num_ch, int run_num){

  std::vector<double> x; // The values from the x axis in V

  //This wil skip a channel if that channel tripped 
  for (int i=0; i < num_ch; i++){
    if (i!=3){
      x.push_back(i);
    }
  }

  TGraph *scatterPlot = new TGraph(x.size(), &x[0], &y[0]);

  gStyle->SetPalette(6);
  gStyle->SetOptTitle(1);
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);
  gStyle->SetStatBorderSize(0);
  gStyle->SetStatX(0);
  gStyle->SetStatY(.89);

  // Create a canvas to draw the scatter plot
  TCanvas *canvas = new TCanvas("canvas", "Scatter Plot with Linear Fit", 800, 600);

  scatterPlot->SetMarkerStyle(20); // Marker style (circle)
  scatterPlot->SetMarkerSize(0.5); // Marker size

  // Draw the scatter plot
  scatterPlot->Draw("AP"); // "AP" option indicates points

  // Set the titles of the graphs
  scatterPlot->SetTitle(Form("Mean Pulse Height vs. CH"));
  scatterPlot->GetXaxis()->SetTitle("Channel");
  scatterPlot->GetYaxis()->SetTitle("Mean Pulse Height");

  // Define a fitting function (Linear fit)
  TF1 *linearFitFunction = new TF1("linearFit", "pol0");

  // Set initial parameters for the linear fit if needed
  linearFitFunction->SetParameter(0, 0); // Slope
  linearFitFunction->SetParameter(1, 20); // Intercept

  // Sets the colour of the fit
  linearFitFunction->SetLineColor(kBlue);

  // Perform the linear fit
  scatterPlot->Fit(linearFitFunction);

  // Print the fit parameters
  double slope = linearFitFunction->GetParameter(1);
  double intercept = linearFitFunction->GetParameter(0);
  std::cout << "Linear Fit: y = " << slope << " * x + " << intercept << std::endl;

  // Update the canvas
  canvas->Update();

  // Save the canvas as a PNG file
  canvas->SaveAs(Form("Mean_distribution_Run_%d.png", run_num));

  // Keep the program running to display the scatter plot
  canvas->WaitPrimitive();
}

//Creates a json file 
void createJson(const ChannelParameters& params, int ch_name, int run_num, double gain){
    int num_ch=ch_name;
    nlohmann::json item;
    item["runNumber"] = Form("Run_%d", run_num);
    item["channel"] = ch_name;
    item["mean"] = params.mean;
    item["sigma"] = params.sigma;
    item["efficency"] = params.efficiency;
    item["const"] = params.amplitude;

    jsonArray.push_back(item);

    // Convert the JSON object to a string
    std::string jsonString = jsonArray.dump();
    
    graphMeans(meansVector, ch_name, num_ch, jsonArray, gain); // Graph the means from each run for the channel

    // Display the JSON data
    //std::cout << jsonString << std::endl;

    std::ofstream jsonFile(Form("data_Run_%d.json", run_num));

// Save the JSON data to a file
    //std::ofstream jsonFile("data.json");
    jsonFile << jsonArray;
    jsonFile.close();
  
}

int main( int argc, char* argv[], double gain , int rightBounds) {//START MAIN 
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //Uncomment this section for an anlysis of a group of runs
 /*

  //Read ROOT file;
  if ( argc != 6 ){
    std::cerr<<"Usage: ptf_ttree_analysis.app ptf_analysis.root\n"; //If there are not 6 arguments given this error will be printed and the program will stop
    exit(0); }
  
  int num_ch = 19; //number of active channels
  int f_ch = 0; //first channel

  int run_num  = extractIntFromArgv(argv, 1); //Uses argument 1 to find the run number 
  int run_num2 = extractIntFromArgv(argv, 2); //Uses argument 2 to find the run number 
  int run_num3 = extractIntFromArgv(argv, 3); //Uses argument 3 to find the run number 
  int run_num4 = extractIntFromArgv(argv, 4); //Uses argument 4 to find the run number 
  int run_num5 = extractIntFromArgv(argv, 5); //Uses argument 5 to find the run number 

  //Initialize histograms and bins of combined channels
  //double onePE_p[num_ch];

  //Define place to store mean pulse heights for all channels
 // double mph_arr[14];
  TH1F *hist_all[19];
  THStack * hs = new THStack("hs"," stacked");

  TFile * fin  = new TFile( argv[1], "read" );
  TFile * fin2 = new TFile( argv[2], "read" );
  TFile * fin3 = new TFile( argv[3], "read" );
  TFile * fin4 = new TFile( argv[4], "read" );
  TFile * fin5 = new TFile( argv[5], "read" );

  for (int ch=0;ch<num_ch;ch++) {//START CHANNEL LOOP

    int ch_name = ch + f_ch;

    ChannelParameters graph1 = processChannel(fin , ch_name, run_num, 1, hist_all, rightBounds); //This will create the graph the first run
    ChannelParameters graph2 = processChannel(fin2, ch_name, run_num2, 2, hist_all, rightBounds); //This will create the graph the second run
    ChannelParameters graph3 = processChannel(fin3, ch_name, run_num3, 3, hist_all, rightBounds); //This will create the graph the third run
    ChannelParameters graph4 = processChannel(fin4, ch_name, run_num4, 4, hist_all, rightBounds); //This will create the graph the fourth run
    ChannelParameters graph5 = processChannel(fin5, ch_name, run_num5, 5, hist_all, rightBounds); //This will create the graph the fifth run
    
    //This will skip a channel in case that channel tripped
    if (ch!=17){
      graphVector1.push_back(meansVector[0]); // This adds the means of the fifth graph to a vector so it can be graphed in the function meanDistribution
      graphVector2.push_back(meansVector[1]); // This adds the means of the fifth graph to a vector so it can be graphed in the function meanDistribution
      graphVector3.push_back(meansVector[2]); // This adds the means of the fifth graph to a vector so it can be graphed in the function meanDistribution
      graphVector4.push_back(meansVector[3]); // This adds the means of the fifth graph to a vector so it can be graphed in the function meanDistribution
      graphVector5.push_back(meansVector[4]); // This adds the means of the fifth graph to a vector so it can be graphed in the function meanDistribution
    }

    createJson(graph3, ch_name, run_num3, gain);

    meansVector.clear(); // This will clear the vector so we can use it for the next channel

    }//END CHANNEL LOOP 

  TCanvas *c = new TCanvas("c","",1200,500);

  TLegend *leg = new TLegend(0.6,0.5,0.7,0.8);
  
  for (int i=0; i<18; i++){

    hist_all[i]->SetLineColor(20+i);
    hs->Add(hist_all[i]);
    //hist_all[i]->Draw("hist same");
    leg->AddEntry(hist_all[i], Form("Ch. %i",i));

  }

  hs->Draw("nostack");

  leg->Draw();

  c->SaveAs("Pulse_Height_All_Channels.png");

  meanDistribution(graphVector1, num_ch, run_num); // Graph the means from each run for the channel
  meanDistribution(graphVector2, num_ch, run_num2); // Graph the means from each run for the channel
  meanDistribution(graphVector3, num_ch, run_num3); // Graph the means from each run for the channel
  meanDistribution(graphVector4, num_ch, run_num4); // Graph the means from each run for the channel
  meanDistribution(graphVector5, num_ch, run_num5); // Graph the means from each run for the channel
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  

*/
  /////////////////////////////////////////////////////////////////////////////////////////////////// For just one graph
  //Uncomment this section to analyse one run
  std::cout << "RIGHT BOUND !!!!!!!!!!!!!!!!!!1 " << rightBounds << std::endl;
  rightBounds = 100;
  //Read ROOT file;
  if ( argc != 2 ){
    std::cerr<<"Usage: ptf_ttree_analysis.app ptf_analysis.root\n"; //If there are not 6 arguments given this error will be printed and the program will stop
    exit(0); }
  
  int num_ch = 19; //number of active channels
  int f_ch = 0; //first channel

  int run_num  = extractIntFromArgv(argv, 1); //Uses argument 1 to find the run number 

  TH1F *hist_all[19];
  THStack * hs = new THStack("hs"," stacked");

  TFile * fin  = new TFile( argv[1], "read" );

for (int ch=0;ch<num_ch;ch++) {//START CHANNEL LOOP
    int ch_name = ch + f_ch;

    ChannelParameters graph1 = processChannel(fin , ch_name, run_num, 1, hist_all, rightBounds); //This will create the graph the first run
    createJson(graph1, ch_name, run_num, gain);

    if (ch!=17){
      std::cout << "Length of the vector: " << ch << std::endl;

      graphVector1.push_back(meansVector[0]); // This adds the means of the fifth graph to a vector so it can be graphed in the function meanDistribution
    }
    //graphMeans(meansVector, ch_name, num_ch, jsonArray); // Graph the means from each run for the channel


    meansVector.clear(); // This will clear the vector so we can use it for the next channel


    }//END CHANNEL LOOP 

  TCanvas *c = new TCanvas("c","",1200,500);

  TLegend *leg = new TLegend(0.6,0.5,0.7,0.8);
  
  for (int i=0; i<18; i++){

    hist_all[i]->SetLineColor(20+i);
    hs->Add(hist_all[i]);
    //hist_all[i]->Draw("hist same");
    leg->AddEntry(hist_all[i], Form("Ch. %i",i));

  }

  hs->Draw("nostack");
  leg->Draw();
  c->SaveAs("Pulse_Height_All_Channels.png");


  meanDistribution(graphVector1, num_ch, run_num); // Graph the means from each run for the channel


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  /*---------------------------------------------------------------------------------*/
  /* IF EVERYTHING BREAKS BRING THIS BACK!!!!!!!!!!!!!!!!!!!!!!!!!!!1


int main( int argc, char* argv[] )
{//START MAIN 

   //Specify desired plots
  ---------------------------------------------------------------------------------
  //ADD BACK
  bool plot_mph = true;+++++++++++++++
  bool plot_p = true;
  bool plot_RMS = false;
  bool plot_scan_pt = false;
  bool plot_events = true;
  bool plot_eff = true;
  bool plot_pP = true;
  bool plot_Pw = true;
  bool plot_h = true;*/
 
  //Define paramaters
  /*---------------------------------------------------------------------------------
  std::string myString = argv[1];
  int p = myString.length();
  size_t startPos = myString.length() - 9;
  std::string extractedString = myString.substr(startPos, 4);
  int extractedInt = std::stoi(extractedString);


  int num_ch = 8; //number of active channels
  int f_ch = 0; //first channel
  int run_num = extractedInt;
  int dead_ch = 0;
  

  //Initialize histograms and bins of combined channels
  ---------------------------------------------------------------------------------
  double onePE_p[num_ch];

  //Define place to store mean pulse heights for all channels
  double mph_arr[14];

  
  //Read ROOT file;
  if ( argc != 3 ){
    std::cerr<<"Usage: ptf_ttree_analysis.app ptf_analysis.root\n";
    exit(0); }
  TFile * fin = new TFile( argv[1], "read" );
  TTree * tt0;    
  WaveformFitResult * wf0;

  for (int ch=0;ch<num_ch;ch++)
    {//START CHANNEL LOOP
      std::cout << ch;


      int ch_name = ch + f_ch;
      std::cout << "CHANNEL " << ch_name << std::endl;
      
  
      // Getting the waveform fit TTree for desired channel;
      char channel[50];
  
      sprintf(channel,"ptfanalysis%i",ch_name);
   

      tt0 = (TTree*)fin->Get(channel);
      wf0 = new WaveformFitResult;
      if(tt0) wf0->SetBranchAddresses( tt0 );
  
      tt02 = (TTree*)fin->Get(channel);
      wf02 = new WaveformFitResult;
      if(tt02) wf02->SetBranchAddresses( tt02 );*//*


      //TH1F *h_p_sum_all = new TH1F("h_p_sum_all","pulse heights all CH ",200,0,200*0.48828125);
      //TH1F *h_p_sum_all = new TH1F(Form("h_p_sum_all_CH%d", ch_name), Form("pulse heights all CH %d", ch_name), 200, 0, 200 * 0.48828125);
      TH1F *h_p_sum_all = new TH1F(Form("h_p_sum_all_CH%d_Run%d", ch_name, run_num), Form("All Pulse Heights for CH %d Run %d", ch_name, run_num), 200, 0, 200 * 0.48828125);
      //TH1F *h_p_sum_all2 = new TH1F(Form("h_p_sum_all_CH%d_Run%d2", ch_name, run_num), Form("All Pulse Heights for CH %d Run %d2", ch_name, run_num), 200, 0, 200 * 0.48828125);


     
      
      std::cout << "Analyzing " << tt0->GetEntries() << " waveforms" << std::endl;
      
      for(int i = 0; i < tt0->GetEntries()-1 ; i++)
	{//START WAVEFORM LOOP

      
	  tt0->GetEvent(i);
	  wf0->numPulses; 
    //std::cout << "Number of pulses found: " << wf0->numPulses << std::endl;


	  for(int k = 0; k < wf0->numPulses; k++ )
	    {//START PULSE LOOP

		      
	      //Fill timing histogram for current position and current waveform
	      // h_time[xpoint][ypoint]->AddBinContent(wf0->pulseTimes[k]);

	      //Fill all pulse height histogram at this point
	      //h_p_all[xpoint][ypoint]->Fill(wf0->pulseCharges[k]*1000.0);

			  

	      h_p_sum_all->Fill(wf0->pulseCharges[k]*1000.0);


	    
	      if(wf0->pulseTimes[k] > 250 and wf0->pulseTimes[k] < 350 and wf0->pulseCharges[k]*1000.0 > 2.0 )
		{//FILTER PULSE TIME

			    
			  			 
		}//DONE FILTER PULSE TIME

	    }//DONE PULSE LOOP
		  
		  


	}//DONE WAVEFORM LOOP

     
      h_p_sum_all->Rebin(4);
 
      //Style;
      gStyle->SetPalette(6);
      gStyle->SetOptTitle(1); gStyle->SetOptStat(0);
      gStyle->SetOptFit(1111); gStyle->SetStatBorderSize(0);
      gStyle->SetStatX(.89); gStyle->SetStatY(.89);

     
      TCanvas *a = new TCanvas("a","",1200,500);
    
      gStyle->SetOptFit(0);

      h_p_sum_all->Draw(); 
      TF1 *fitFunction = new TF1("fit", "gaus");
 
      // Set initial parameters for the Gaussian fit
      //fitFunction->SetParLimits(0,-1,1); Setting limits not need just here for reference
      fitFunction->SetParameter(0, 1); // Amplitude
      fitFunction->SetParameter(1, 5); // Mean
      fitFunction->SetParameter(2, 1); // Sigma

      fitFunction->SetLineColor(kBlue);

      // Perform the fit
      h_p_sum_all->Fit(fitFunction, "R", "", 0, 100);

//////////////////////////////////////////////////////
      int totalBins = h_p_sum_all->GetNbinsX();
      double meanValue = fitFunction->GetParameter(1);
      std::cout << "Mean Value " << meanValue << std::endl;
      double meanRange = meanValue * 0.3;
      double rightMean = ceil(meanValue + meanRange);
      double leftMean = floor(meanValue - meanRange);
      std::cout << "Mean Value  plus " << leftMean << std::endl;
      std::cout << "Mean Value  minus " << rightMean << std::endl;

      int rightMeanBin = h_p_sum_all->GetXaxis()->FindBin(rightMean);
      int leftMeanBin = h_p_sum_all->GetXaxis()->FindBin(leftMean);

      double maxHeightRange = 0;
      double maxBinRange = 0;
      double height = 0;

      for (int i = leftMeanBin; i <= rightMeanBin; ++i) {
          
          height = h_p_sum_all->GetBinContent(i);
          if (height>maxHeightRange) {
              maxHeightRange=height;
              maxBinRange=i;
          }
      }
      int fitRange = maxBinRange * 0.75;

      int upper = ceil(maxBinRange + fitRange);
      int lower = floor(maxBinRange - fitRange);

      double xUpper = h_p_sum_all->GetBinCenter(upper);
      double xLower = h_p_sum_all->GetBinCenter(lower);
/////////////////////////////////////////////////////
      h_p_sum_all->Draw(); 
      TF1 *fitFunction2 = new TF1("fit", "gaus");
 
      // Set initial parameters for the Gaussian fit
      //fitFunction2->SetParLimits(0,-1,1); Setting limits not need just here for reference
      fitFunction2->SetParameter(0, 1); // Amplitude
      fitFunction2->SetParameter(1, 5); // Mean
      fitFunction2->SetParameter(2, 1); // Sigma

      fitFunction2->SetLineColor(kRed);

      // Perform the fit
      h_p_sum_all->Fit(fitFunction2, "R+", "", xLower, xUpper);

      // Update the canvas
      a->Update();

      //a->SaveAs(Form("Pulse_Height_CH%d.png",ch_name));
      a->SaveAs(Form("Pulse_Height_Fit_CH%d_Run%d.png", ch_name, run_num));

      std::cout<<"The efficiency for channel "<< ch_name << " is "<< 100*h_p_sum_all->GetEntries()/tt0->GetEntries()<<" %"<<std::endl;
      


     }//END CHANNEL LOOP 
}//DONE MAIN
     
*/
/*   
  std::cout<<"Mean pulse height" << ":"<< std::endl;

  for (int i = 0; i<=num_ch; i++){

     std::cout<< mph_arr[i] << std::endl;
   }
   return 0;
*/

}//DONE MAIN




