#include "BindlessAccessAnalyzer.h"
#include "DxilModuleWrapper.h"
#pragma warning(push)
#pragma warning(disable: 4267 4244)
#include "dxc/DXIL/DxilModule.h"
#include "dxc/DXIL/DxilOperations.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#pragma warning(pop)


namespace vista
{
	ResourceHeapIndexInfo AnalyzeIndexSource(hlsl::DxilModule& module, llvm::Value* indexValue)
	{
		ResourceHeapIndexInfo info;
		if (llvm::ConstantInt* constInt = llvm::dyn_cast<llvm::ConstantInt>(indexValue))
		{
			info = constInt->getZExtValue();
			return info;
		}

		auto GetCBufferInfo = [&](llvm::Value* handleValue) -> hlsl::DxilCBuffer const*
		{
			llvm::Value* currentHandle = handleValue;
			while (currentHandle) 
			{
				llvm::CallInst* call = llvm::dyn_cast<llvm::CallInst>(currentHandle);
				if (!call)
				{
					break;
				}

				hlsl::OP* dxilOp = module.GetOP();
				hlsl::DXIL::OpCode opCode = dxilOp->getOpCode(call);
				if (opCode == hlsl::DXIL::OpCode::CreateHandleFromBinding) 
				{
					llvm::Constant* resBind = llvm::dyn_cast<llvm::Constant>(call->getArgOperand(1));
					if (resBind) 
					{
						//ResBind struct: { rangeId, space, lowerBound, resourceClass }
						llvm::ConstantInt* resClassVal = llvm::cast<llvm::ConstantInt>(resBind->getAggregateElement(3u));
						if (static_cast<hlsl::DXIL::ResourceClass>(resClassVal->getZExtValue()) == hlsl::DXIL::ResourceClass::CBuffer) 
						{
							llvm::ConstantInt* rangeIdVal = llvm::cast<llvm::ConstantInt>(resBind->getAggregateElement(0u));
							return &module.GetCBuffer((Uint32)rangeIdVal->getZExtValue());
						}
					}
					break; 
				}
				else if (opCode == hlsl::DXIL::OpCode::CreateHandle) 
				{
					// Signature: CreateHandle(opcode, resClass, rangeId, ...)
					llvm::ConstantInt* resClassVal = llvm::cast<llvm::ConstantInt>(call->getArgOperand(1));
					if (static_cast<hlsl::DXIL::ResourceClass>(resClassVal->getZExtValue()) == hlsl::DXIL::ResourceClass::CBuffer) 
					{
						llvm::ConstantInt* rangeIdVal = llvm::cast<llvm::ConstantInt>(call->getArgOperand(2));
						return &module.GetCBuffer((Uint32)rangeIdVal->getZExtValue());
					}
					break;
				}
				else if (opCode == hlsl::DXIL::OpCode::AnnotateHandle) 
				{
					currentHandle = call->getArgOperand(1);
				}
				else 
				{
					break;
				}
			}
			return nullptr; 
		};

		//The index comes directly from a load instruction.
		if (llvm::CallInst* call = llvm::dyn_cast<llvm::CallInst>(indexValue))
		{
			hlsl::OP* dxilOp = module.GetOP();
			if (dxilOp->getOpCode(call) == hlsl::DXIL::OpCode::BufferLoad) 
			{
				//CBV: BufferLoad(handle, byte_offset, alignment)
				hlsl::DxilCBuffer const* cbuffer = GetCBufferInfo(call->getArgOperand(1));
				if (cbuffer) 
				{
					if (llvm::ConstantInt* offsetConst = llvm::dyn_cast<llvm::ConstantInt>(call->getArgOperand(2)))
					{
						CBufferOriginInfo origin{};
						origin.ResourceName = cbuffer->GetGlobalName();
						origin.ResourceBinding = cbuffer->GetLowerBound();
						origin.ByteOffset = (Uint32)offsetConst->getZExtValue();
						return origin;
					}
				}
			}
		}
		//The index comes from extracting a value from a legacy load.
		else if (llvm::ExtractValueInst* extractInst = llvm::dyn_cast<llvm::ExtractValueInst>(indexValue))
		{
			if (llvm::CallInst* call = llvm::dyn_cast<llvm::CallInst>(extractInst->getOperand(0)))
			{
				hlsl::OP* dxilOp = module.GetOP();
				if (dxilOp->getOpCode(call) == hlsl::DXIL::OpCode::CBufferLoadLegacy) 
				{
					hlsl::DxilCBuffer const* cbuffer = GetCBufferInfo(call->getArgOperand(1));
					if (cbuffer) 
					{
						Uint32 baseByteOffset = 0;
						if (llvm::ConstantInt* offsetConst = llvm::dyn_cast<llvm::ConstantInt>(call->getArgOperand(2)))
						{
							baseByteOffset = (Uint32)offsetConst->getZExtValue();
						}

						Uint32 const componentIndex = *extractInst->getIndices().begin();
						CBufferOriginInfo origin{};
						origin.ResourceName = cbuffer->GetGlobalName();
						origin.ResourceBinding = cbuffer->GetLowerBound();
						origin.ByteOffset = baseByteOffset * 16 + (componentIndex * 4); 
						return origin;
					}
				}
			}
		}
		return info; 
	}

