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

int main( int argc, char* argv[] ) {

    if ( argc != 2 ){
        std::cerr<<"Usage: ptf_ttree_analysis.app ptf_analysis.root\n";
        exit(0);
    }

    TFile * fin = new TFile( argv[1], "read" );
    
    // Set up canvas
    TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
    TLegend *legend = new TLegend(0.5,0.1,1.0,0.9);
    legend->SetHeader("Legend","C");
    int color[16]={0,0,0,0,1,920,632,416,600,400,616,432,800,820,880,860};
    
    // Init other histograms
    TH1F *dark_time = new TH1F("dark-time", "Dark noise pulse times across all channel",1024,0,8192); //1024
    TH1F *dark_pulses = new TH1F("dark-pulses", "Dark noise pulses per waveform across all channels",10,2,12);
    TH2F *concurrent_hist = new TH2F("concurrent-pulses", "2D Hist of dark noise occurance across channels",11,0,11,12,0,12);

    // Set up for scatter plot
    int n=0;
    int num_pulses[320000];
    int num_channels[320000]={0};

    // Set up WaveformFitResult and histograms for channels 4-15
    TTree* tt[16];
    WaveformFitResult* wf[16];
    TH1F* ph[16];
    for (int ch=4; ch<=15; ch++) {
        string filename = "ptfanalysis"+to_string(ch);
        tt[ch] = (TTree*)fin->Get(filename.c_str());
        wf[ch] = new WaveformFitResult;
        if(tt[ch]) wf[ch]->SetBranchAddresses(tt[ch]);
        string hist_name = "dark-ph-" + to_string(ch);
        ph[ch] = new TH1F(hist_name.c_str(), "Dark noise pulse heights", 130,0,bin_unit*130);
        if (ch==4) {
            ph[ch]->GetXaxis()->SetTitle("Pulse height (mV)");
            ph[ch]->GetYaxis()->SetTitle("Number of events");
        }
    }

    // Init dark rate calculation variabels
    int num_dark_pulses[16]={0};
    int dark_rates[16];

    // For each event:
    for (int i=0; i<tt[4]->GetEntries()-1; i++) {
        // Load pulse info for each channel
        int wf_pulses[16]={0};              // set up num pulses per waveform
        for (int j=4; j<=15; j++) {
            tt[j]->GetEvent(i);             // get waveform
            

            // For each pulse
            for (int k=0; k<wf[j]->numPulses; k++) {
                // Collect pulse height and number of pulses in waveform
                if (wf_pulses[j]==0) {
                    ph[j]->Fill(wf[j]->pulseCharges[k]*1000.0);
                    num_dark_pulses[j]++;
                }
                wf_pulses[j]++;
                // Collect pulse time
                if (wf[j]->pulseTimes[k]<8150 && wf[j]->pulseTimes[k]>50) dark_time->Fill(wf[j]->pulseTimes[k]);
                // Collect number of pulses per waveform
                dark_pulses->Fill(wf_pulses[j]);
            }
        }
            
        // Check occurance of dark pulses across different channels
        for (int j=4; j<=15; j++) {           
            if (wf_pulses[j]!=0) {
                num_pulses[n]=wf_pulses[j];
                for (int c=4; c<=15; c++) {
                    if (wf_pulses[c]!=0){//==wf_pulses[j]) { 
                        if (wf[c]->pulseTimes[0]<=wf[j]->pulseTimes[0]+100 || wf[c]->pulseTimes[0]>=wf[j]->pulseTimes[0]-100) {
                            num_channels[n]++;
                            // if(num_channels[n]>1 && num_pulses[n]>1) cout<<i<<endl;
                        }
                    }
                }
                concurrent_hist->Fill(num_pulses[n],num_channels[n]);
                if (i==80154) cout<<"chan"<<j<<": "<<wf_pulses[j]<<"pulses, "<<num_channels[n]<<" channels, pulse time: "<<wf[j]->pulseTimes[0]<<endl;
                n++;
            }
        }
    }
    
    for (int y=6; y<=17; y++) {
        int ch;
        if (y>15) {
            ch = y-12;
        } else {
            ch = y;
        }

        // Calculate dark noise rate
        int time = tt[4]->GetEntries() * 8192 * pow(10,-9); //[seconds]
        dark_rates[ch] = num_dark_pulses[ch]/time; //[pulses/second]

        // Draw pulse height hist
        ph[ch]->SetLineColor(color[ch]);
        ph[ch]->Draw("][sames");
        // cout<<"channel: "<<y<<", line: 121"<<endl;
        ph[ch]->Fit("gaus","0Q","C",5,14);
        TF1 *fit = (TF1*)ph[ch]->GetListOfFunctions()->FindObject("gaus");
        double mean = fit->GetParameter(1);
        string legendname = "Chan" + to_string(ch) + " dark rate: " + to_string(dark_rates[ch]) + " pulses/sec. Mean: " + to_string(mean).substr(0, 4);
        legend->AddEntry(ph[ch],legendname.c_str(),"l");
        
    }

    legend->Draw();
    c1->SaveAs("mpmt_dark_noise_ph.png");
    
    TCanvas *c2 = new TCanvas("C2", "C2", 800,600);
    TGraph *concurrent_pulses = new TGraph(n,num_pulses,num_channels);
    concurrent_pulses->SetTitle("Dark noise occurance across channels");
    concurrent_pulses->GetXaxis()->SetRangeUser(0,10);
    concurrent_pulses->GetYaxis()->SetRangeUser(0,12);
    concurrent_pulses->GetXaxis()->SetTitle("Num pulses in event");
    concurrent_pulses->GetYaxis()->SetTitle("Num channels with pulse");
    concurrent_pulses->Draw("a*");
    c2->SaveAs("mpmt_concurrent_pulses.png");

    TCanvas *c3 = new TCanvas("C3");
    concurrent_hist->GetXaxis()->SetTitle("Num pulses in event");
    concurrent_hist->GetYaxis()->SetTitle("Num channels with pulse");
    concurrent_hist->Draw("COLZ");
    c3->SaveAs("mpmt_concurrent_hist.png");
    
    fin->Close();
    return 0;
}
