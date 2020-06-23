#include "PulseFinding.hpp"
#include <iostream>

void find_pulses(int algo_type, TH1D *hwaveform, WaveformFitResult *fitresult ){

  // Reset the number of pulses
  fitresult->numPulses = 0;

  if(algo_type == 0){
    simple_threshold_technique(hwaveform, fitresult);
  }else{
    std::cerr << "Invalid pulse finding algorithm = " << algo_type
              << ". Exiting"<< std::endl;
    exit(0);
  }
  
  
}



void simple_threshold_technique(TH1D *hwaveform, WaveformFitResult *fitresult ){
  
  // Loop over waveform; look for every case of waveform going below fixed threshold
  int threshold = 2045 - 15;

  int nsamples = hwaveform->GetNbinsX();

  int min_bin = 9999, min_value = 9999; // Find the minimum bin and value for each pulse.
  bool in_pulse = false; // are we currently in a pulse?
  for(int ib = 1; ib < nsamples+1; ib++){
    int sample = hwaveform->GetBinContent(ib);

    if(sample < min_value){
      min_value = sample;
      min_bin = ib;
    }
    
    if(sample < threshold && !in_pulse){ // found a pulse
      in_pulse = true;      
    }
    
    if(sample >= threshold && in_pulse){ /// finished this pulse
      in_pulse = false;
      if(fitresult->numPulses < MAX_PULSES){
        fitresult->pulseTimes[fitresult->numPulses] = min_bin * 8.0;
        fitresult->pulseCharges[fitresult->numPulses] = min_value;        
        if(0)std::cout << "Pulse found : " << fitresult->numPulses << " " << min_bin
                  << " " << min_value << " " << std::endl;
        fitresult->numPulses++;      
      }
      min_bin = 9999, min_value = 9999;
    }
    
  }

  if(fitresult->numPulses){ std::cout << "number of pulses found: " << fitresult->numPulses << " " << std::endl;}
  
}

