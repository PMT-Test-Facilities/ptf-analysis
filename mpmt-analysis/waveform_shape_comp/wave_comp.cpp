// Ashley program to plot waveforms from different root files to compare their shapes
// 2020-03-18

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

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
//using namespace std;

int main( int argc, char* argv[] ) {



    // Check that the proper number of command line arguments was provided
    if ( argc != 5 ){
        std::cerr<<"Usage: ptf_ttree_analysis.app ptf_analysis_1.root ptf_analysis_2.root ptf_analysis_3.root ptf_analysis_4.root\n";
        exit(0);
    }



    // Grab the root files
    TFile *file1 = new TFile(argv[1], "read");
    TFile *file2 = new TFile(argv[2], "read");
    TFile *file3 = new TFile(argv[3], "read");
    TFile *file4 = new TFile(argv[4], "read");



    //Grab the specific waveform from each file that you want plotted
    TH1D *waveform1 = (TH1D*)file1->Get("PMT0_NoWaveforms/hwf_4;1");
    TH1D* waveform2 = (TH1D*)file2->Get("PMT0_NoWaveforms/hwf_5;1");
    TH1D* waveform3 = (TH1D*)file3->Get("PMT0_NoWaveforms/hwf_0;1");
    TH1D* waveform4 = (TH1D*)file4->Get("PMT0_NoWaveforms/hwf_4;1");



    //Create Canvas and Plot!
    TCanvas *c = new TCanvas("C");
    
    waveform4->SetLineWidth(2);
    waveform4->Draw("hist");
    waveform4->SetMarkerColor(7);
    waveform4->SetLineColor(kCyan);

    waveform3->SetLineWidth(2);
    waveform3->Draw("histSAME");
    waveform3->SetMarkerColor(6);
    waveform3->SetLineColor(kPink);

    waveform2->SetLineWidth(2);
    waveform2->Draw("histSAME");
    waveform2->SetMarkerColor(4);
    waveform2->SetLineColor(kBlue);

    waveform1->SetLineWidth(2);
    waveform1->Draw("histSAME");
    waveform1->SetMarkerColor(2);
    waveform1->SetLineColor(kRed);



    //formatting (only need to do this for the first one)
    waveform4->GetXaxis()->SetTitle("Time (ns)");
    waveform4->GetXaxis()->SetRangeUser(2000, 2600);
    waveform4->GetYaxis()->SetTitle("Voltage (V)");

    // Legend
    TLegend *leg = new TLegend(0.8,1,1,0.8);
    leg->AddEntry(waveform1, "3e6 Gain");
    leg->AddEntry(waveform2, "5e6 Gain");
    leg->AddEntry(waveform3, "1e7 Gain");
    leg->AddEntry(waveform4, "2e7 Gain");
    leg->SetTextSize(0.036);
    leg->SetLineWidth(0);
    leg->Draw("SAME");

    c->SaveAs("overlaid-waveforms.png");
    
    
    
    file1->Close();
    file2->Close();
    file3->Close();
    file4->Close();
    return 0;
}
