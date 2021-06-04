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
    
    Double_t w = 800;
    Double_t h = 600;
    TCanvas *c1 = new TCanvas("c1", "c1", w, h);
    TLegend *legend = new TLegend(0.5,0.1,1.0,0.9);
    legend->SetHeader("Legend","C");
    
    int color[16];
    color[4]=1;
    color[5]=920;
    color[6]=632;
    color[7]=416;
    color[8]=600;
    color[9]=400;
    color[10]=616;
    color[11]=432;
    color[12]=800;
    color[13]=820;
    color[14]=880;
    color[15]=860;
    
    TH1F *dark_time = new TH1F("dark-time", "Dark noise pulse times across all channel",1024,0,8192);

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
            
            // For each pulse
            for (int k=0; k<wf->numPulses; k++) {
                // Collect pulses separately
                                
                ph->Fill(wf->pulseCharges[k]*1000.0);
                num_pulses++;
                dark_time->Fill(wf->pulseTimes[k]);
                break;      // uncomment for analysis of dark rate without separation
                

            }
        }
        
        // Dark noise rate
        int time = tt->GetEntries() * 8192 * pow(10,-9); //[seconds]
        int dark_rate = num_pulses/time; //[pulses/second]
        
        ph->SetLineColor(color[ch]);
        
        ph->Draw("][sames");
        
        ph->Fit("gaus","0","C",5,14);
        TF1 *fit = (TF1*)ph->GetListOfFunctions()->FindObject("gaus");
        double mean = fit->GetParameter(1);

        
        string legendname = "Chan" + to_string(ch) + " dark rate: " + to_string(dark_rate) + " pulses/sec. Mean: " + to_string(mean).substr(0, 4); //ph->GetMean()
        legend->AddEntry(ph,legendname.c_str(),"l");
    }
    
    legend->Draw();
    c1->SaveAs("mpmt_dark_noise_ph.png");
    
    TCanvas *c2 = new TCanvas("C2");
    dark_time->Draw();
    dark_time->GetXaxis()->SetTitle("Pulse time (ns)");
    dark_time->GetYaxis()->SetTitle("Number of events");
    c2->SaveAs("mpmt_dark_noise_time.png");
    
    fin->Close();
    return 0;
}
