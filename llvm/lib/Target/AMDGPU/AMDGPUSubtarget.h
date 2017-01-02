//=====-- AMDGPUSubtarget.h - Define Subtarget for AMDGPU ------*- C++ -*-====//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//==-----------------------------------------------------------------------===//
//
/// \file
/// \brief AMDGPU specific subclass of TargetSubtarget.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AMDGPU_AMDGPUSUBTARGET_H
#define LLVM_LIB_TARGET_AMDGPU_AMDGPUSUBTARGET_H

#include "AMDGPU.h"
#include "AMDGPUFrameLowering.h"
#include "AMDGPUISelLowering.h"
#include "AMDGPUInstrInfo.h"
#include "AMDGPUSubtarget.h"
#include "Utils/AMDGPUBaseInfo.h"
#include "llvm/CodeGen/GlobalISel/GISelAccessor.h"
#include "llvm/Target/TargetSubtargetInfo.h"

#define GET_SUBTARGETINFO_HEADER
#include "AMDGPUGenSubtargetInfo.inc"

namespace llvm {

class StringRef;
class SIMachineFunctionInfo;

class AMDGPUSubtarget : public AMDGPUGenSubtargetInfo {

public:
  enum Generation {
    R600 = 0,
    R700,
    EVERGREEN,
    NORTHERN_ISLANDS,
    SOUTHERN_ISLANDS,
    SEA_ISLANDS,
    VOLCANIC_ISLANDS,
  };

  enum {
    FIXED_SGPR_COUNT_FOR_INIT_BUG = 80
  };

  enum {
    ISAVersion0_0_0,
    ISAVersion7_0_0,
    ISAVersion7_0_1,
    ISAVersion8_0_0,
    ISAVersion8_0_1,
    ISAVersion8_0_3
  };

private:
  bool DumpCode;
  bool R600ALUInst;
  bool HasVertexCache;
  short TexVTXClauseSize;
  Generation Gen;
  bool FP64;
  bool FP64Denormals;
  bool FP32Denormals;
  bool FPExceptions;
  bool FastFMAF32;
  bool HalfRate64Ops;
  bool CaymanISA;
  bool FlatAddressSpace;
  bool FlatForGlobal;
  bool EnableIRStructurizer;
  bool EnablePromoteAlloca;
  bool EnableIfCvt;
  bool EnableLoadStoreOpt;
  bool EnableUnsafeDSOffsetFolding;
  bool EnableXNACK;
  unsigned WavefrontSize;
  bool CFALUBug;
  int LocalMemorySize;
  unsigned MaxPrivateElementSize;
  bool EnableVGPRSpilling;
  bool SGPRInitBug;
  bool IsGCN;
  bool GCN1Encoding;
  bool GCN3Encoding;
  bool CIInsts;
  bool HasSMemRealTime;
  bool Has16BitInsts;
  bool FeatureDisable;
  int LDSBankCount;
  unsigned IsaVersion;
  bool EnableSIScheduler;
  bool DebuggerInsertNops;
  bool DebuggerReserveTrapVGPRs;

  std::unique_ptr<AMDGPUFrameLowering> FrameLowering;
  std::unique_ptr<AMDGPUTargetLowering> TLInfo;
  std::unique_ptr<AMDGPUInstrInfo> InstrInfo;
  std::unique_ptr<GISelAccessor> GISel;
  InstrItineraryData InstrItins;
  Triple TargetTriple;

public:
  AMDGPUSubtarget(const Triple &TT, StringRef CPU, StringRef FS,
                  TargetMachine &TM);
  AMDGPUSubtarget &initializeSubtargetDependencies(const Triple &TT,
                                                   StringRef GPU, StringRef FS);

  void setGISelAccessor(GISelAccessor &GISel) {
    this->GISel.reset(&GISel);
  }

  const AMDGPUFrameLowering *getFrameLowering() const override {
    return FrameLowering.get();
  }
  const AMDGPUInstrInfo *getInstrInfo() const override {
    return InstrInfo.get();
  }
  const AMDGPURegisterInfo *getRegisterInfo() const override {
    return &InstrInfo->getRegisterInfo();
  }
  AMDGPUTargetLowering *getTargetLowering() const override {
    return TLInfo.get();
  }
  const InstrItineraryData *getInstrItineraryData() const override {
    return &InstrItins;
  }

  const CallLowering *getCallLowering() const override;

  void ParseSubtargetFeatures(StringRef CPU, StringRef FS);

  bool hasVertexCache() const {
    return HasVertexCache;
  }

  short getTexVTXClauseSize() const {
    return TexVTXClauseSize;
  }

  Generation getGeneration() const {
    return Gen;
  }

  bool hasHWFP64() const {
    return FP64;
  }