	std::vector<BindlessResourceAccess> AnalyzeBindlessAccesses(const void* shaderBytecode, Uint64 bytecodeSize) {
		DxilModuleWrapper dxilModule(shaderBytecode, bytecodeSize);
		hlsl::DxilModule& hlslModule = dxilModule.GetDxilModule();

		std::vector<BindlessResourceAccess> results;
		llvm::Module* module = hlslModule.GetModule();
		hlsl::OP* dxilOp = hlslModule.GetOP();

		for (llvm::Function& F : *module) 
		{
			if (F.isDeclaration())
			{
				continue;
			}

			for (llvm::BasicBlock& BB : F) 
			{
				for (llvm::Instruction& I : BB) 
				{
					auto* createHandleCall = llvm::dyn_cast<llvm::CallInst>(&I);
					if (!createHandleCall || dxilOp->getOpCode(createHandleCall) != hlsl::DXIL::OpCode::CreateHandleFromHeap) 
					{

						continue;
					}

					BindlessResourceAccess result;
					result.Index = AnalyzeIndexSource(hlslModule, createHandleCall->getArgOperand(1));
					for (llvm::User* user : createHandleCall->users()) 
					{
						if (llvm::CallInst* annotateCall = llvm::dyn_cast<llvm::CallInst>(user))
						{
							if (dxilOp->getOpCode(annotateCall) == hlsl::DXIL::OpCode::AnnotateHandle) 
							{
								llvm::Constant* props = llvm::dyn_cast<llvm::Constant>(annotateCall->getArgOperand(2));
								if (props) 
								{
									//UNPACKING BASED ON DxilResourceProperties.h
									llvm::ConstantInt* props0Val = llvm::cast<llvm::ConstantInt>(props->getAggregateElement(0u));
									Uint32 const rawDword0 = (Uint32)props0Val->getZExtValue();
									Uint32 const kindVal = rawDword0 & 0xFF;
									result.ResourceKind = static_cast<hlsl::DXIL::ResourceKind>(kindVal);
									Bool const isUAV = (rawDword0 >> 12) & 1;
									result.ResourceClass = isUAV ? hlsl::DXIL::ResourceClass::UAV : hlsl::DXIL::ResourceClass::SRV;
									break;
								}
							}
						}
					}

					if (result.ResourceKind == hlsl::DXIL::ResourceKind::Invalid || result.ResourceKind == hlsl::DXIL::ResourceKind::Sampler) 
					{
						llvm::ConstantInt* isSamplerConst = llvm::dyn_cast<llvm::ConstantInt>(createHandleCall->getArgOperand(2));
						if (isSamplerConst && isSamplerConst->getZExtValue() != 0) 
						{
							result.ResourceClass = hlsl::DXIL::ResourceClass::Sampler;
							result.ResourceKind = hlsl::DXIL::ResourceKind::Sampler;
						}
					}
					results.push_back(result);
				}
			}
		}
		return results;
	}
}
