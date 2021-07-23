#include "WaveformFitResult.hpp"
#include "ScanPoint.hpp"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TGaxis.h"
#include "TF1.h"
#include "TLegend.h"

#include <math.h>
#include <iostream>
#include <string>
#include <cstring>
using namespace std;

const double bin_unit = 0.4883;

int main( int argc, char* argv[] ) {

    // Error message
    if ( argc < 2 ){
        std::cerr<<"Usage: ptf_ttree_analysis.app ptf_analysis.root\n";
        exit(0);
    }

    // Take additional/optional argument for event number
    int event_num = 0;
    bool check_event = false;
    if (argc == 3) {
        check_event = true;
        event_num = atoi(argv[2]);
    }

    TFile * fin = new TFile( argv[1], "read" );
    
    // Set up canvas for overlayed dark noise pulse heights
    TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
    TLegend *legend = new TLegend(0.5,0.1,1.0,0.9);
    legend->SetHeader("Legend","C");
    int color[16]={0,0,0,0,1,920,632,416,600,400,616,432,800,820,880,860};
    
    // Init other histograms
    TH1F *pulse_times = new TH1F("pulse_times", "Dark noise pulse times across all channel",1024,0,8192); //1024
    TH1F *pulse_count = new TH1F("pulse_count", "Number of dark noise pulses per event (across all channels)",13,0,12);
    TH1F *multi_hits = new TH1F("multi_hits","Number of channels with simultaneous hits",11,0,11);
    TH1F* multi_pulses = new TH1F("multi-hit","Number of pulses per channel for multiple hits (>2 simultaneous hits)",13,3,16);

    // Set up for scatter plots
    int n=0;                        // array indexing
    int num_channels[320000]={0};   // can replace ??

    // Set up WaveformFitResults and dark pulse ph histograms for channels 4-15
    TTree* tt[16];
    WaveformFitResult* wf[16];
    TH1F* ph[16];
    double mean[16]={0};
    for (int ch=4; ch<=15; ch++) {
        string filename = "ptfanalysis"+to_string(ch);
        tt[ch] = (TTree*)fin->Get(filename.c_str());
        wf[ch] = new WaveformFitResult;
        if(tt[ch]) wf[ch]->SetBranchAddresses(tt[ch]);
        string hist_name = "dark-ph-" + to_string(ch);
        ph[ch] = new TH1F(hist_name.c_str(), "Dark noise pulse heights", 130,0,bin_unit*130);
    }

    // Dark rate calculation variabels
    int num_dark_pulses[16]={0};
    int dark_rates[16];
    int time;

    // Multi-hit event rate calculation variables
    int num_multi_hits=0;
    double multi_rate;

    int chan[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    int pt[16];
    double pulse_range[2] = {0,8192};
    int counter = 0;

    // TCanvas *c_timing[16];
    // TH2F *timing[16];

    // for (int j=4; j<=15; j++) {
    //     c_timing[j] = new TCanvas("C_TIMING");
    //     string th2f_name = "Time of hits in other channel relative to chan " + to_string(j);
    //     timing[j] = new TH2F("timing_hist", th2f_name.c_str(),16,0,16,200,-100,100);
    // }

    // For each event:
    for (int i=0; i<tt[4]->GetEntries()-1; i++) {
        int wf_pulses[16]={0};              // Init array for num pulses per waveform
        // For each channel:
        for (int j=4; j<=15; j++) {
            tt[j]->GetEvent(i);             // get waveform

            // For each pulse
            for (int k=0; k<wf[j]->numPulses; k++) {
                
                // Save pulse time of channels with a hit, if given a specified event number
                if (check_event && i==event_num && k==0) pt[j]=wf[j]->pulseTimes[k];
                if (counter==0 && i==event_num) {
                    pulse_range[0] = wf[j]->pulseTimes[k]-50;
                    pulse_range[1] = wf[j]->pulseTimes[k]+50;
                    counter++;
                }

                // Collect pulse height and num pulses per waveform
                // Note: maximum of 1 pulse per waveform is counted (i.e. in the case of multiple consecutive pulses, count only one)
                // Note: pulse height of only the first pulse in the waveform is recorded
                if (wf_pulses[j]==0) {
                    ph[j]->Fill(wf[j]->pulseCharges[k]*1000.0);
                    // if (wf[j]->pulseCharges[k]*1000>100) cout << "event: " << i << endl;           // collect pulse charge of max pulse height
                    num_dark_pulses[j]++;
                }

                wf_pulses[j]++;             // collect num pulses per waveform

                // Collect pulse time
                // Note: Beginning and end times were removed due to weird binning
                if (wf[j]->pulseTimes[k]<8150 && wf[j]->pulseTimes[k]>50) {
                    pulse_times->Fill(wf[j]->pulseTimes[k]);
                }

                // Record num of pulses per waveform
                pulse_count->Fill(wf_pulses[j]);
            }
        }

        // MULTIPLE-HIT EVENT RATE ANALYSIS
        if  (i==0) cout<<"============== MULTI-HIT EVENT DETAILS =============="<<endl;
        int count = 1;                      // Init counter for multi-hit event rate
        // For each channel:
        for (int j=4; j<=15; j++) {  

            // If channel j has a pulse:
            //  - record the pulse time in channel c - pulse time in channel j
            //  - count the number of other channels also with a pulse occuring at a similar time
            if (wf_pulses[j]!=0) {
                for (int c=4; c<=15; c++) {
                    if (wf_pulses[c]==0) continue;          //alt condition for same num pulses: "!=wf_pulses[j]) {" 

                    // timing[j]->Fill(c,wf[c]->pulseTimes[0] - wf[j]->pulseTimes[0]);

                    if (wf[c]->pulseTimes[0]<=wf[j]->pulseTimes[0]+100 && wf[c]->pulseTimes[0]>=wf[j]->pulseTimes[0]-100) {
                        num_channels[n]++;                  // Increase count for num channels with simulatenous hit
                    }
                }
            
                multi_hits->Fill(num_channels[n]);          // Record num channels with simultaneous hit
                
                // Count the number of events with >2 multi-hit pulses
                // Pprint statements to see details of specified multi-hit event
                if (num_channels[n]>2) {//<5 && num_channels[n]>1) {
                    if (count==1) {
                        if (check_event && i==event_num) {
                            cout << "Event num: " << i << endl;
                            cout<<"Num channels with simultaenous hits: " << num_channels[n] << endl;
                        }
                        num_multi_hits++;                   // Count number of multi-hit(>2) events
                    }
                    count++;
                    multi_pulses->Fill(j);                  // Record channels with multi-hit event
                    if (check_event && i==event_num) cout<<"chan"<<j<<": "<<wf_pulses[j]<<" pulse(s), first pulse at: "<<wf[j]->pulseTimes[0]<<" ns"<<endl;

                }
                n++;
            }
        }
    }
    
    // Dark noise rate calculation and ROOT plotting
    for (int y=6; y<=17; y++) {

        int ch = (y>15) ? y-12 : y;

        // Calculate dark noise rate
        time = tt[ch]->GetEntries() * 8192 * pow(10,-9); //[seconds]
        dark_rates[ch] = num_dark_pulses[ch]/time; //[pulses/second]

        // Draw pulse height hist
        ph[ch]->SetLineColor(color[ch]);
        ph[ch]->Draw("][sames");
        ph[ch]->Fit("gaus","0Q","C",5,14);
        TF1 *fit = (TF1*)ph[ch]->GetListOfFunctions()->FindObject("gaus");
        mean[y] = fit->GetParameter(1);
        string legendname = "Chan" + to_string(ch) + " dark rate: " + to_string(dark_rates[ch]) + " pulses/sec. Mean: " + to_string(mean[y]).substr(0, 4);
        legend->AddEntry(ph[ch],legendname.c_str(),"l");
        ph[ch]->GetXaxis()->SetTitle("Pulse height (mV)");
        ph[ch]->GetYaxis()->SetTitle("Number of events");        
    }

    // Calculate and print the multi-hit event rate:
    multi_rate = num_multi_hits/time;
    cout << "Multi-hit event rate is: " << multi_rate << " events/second" << endl;
    // Calculate and print multi-hit even occurance:
    cout << "Num multi-hit events out of total num events is: " << num_multi_hits << "/" << tt[4]->GetEntries() << " = 0.0002873680035" << endl;
    cout<<"=============== MULTI-HIT EVENT DETAILS END ==============="<<endl;

    legend->Draw();
    c1->SaveAs("mpmt_dark_noise_ph.png");

    // Plot graphs
    TCanvas *c4 = new TCanvas("C4");
    pulse_times->Draw();
    pulse_times->GetXaxis()->SetTitle("Pulse time (ns)");
    pulse_times->GetYaxis()->SetTitle("Number of events");
    pulse_times->Fit("pol1","Q");
    c4->SaveAs("mpmt_dark_noise_time.png");
    
    TCanvas *c5 = new TCanvas("C5");
    gPad->SetLogy();
    multi_hits->Draw();
    multi_hits->GetXaxis()->SetTitle("Number of channels with simultaneous hits");
    multi_hits->GetYaxis()->SetTitle("Number of events");
    c5->SaveAs("mpmt_multi_hits.png");

    TCanvas *c6 = new TCanvas("C6");
    pulse_count->Draw();
    pulse_count->GetXaxis()->SetTitle("Num pulses per waveform");
    pulse_count->GetYaxis()->SetTitle("Number of events");
    gPad->SetLogy();
    c6->SaveAs("mpmt_dark_noise_count.png");

    TCanvas *c7 = new TCanvas("C7");
    TGraph *chan_vs_dark = new TGraph(15,chan,dark_rates);
    chan_vs_dark->GetXaxis()->SetRangeUser(0,15);
    chan_vs_dark->GetXaxis()->SetTitle("Channel number");
    chan_vs_dark->GetYaxis()->SetTitle("Dark noise rate");
    chan_vs_dark->SetTitle("Dark noise rate according to channel");
    chan_vs_dark->Draw("a*");
    c7->SaveAs("mpmt_dark_rate_channel.png");

    TCanvas *c8 = new TCanvas("C8");
    multi_pulses->Draw();
    multi_pulses->GetXaxis()->SetTitle("Channel number");
    multi_pulses->GetYaxis()->SetTitle("Number of events");
    c8->SaveAs("mpmt_multi_hit_count.png");

    if (check_event) {
        TCanvas *c9 = new TCanvas("C9");
        TGraph *multi_hit_time = new TGraph(16,chan,pt);
        multi_hit_time->GetXaxis()->SetTitle("Channel number");
        multi_hit_time->GetYaxis()->SetTitle("Pulse times");
        multi_hit_time->GetYaxis()->SetRangeUser(pulse_range[0],pulse_range[1]);
        string graph_title = "Multi-hit event " + to_string(event_num) + " pulse times according to channel";
        multi_hit_time->SetTitle(graph_title.c_str());
        multi_hit_time->Draw("a*");
        string fname = "mpmt_multi_hits_" + to_string(event_num) + ".png";
        c9->SaveAs(fname.c_str());
    }
    
    // for (int j=4; j<=6; j++) {
    //     timing[j]->GetXaxis()->SetTitle("Chan x");
    //     string Yaxis_name = "Pulse time in chan x - pulse time in chan " + to_string(j);
    //     timing[j]->GetYaxis()->SetTitle(Yaxis_name.c_str());
    //     timing[j]->Draw("COLZ");
    //     string file_name = "pt_comparison_ch_" + to_string(j) + ".png";
    //     c_timing[j]->SaveAs(file_name.c_str());
    // }

    fin->Close();
    return 0;
}