#pragma warning(push)
#pragma warning(disable: 4267 4244)
#include "dxc/DXIL/DxilModule.h"
#include "dxc/DXIL/DxilOperations.h"
#include "dxc/DXIL/DxilInstructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#pragma warning(pop)

#include <vector>
#include <map>
#include <set>

using namespace llvm;
using namespace hlsl;

namespace vista
{
	struct BindlessAccessInfo
	{
		Bool isConstantIndex;
		Uint32 constantValue;        // Only valid if isConstantIndex = true

		Bool indexFromConstantBuffer;
		Uint32 cbSlot;              // Which constant buffer (b0, b1, etc.)
		Uint32 cbOffset;            // Byte offset within the CB
		Bool cbOffsetKnown;         // Whether we could determine the offset

		CallInst* createHandleCall; // The createHandleFromHeap instruction
		Uint32 instructionIndex;    // For ordering/identification
	};

	struct CBLoadInfo 
	{
		Uint32 cbSlot;
		Uint32 byteOffset;
		Bool isConstantOffset;
		CallInst* loadInstruction;
	};

	class DXILBindlessAnalyzer 
	{
	public:
		DXILBindlessAnalyzer() : instructionCounter(0) {}

		Bool AnalyzeDXILModule(Module* pModule) 
		{
			if (!pModule)
			{
				return false;
			}

			CollectConstantBufferLoads(pModule);
			FindBindlessAccesses(pModule);
			return true;
		}

		std::vector<BindlessAccessInfo> const& GetBindlessAccesses() const 
		{
			return m_bindlessAccesses;
		}

	private:
		void CollectConstantBufferLoads(Module* pModule) 
		{
			for (Function& F : pModule->functions()) 
			{
				for (BasicBlock& BB : F) 
				{
					for (Instruction& I : BB) 
					{
						if (CallInst* CI = dyn_cast<CallInst>(&I)) 
						{
							if (IsCBufferLoad(CI)) 
							{
								AnalyzeConstantBufferLoad(CI);
							}
						}
					}
				}
			}
		}

		void FindBindlessAccesses(Module* pModule) 
		{
			for (Function& F : pModule->functions()) 
			{
				for (BasicBlock& BB : F) 
				{
					for (Instruction& I : BB) 
					{
						if (CallInst* CI = dyn_cast<CallInst>(&I)) 
						{
							if (IsCreateHandleFromHeap(CI)) 
							{
								AnalyzeBindlessAccess(CI);
							}
						}
						instructionCounter++;
					}
				}
			}
		}

		Bool IsCBufferLoad(CallInst* pCall) const
		{
			Function* pCalledFunc = pCall->getCalledFunction();
			if (!pCalledFunc) return false;

			StringRef funcName = pCalledFunc->getName();
			return funcName.startswith_lower("dx.op.cbufferLoadLegacy") || funcName.startswith_lower("dx.op.cbufferLoad");
		}

		bool IsCreateHandleFromHeap(CallInst* pCall) 
		{
			Function* pCalledFunc = pCall->getCalledFunction();
			if (!pCalledFunc) return false;

			StringRef funcName = pCalledFunc->getName();
			return funcName.startswith_lower("dx.op.createHandleFromHeap");
		}

		void AnalyzeConstantBufferLoad(CallInst* pCall) 
		{
			// cbufferLoadLegacy signature: (opcode, handle, byteOffset)
			CBLoadInfo cbInfo;
			cbInfo.loadInstruction = pCall;

			Value* cbHandle = pCall->getArgOperand(1);
			cbInfo.cbSlot = GetConstantBufferSlot(cbHandle);

			Value* offsetArg = pCall->getArgOperand(2);
			if (ConstantInt* constOffset = dyn_cast<ConstantInt>(offsetArg)) 
			{
				cbInfo.isConstantOffset = true;
				cbInfo.byteOffset = static_cast<Uint32>(constOffset->getZExtValue());
			}
			else {
				cbInfo.isConstantOffset = false;
				cbInfo.byteOffset = 0; 
			}
			m_cbLoads[pCall] = cbInfo;
		}

		Uint32 GetConstantBufferSlot(Value* cbHandle) const
		{
			// Trace back to the createHandleFromBinding call
			if (CallInst* handleCall = dyn_cast<CallInst>(cbHandle)) 
			{
				Function* func = handleCall->getCalledFunction();
				if (func && func->getName().startswith_lower("createHandleFromBinding")) 
				{
					Value* bindArg = handleCall->getArgOperand(1);
					if (ConstantStruct* bindStruct = dyn_cast<ConstantStruct>(bindArg)) 
					{
						if (bindStruct->getNumOperands() >= 1) 
						{
							if (ConstantInt* slotConst = dyn_cast<ConstantInt>(bindStruct->getOperand(0)))
							{
								return (Uint32)slotConst->getZExtValue();
							}
						}
					}
				}
			}
			return 0; 
		}

		void AnalyzeBindlessAccess(CallInst* pCall) 
		{
			// createHandleFromHeap signature: (opcode, index, samplerHeap, nonUniformIndex)
			Value* indexArg = pCall->getArgOperand(1);

			BindlessAccessInfo info;
			info.createHandleCall = pCall;
			info.instructionIndex = instructionCounter;
			info.indexFromConstantBuffer = false;
			info.cbOffsetKnown = false;

			if (ConstantInt* constIndex = dyn_cast<ConstantInt>(indexArg)) 
			{
				// Constant index - easy case
				info.isConstantIndex = true;
				info.constantValue = (Uint32)constIndex->getZExtValue();
			}
			else 
			{
				info.isConstantIndex = false;
				TraceIndexToConstantBuffer(indexArg, info);
			}

			m_bindlessAccesses.push_back(info);
		}

