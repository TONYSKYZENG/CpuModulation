//
// Created by tony on 10/02/23.
//

#include <Modulation/CpuModulation.h>
#include <time.h>
#include <sched.h>
#include <pthread.h>
#include<cstdlib>
#include<time.h>
#include <Utils/UtilityFunctions.hpp>
#include <chrono>
namespace CM {
void ModulatedWorkers::inlineCoreBind(int cpuId) {
  cpu_set_t mask;
  if(cpuId<0)
  {
    return;
  }
  CPU_ZERO(&mask);
  CPU_SET(cpuId, &mask);
  if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) < 0) {
    exit(0);
  }
  return;
}
void ModulatedWorkers::dummyFunction(uint64_t k) {
  uint64_t ru=0;
  for(uint32_t i=0;i<100;i++)
  {
    ru=i;
    for(uint64_t j=0;j<k;j++)
    {
      ru+=j;
    }
    dummyArrary[i]=ru;
  }

}
void ModulatedWorkers::inlineMain() {
  assert(bar1p);
  assert(bar2p);
  assert(controlInfo);
  inlineCoreBind(myCore);
  printf("bind to core %d\r\n",myCore);
 //. uint64_t k=0;
  while (1)
  { if(controlInfo->markState==0)
    {
    std::printf("exit core %d\r\n",myCore);
      return;
    }

    bar1p->arrive_and_wait();
    auto start = std::chrono::high_resolution_clock::now();
    // 取指执行的时间为 10000 μs
    while(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() < fullLengthUs) ;
    usleep(emptyLengthUs);
    bar2p->arrive_and_wait();



  }
}
void CpuModulation::setUpWorkers(int wks) {
  myWorkers=vector<ModulatedWorkersPtr>(wks);
  bar1p=std::make_shared<std::barrier<>>(wks);
  bar2p=std::make_shared<std::barrier<>>(wks);
  controlInfo=newModulationControlInfo();
  for(size_t i=0;i<wks;i++)
  {
    myWorkers[i]=newModulatedWorkers();
    myWorkers[i]->setCore(i);
    myWorkers[i]->setControlInfo(controlInfo);
    myWorkers[i]->setBarriers(bar1p,bar2p);
  }
}
void CpuModulation::runModulation() {
  struct timeval ts;

  size_t wks=myWorkers.size();
  controlInfo->markState=1;
  for(size_t i=0;i<wks;i++)
  {
    myWorkers[i]->setDuty(fullLengthUs,emptyLengthUs);
  }
  for(size_t i=0;i<wks;i++)
  {
    myWorkers[i]->startThread();
  }
  // thrust to run
  std::printf("modulation start:%ld+%ld=%ld\r\n",fullLengthUs,emptyLengthUs,periodUs);
  gettimeofday(&ts, NULL);
  usleep(runLengthUs);
  std::printf("modulation done\r\n");
  controlInfo->markState=0;
  for(size_t i=0;i<wks;i++)
  {
    myWorkers[i]->joinThread();
  }
  std::printf("clean up done\r\n");
}
} // CM