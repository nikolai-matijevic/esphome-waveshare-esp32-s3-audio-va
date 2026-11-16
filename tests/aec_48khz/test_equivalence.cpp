#include "capture_decimator.h"
#include "reference_decimator.h"
#include <random>
#include <vector>
#include <cassert>
#include <iostream>
int main() {
 std::mt19937 rng(42);std::vector<int16_t>a(48000*4);
 for(auto &v:a) v=static_cast<int16_t>(rng());
 auto b=a;
 esphome::aec_speexdsp::CaptureDecimator fast;esphome::aec_speexdsp::ReferenceDecimator reference;
 auto n=fast.process(a.data(),48000,4);auto m=reference.process(b.data(),48000,4);
 assert(n==m);assert(std::equal(a.begin(),a.begin()+n*4,b.begin()));
 std::cout<<"PASS: optimized filter equals full int64 convolution for full-scale random input\n";
}
