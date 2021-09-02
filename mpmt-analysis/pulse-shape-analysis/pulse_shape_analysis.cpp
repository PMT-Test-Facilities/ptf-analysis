#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TStyle.h"
#include "TLegend.h"

#include <iostream>
#include <cstring>
#include <string>
using namespace std;

int main( int argc, char* argv[] ) {

    TCanvas *c1 = new TCanvas("C1","C1",1600,1300);
    TLegend *legend = new TLegend(0.6,0.1,0.9,0.4);
    legend->SetHeader("Legend","C");

    TH1F * pulse_shapes[6];
    TFile * files[6] = {    new TFile("mpmt_Analysis_run0937.root"),
                            new TFile("mpmt_Analysis_run0947.root"),
                            new TFile("mpmt_Analysis_run0949.root"),
                            new TFile("mpmt_Analysis_run0951.root"),
                            new TFile("mpmt_Analysis_run0953.root"),
                            new TFile("mpmt_Analysis_run0955.root")};
    string num_pe[6] = {"1pe", "5pe", "10pe", "20pe", "30pe", "40pe"};

    int color[6]={1,633,600,419,618,807}; //,891,14,861,413,803,874,430,895,873,870,801,637,625,602};

    // Loop through ROOT files of different runs (1pe, 5pe, 10pe, 20pe, 30pe, 40pe)
    // and save the pulse shape.
    for (int i=0; i<6; i++) {
        pulse_shapes[i] = (TH1F*)files[i]->Get("pulse_shape");
        string name = num_pe[i] + " pulse shape";
        pulse_shapes[i]->SetName(name.c_str());
        pulse_shapes[i]->SetLineColor(color[i]);

        // Set up pad for current pulse shape
        gStyle->SetOptStat(11);
        TPad *pad = new TPad("pad", "", 0,0,1,1);
        if (i!=0) pad->SetFillStyle(4000);

        // Find baseline and pulse height
        float baseline = 0;
        for (int j=50; j<100; j++) baseline+=pulse_shapes[i]->GetBinContent(j);
        baseline = baseline/50;
        float ph = pulse_shapes[i]->GetBinContent(pulse_shapes[i]->GetMaximumBin())-pulse_shapes[i]->GetBinContent(pulse_shapes[i]->GetMinimumBin());
        
        // Scale pad to align baseline and minimum of pulse
        double ymin = pulse_shapes[i]->GetBinContent(pulse_shapes[i]->GetMinimumBin()) - 0.1*ph;
                double ymax = baseline+0.3*ph;
        //double ymax = baseline-0.5*ph;
        double dy = (ymax-ymin)/0.8;
        double xmin = 2130;
        double xmax = 2290;
        //double xmin = 2180;
        //double xmax = 2200;
        double dx = (xmax-xmin)/0.8;
        if (i==0) pulse_shapes[i]->GetYaxis()->SetRangeUser(ymin,ymax);pulse_shapes[i]->GetXaxis()->SetRangeUser(xmin,xmax);
        pad->Range(xmin-0.1*dx,ymin-0.1*dy,xmax+0.1*dx,ymax+0.1*dy);
        pad->Draw();

        // Draw current pulse shape
        pad->cd();
        pulse_shapes[i]->SetLineColor(color[i]);
        pulse_shapes[i]->GetYaxis()->SetAxisColor(color[i]);
        if (i==0) pulse_shapes[i]->Draw("C");
        else pulse_shapes[i]->Draw("SAMES C");
        pad->Update();
        c1->cd();

        legend->AddEntry(pulse_shapes[i],name.c_str(),"l");
    }

    legend->Draw();
    c1->SaveAs("pulse_shape_pe.root");
    c1->SaveAs("pulse_shape_pe.png");

    return 0;
}
