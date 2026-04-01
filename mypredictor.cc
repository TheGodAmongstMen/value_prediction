#include <cinttypes>
#include "cvp.h"
#include "mypredictor.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <unordered_map>

#define _STRIDE

#ifdef _STRIDE
unordered_map<uint64_t, stride> strides;
#else
unordered_map<uint64_t, uint64_t> lastVal;
#endif
unordered_map<uint64_t, state> takeState;
unordered_map<uint64_t, uint64_t> pc2seq;


// Global branch and path history
static uint64_t ghr = 0, phist = 0;

// Load/Store address history
static uint64_t addrHist = 0;



bool getPrediction(uint64_t seq_no, uint64_t pc, uint8_t piece, uint64_t& predicted_value) {
    pc2seq[seq_no]=pc;  
    if (!shouldPredict(&takeState[pc])) {
        return false;   // not confident enough to predict
    }

    #ifdef _STRIDE
    uint64_t guessVal = getStride(&strides[pc]) + strides[pc].lastVal;
    #else
    uint64_t guessVal = lastVal[pc];
    #endif
    predicted_value = guessVal;
    return true;
}




void speculativeUpdate(uint64_t seq_no,    		// dynamic micro-instruction # (starts at 0 and increments indefinitely)
                       bool eligible,			// true: instruction is eligible for value prediction. false: not eligible.
		       uint8_t prediction_result,	// 0: incorrect, 1: correct, 2: unknown (not revealed)
		       // Note: can assemble local and global branch history using pc, next_pc, and insn.
		       uint64_t pc,
		       uint64_t next_pc,
		       InstClass insn,
		       uint8_t piece,
		       // Note: up to 3 logical source register specifiers, up to 1 logical destination register specifier.
		       // 0xdeadbeef means that logical register does not exist.
		       // May use this information to reconstruct architectural register file state (using log. reg. and value at updatePredictor()).
		       uint64_t src1,
		       uint64_t src2,
		       uint64_t src3,
		       uint64_t dst) {
  
   // In this example, we will only attempt to predict ALU/LOAD/SLOWALU 
  
   bool isPredictable = insn == aluInstClass || insn == loadInstClass || insn == slowAluInstClass;

  if(!isPredictable) return;


  // It's an instruction we are interested in predicting, update the first table history
  // Note that some other type of predictors may not want to update at this time if the
  // prediction is unknown to be correct or incorrect

  // At this point, any branch-related information is architectural, i.e.,
  // updating the GHR/LHRs is safe.
  bool isCondBr = insn == condBranchInstClass;
  bool isIndBr = insn == uncondIndirectBranchInstClass;

  // Infrastructure provides perfect branch prediction.
  // As a result, the branch-related histories can be updated now.
  if(isCondBr)
    ghr = (ghr << 1) | (pc + 4 != next_pc);

  if(isIndBr)
	phist = (phist << 4) | (next_pc & 0x3);
}

void updatePredictor(uint64_t seq_no,		// dynamic micro-instruction #
		     uint64_t actual_addr,	// load or store address (0xdeadbeef if not a load or store instruction)
		     uint64_t actual_value,	// value of destination register (0xdeadbeef if instr. is not eligible for value prediction)
		     uint64_t actual_latency) {	// actual execution latency of instruction
  
  uint64_t pc = pc2seq[seq_no];

  #ifdef _STRIDE
  if((strides[pc].lastVal)+getStride(&strides[pc]) == actual_value)
          stateTaken(&takeState[pc]);
  else
          stateNotTaken(&takeState[pc]);
  strideUpdate(&strides[pc],actual_value);


  #else
  if(lastVal[pc] == actual_value)
          stateTaken(&takeState[pc]);
  else
          stateNotTaken(&takeState[pc]);
  lastVal[pc] = actual_value;
  #endif
  
  // It is now safe to update the address history register
  //if(insn == loadInstClass || insn == storeInstClass) 
  if(actual_addr != 0xdeadbeef)
   addrHist = (addrHist << 4) | actual_addr;
}

void beginPredictor(int argc_other, char **argv_other) {
   if (argc_other > 0)
      printf("CONTESTANT ARGUMENTS:\n");

   for (int i = 0; i < argc_other; i++)
      printf("\targv_other[%d] = %s\n", i, argv_other[i]);
    // code of crash your computer
  /*while(1){
    //2MB of memory per iteration
    __uint128_t *Fuckr = new __uint128_t[4096*32];
  }*/
}

void endPredictor() {
	printf("CONTESTANT OUTPUT--------------------------\n");
	printf("--------------------------\n");
}
