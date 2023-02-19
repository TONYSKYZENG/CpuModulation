// Copyright (C) 2021 by the CpuModulation team (https://github.com/CpuModulation)

/**
 * @brief This is the main entry point of the entire program.
 * We use this as the entry point for benchmarking.
 */
#include <iostream>
#include <Modulation/CpuModulation.h>
using namespace std;
using namespace CM;
int main(int argc,char *argv[]) {
  CpuModulation cm;
  if(argc<3)
  {
    std::printf("usage: [program] [length in ms] [frequency in Hz] [full rate optional]");
    return 0;
  }
  uint64_t len= atoi(argv[1])*1000;
  double f=atof(argv[2]);
  double t=1e6/f;
  double fullRate=0.5;
  if(argc>=4)
  {
    fullRate= atof(argv[3]);
    if(fullRate>=1||fullRate<=0)
    {
      fullRate=0.5;
    }
  }
  uint64_t tFull=(uint64_t)(t*fullRate);
  uint64_t tEmpty=(uint64_t)(t*(1-fullRate));
  std::printf("len=%ld us,frequency=%lf Hz,full run=%ld us,empty run=%ld us\r\n",len,f,tFull,tEmpty);
  cm.setRunLength(len);
  cm.setDuty(tFull,tEmpty);
  cm.setUpWorkers( std::thread::hardware_concurrency());
  cm.runModulation();
  return 0;
}