		void TraceIndexToConstantBuffer(Value* value, BindlessAccessInfo& info) 
		{
			std::set<Value*> visited;
			TraceIndexRecursive(value, info, visited);
		}

		Bool TraceIndexRecursive(Value* value, BindlessAccessInfo& info, std::set<Value*>& visited) 
		{
			if (visited.count(value)) return false;

			visited.insert(value);

			if (ExtractValueInst* extract = dyn_cast<ExtractValueInst>(value)) 
			{
				// This is likely extracting from a CB load result
				Value* aggregate = extract->getAggregateOperand();
				if (CallInst* aggregateCall = dyn_cast<CallInst>(aggregate)) 
				{
					auto it = m_cbLoads.find(aggregateCall);
					if (it != m_cbLoads.end()) 
					{
						info.indexFromConstantBuffer = true;
						info.cbSlot = it->second.cbSlot;
						info.cbOffsetKnown = it->second.isConstantOffset;

						if (info.cbOffsetKnown) 
						{
							// Calculate the actual offset based on extract index
							Uint32 baseOffset = it->second.byteOffset;
							Uint32 fieldIndex = extract->getIndices()[0];
							info.cbOffset = baseOffset + (fieldIndex * 4); // Assuming 4-byte fields
						}
						return true;
					}
				}
			}
			else if (Instruction* inst = dyn_cast<Instruction>(value)) 
			{
				switch (inst->getOpcode()) 
				{
				case Instruction::Add:
				case Instruction::Mul:
				case Instruction::Sub:
					for (auto& op : inst->operands()) 
					{
						if (TraceIndexRecursive(op.get(), info, visited)) 
						{
							// Note: For arithmetic operations, we found a CB dependency
							// but the exact offset calculation becomes complex
							info.cbOffsetKnown = false;
							return true;
						}
					}
					break;

				case Instruction::Load:
					return TraceIndexRecursive(inst->getOperand(0), info, visited);

				case Instruction::PHI:
					PHINode* phi = cast<PHINode>(inst);
					for (unsigned i = 0; i < phi->getNumIncomingValues(); ++i) 
					{
						if (TraceIndexRecursive(phi->getIncomingValue(i), info, visited))
						{
							info.cbOffsetKnown = false; // Multiple sources, offset unclear
							return true;
						}
					}
					break;
				}
			}

			return false;
		}

		private:
			std::vector<BindlessAccessInfo> m_bindlessAccesses;
			std::map<CallInst*, CBLoadInfo> m_cbLoads;
			Uint32 instructionCounter;

	};

	std::vector<BindlessAccessInfo> AnalyzeDXILBlob(const void* pDXILBlob, size_t blobSize) 
	{
		DXILBindlessAnalyzer analyzer;
		VISTA_TODO("properly load the DXIL module using DxilModule::LoadFromBitcode or similar");
		//Module* pModule = LoadDXILModule(pDXILBlob, blobSize);
		//analyzer.AnalyzeDXILModule(pModule);
		return analyzer.GetBindlessAccesses();
	}

	// Print analysis results
	//void PrintBindlessAnalysis(const std::vector<BindlessAccessInfo>& accesses) {
	//	printf("Found %zu bindless accesses:\n\n", accesses.size());
	//
	//	for (size_t i = 0; i < accesses.size(); ++i) 
	//	{
	//		const auto& access = accesses[i];
	//		printf("Bindless Access #%zu (Instruction %u):\n", i, access.instructionIndex);
	//
	//		if (access.isConstantIndex) {
	//			printf("  Type: Constant Index\n");
	//			printf("  Value: %u\n", access.constantValue);
	//		}
	//		else if (access.indexFromConstantBuffer) {
	//			printf("  Type: Constant Buffer Index\n");
	//			printf("  CB Slot: b%u\n", access.cbSlot);
	//			if (access.cbOffsetKnown) {
	//				printf("  CB Offset: %u bytes\n", access.cbOffset);
	//			}
	//			else {
	//				printf("  CB Offset: Unknown (computed)\n");
	//			}
	//		}
	//		else {
	//			printf("  Type: Complex/Unknown Index\n");
	//		}
	//		printf("\n");
	//	}
	//}

	struct RuntimeBindlessInfo 
	{
		std::vector<BindlessAccessInfo> staticAnalysis;

		std::vector<Uint32> ResolveIndices(void const* cbData[], Uint64 cbSizes[]) const
		{
			std::vector<Uint32> resolvedIndices;
			for (auto const& access : staticAnalysis)
			{
				if (access.isConstantIndex) 
				{
					resolvedIndices.push_back(access.constantValue);
				}
				else if (access.indexFromConstantBuffer && access.cbOffsetKnown) 
				{
					if (access.cbSlot < 16 && // Reasonable CB limit
						access.cbOffset + 4 <= cbSizes[access.cbSlot]) 
					{
						Uint8 const* cb = static_cast<Uint8 const*>(cbData[access.cbSlot]);
						Uint32 actualIndex = *reinterpret_cast<Uint32 const*>(cb + access.cbOffset);
						resolvedIndices.push_back(actualIndex);
					}
					else 
					{
						resolvedIndices.push_back(0xFFFFFFFF); 
					}
				}
				else 
				{
					resolvedIndices.push_back(0xFFFFFFFF); 
				}
			}

			return resolvedIndices;
		}
	};

}