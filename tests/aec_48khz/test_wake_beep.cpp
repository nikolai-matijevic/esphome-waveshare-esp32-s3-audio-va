#include "wake_beep.h"
#include <cassert>
#include <iostream>
#include <vector>
using namespace esphome::aec_speexdsp;
int main() {
 WakeBeep beep;beep.start();std::vector<int16_t> block(512*2);
 size_t total=0;unsigned crossings=0;int16_t last=0;
 do {
  std::fill(block.begin(),block.end(),0);
  auto n=beep.render(block.data(),512,2);total+=n;
  for(size_t i=0;i<n;++i) {
   assert(block[2*i]==block[2*i+1]);
   if(last<0 && block[2*i]>=0) ++crossings;
   last=block[2*i];
  }
 }while(beep.active());
 assert(total==PHYSICAL_SAMPLE_RATE/10);assert(crossings>=98 && crossings<=100);
 assert(beep.render(block.data(),512,2)==0);
 std::cout<<"PASS: 100 ms / 1 kHz stereo wake beep at "<<PHYSICAL_SAMPLE_RATE<<" Hz\n";
}