  bool hasCaymanISA() const {
    return CaymanISA;
  }

  bool hasFP32Denormals() const {
    return FP32Denormals;
  }

  bool hasFP64Denormals() const {
    return FP64Denormals;
  }

  bool hasFPExceptions() const {
    return FPExceptions;
  }

  bool hasFastFMAF32() const {
    return FastFMAF32;
  }

  bool hasHalfRate64Ops() const {
    return HalfRate64Ops;
  }

  bool hasFlatAddressSpace() const {
    return FlatAddressSpace;
  }

  bool hasSMemRealTime() const {
    return HasSMemRealTime;
  }

  bool has16BitInsts() const {
    return Has16BitInsts;
  }

  bool useFlatForGlobal() const {
    return FlatForGlobal;
  }

  bool hasBFE() const {
    return (getGeneration() >= EVERGREEN);
  }

  bool hasBFI() const {
    return (getGeneration() >= EVERGREEN);
  }

  bool hasBFM() const {
    return hasBFE();
  }

  bool hasBCNT(unsigned Size) const {
    if (Size == 32)
      return (getGeneration() >= EVERGREEN);

    if (Size == 64)
      return (getGeneration() >= SOUTHERN_ISLANDS);

    return false;
  }

  bool hasMulU24() const {
    return (getGeneration() >= EVERGREEN);
  }

  bool hasMulI24() const {
    return (getGeneration() >= SOUTHERN_ISLANDS ||
            hasCaymanISA());
  }

  bool hasFFBL() const {
    return (getGeneration() >= EVERGREEN);
  }

  bool hasFFBH() const {
    return (getGeneration() >= EVERGREEN);
  }

  bool hasCARRY() const {
    return (getGeneration() >= EVERGREEN);
  }

  bool hasBORROW() const {
    return (getGeneration() >= EVERGREEN);
  }

  bool IsIRStructurizerEnabled() const {
    return EnableIRStructurizer;
  }

  bool isPromoteAllocaEnabled() const {
    return EnablePromoteAlloca;
  }

  bool isIfCvtEnabled() const {
    return EnableIfCvt;
  }

  bool loadStoreOptEnabled() const {
    return EnableLoadStoreOpt;
  }

  bool unsafeDSOffsetFoldingEnabled() const {
    return EnableUnsafeDSOffsetFolding;
  }

  unsigned getWavefrontSize() const {
    return WavefrontSize;
  }

  unsigned getStackEntrySize() const;

  bool hasCFAluBug() const {
    assert(getGeneration() <= NORTHERN_ISLANDS);
    return CFALUBug;
  }

  int getLocalMemorySize() const {
    return LocalMemorySize;
  }

  unsigned getMaxPrivateElementSize() const {
    return MaxPrivateElementSize;
  }

  bool hasSGPRInitBug() const {
    return SGPRInitBug;
  }

  int getLDSBankCount() const {
    return LDSBankCount;
  }

  unsigned getAmdKernelCodeChipID() const;

  AMDGPU::IsaVersion getIsaVersion() const;

  bool enableMachineScheduler() const override {
    return true;
  }

  void overrideSchedPolicy(MachineSchedPolicy &Policy,
                           MachineInstr *begin, MachineInstr *end,
                           unsigned NumRegionInstrs) const override;

  // Helper functions to simplify if statements
  bool isTargetELF() const {
    return false;
  }

  bool enableSIScheduler() const {
    return EnableSIScheduler;
  }

  bool debuggerInsertNops() const {
    return DebuggerInsertNops;
  }

  bool debuggerReserveTrapVGPRs() const {
    return DebuggerReserveTrapVGPRs;
  }

  bool dumpCode() const {
    return DumpCode;
  }
  bool r600ALUEncoding() const {
    return R600ALUInst;
  }
  bool isAmdHsaOS() const {
    return TargetTriple.getOS() == Triple::AMDHSA;
  }
  bool isVGPRSpillingEnabled(const Function& F) const;

  bool isXNACKEnabled() const {
    return EnableXNACK;
  }

  unsigned getMaxWavesPerCU() const {
    if (getGeneration() >= AMDGPUSubtarget::SOUTHERN_ISLANDS)
      return 10;

    // FIXME: Not sure what this is for other subtagets.
    llvm_unreachable("do not know max waves per CU for this subtarget.");
  }

  bool enableSubRegLiveness() const override {
    return true;
  }

  /// \brief Returns the offset in bytes from the start of the input buffer
  ///        of the first explicit kernel argument.
  unsigned getExplicitKernelArgOffset() const {
    return isAmdHsaOS() ? 0 : 36;
  }

  unsigned getMaxNumUserSGPRs() const {
    return 16;
  }
};

} // End namespace llvm

#endif