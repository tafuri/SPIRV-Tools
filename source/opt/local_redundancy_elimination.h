// Copyright (c) 2017 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef LIBSPIRV_OPT_LOCAL_REDUNDANCY_ELIMINATION_H_
#define LIBSPIRV_OPT_LOCAL_REDUNDANCY_ELIMINATION_H_

#include "ir_context.h"
#include "pass.h"
#include "value_number_table.h"

namespace spvtools {
namespace opt {

// This pass implements local redundancy elimination. Its goal is to reduce the
// number of times the same value is computed. It works on each basic block
// independently, ie local. For each instruction in a basic block, it gets the
// value number for the result id, |id|, of the instruction. If that value
// number has already been computed in the basic block, it tries to replace the
// uses of |id| by the id that already contains the same value. Then the
// current instruction is deleted.
class LocalRedundancyEliminationPass : public Pass {
 public:
  const char* name() const override { return "local-redundancy-elimination"; }
  Status Process(ir::IRContext*) override;
  virtual ir::IRContext::Analysis GetPreservedAnalyses() override {
    return ir::IRContext::kAnalysisDefUse |
           ir::IRContext::kAnalysisInstrToBlockMapping |
           ir::IRContext::kAnalysisDecorations |
           ir::IRContext::kAnalysisCombinators | ir::IRContext::kAnalysisCFG |
           ir::IRContext::kAnalysisDominatorAnalysis;
  }

 protected:
  // Deletes instructions in |block| whose value is in |value_to_ids| or is
  // computed earlier in |block|.
  //
  // |vnTable| must have computed a value number for every result id defined
  // in |bb|.
  //
  // |value_to_ids| is a map from value number to ids.  If {vn, id} is in
  // |value_to_ids| then vn is the value number of id, and the definition of id
  // dominates |bb|.
  //
  // Returns true if the module is changed.
  bool EliminateRedundanciesInBB(ir::BasicBlock* block,
                                 const ValueNumberTable& vnTable,
                                 std::map<uint32_t, uint32_t>* value_to_ids);
};

}  // namespace opt
}  // namespace spvtools

#endif  // LIBSPIRV_OPT_LOCAL_REDUNDANCY_ELIMINATION_H_
