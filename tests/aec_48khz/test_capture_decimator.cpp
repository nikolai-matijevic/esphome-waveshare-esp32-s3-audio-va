#include "capture_decimator.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
using esphome::aec_speexdsp::CaptureDecimator;
std::vector<int16_t> tone(double frequency, size_t frames) {
  std::vector<int16_t> pcm(frames*4);
  for(size_t n=0;n<frames;++n) {
    int16_t v=std::lround(10000*std::sin(2*3.141592653589793*frequency*n/48000));
    pcm[n*4]=pcm[n*4+1]=v; pcm[n*4+2]=-v; pcm[n*4+3]=0;
  }
  return pcm;
}
double gain(double frequency) {
  auto pcm=tone(frequency,48000); CaptureDecimator d;
  auto count=d.process(pcm.data(),48000,4); assert(count==16000);
  double sum=0;
  for(size_t n=100;n<count;++n) {
    assert(pcm[n*4]==pcm[n*4+1]);
    assert(std::abs(pcm[n*4]+pcm[n*4+2])<=1);
    assert(pcm[n*4+3]==0);
    sum+=double(pcm[n*4])*pcm[n*4];
  }
  return std::sqrt(sum/(count-100))/(10000/std::sqrt(2.));
}
int main() {
  assert(std::abs(gain(1000)-1)<0.01);
  assert(gain(10000)<0.004); assert(gain(16000)<0.004);
  auto whole=tone(1357,4800), chunked=whole; CaptureDecimator a,b;
  size_t expected=a.process(whole.data(),4800,4), in=0,out=0;
  const size_t chunks[]={1,17,384,2,511,127,3}; size_t c=0;
  while(in<4800) {
    size_t n=std::min(chunks[c++%7],4800-in);
    size_t k=b.process(chunked.data()+in*4,n,4);
    std::copy_n(chunked.data()+in*4,k*4,chunked.data()+out*4);
    in+=n;out+=k;
  }
  assert(out==expected); assert(std::equal(whole.begin(),whole.begin()+out*4,chunked.begin()));
  std::vector<int16_t> dc(4800*4,32767); CaptureDecimator d;
  auto n=d.process(dc.data(),4800,4);for(size_t i=100*4;i<n*4;++i) assert(dc[i]==32767);
  std::cout<<"PASS: unity passband/DC, alias rejection, slot alignment, arbitrary block boundaries\n";
}
