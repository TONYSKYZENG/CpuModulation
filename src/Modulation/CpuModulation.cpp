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
#include <pthread.h>
namespace CM {
pthread_barrier_t barrierFirst,barrierSecond;
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
void ModulatedWorkers::dummyFunction() {
  inCnt++;
  inCnt2+=inCnt;
  if(inCnt%100)
  {
    dummyArrary[inCnt2%8192]=inCnt2;
  }

}
void ModulatedWorkers::inlineMain() {
  assert(bar1p);
  assert(bar2p);
  assert(controlInfo);
  inlineCoreBind(myCore);
  printf("bind to core %d\r\n",myCore);
  while (1)
  { if(controlInfo->stopState==1)
    {
    std::printf("exit core %d\r\n",myCore);
      return;
    }
    pthread_barrier_wait(&barrierFirst);
    while (controlInfo->markState==1)
    {
    dummyFunction();
    }
    pthread_barrier_wait(&barrierSecond);
  }
}
void CpuModulation::setUpWorkers(int wks) {
  myWorkers=vector<ModulatedWorkersPtr>(wks);
  bar1p=std::make_shared<std::barrier<>>(wks+1);
  bar2p=std::make_shared<std::barrier<>>(wks+1);
  pthread_barrier_init(&barrierFirst, NULL, wks+1);
  pthread_barrier_init(&barrierSecond, NULL, wks+1);
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
  controlInfo->stopState=0;
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
  while(UtilityFunctions::timeLastUs(ts)<runLengthUs&&controlInfo->markState>0)
  {
    controlInfo->markState=1;
    pthread_barrier_wait(&barrierFirst);
    // bar1p->arrive_and_wait();
    while(UtilityFunctions::timeLastUs(ts)%periodUs<fullLengthUs);
    controlInfo->markState=2;
    if(UtilityFunctions::timeLastUs(ts)+emptyLengthUs>=runLengthUs)
    {
      controlInfo->stopState=1;
       std::printf("main thread should stop\r\n");
      // break;
    }
    pthread_barrier_wait(&barrierSecond);
     //bar2p->arrive_and_wait();
    while(UtilityFunctions::timeLastUs(ts)%periodUs>=fullLengthUs);
    if(UtilityFunctions::timeLastUs(ts)>=runLengthUs)
    {
      controlInfo->markState=0;
      std::printf("main thread done\r\n");
    }
  }
  //usleep(runLengthUs);
  std::printf("modulation done\r\n");
  //controlInfo->markState=0;
  for(size_t i=0;i<wks;i++)
  {
    myWorkers[i]->joinThread();
  }
  std::printf("clean up done\r\n");
}
} // CM