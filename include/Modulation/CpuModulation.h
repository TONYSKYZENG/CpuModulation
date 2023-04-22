/*! \file CpuModulation.h*/
//
// Created by tony on 10/02/23.
//

#ifndef CPUMODULATION_INCLUDE_MODULATION_CPUMODULATION_H_
#define CPUMODULATION_INCLUDE_MODULATION_CPUMODULATION_H_
#include <Utils/AbstractC20Thread.hpp>
#include <Utils/ConfigMap.hpp>
#include <atomic>
#include <memory>
namespace CM {
using namespace INTELLI;
/**
 *  @defgroup CM_MODULATION The classes to conduct CPU modulation
 *  @{
 * This package is used to store configuration information in an unified map and
 * get away from too many stand-alone functtions
*/
/**
 * @ingroup CM_MODULATION
 * @class ModulationInfo The class to store control info, accessed by workers and updated by top
 */
class ModulationControlInfo{
 public:
  ModulationControlInfo(){}
  ~ModulationControlInfo(){}
  /**
   * @brief 0 stop, 1 full run, 2 empty run
   */
  std::atomic_uint32_t markState=0;
  std::atomic_uint32_t stopState=0;
};
/**
 * @ingroup CM_MODULATION
 * @typedef ModulationControlInfoPtr
 * @brief The class to describe a shared pointer to @ref ModulationControlInfo
 */
typedef std::shared_ptr<ModulationControlInfo> ModulationControlInfoPtr;
/**
 * @ingroup CM_MODULATION
 * @def newModulationControlInfo
 * @brief (Macro) To creat a new @ref ModulationControlInfo under shared pointer.
 */
#define  newModulationControlInfo make_shared<CM::ModulationControlInfo>
//typedef std::shared_ptr<std::barrier<>> BarrierPtr;
/**
 * @ingroup CM_MODULATION
 * @class ModulatedWorkers The work threads to conduct cpu modulation
 */
class ModulatedWorkers : public AbstractC20Thread{
 public:
  ModulatedWorkers(){}
  ~ModulatedWorkers(){}
  /**
   * @brief set the core bind of this thread
   * @param cid the core to be binded
   * @note call before start
   */
  void setCore(int cid)
  {
    myCore=cid;
  }
  ModulationControlInfoPtr controlInfo= nullptr;
  void setControlInfo(ModulationControlInfoPtr _ci)
  {
    controlInfo=_ci;
  }
  BarrierPtr bar1p= nullptr,bar2p= nullptr;
  void setBarriers(BarrierPtr b1,BarrierPtr b2)
  {
    bar1p=b1;
    bar2p=b2;
  }
  void setDuty(uint64_t fullUs,uint64_t emptyUs)
  {
    fullLengthUs=fullUs;
    emptyLengthUs=emptyUs;
  }
 protected:
  /**
  * @brief The inline 'main" function of thread, as an interface
  * @note Normally re-write this in derived classes
  */
  virtual void inlineMain();
  void inlineCoreBind(int cid);
  uint64_t fullLengthUs=0,emptyLengthUs=0;
  int myCore;
  uint64_t inCnt=0;
  uint64_t inCnt2=0;
  uint64_t dummyArrary[8192];
  void dummyFunction();
};
typedef std::shared_ptr<ModulatedWorkers> ModulatedWorkersPtr;
#define newModulatedWorkers  make_shared<CM::ModulatedWorkers>
/**
 * @ingroup CM_MODULATION
 * @class CpuModulation The top class to conduct CpuModulation
 */
class CpuModulation {
 protected:
  vector<ModulatedWorkersPtr>myWorkers;
  uint64_t runLengthUs=0;
  uint64_t fullLengthUs=0,emptyLengthUs=0,periodUs=0;
  uint64_t nowRunUs=0;

 public:
  CpuModulation(){}
  ~CpuModulation(){}
  ModulationControlInfoPtr controlInfo= nullptr;
  BarrierPtr bar1p= nullptr,bar2p= nullptr;
  void setUpWorkers(int wks);
  void setRunLength(uint64_t runUs)
  {
    runLengthUs=runUs;
  }

  void setDuty(uint64_t fullUs,uint64_t emptyUs)
  {
    fullLengthUs=fullUs;
    emptyLengthUs=emptyUs;
    periodUs=fullLengthUs+emptyLengthUs;
  }
  void runModulation(void);
};
/**
 * @}
 */
} // CM

#endif //CPUMODULATION_INCLUDE_MODULATION_CPUMODULATION_H_
