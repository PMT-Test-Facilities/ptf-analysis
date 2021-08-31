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
#include "TPaveStats.h" 

#include <math.h>
#include <iostream>
#include <string>
#include <cstring>
using namespace std;

/* 
This program takes a processed ROOT output file from mpmt_analysis and 
conducts dark noise and multi-hit event analyses.

This program had been used to solely look at run 838 on the integrated mPMT where
only channels 4-16 were active.

It will output the following plots and information:
    Dark noise analysis:
    - Dark noise pulse height histogram (mpmt_dark_noise_ph.png)
    - Dark noise pulse time histogram (mpmt_dark_noise_time.png)
    - Dark noise rate vs active channels graph (mpmt_dark_rate_channel.png)
    - Dark noise pulses per event histogram (mpmt_dark_noise_count.png)
    - Prints total run time (command line)

    Multi-hit event analysis:
    - Histogram of number of hit channels (mpmt_multi_hits.png)
    - Histogram of number of total hits in active channels during multi-hit events (mpmt_multi_hit_pulses.png)
    - Histogram of pulse time distribution relative to highest charged pulse in multi-hit events (mpmt_multi_hit_pt_dist.png)
    - Histogram of first and second largest pulse charges in multi-hit events (mpmt_multi_hit_charge.png)
    - 2D histogram of pulse timing relative to channel x for multi-hit events (mpmt_multi_hit_pt_chx.png) 
    - Prints information on multi-hit event rate and total run time
    
    When optional command line argument of multi-hit event number (i.e. 108036) is provided,
    the following additional information are printed:
    - Number of channels with hits 
    - Maximum and second largest charge detected
    - All hit channels, pulse time, and pulse charge.
    - Additionally, graph of pulse time vs active channels is produced (mpmt_multi_hit_108036.png)

*/

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
    
    // Set up for scatter plots
    int n=0;                        // array indexing
    int num_channels[320000]={0};

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
    int num_dark_pulses[16]={0};    // total number of dark noise pulses in each channel
    int dark_rates[16];             // dark noise rates indexed by channel
    double time;                    // total time 

    // Multi-hit event rate calculation variables
    int num_multi_hits=0;
    double multi_rate;

    int chan[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    int event_pt[16];
    double pulse_range[2] = {0,8192};
    int counter = 0;

    // Construct histograms
    TH1F * max_pulse = new TH1F("largest pulse","Charge distribution of largest pulses in multi-hit events",80,0,80);
    TH1F * sec_pulse = new TH1F("second largest pulse","Charge distribution of first & second largest pulse in every multi-hit event",80,0,80);

    TH1F *multi_hits = new TH1F("multi_hits","Number of channels with simultaneous hits",11,0,11);
    TH1F* multi_pulses = new TH1F("multi-hit","Number of total hits in each channel in multi-hit (>2 simultaneous hits) events",14,3,17);

    TH1F *rel_pt = new TH1F("rel_pt","Pulse time distribution relative to highest charged pulse in multi-hit events",10,-40,40);
    TH2F *pt_offset[16];
    for (int i=4; i<16; i++) {
        string pt_offset_name = "pt_offset_" + to_string(i);
        string pt_offset_title = "Pulse timing offset (relative to channel " + to_string(i) + ") in multi-hit events";
        pt_offset[i] = new TH2F(pt_offset_name.c_str(),pt_offset_title.c_str(),16,0,16,10,-40,40);
    }

    // For each event:
    for (int i=0; i<tt[4]->GetEntries()-1; i++) {
        int wf_pulses[16]={0};              // Init array for num pulses per waveform (in channel j)
        int max_ch = 100;                   // Init int to hold channel number with largest pulse (max charge)
        double max_charge = 0;              // Maximum charge in this event
        double sec_charge = 0;              // Second largest charge in this event
        // For each channel:
        for (int j=4; j<=15; j++) {
            tt[j]->GetEvent(i);             // get waveform
            
            // For each pulse
            for (int k=0; k<wf[j]->numPulses; k++) {
                
                // Save pulse time of channels with a hit, if given a specified event number
                if (check_event && i==event_num && k==0) {
                    if (j<8) event_pt[j]=wf[j]->pulseTimes[k];
                    if (j>=8 && j<12) event_pt[j]=wf[j]->pulseTimes[k]-16;
                    if (j>=12) event_pt[j]=wf[j]->pulseTimes[k]-8;
                    
                }
                if (counter==0 && i==event_num) {
                    pulse_range[0] = wf[j]->pulseTimes[k]-50;
                    pulse_range[1] = wf[j]->pulseTimes[k]+50;
                    counter++;
                }


                // For the first pulse in waveform event i of channel j:
                //      Note: maximum of 1 pulse per waveform is counted (i.e. in the case of multiple consecutive pulses, count only one)
                if (wf_pulses[j]==0) {

                    // Collect pulse height and num pulses per waveform (Note: pulse height of only the first pulse in the waveform is recorded)
                    ph[j]->Fill(wf[j]->pulseCharges[k]*1000.0);     // collect pulse height of first pulse
                    num_dark_pulses[j]++;                           // increment total num of pulses in channel j

                    // Record/update largest and second largest charge recorded in waveform event i:
                    if (wf[j]->pulseCharges[0]*1000/8.5 > max_charge) {
                        sec_charge=max_charge;
                        max_charge=wf[j]->pulseCharges[0]*1000/8.5;
                        max_ch = j;     // record/update channel with the largest pulse
                    } else {
                        if (wf[j]->pulseCharges[0]*1000/8.5>sec_charge) sec_charge=wf[j]->pulseCharges[0]*1000/8.5;
                    }

                }

                wf_pulses[j]++;             // increment num pulses in waveform event i in channel j

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
            // Count num channels with simultaneous hits
            if (wf_pulses[j]!=0) {
                for (int c=4; c<=15; c++) {
                    if (wf_pulses[c]==0) continue;          // alt condition for same num pulses: "!=wf_pulses[j]) {" 
                    if (wf[c]->pulseTimes[0]<=wf[j]->pulseTimes[0]+100 && wf[c]->pulseTimes[0]>=wf[j]->pulseTimes[0]-100) {
                        num_channels[n]++;                  // inc count for num channels with simulatenous hit
                    }
                    
                }
            
                multi_hits->Fill(num_channels[n]);          // Record num channels with simultaneous hit
                
                // Count the number of events with >2 simultaneous hits (multi-hits)
                if (num_channels[n]>2) {

                    // record relative pulse time
                    if (j!=max_ch) rel_pt->Fill(wf[j]->pulseTimes[0]-wf[max_ch]->pulseTimes[0]);

                    // record pulse time offset from channel j
                    for (int k=4; k<16; k++) pt_offset[j]->Fill(k,wf[k]->pulseTimes[0]-wf[j]->pulseTimes[0]);

                    if (count==1) {
                        if (check_event && i==event_num) {
                            cout << "Event num: " << i << endl;
                            cout<<"Num channels with simultaenous hits: " << num_channels[n] << endl;
                            cout<<"Max charge "<<max_charge<<" PE, second largest charge "<<sec_charge<<" PE"<<endl;
                        }
                        num_multi_hits++;                   // inc number of multi-hit(>2) events

                        // Save charge of largest and second largest pulses
                        max_pulse->Fill(max_charge);
                        sec_pulse->Fill(sec_charge);
                    }
                    count++;
                    multi_pulses->Fill(j);                  // Record channels with multi-hit event

                    // Print channel and pulse info
                    if (check_event && i==event_num) {
                        cout<<"chan"<<j<<": "<<wf_pulses[j]<<" pulse(s), ";
                        cout<<"first pulse: at "<<wf[j]->pulseTimes[0]<<" ns, ";
                        cout<<"with charge "<<wf[j]->pulseCharges[0]*1000/8.5<<"PE"<<endl;
                    }
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
    cout << "Total time of run is: " << time << " seconds" <<endl;
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
    c8->SaveAs("mpmt_multi_hit_pulses.png");

    if (check_event) {
        TCanvas *c9 = new TCanvas("C9");
        TGraph *multi_hit_time = new TGraph(16,chan,event_pt);
        multi_hit_time->GetXaxis()->SetTitle("Channel number");
        multi_hit_time->GetYaxis()->SetTitle("Pulse times");
        multi_hit_time->GetYaxis()->SetRangeUser(pulse_range[0],pulse_range[1]);
        string graph_title = "Multi-hit event " + to_string(event_num) + " pulse times according to channel";
        multi_hit_time->SetTitle(graph_title.c_str());
        multi_hit_time->Draw("a*");
        string fname = "mpmt_multi_hit_" + to_string(event_num) + ".png";
        c9->SaveAs(fname.c_str());
    }

    TCanvas *c10 = new TCanvas("C10");
    sec_pulse->SetLineColor(2);
    sec_pulse->Draw();
    sec_pulse->GetXaxis()->SetTitle("Charge of pulse (PE)");
    sec_pulse->GetYaxis()->SetTitle("Frequency");
    max_pulse->SetLineColor(1);
    max_pulse->Draw("SAMES");
    c10->Modified();
    c10->Update();
    TPaveStats *p2 = (TPaveStats*)c10->GetPrimitive("stats");
    p2->SetX1NDC(0.2); p2->SetX2NDC(0.4);
    p2->SetTextColor(2);
    c10->Modified();
    c10->SaveAs("mpmt_multi_hit_charge.png");

    TCanvas *c11 = new TCanvas("C11");
    rel_pt->Draw();
    rel_pt->GetXaxis()->SetTitle("Pulse time relative to largest channel in multi-hit event");
    rel_pt->GetYaxis()->SetTitle("Num of events");
    c11->SaveAs("mpmt_multi_hit_pt_dist.png");
    
    TCanvas *c12 = new TCanvas("C12");
    for (int i=4; i<16; i++) {    
        c12->Clear();
        pt_offset[i]->GetYaxis()->SetTitle("Pulse time relative to pulses in channel 4");
        pt_offset[i]->GetXaxis()->SetTitle("Channel");
        pt_offset[i]->Draw("COLZ");
        string c12_name = "mpmt_multi_hit_pt_ch"+to_string(i)+".png";
        c12->SaveAs(c12_name.c_str());
    }

    fin->Close();
    return 0;
}