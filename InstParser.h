//===-- llvm/IR/AsmWriter.h - Printing LLVM IR as an assembly file - C++ --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This files defines the interface for the InstParser class used to print
// LLVM IR and various helper classes that are used in printing.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_IR_ASSEMBLYWRITER_H
#define LLVM_IR_ASSEMBLYWRITER_H

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/TypeFinder.h"
#include "llvm/Support/FormattedStream.h"
#include "CFG.h"
namespace llvm {

    class BasicBlock;
    class Function;
    class GlobalValue;
    class Comdat;
    class Module;
    class NamedMDNode;
    class Value;
    //class CFG;
    class SlotTracker {
        public:
            /// ValueMap - A mapping of Values to slot numbers.
            typedef DenseMap<const Value*, unsigned> ValueMap;

        private:
            /// TheModule - The module for which we are holding slot numbers.
            const Module* TheModule;

            /// TheFunction - The function for which we are holding slot numbers.
            const Function* TheFunction;
            bool FunctionProcessed;

            /// mMap - The slot map for the module level data.
            ValueMap mMap;
            unsigned mNext;

            /// fMap - The slot map for the function level data.
            ValueMap fMap;
            unsigned fNext;

            /// mdnMap - Map for MDNodes.
            DenseMap<const MDNode*, unsigned> mdnMap;
            unsigned mdnNext;

            /// asMap - The slot map for attribute sets.
            DenseMap<AttributeSet, unsigned> asMap;
            unsigned asNext;
        public:
            /// Construct from a module
            explicit SlotTracker(const Module *M);
            /// Construct from a function, starting out in incorp state.
            explicit SlotTracker(const Function *F);

            /// Return the slot number of the specified value in it's type
            /// plane.  If something is not in the SlotTracker, return -1.
            int getLocalSlot(const Value *V);
            int getGlobalSlot(const GlobalValue *V);
            int getMetadataSlot(const MDNode *N);
            int getAttributeGroupSlot(AttributeSet AS);

            /// If you'd like to deal with a function instead of just a module, use
            /// this method to get its data into the SlotTracker.
            void incorporateFunction(const Function *F) {
                TheFunction = F;
                FunctionProcessed = false;
            }

            /// After calling incorporateFunction, use this method to remove the
            /// most recently incorporated function from the SlotTracker. This
            /// will reset the state of the machine back to just the module contents.
            void purgeFunction();

            /// MDNode map iterators.
            typedef DenseMap<const MDNode*, unsigned>::iterator mdn_iterator;
            mdn_iterator mdn_begin() { return mdnMap.begin(); }
            mdn_iterator mdn_end() { return mdnMap.end(); }
            unsigned mdn_size() const { return mdnMap.size(); }
            bool mdn_empty() const { return mdnMap.empty(); }

            /// AttributeSet map iterators.
            typedef DenseMap<AttributeSet, unsigned>::iterator as_iterator;
            as_iterator as_begin()   { return asMap.begin(); }
            as_iterator as_end()     { return asMap.end(); }
            unsigned as_size() const { return asMap.size(); }
            bool as_empty() const    { return asMap.empty(); }

            /// This function does the actual initialization.
            inline void initialize();

            // Implementation Details
        private:
            /// CreateModuleSlot - Insert the specified GlobalValue* into the slot table.
            void CreateModuleSlot(const GlobalValue *V);

            /// CreateMetadataSlot - Insert the specified MDNode* into the slot table.
            void CreateMetadataSlot(const MDNode *N);

            /// CreateFunctionSlot - Insert the specified Value* into the slot table.
            void CreateFunctionSlot(const Value *V);

            /// \brief Insert the specified AttributeSet into the slot table.
            void CreateAttributeSetSlot(AttributeSet AS);

            /// Add all of the module level global variables (and their initializers)
            /// and function declarations, but not the contents of those functions.
            void processModule();

            /// Add all of the functions arguments, basic blocks, and instructions.
            void processFunction();

            SlotTracker(const SlotTracker &) LLVM_DELETED_FUNCTION;
            void operator=(const SlotTracker &) LLVM_DELETED_FUNCTION;
    };

    /// Create a new SlotTracker for a Module 
    //SlotTracker *createSlotTracker(const Module *M);

    //===----------------------------------------------------------------------===//
    // TypePrinting Class: Type printing machinery
    //===----------------------------------------------------------------------===//

    class TypePrinting {
        TypePrinting(const TypePrinting &) LLVM_DELETED_FUNCTION;
        void operator=(const TypePrinting&) LLVM_DELETED_FUNCTION;
        public:

        /// NamedTypes - The named types that are used by the current module.
        TypeFinder NamedTypes;

        /// NumberedTypes - The numbered types, along with their value.
        DenseMap<StructType*, unsigned> NumberedTypes;


        TypePrinting() {}
        ~TypePrinting() {}

        void incorporateTypes(const Module &M);

        void print(Type *Ty, raw_ostream &OS);

        void printStructBody(StructType *Ty, raw_ostream &OS);
    };

    class InstParser {
        protected:
            formatted_raw_ostream &Out;
            const Module *TheModule;

        private:
            std::unique_ptr<SlotTracker> ModuleSlotTracker;
            SlotTracker &Machine;
            TypePrinting TypePrinter;
            AssemblyAnnotationWriter *AnnotationWriter;
            SetVector<const Comdat *> Comdats;

        public:
            /// Construct an InstParser with an external SlotTracker
            InstParser(formatted_raw_ostream &o, SlotTracker &Mac,
                    const Module *M, AssemblyAnnotationWriter *AAW);
            void InsertCFGLabel(CFG* cfg,const BasicBlock *BB, State* s, string func);
            /// Construct an InstParser with an internally allocated SlotTracker
            InstParser(formatted_raw_ostream &o, const Module *M,
                    AssemblyAnnotationWriter *AAW);

            virtual ~InstParser();

            void printMDNodeBody(const MDNode *MD);
            void printNamedMDNode(const NamedMDNode *NMD);

            void printModule(const Module *M);

            void writeOperand(const Value *Op, bool PrintType);
            void writeParamOperand(const Value *Operand, AttributeSet Attrs,unsigned Idx);
            void writeAtomic(AtomicOrdering Ordering, SynchronizationScope SynchScope);
            void writeAtomicCmpXchg(AtomicOrdering SuccessOrdering,
                    AtomicOrdering FailureOrdering,
                    SynchronizationScope SynchScope);

            void writeAllMDNodes();
            void writeMDNode(unsigned Slot, const MDNode *Node);
            void writeAllAttributeGroups();

            void printTypeIdentities();
            void printGlobal(const GlobalVariable *GV);
            void printAlias(const GlobalAlias *GV);
            void printComdat(const Comdat *C);
            void printFunction(const Function *F);
            void printArgument(const Argument *FA, AttributeSet Attrs, unsigned Idx);
            void printBasicBlock(const BasicBlock *BB);
            void printInstructionLine(const Instruction &I);
            void printInstruction(const Instruction &I);

        private:
            void init();

            // printInfoComment - Print a little comment after the instruction indicating
            // which slot it occupies.
            void printInfoComment(const Value &V);
        public:
            void setConstraint(CFG *cfg, State * &s, BasicBlock::iterator &it, string func);
            //Set the variableList
            //return the current variable's ID
            int setVariable(CFG* cfg, const Instruction *I, int ID, string func);
            string getDesVarName(const Instruction *I);
    };

} // namespace llvm


#endif //LLVM_IR_ASMWRITER_H
