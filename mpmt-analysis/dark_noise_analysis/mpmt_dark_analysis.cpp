#include "WaveformFitResult.hpp"
#include "ScanPoint.hpp"
#include "TCanvas.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1D.h"
#include "TH2F.h"
#include "THStack.h"
#include "TGraph.h"
#include "TGaxis.h"
#include "TF1.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TPaveStats.h"
#include "TProfile.h"

#include <math.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
using namespace std;

const double bin_unit = 0.4883;

int x=0;

int main( int argc, char* argv[] ) {

    if ( argc != 2 ){
        std::cerr<<"Usage: ptf_ttree_analysis.app ptf_analysis.root\n";
        exit(0);
    }

    TFile * fin = new TFile( argv[1], "read" );
    
    // Set up canvas
    Double_t w = 800;
    Double_t h = 600;
    TCanvas *c1 = new TCanvas("c1", "c1", w, h);
    TLegend *legend = new TLegend(0.5,0.1,1.0,0.9);
    legend->SetHeader("Legend","C");
    int color[16]={0,0,0,0,1,920,632,416,600,400,616,432,800,820,880,860};
    
    // Init other histograms
    TH1F *dark_time = new TH1F("dark-time", "Dark noise pulse times across all channel",1024,0,8192); //1024
    TH1F *dark_pulses = new TH1F("dark-pulses", "Dar noise pulses per waveform across all channels",10,2,12);
    
//    int times[16][713371];
    int concurrent_ch[50000];
    int concurrent_i[50000];
    int n=0;
    
    // For channels 4-15, view pulse height
    for (int j=6; j<=17; j++) {      //6--17
        int ch;
        if (j>15) {
            ch = j-12;
        } else {
            ch = j;
        }
                
        string filename = "ptfanalysis"+to_string(ch);
        TTree * tt = (TTree*)fin->Get(filename.c_str());
        WaveformFitResult * wf = new WaveformFitResult;
        if(tt) wf->SetBranchAddresses(tt);
        
        // Initiliaze histogram
        TH1F *ph = new TH1F("dark-ph", "Dark noise pulse heights", 130,0,bin_unit*130);
        if (ch==6) {
            ph->GetXaxis()->SetTitle("Pulse height (mV)");
            ph->GetYaxis()->SetTitle("Number of events");
        }

        int num_pulses = 0;
        // For each waveform
        for(int i = 0; i < tt->GetEntries()-1; i++){
            tt->GetEvent(i);
            bool pulse_detected = false;
            int wf_pulses = 0;
            
            // For each pulse
            for (int k=0; k<wf->numPulses; k++) {
                
                // Collect pulse heights
                if (!pulse_detected) {
                    ph->Fill(wf->pulseCharges[k]*1000.0);
                    num_pulses++;
                    pulse_detected = true;
                    
                }
                
                // Collect pulse times
                if (wf->pulseTimes[k]<8150 && wf->pulseTimes[k]>50) dark_time->Fill(wf->pulseTimes[k]);
                wf_pulses++;
                
            }
            
            // Collect number of pulses per waveform
            dark_pulses->Fill(wf_pulses);
            
//             Collect event with dark pulse
            if(pulse_detected) {
                concurrent_ch[n]=ch;
                concurrent_i[n]=i;
                n++;
            }
        }
        
        x+=num_pulses;
        
        // Calculate dark noise rate
        int time = tt->GetEntries() * 8192 * pow(10,-9); //[seconds]
        int dark_rate = num_pulses/time; //[pulses/second]
        
        // Draw pulse height hist
        ph->SetLineColor(color[ch]);
        ph->Draw("][sames");
        ph->Fit("gaus","0Q","C",5,14);
        TF1 *fit = (TF1*)ph->GetListOfFunctions()->FindObject("gaus");
        double mean = fit->GetParameter(1);
        string legendname = "Chan" + to_string(ch) + " dark rate: " + to_string(dark_rate) + " pulses/sec. Mean: " + to_string(mean).substr(0, 4); //ph->GetMean()
        legend->AddEntry(ph,legendname.c_str(),"l");
    }
    
//    cout<<x<<endl;
    
    legend->Draw();
    c1->SaveAs("mpmt_dark_noise_ph.png");
    
    TCanvas *c2 = new TCanvas("C2");
    dark_time->Draw();
    dark_time->GetXaxis()->SetTitle("Pulse time (ns)");
    dark_time->GetYaxis()->SetTitle("Number of events");
    dark_time->Fit("pol1");
    c2->SaveAs("mpmt_dark_noise_time.png");
    
    TCanvas *c3 = new TCanvas("C3");
    dark_pulses->Draw();
    dark_pulses->GetXaxis()->SetTitle("Num pulses per waveform");
    dark_pulses->GetYaxis()->SetTitle("Number of events");
//    dark_pulses->Print("all");
    c3->SaveAs("mpmt_dark_noise_pulses.png");
    
//    // Calculate frequency of dark pulse occuring at same time across multiple channels
//    for (c=4; c<=15; c++) {
//        for (e=0; e<713371; e++) {
//            int target = times[c][e];
//            for (ci=4; ci<=15; ci++) {
//                if (times[ci][e]==target) concurrent_pulses++;
//            }
//        }
//    }
    
//    cout<<"Frequency of dark pulse occuring at same time across multiple channels"<< concurrent_pulses/713371<<endl;
    
    
    w=1000;
    h=1500;
    TCanvas *c4 = new TCanvas("C4","C4",w,h);
    TGraph *concurrent_pulses = new TGraph(n,concurrent_ch,concurrent_i);
    concurrent_pulses->SetTitle("Dark noise occurance across channels");
//    concurrent_pulses->GetXaxis()->SetRangeUser(3,16);
//    concurrent_pulses->GetYaxis()->SetRangeUser(0,713371);
    concurrent_pulses->GetXaxis()->SetTitle("Channel");
    concurrent_pulses->GetYaxis()->SetTitle("Pulse event");
    concurrent_pulses->Draw("ap");
    c4->SaveAs("mpmt_concurrent_pulses.png");
    
    fin->Close();
    return 0;
}
